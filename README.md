Building requires GCC version 8.3.0

Lab 3-1
- Modify the first instruction of `user_prog()` to be `bx lr`, which causes it to return immediately

Lab 3-2
- Modify the first instruction of `user_prog()` to be `b bxCallout` which calls handleBxCallout
- return from handleBxCallout

Lab 3-3
- Address is value of pointer
- Instruction is value of dereferenced pointer
- Opcode is bits 5-12 of instruction 
- Length is always 4
- Instruction is control flow if it is a branch or conditional instruction

Lab 3-4
- Patch first control instruction by calling `patchNextCtrlInstr()`

- `patchNextCtrlInstr()` loops until an instruction is a control instruction and calls armDecode for each instruction, tracking the number of instructions in a block and its start address. It prints out the block information. `patchInstr()` is called on the control instruction.

- `armDecode()` is the same code from Lab3-3 that prints out the address, opcode, length, and whether it is a control flowinstruction

- `patchInstr()` extends Lab3-2 to check the type of control flow instruction and modifies the instruction to call the correct handler. It saves the original instruction and address.

- For all callouts, the first callout to be called sets the address of the last return address.

- `handleBlCallout` calculates the address to jump to by adding the last 24 bits of the instruction with the original addres + 2instruction and sets the retPC to that value. The link register is set to instruction after the original instruction. It then calls `patchNextCtrlInstr()` on the calculated address. After `patchNextCtrlInstr()` returns it will jump to the instruction in retPC. The original instruction is unpatched.

- `handleBCallout` does the same thing as handleBlCallout except it does not set the lr register.

- `handleBxCallout` calculates the address to jump to by using the value of the register specified by the last 4 bits of the instruction and sets retPC to that value. It then calls `stopProfiling()` on the calculated address if it is the last return address since the function is about to return to main. Otherwise, it patches the next control instruction. After `patchNextCtrlInstr()` returns it will jump to the address specified by the retPC. The original instruction is unpatched.

- `handleBccCallout` checks to see if the flags set in the CPSR match the condition necessary for branching. If there is no branch the retPC is set to the next instruction. Otherwise, the retPC is set to the address calculated by usingthe offset specified by the last 23 bits of the instruction. `patchNextCtrlInstr()` is called on retPC. When it returns it will jump to the address specified by the retPC. The original instruction is unpatched.

Lab 3-5
- patchInstr is extended to check for conditional instructions and call condCallout
- condCallout is a copy of bCallout with handleCondCallout called instead of handleBCallout
- `handleCondCallout` checks to see if the flags set in the CPSR match the condition necessary for executing the instruction. If there is no execution the retPC is set to the next instruction. Otherwise, the retPC is set to the address of the original instrucion. `patchNextCtrlInstr()` is called on the next instruction after the original instruction. When it returns it will jump to the original instruction or the one after the original instruction. The original instruction is unpatched.
- The return address is now checked to see if it is within the function. If it is, the next control flow instruction is patched, otherwise StopProfiling is called.

Lab 3-6
- patchInstr, armDecode, and patchNextCtrlInstr are extended to check for pop instructions that modify pc and call popCallout
- popCallout is a copy of bCallout with handlePopCallout called instead of handleBCallout and `regs->sp` saved in sp
- `handlePopCallout` checks to see if the instruction if pops multiple registers. It emulates the instruction by copying the value pointed to by the stack register to the correct register. `patchNextCtrlInstr()` is not called since the function is returning. StopProfiling is called which does nothing. The original instruction is unpatched.
