//
// Created by Kangkook Jee on 10/15/20.
//
#ifndef ARM_DISASM_H
#define ARM_DISASM_H

#include <stdint.h>

#define ARM_BL      0xeb
#define ARM_B       0xea
#define ARM_BGT     0xca
#define ARM_BX      0xe1

// ARM conditional prefix
#define ARM_COND_GT 0xc
#define ARM_COND_NE 0x1
#define ARM_COND_EQ 0x0
#define ARM_COND_LE 0xd
#define ARM_COND_AL 0xe
// CPSR
#define CPSR_Z 0x4

#define IS_ARM_B(_XX)  ((_XX & 0x0f00) == 0x0a00)
#define IS_ARM_BL(_XX) ((_XX & 0x0f00) == 0x0b00)
#define IS_ARM_BX(_XX) ((_XX & 0x0ff0) == 0x0120)
#define IS_ARM_COND(_XX) ((_XX & 0xf000) != 0xe000 && (_XX & 0xf000) != 0xf000)
#define IS_ARM_POP_PC(_XX) (IS_ARM_POPM_PC(_XX) || IS_ARM_POPS_PC(_XX))
#define IS_ARM_POPM_PC(_XX) ((_XX & 0x0ff00000) == 0x08b00000 && (_XX & 0x00008000))
#define IS_ARM_POPS_PC(_XX) ((_XX & 0x0ff00000) == 0x04900000 && (_XX & 0x0000f000))

typedef struct {
	uint8_t cond;
	uint16_t opcode; // a single byte is enough to identify opcode.
	uint8_t len;
} ARMInstr;

typedef struct {
	void *saddr;
	uint16_t ninstr;
} basicblk;

#define IS_ARM_CFLOW(_XX) (IS_ARM_B(_XX) || IS_ARM_BL(_XX) || IS_ARM_BX(_XX) || IS_ARM_COND(_XX))
#endif // ARM_DISASM_H
