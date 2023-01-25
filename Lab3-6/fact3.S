	.global	user_prog
	.set	user_prog,fact
fact:
	.fnstart
@ %bb.0:
	.save	{r11, lr}
	push	{r11, lr}
	.setfp	r11, sp
	mov	r11, sp
	.pad	#16
	sub	sp, sp, #16
	str	r0, [sp, #8]
	ldr	r0, [sp, #8]
	cmp	r0, #0
	bne	.LBB0_2
@ %bb.1:
	ldr	r0, .LCPI0_0
	str	r0, [r11, #-4]
	b	.LBB0_3
.LBB0_2:
	ldr	r0, [sp, #8]
	ldr	r1, [sp, #8]
	sub	r1, r1, #1
	str	r0, [sp, #4]            @ 4-byte Spill
	push 	{r1}
	pop 	{r0}
	bl	fact
	ldr	r1, [sp, #4]            @ 4-byte Reload
	mul	r0, r1, r0
	str	r0, [r11, #-4]
.LBB0_3:
	ldr	r0, [r11, #-4]
	mov	sp, r11
	pop	{r11, pc}
	.p2align	2
@ %bb.4:
.LCPI0_0:
	.long	1                       @ 0x1
.Lfunc_end0:
	.size	fact, .Lfunc_end0-fact
	.cantunwind
	.fnend
                                        @ -- End function
