#include "defines.h"
#include "kozos.h"
#include "interrupt.h"
#include "lib.h"

/* call system task and user thread */
static int start_threads(int argc, char *argv[]) {
    kz_run(test08_1_main, "command", 0x100, 0, NULL);
    return 0;
}

int main(void) {
    INTR_DISABLE; /* disable interrupt */

    puts("kozos boot succeed!\n");

    /* start OS operation */
    kz_start(start_threads, "start", 0x100, 0, NULL);
    /* No return to here */

    return 0;
}
