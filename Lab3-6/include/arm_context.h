#ifndef ARM_CONTEXT_H
#define ARM_CONTEXT_H

#include <stdint.h>

typedef struct {
    uint32_t   CPSR;
    uint32_t   r[13];
    uint32_t   sp;      // r[13]
    uint32_t   lr;      // r[14]  
    int32_t   *retPC;   // r[15]
} SaveRegs;
#endif
