#include "defines.h"

extern void start(void);
extern void intr_softerr(void);
extern void intr_syscall(void);
extern void intr_serintr(void);
extern void intr_timintr(void);

/*
 * set an interrupt vector
 * by the definition of the linker script, set at the top the address space
 */

void (*vectors[])(void) = {
   start, NULL,NULL,NULL,NULL,NULL,NULL,NULL, 
   intr_syscall, intr_softerr, intr_softerr, intr_softerr,
   NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL, 
   NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL, 
   NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL, 
   intr_timintr, intr_timintr, intr_timintr, intr_timintr, 
   intr_timintr, intr_timintr, intr_timintr, intr_timintr, 
   NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL, 
   intr_serintr, intr_serintr, intr_serintr, intr_serintr, 
   intr_serintr, intr_serintr, intr_serintr, intr_serintr, 
   intr_serintr, intr_serintr, intr_serintr, intr_serintr, 
};
