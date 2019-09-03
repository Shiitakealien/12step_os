#include "defines.h"
#include "kozos.h"
#include "consdrv.h"
#include "lib.h"
#include "timerdrv.h"

/* start to use a console driver */
static void send_use(int index) {
    char *p;
    p = kz_kmalloc(3);
    p[0] = '0';
    p[1] = CONSDRV_CMD_USE;
    p[2] = '0' + index;
    kz_send(MSGBOX_ID_CONSOUTPUT, 3, p);
}

static void send_start(int msec) {
    struct timerreq *req;
    req = kz_kmalloc(sizeof(*req));
    req->id = MSGBOX_ID_CONSINPUT;
    req->msec = msec;
    kz_send(MSGBOX_ID_TIMDRIVE, TIMERDRV_CMD_START, (char *)req);
}

/* output to a console */
static void send_write(char *str) {
    char *p;
    int len;
    len = strlen(str);
    p = kz_kmalloc(len + 2);
    p[0] = '0';
    p[1] = CONSDRV_CMD_WRITE;
    memcpy(&p[2], str, len);
    kz_send(MSGBOX_ID_CONSOUTPUT, len + 2, p);
}

int command_main(int argc, char *argv[]) {
    char *p;
    int size;

    send_use(SERIAL_DEFAULT_DEVICE);

    while (1) {
        send_write("command> "); /* print prompt */

        /* receive a message from console */
        kz_recv(MSGBOX_ID_CONSINPUT, &size, &p);
        if (p == NULL) {
            send_write("expired.\n");
            continue;
        }
        p[size] = '\0';

        if (!strncmp(p, "echo", 4)) { /* echo command */
            send_write(p + 4); /* print a string after "echo" */
            send_write("\n");
        } else if (!strncmp(p, "timer", 5)) { /* start a timer command */
            send_write("start timer.\n");
            send_start(1000);
        } else {
            send_write("unknown.\n");
        }

        kz_kmfree(p);
    }
    
    return 0;
}
