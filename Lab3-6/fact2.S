        .global user_prog
        .set    user_prog,fact

fact:
        .fnstart
        cmp     r0, #0
        moveq   r0, #1
        bxeq    lr
        mov     r1, r0
        mov     r0, #1
.LBB0_1:                                @ =>This Inner Loop Header: Depth=1
        mul     r0, r1, r0
        subs    r1, r1, #1
        bne     .LBB0_1
        bx      lr
.Lfunc_end0:
        .size   fact, .Lfunc_end0-fact
        .cantunwind
        .fnend
