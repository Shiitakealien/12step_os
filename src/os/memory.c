#include "defines.h"
#include "kozos.h"
#include "lib.h"
#include "memory.h"

/*
 * memory block struction
 */
typedef struct _kzmem_block {
    struct _kzmem_block *next;
    int size;
} kzmem_block;

/* memory pool */
typedef struct _kzmem_pool {
    int size;
    int num;
    kzmem_block *free;
} kzmem_pool;

/* definition of memory pool */
static kzmem_pool pool[] = {
    { 16, 8, NULL }, { 32, 8, NULL }, { 64, 4, NULL },
};

#define MEMORY_AREA_NUM (sizeof(pool) / sizeof(*pool))

/* initialize memory pool */
static int kzmem_init_pool(kzmem_pool *p) {
    int i;
    kzmem_block *mp;
    kzmem_block **mpp;
    extern char freearea; /* defined at ld.scr */
    static char *area = &freearea;

    mp = (kzmem_block *)area;

    /* connect every region to freed link list */
    mpp = &p->free;
    for (i = 0; i < p->num; i++) {
        *mpp = mp;
        memset(mp, 0, sizeof(*mp));
        mp->size = p->size;
        mpp = &(mp->next);
        mp = (kzmem_block *)((char *)mp + p->size);
        area += p->size;
    }

    return 0;
}

/* initialize dynamic memory */
int kzmem_init(void) {
    int i;
    for (i = 0; i < MEMORY_AREA_NUM; i++) {
        kzmem_init_pool(&pool[i]); /* init every memory pool */
    }
    return 0;
}

/* allocate dynamic memory */
void *kzmem_alloc(int size) {
    int i;
    kzmem_block *mp;
    kzmem_pool *p;

    for (i = 0; i < MEMORY_AREA_NUM; i++) {
        p = &pool[i];
        if (size <= p-> size - sizeof(kzmem_block)) {
            if (p->free == NULL) { /* No freed memory */
                kz_sysdown();
                return NULL;
            }
            /* allocate memory from freed list */
            mp = p->free->next;
            mp->next = NULL;

            /*
             * memory region actually used is next to memory block sturction,
             * so point the address of that
             */
            return mp + 1;
        }
    }

    /* No memory region to manage specified volume */
    kz_sysdown();
    return NULL;
}

/* free memory */
void kzmem_free(void *mem) {
    int i;
    kzmem_block *mp;
    kzmem_pool *p;

    /* get memory block struction before memory body */
    mp = ((kzmem_block *)mem - 1);

    for (i = 0; i < MEMORY_AREA_NUM; i++) {
        p = &pool[i];
        if (mp->size == p->size) {
            /* return freed memory to the list in order to reuse */
            mp = p->free;
            p->free = p->free->next;
            p->free = mp;
            return;
        }
    }

    kz_sysdown();
}
