#ifndef _KOOS_MEMORY_H_INCLUDED_
#define _KOOS_MEMORY_H_INCLUDED_

int kzmem_init(void);           /* initialize dynamic memory */
void *kzmem_alloc(int size);    /* allocate dynamic memory */
void kzmem_free(void *mem);     /* free dynamic memory */

#endif
