#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <sys/time.h>

#include "macros.h"
#include "arm_context.h"
#include "arm_disas.h"

/* addresses of asm callout glue code */
extern void *blCallout;
extern void *bCallout;
extern void *bxCallout;
extern void *bxccCallout;
extern void *bccCallout;
extern void *condCallout;
extern void *popCallout;
void *callTarget;

extern int user_prog(int);

void StartProfiling(void *);

void StopProfiling(void);

void armDecode(int32_t *, ARMInstr *);

void *callTarget;

void patchNextCtrlInstr(int32_t *);

/*********************************************************************
 *
 *  callout handlers
 *
 *   These get called by asm glue routines.
 *
 *********************************************************************/

static int32_t origInstr;
static int32_t *origAddr;
static int32_t *funcEnd;

void
handleBlCallout(SaveRegs *regs) {
	/* unpatch instruction */
	*origAddr = origInstr;
	__clear_cache(origAddr, origAddr + 1);
	/* emulate branch */
	regs->lr = (uint32_t)origAddr + 4;
	regs->retPC = origAddr + ((origInstr << 8) >> 8) + 2;
	patchNextCtrlInstr(regs->retPC);
}

void
handleBCallout(SaveRegs *regs) {
	/* unpatch instruction */
	*origAddr = origInstr;
	__clear_cache(origAddr, origAddr + 1);

	/* emulate branch */
	regs->retPC = origAddr + ((origInstr << 8) >> 8) + 2;
	patchNextCtrlInstr(regs->retPC);
}

void
handleBxCallout(SaveRegs *regs) {
	/* unpatch instruction */
	*origAddr = origInstr;
	__clear_cache(origAddr, origAddr + 1);

	switch (origInstr & 0xf) {
	case '\xd':
		regs->lr = regs->sp;
		break;
	case '\xe':
		break;
	case '\xf':
		NOT_IMPLEMENTED();
		break;
	default:
		regs->lr = regs->r[origInstr & 0xf];
		break;
	}

	// Check if last return
	if (regs->retPC >= (int32_t *)&user_prog && regs->retPC <= funcEnd)
		patchNextCtrlInstr(regs->retPC);
	else
		StopProfiling();
}

void
handleBxccCallout(SaveRegs *regs) {
	uint8_t regNum;

	/* unpatch instruction */
	*origAddr = origInstr;
	__clear_cache(origAddr, origAddr + 1);

	/* emulate branch */
	switch ((origInstr & 0xf0000000) >> 28) {
	case ARM_COND_EQ:
		/* eq */
		/* Z = 1 */
		if ((regs->CPSR & 0xf0000000) >> 28 & CPSR_Z) {
			regNum = origInstr & 0xf;
			switch (regNum) {
			case '\xd':
				regs->retPC = (int32_t *)regs->sp;
				break;
			case '\xe':
				regs->retPC = (int32_t *)regs->lr;
				break;
			case '\xf':
				NOT_IMPLEMENTED();
				break;
			default:
				regs->retPC = (int32_t *)regs->r[regNum];
				break;
			}
		} else {
			regs->retPC = origAddr + 1;
			patchNextCtrlInstr(regs->retPC);
			return;
		}
		break;
	default:
		NOT_IMPLEMENTED();
		break;
	}
}

/* bgt, bne */
void
handleBccCallout(SaveRegs *regs) {
	/* unpatch instruction */
	*origAddr = origInstr;
	__clear_cache(origAddr, origAddr + 1);

	/* emulate branch */
	/* N Z C V */
	switch ((origInstr & 0xf0000000) >> 28) {
	case ARM_COND_NE:
		/* bne */
		/* Z = 0 */
		if (((regs->CPSR & 0xf0000000) >> 28 & CPSR_Z) == 0) {
			regs->retPC = origAddr + ((origInstr << 8) >> 8) + 2;
			patchNextCtrlInstr(regs->retPC);
			return;
		}
		break;
	case ARM_COND_GT:
		switch ((regs->CPSR & 0xf0000000) >> 28) {
		/* bgt */
		/* Z = 0 & N = V */
		case '\x0':
		case '\x2':
		case '\x9':
		case '\xb':
			regs->retPC = origAddr + ((origInstr << 8) >> 8) + 2;
			patchNextCtrlInstr(regs->retPC);
			return;
		default:
			break;
		}
	default:
		break;
	}

	regs->retPC = origAddr + 1;
	patchNextCtrlInstr(regs->retPC);
}

void
handleCondCallout(SaveRegs *regs) {
	/* unpatch instruction */
	*origAddr = origInstr;
	__clear_cache(origAddr, origAddr + 1);

	/* emulate instruction */
	/* N Z C V */
	switch ((origInstr & 0xf0000000) >> 28) {
	case ARM_COND_EQ:
		if ((regs->CPSR & 0xf0000000) >> 28 & CPSR_Z) {
			regs->retPC = origAddr;
			patchNextCtrlInstr(regs->retPC + 1);
		} else {
			regs->retPC = origAddr + 1;
			patchNextCtrlInstr(regs->retPC);
		}
	}
}

/*
 * Reserved for an extra credit assignment.
 */
