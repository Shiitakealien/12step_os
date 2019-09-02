#include "defines.h"
#include "kozos.h"
#include "lib.h"

int test11_1_main(int argc, char *argv[]) {
    char *p;
    int size;

    puts("test11_1 started.\n");

    /* receive a static region as a message */
    puts("test11_1 recv in.\n");
    kz_recv(MSGBOX_ID_MSGBOX1, &size, &p); /* receive */
    puts("test11_1 recv out.\n");
    puts(p);

    /* receive a region allocated dynamically as a message */
    puts("test11_1 recv in.\n");
    kz_recv(MSGBOX_ID_MSGBOX1, &size, &p); /* receive */
    puts("test11_1 recv out.\n");
    puts(p);
    kz_kmfree(p); /* free memory */

    /* send a static region as a message */
    puts("test11_1 send in.\n");
    kz_send(MSGBOX_ID_MSGBOX2, 15, "static memory\n"); /* send */
    puts("test11_1 send out.\n");

    /* send a region allocated dynamically as a message */
    p = kz_kmalloc(18); /* allocate memory */
    strcpy(p, "allocated memory\n");
    puts("test11_1 send in.\n");
    kz_send(MSGBOX_ID_MSGBOX2, 18, p); /* send */
    puts("test11_1 send out.\n");
    /* memory will be freed at receiver */

    puts("test11_1 exit.\n");
    
    return 0;
}
