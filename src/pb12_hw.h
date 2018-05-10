#ifndef PB12_HW_H
#define PB12_HW_H

#include "pb12_mem.h"
#include "pb12_cpu.h"

typedef struct S_PB12_HW {
    PB12_MEM mem;
    PB12_CPU cpu;

    /* Because interrupt vectors cannot really be implemented... */
    int trap_num;
    int trap_op;
} PB12_HW;

#endif /* PB12_HW_H */
