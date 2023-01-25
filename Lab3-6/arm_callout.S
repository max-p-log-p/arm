    .global bCallout
bCallout:
    push    {sp,lr,pc}
    push    {r0-r12}
    mrs     r0, cpsr    /* r0 is scratch, temporarily storing cspr */
    push    {r0}
    mov     r0, sp      /* setting the first arg (*SaveRegs) */

    bl handleBCallout

    pop     {r0}
    msr     cpsr, r0
    pop     {r0-r12}
    add     sp, sp, #4
    pop     {lr,pc}

    .global bccCallout
bccCallout:
    push    {sp,lr,pc}
    push    {r0-r12}
    mrs     r0, cpsr    /* r0 is scratch, temporarily storing cspr -- control register */
    push    {r0}
    mov     r0, sp      /* setting the first arg (*SaveRegs) */

    bl handleBccCallout

    pop     {r0}
    msr     cpsr, r0
    pop     {r0-r12}
    add     sp, sp, #4
    pop     {lr,pc}

    .global bxccCallout
bxccCallout:
    push    {sp,lr,pc}
    push    {r0-r12}
    mrs     r0, cpsr    /* r0 is scratch, temporarily storing cspr -- control register */
    push    {r0}
    mov     r0, sp      /* setting the first arg (*SaveRegs) */

    bl handleBxccCallout

    pop     {r0}
    msr     cpsr, r0
    pop     {r0-r12}
    add     sp, sp, #4
    pop     {lr,pc}

.global blCallout
blCallout:
    push    {sp,lr,pc}
    push    {r0-r12}
    mrs     r0, cpsr    /* r0 is scratch, temporarily storing cspr */
    push    {r0}
    mov     r0, sp      /* setting the first arg (*SaveRegs) */

    bl handleBlCallout

    pop     {r0}
    msr     cpsr, r0
    pop     {r0-r12}
    add     sp, sp, #4
    pop     {lr,pc}

    .global bxCallout
bxCallout:
    push    {sp,lr,pc}  /* sp, pc not needed, just for data structure compliance */
    push    {r0-r12}
    mrs     r0, cpsr    /* r0 is scratch, temporarily storing cspr */
    push    {r0}
    mov     r0, sp      /* setting the first arg (*SaveRegs) */

    bl handleBxCallout

    pop     {r0}
    msr     cpsr, r0
    pop     {r0-r12}
    add     sp, sp, #4  /* removing $sp from stack */
    pop     {lr}
    add     sp, sp, #4  /* removing $pc from stack */
    bx      lr

@
@ For part3-5.
@
    .global condCallout
condCallout:
    push    {sp,lr,pc}  /* sp, pc not needed, just for data structure compliance */
    push    {r0-r12}
    mrs     r0, cpsr    /* r0 is scratch, temporarily storing cspr */
    push    {r0}
    mov     r0, sp      /* setting the first arg (*SaveRegs) */

    bl handleCondCallout

    pop     {r0}
    msr     cpsr, r0
    pop     {r0-r12}
    add     sp, sp, #4
    pop     {lr,pc}

@
@ For an extra credit assignment
@

    .global popCallout
popCallout:
    push    {sp,lr,pc}
    push    {r0-r12}
    mrs     r0, cpsr    /* r0 is scratch, temporarily storing cspr */
    push    {r0}
    mov     r0, sp      /* setting the first arg (*SaveRegs) */

    bl handlePopCall

    pop     {r0}
    msr     cpsr, r0
    pop     {r0-r12}

    pop     {r3}
    pop     {lr}
    pop     {r2}
    mov     sp, r3      /* set new sp */
    bx      r2
