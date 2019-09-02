#include "defines.h"
#include "kozos.h"
#include "interrupt.h"
#include "lib.h"

/* call system task and user thread */
static int start_threads(int argc, char *argv[]) {
    kz_run(test11_1_main, "test11_1", 1, 0x100, 0, NULL);
    kz_run(test11_2_main, "test11_2", 2, 0x100, 0, NULL);

    kz_chpri(15);   /* move idle */
    INTR_ENABLE;    /* enable interrupt */
    while (1) {
        asm volatile ("sleep");
    }

    return 0;
}

int main(void) {
    INTR_DISABLE; /* disable interrupt */

    puts("kozos boot succeed!\n");

    /* start OS operation */
    kz_start(start_threads, "idle", 0, 0x100, 0, NULL);
    /* No return to here */

    return 0;
}
