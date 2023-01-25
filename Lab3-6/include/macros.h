//
// Created by Kangkook Jee on 10/15/20.
//

#ifndef ARM_MACROS_H
#define ARM_MACROS_H

#include <assert.h>
#include <stdint.h>

#define ASSERT              assert
#define NOT_REACHED()       assert(0)
#define NOT_IMPLEMENTED()   assert(0)
#define UNPREDICTABLE()   assert(0)
#define DPRINT              printf

#define BIT(_v, _b)         (((_v) >> (_b)) & 0x1)
#define BITS(_v, _l, _h)    (((uint32_t)(_v) << (31-(_h))) >> ((_l)+31-(_h)))

#endif // ARM_MACROS_H
