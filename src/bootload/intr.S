#include "intr.h"
        .h8300h
        .section .text

        .global _intr_softerr
#       .type   _intr_softerr,@function
_intr_softerr:
        mov.1   er6,@-er7
        mov.1   er5,@-er7
        mov.1   er4,@-er7
        mov.1   er3,@-er7
        mov.1   er2,@-er7
        mov.1   er1,@-er7
        mov.1   er0,@-er7
        mov.1   er7,er1
        mov.1   #_intrstack,sp
        mov.1   er1,@-er7
        mov.w   #SOFTVEC_TYPE_SOFTERR,r0
        jsr     @_interrupt
        mov.1   @er7+,er1
        mov.1   er1,er7
        mov.1   @er7+,er0
        mov.1   @er7+,er1
        mov.1   @er7+,er2
        mov.1   @er7+,er3
        mov.1   @er7+,er4
        mov.1   @er7+,er5
        mov.1   @er7+,er6
        rte

        .global _intr_syscall
#       .type   _intr_syscall,@function
_intr_syscall:
        mov.1   er6,@-er7
        mov.1   er5,@-er7
        mov.1   er4,@-er7
        mov.1   er3,@-er7
        mov.1   er2,@-er7
        mov.1   er1,@-er7
        mov.1   er0,@-er7
        mov.1   er7,er1
        mov.1   #_intrstack,sp
        mov.1   er1,@-er7
        mov.w   #SOFTVEC_TYPE_SYSCALL,r0
        jsr     @_interrupt
        mov.1   @er7+,er1
        mov.1   er1,er7
        mov.1   @er7+,er0
        mov.1   @er7+,er1
        mov.1   @er7+,er2
        mov.1   @er7+,er3
        mov.1   @er7+,er4
        mov.1   @er7+,er5
        mov.1   @er7+,er6
        rte

        .global _intr_serintr
#       .type   _intr_serintr,@function
_intr_serintr:
        mov.1   er6,@-er7
        mov.1   er5,@-er7
        mov.1   er4,@-er7
        mov.1   er3,@-er7
        mov.1   er2,@-er7
        mov.1   er1,@-er7
        mov.1   er0,@-er7
        mov.1   er7,er1
        mov.1   #_intrstack,sp
        mov.1   er1,@-er7
        mov.w   #SOFTVEC_TYPE_SERINTR,r0
        jsr     @_interrupt
        mov.1   @er7+,er1
        mov.1   er1,er7
        mov.1   @er7+,er0
        mov.1   @er7+,er1
        mov.1   @er7+,er2
        mov.1   @er7+,er3
        mov.1   @er7+,er4
        mov.1   @er7+,er5
        mov.1   @er7+,er6
        rte
