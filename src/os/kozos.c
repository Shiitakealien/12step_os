#include "defines.h"
#include "kozos.h"
#include "intr.h"
#include "interrupt.h"
#include "syscall.h"
#include "lib.h"

#define THREAD_NUM 6
#define THREAD_NAME_SIZE 15

/* thread context */
typedef struct _kz_context {
    uint32 sp; /* stack pointer */
} kz_context;

/* task controll bloack (TCB) */
typedef struct _kz_thread  {
    struct _kz_thread *next;
    char name[THREAD_NAME_SIZE + 1]; /* thread name */
    char *stack; /* stack */

    struct { /* parameters for thread startup(thread_init()) */
        kz_func_t func; /* thread main function */
        int argc;       /* argc for thread main function */
        char **argv;    /* argv for thread main function */
    } init;

    struct { /* buffer for system call */
        kz_syscall_type_t type;
        kz_syscall_param_t *param;
    } syscall;

    kz_context context; /* context information */
} kz_thread;

/* thread ready queue */
static struct {
    kz_thread *head;
    kz_thread *tail;
} readyque;

static kz_thread *current; /* current thread */
static kz_thread threads[THREAD_NUM]; /* task controll block */
static kz_handler_t handlers[SOFTVEC_TYPE_NUM]; /* interrupt handler */

void dispatch(kz_context *context);

/* get current thread from ready queue */
static int getcurrent(void) {
    if (current == NULL) {
        return -1;
    }

    /* current thread must be head */
    readyque.head = current->next;
    if (readyque.head == NULL) {
        readyque.tail = NULL;
    }
    current->next = NULL;

    return 0;
}

/* connect current thread to ready queue */
static int putcurrent(void) {
    if (current == NULL) {
        return -1;
    }

    /* connect to the tail of ready queue */
    if (readyque.tail) {
        readyque.tail->next = current;
    } else {
        readyque.head = current;
    }
    readyque.tail = current;

    return 0;
}

static void thread_end(void) {
    kz_exit();
}

/* start up thread */
static void thread_init(kz_thread *thp) {
    /* call thread main function */
    thp->init.func(thp->init.argc, thp->init.argv);
    thread_end();
}

/* process system call */
static kz_thread_id_t thread_run(kz_func_t func, char *name, int stacksize, int argc, char *argv[]) {
    int i;
    kz_thread *thp;
    uint32 *sp;
    extern char userstack; /* defined in ld.scr */
    static char *thread_stack = &userstack;

    /* search vacunt task controll block */
    for (i = 0; i < THREAD_NUM; i++) {
        thp = &threads[i];
        if (!thp->init.func) /* found */
            break;
    }
    if (i == THREAD_NUM) /* not found */
        return -1;

    memset(thp, 0, sizeof(*thp));

    /* set up task controll block (TCB) */
    strcpy(thp->name, name);
    thp->next = NULL;

    thp->init.func = func;
    thp->init.argc = argc;
    thp->init.argv = argv;

    /* get stack */
    memset(thread_stack, 0, stacksize);
    thread_stack += stacksize;

    thp->stack = thread_stack; /* set stack */
    /* initialize stack */
    sp = (uint32 *)thp->stack;
    *(--sp) = (uint32)thread_end;

    /*
     * set up program counter
     */
    *(--sp) = (uint32)thread_init;

    *(--sp) = 0; /* ER6 */
    *(--sp) = 0; /* ER5 */
    *(--sp) = 0; /* ER4 */
    *(--sp) = 0; /* ER3 */
    *(--sp) = 0; /* ER2 */
    *(--sp) = 0; /* ER1 */

    /* args for thread start up (thread_init()) */
    *(--sp) = (uint32)thp; /* ER0 */

    /* set up thread context */
    thp->context.sp = (uint32)sp;

    /* put system calling thread to ready queue */
    putcurrent();

    /* connect new thread to ready queue */
    current = thp;
    putcurrent();

    return (kz_thread_id_t)current;
}

/* process system call */
static int thread_exit(void) {
    /*
     * TODO : enable to release stack and reuse it
     */
    puts(current->name);
    puts(" EXIT.\n");
    memset(current, 0, sizeof(*current));
    return 0;
}

/* add a handler into interrupt vector */
static int setintr(softvec_type_t type, kz_handler_t handler) {
    static void thread_intr(softvec_type_t type, unsigned long sp);

    /*
     * To receive interrupt, add a function as an entrance of interrupt
     * into software interrupt vector
     */
    softvec_setintr(type, thread_intr);

    handlers[type] = handler; /* add interrupt handler called from OS */

    return 0;
}

static void call_functions(kz_syscall_type_t type, kz_syscall_param_t *p) {
    /* Note : current is overwritten in System Call */
    switch (type) {
        case KZ_SYSCALL_TYPE_RUN: /* kz_run() */
        p->un.run.ret = thread_run(p->un.run.func, p->un.run.name,
                                    p->un.run.stacksize,
                                    p->un.run.argc, p->un.run.argv);
        break;
        case KZ_SYSCALL_TYPE_EXIT: /* kz_exit() */
        /* TCB is deleted, so MUST NOT write */
        thread_exit();
        break;
        default:
        break;
    }
}

/* process system call */
static void syscall_proc(kz_syscall_type_t type, kz_syscall_param_t *p) {
    /* 
     * call the thread calling sytem call with removing it from ready queue
     * So, in order to keep the calling thread operaing, must call putcurrent()
     * in proccessing function
     */
    getcurrent();
    call_functions(type, p);
}

/* schedule threads */
static void schedule(void) {
    if (!readyque.head) /* not found */
        kz_sysdown();

    current = readyque.head; /* set as cuurent */
}

static void syscall_intr(void) {
    syscall_proc(current->syscall.type, current->syscall.param);
}

static void softerr_intr(void) {
    puts(current->name);
    puts(" DOWN.\n");
    getcurrent(); /* remove from ready queue */
    thread_exit(); /* exit thread */
}

/* entrance function of interrupt */
static void thread_intr(softvec_type_t type, unsigned long sp) {
    /* preserve the current context */
    current->context.sp = sp;

    /*
     * procces of every interrupt
     * in the case of SOFTVEC_TYPE_SYSCALL or SOFTVEC_TYPE_SOFTERR,
     * call syscall_intr() or softerr_intr() which are registered in handler
     */
    if (handlers[type])
        handlers[type]();

    schedule(); /* schedule threads */

    /*
     * dispatch the thread (written in startup.s)
     */
    dispatch(&current->context);
    /* No return to here */
}

void kz_start(kz_func_t func, char *name, int stacksize, int argc, char *argv[]) {
    /*
     * set current as NULL, since current may be checked
     * in library functions below
     */
    current = NULL;

    readyque.head = readyque.tail = NULL;
    memset(threads, 0, sizeof(threads));
    memset(handlers, 0, sizeof(handlers));

    /* register an interrupt handler */
    setintr(SOFTVEC_TYPE_SYSCALL, syscall_intr); /* system call */
    setintr(SOFTVEC_TYPE_SOFTERR, softerr_intr); /* detect down factor */

    /* create thread by call function dircetly */
    current = (kz_thread *)thread_run(func, name, stacksize, argc, argv);

    /* call the 1st thread */
    dispatch(&current->context);

    /* No return to here */
}

void kz_sysdown(void) {
    puts("system error!\n");
    while (1)
        ;
}

/* library function to call system call */
void kz_syscall(kz_syscall_type_t type, kz_syscall_param_t *param) {
    current->syscall.type = type;
    current->syscall.param = param;
    asm volatile ("trapa #0"); /* issue a trap interrupt */
} 
