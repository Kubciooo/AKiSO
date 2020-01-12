.text
.global main

main:
        push    {fp, lr}
        add     fp, sp, #4
        sub     sp, sp, #16
        sub     r3, fp, #12
        mov     r1, r3
        ldr     r0, .L8
        bl      scanf
        sub     r3, fp, #16
        mov     r1, r3
        ldr     r0, .L8
        bl      scanf
        ldr     r3, [fp, #-12]
        ldr     r2, [fp, #-16]
        mov     r1, r2
        mov     r0, r3
        bl      gcd
        str     r0, [fp, #-8]
        ldr     r1, [fp, #-8]
        ldr     r0, .L8
        bl      printf
        mov     r3, #0
        mov     r0, r3
        sub     sp, fp, #4
        pop     {fp, lr}
        bx      lr
.L8:
        .word   .LC0
        gcd:
                str     fp, [sp, #-4]!
                add     fp, sp, #0
                sub     sp, sp, #12
                str     r0, [fp, #-8]
                str     r1, [fp, #-12]
                b       .L2
        .L4:
                ldr     r2, [fp, #-8]
                ldr     r3, [fp, #-12]
                cmp     r2, r3
                ble     .L3
                ldr     r2, [fp, #-8]
                ldr     r3, [fp, #-12]
                sub     r3, r2, r3
                str     r3, [fp, #-8]
                b       .L2
        .L3:
                ldr     r2, [fp, #-12]
                ldr     r3, [fp, #-8]
                sub     r3, r2, r3
                str     r3, [fp, #-12]
        .L2:
                ldr     r2, [fp, #-8]
                ldr     r3, [fp, #-12]
                cmp     r2, r3
                bne     .L4
                ldr     r3, [fp, #-12]
                mov     r0, r3
                add     sp, fp, #0
                ldr     fp, [sp], #4
                bx      lr
        .LC0:
                .ascii  "%d\000"
