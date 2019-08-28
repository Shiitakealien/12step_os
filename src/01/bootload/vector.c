#include "defines.h"

extern void start(void);

/*
 * set an interupt vector
 * by the definition of the linker script, set at the top the address space
 */

void (*vectors[])(void) = {
   start, NULL,NULL,NULL,NULL,NULL,NULL,NULL, 
   NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL, 
   NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL, 
   NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL, 
   NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL, 
   NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL, 
   NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL, 
   NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL, 
};
