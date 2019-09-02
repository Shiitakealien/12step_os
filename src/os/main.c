#include "defines.h"
#include "kozos.h"
#include "interrupt.h"
#include "lib.h"

/* call system task and user task */
static int start_threads(int argc, char *argv[]) {
    kz_run(consdrv_main,  "consdrv",  1, 0x200, 0, NULL);
    kz_run(command_main,  "command",  8, 0x200, 0, NULL);

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
