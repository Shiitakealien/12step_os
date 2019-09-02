#include "defines.h"
#include "kozos.h"
#include "intr.h"
#include "interrupt.h"
#include "serial.h"
#include "lib.h"
#include "consdrv.h"

#define CONS_BUFFER_SIZE    24

static struct consreg {
    kz_thread_id_t id;  /* thread using a console */
    int index;          /* the index of the using console */

    char *send_buf;     /* send buffer */
    char *recv_buf;     /* recv buffer */
    int send_len;       /* size of data in send buffer */
    int recv_len;       /* size of data in recv buffer */

    /* size alignment (for the same reason as kz_msgbox in kozos.c) */
    long dummy[3];
} consreg[CONSDRV_DEVICE_NUM];

/*
 * two functions below (send_char() & send_string()) are called by interrupt
 * process or threads but are unable to re-entry since manipulating a send
 * buffer, so before calling from threads, need to set interrupt unable
 */

/* send the head of a send buffer */
static void send_char(struct consreg *cons) {
    int i;
    serial_send_byte(cons->index, cons->send_buf[0]);
    cons->send_len--;
    /* the head character is send, so slide one byte */
    for (i = 0; i < cons->send_len; i++)
        cons->send_buf[i] = cons->send_buf[i + 1];
}

/* set string in send buffer and start sending */
static void send_string(struct consreg *cons, char *str, int len) {
    int i;
    for (i = 0; i < len; i++) {
        if (str[i] == '\n') /* \n -> \r\n */
            cons->send_buf[cons->send_len++] = '\r';
        cons->send_buf[cons->send_len++] = str[i];
    }
    /*
     * When interrupt is unable, sending starts
     * When interrupt is enable, under sending and nothing to do
     */
    if (cons->send_len && !serial_intr_is_send_enable(cons->index)) {
        serial_intr_send_enable(cons->index); /* enable interrupt */
        send_char(cons); /* start sending */
    }
}

/* 
 * Below are interrupt process called by interrupt hander and async, so be care
 * when calling from library functions and others
 * Foundamentally, called from the following
 *      - re-enterable
 *      - functions never called form threads
 *      - functions which may be called from thread, but with interrupt inhibit
 * And since called with a non-context state, so never call a system call
 * (use a serivice call instead)
 */
static int consdrv_intrproc(struct consreg *cons) {
    unsigned char c;
    char *p;

    if (serial_is_recv_enable(cons->index)) { /* recv interrupt */
        c = serial_recv_byte(cons->index);
        if ( c == '\r') /* \r -> \n */
            c = '\n';

        send_string(cons, &c, 1); /* echo back */

        if (cons->id) {
            if (c != '\n') {
                /* if char is not a new-line, buffer it in a recv buffer */
                cons->recv_buf[cons->recv_len++] = c;
            } else {
                /*
                 * After detecting "Enter" input, report buffer data to
                 * processing thread (using service call)
                 */
                p = kx_kmalloc(CONS_BUFFER_SIZE);
                memcpy(p, cons->recv_buf, cons->recv_len);
                kx_send(MSGBOX_ID_CONSINPUT, cons->recv_len, p);
                cons->recv_len = 0;
            }
        }
    }

    if (serial_is_send_enable(cons->index)) { /* send interrupt */
        if (!cons->id || !cons->send_len) {
            /* If no data to send, terminate */
            serial_intr_send_disable(cons->index);
        } else {
            /* being some data to send, send it */
            send_char(cons);
        }
    }

    return 0;
}

/* interrupt handler */
static void consdrv_intr(void) {
    int i;
    struct consreg *cons;

    for (i = 0; i < CONSDRV_DEVICE_NUM; i++) {
        cons = &consreg[i];
        if (cons->id) {
            if ((serial_is_send_enable(cons->index)) ||
                serial_is_recv_enable(cons->index))
                /* If there is a interrupt, call process */
                consdrv_intrproc(cons);
        }
    }
}

static int consdrv_init(void) {
    memset(consreg, 0, sizeof(consreg));
    return 0;
}

/* process a request from a thread */
static int consdrv_command(struct consreg *cons, kz_thread_id_t id, 
                            int index, int size, char *command) {
    switch (command[0]) {
    case CONSDRV_CMD_USE: /* start to use a console driver */
        cons->id = id;
        cons->index = command[1] - '0';
        cons->send_buf = kz_kmalloc(CONS_BUFFER_SIZE);
        cons->recv_buf = kz_kmalloc(CONS_BUFFER_SIZE);
        cons->send_len = 0;
        cons->recv_len = 0;
        serial_init(cons->index);
        serial_intr_recv_enable(cons->index); /* enable a recv interrupt */
        break;
    
    case CONSDRV_CMD_WRITE: /* output to a console driver */
        /*
         * send_string() manipulates a send buffer and is not re-enterable,
         * so need to set uninterruptable
         */
        INTR_DISABLE;
        send_string(cons, command + 1, size - 1); /* send a string */
        INTR_ENABLE;
        break;

    default:
        break;
    }

    return 0;
}

int consdrv_main(int argc, char *argv[]) {
    int size, index;
    kz_thread_id_t id;
    char *p;

    consdrv_init();
    kz_setintr(SOFTVEC_TYPE_SERINTR, consdrv_intr); /* set interrupt handler */

    while (1) {
        id = kz_recv(MSGBOX_ID_CONSOUTPUT, &size, &p);
        index = p[0] - '0';
        consdrv_command(&consreg[index], id, index, size - 1, p + 1);
        kz_kmfree(p);
    }

    return 0;
}