void
handlePopCall(SaveRegs *regs) {
	/* shift right needs to be logical */
	uint16_t reg_list;
	int8_t regNum;

	/* unpatch instruction */
	*origAddr = origInstr;
	__clear_cache(origAddr, origAddr + 1);

	if (IS_ARM_POPM_PC(origInstr)) { /* multiple registers */
		regNum = -1;
		for (reg_list = (uint16_t)origInstr; reg_list; reg_list >>= 1) {
			++regNum;

			if ((reg_list & 1) == 0)
				continue;

			switch (regNum) {
			case '\xd':
				UNPREDICTABLE();
				break;
			case '\xe':
				regs->lr = *(uint32_t *)regs->sp;
				regs->sp += 4;
				break;
			case '\xf':
				regs->retPC = *(int32_t **)regs->sp;
				regs->sp += 4;
				break;
			default:
				regs->r[regNum] = *(uint32_t *)regs->sp;
				regs->sp += 4;
				break;
			}
		}
	} else if (IS_ARM_POPS_PC(origInstr)) {
		regs->retPC = *(int32_t **)regs->sp;
		regs->sp += 4;
	} else
		NOT_IMPLEMENTED();

	// Check if last return
	if (regs->retPC >= (int32_t *)&user_prog && regs->retPC <= funcEnd)
		patchNextCtrlInstr(regs->retPC);
	else
		StopProfiling();
}

/*********************************************************************
 *
 *  armDecode
 *
 *   Decode an ARM instruction.
 *
 *********************************************************************/

#define IS_LE 1 /* endianness */

void
armDecode(int32_t *addr, ARMInstr *i) {
	int16_t *i16;

	i16 = (int16_t *)addr;

	i->cond = (i16[IS_LE] & 0xf000) >> 12;
	i->opcode = (i16[IS_LE] & 0x0ff0) >> 4;
	i->len = 4;
	printf("addr: %p, cond: %x, opcode: %x, len: %d, isCFlow: %s\n", \
		(void *)addr, i->cond, i->opcode, i->len, \
		(IS_ARM_CFLOW(i16[IS_LE]) || IS_ARM_POP_PC(*addr)) ? "true" : "false");
}

void
patchInstr(int32_t *addr)
{
	int32_t *call_target, offset;
	int16_t *i16;

	origInstr = *addr;
	origAddr = addr;

	i16 = (int16_t *)addr;

	/* bl, bx, bxeq, b, bgt, bne */
	if (IS_ARM_B(i16[1])) {
		if (IS_ARM_COND(i16[1]))
			call_target = (int32_t *) &bccCallout;
		else
			call_target = (int32_t *) &bCallout;
	} else if (IS_ARM_BX(i16[1])) {
		if (IS_ARM_COND(i16[1]))
			call_target = (int32_t *) &bxccCallout;
		else
			call_target = (int32_t *) &bxCallout;
	} else if (IS_ARM_BL(i16[1]))
		call_target = (int32_t *) &blCallout;
	else if (IS_ARM_COND(i16[1]))
		call_target = (int32_t *) &condCallout;
	else if (IS_ARM_POP_PC(origInstr)) {
		call_target = (int32_t *) &popCallout;
	} else
		NOT_IMPLEMENTED();

	offset = ((int32_t)call_target - (int32_t)addr - 8) / 4;

	*addr = 0;
	*addr |= offset & 0x00ffffff;
	*addr |= ARM_B << 24;

	__clear_cache(addr, addr + 4);
}

void
patchNextCtrlInstr(int32_t *addr)
{
	ARMInstr instr;
	basicblk b;
	int16_t *i16;

	b.ninstr = 0;
	b.saddr = addr;
	i16 = (int16_t *)addr;

	for (; !IS_ARM_CFLOW(i16[1]) && !IS_ARM_POP_PC(*addr); ++addr, i16 += 2, ++b.ninstr)
		armDecode(addr, &instr);

	armDecode(addr, &instr);
	++b.ninstr;

	printf("Block start address: %p, ", b.saddr);
	printf("Number of instructions in block: %d\n", b.ninstr);

	patchInstr(addr);
}


/*********************************************************************
 *
 *  StartProfiling, StopProfiling
 *
 *   Profiling hooks. This is your place to inspect and modify the profiled
 *   function.
 *
 *********************************************************************/

#define BX_LR 0xe12fff1e

void
StartProfiling(void *func) {
	for (funcEnd = (int32_t *)func; *funcEnd != BX_LR; ++funcEnd)
		;

	patchNextCtrlInstr((int32_t *)func);
}

void
StopProfiling(void) {
    /*
     * TODO: Add your instrumentation code here.
     */

}

int main(int argc, char *argv[]) {
    int value;
    char *end;
    
    struct timeval tval_before, tval_after, tval_result;


    char buf[16];

    if (argc != 1) {
        fprintf(stderr, "usage: %s\n", argv[0]);
        exit(1);
    }

    #ifdef __FIB__
    printf("running fib()\n");
    #endif 

    #ifdef __FACT__
    printf("running fact() \n");
    #endif 

    #ifdef __FACT2__
    printf("running fact2()\n");
    #endif 

    #ifdef __FACT3__
    printf("running fact3()\n");
    #endif 

    printf("input number: ");
    scanf("%15s", buf);

    value = strtol(buf, &end, 10);

    if (((errno == ERANGE)
         && ((value == LONG_MAX) || (value == LONG_MIN)))
        || ((errno != 0) && (value == 0))) {
        perror("strtol");
        exit(1);
    }

    if (end == buf) {
        fprintf(stderr, "error: %s is not an integer\n", buf);
        exit(1);
    }

    if (*end != '\0') {
        fprintf(stderr, "error: junk at end of parameter: %s\n", end);
        exit(1);
    }

    gettimeofday(&tval_before, NULL);

    StartProfiling(user_prog);

    /* 
     * function execution here.
     */
    value = user_prog(value);

    StopProfiling();

    gettimeofday(&tval_after, NULL);
    printf("%d\n", value);

    timersub(&tval_after, &tval_before, &tval_result);
    printf("Time elapsed: %ld.%06ld\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
    exit(0);
}
