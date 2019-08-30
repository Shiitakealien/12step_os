#include "defines.h"
#include "intr.h"
#include "interrupt.h"

/* initialize software interrupt vector */
int softvec_init(void) {
    int type;
    for (type = 0; type < SOFTVEC_TYPE_NUM; type++)
        softvec_setintr(type, NULL);
    return 0;
}

/* set up software interrupt vector */
int softvec_setintr(softvec_type_t type, softvec_handler_t handler) {
    SOFTVECS[type] = handler;
    return 0;
}

/*
 * common interrupt handler
 * switch each handler, by software interrupt vector
 */
void interrupt(softvec_type_t type, unsigned long sp) {
    softvec_handler_t handler = SOFTVECS[type];
    if (handler)
        handler(type, sp);
}
