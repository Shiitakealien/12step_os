#ifndef _INTR_H_INCLUDED_
#define _INTR_H_INCLUDED_

/* definition of software interrupt vector */

#define SOFTVEC_TYPE_NUM        4

#define SOFTVEC_TYPE_SOFTERR    0
#define SOFTVEC_TYPE_SYSCALL    1
#define SOFTVEC_TYPE_SERINTR    2
#define SOFTVEC_TYPE_TIMINTR    3

#endif
