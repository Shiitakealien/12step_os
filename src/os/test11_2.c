#include "defines.h"
#include "kozos.h"
#include "lib.h"

int test11_2_main(int argc, char *argv[]) {
    char *p;
    int size;

    puts("test11_2 started.\n");

    /* send a static region as a message */
    puts("test11_2 send in.\n");
    kz_send(MSGBOX_ID_MSGBOX1, 15, "static memory\n"); /* send */
    puts("test11_2 send out.\n");

    /* send a region allocated dynamically as a message */
    p = kz_kmalloc(18); /* allocate memory */
    strcpy(p, "allocated memory\n");
    puts("test11_2 send in.\n");
    kz_send(MSGBOX_ID_MSGBOX1, 18, p); /* send */
    puts("test11_2 send out.\n");
    /* memory will be freed at receiver */

    /* receive a static region as a message */
    puts("test11_2 recv in.\n");
    kz_recv(MSGBOX_ID_MSGBOX2, &size, &p); /* receive */
    puts("test11_2 recv out.\n");
    puts(p);

    /* receive a region allocated dynamically as a message */
    puts("test11_2 recv in.\n");
    kz_recv(MSGBOX_ID_MSGBOX2, &size, &p); /* receive */
    puts("test11_2 recv out.\n");
    puts(p);
    kz_kmfree(p); /* free memory */
    
    puts("test11_2 exit.\n");
    
    return 0;
}
