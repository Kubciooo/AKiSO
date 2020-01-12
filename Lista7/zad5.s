;arm-none-eabi-gcc -specs=rdimon.specs zad5.s -o zad5
;qemu-arm ./zad5

.text
	.global	main

gcd:
.L2:
	cmp	r0, r1
	bxeq	lr
	subgt	r0, r0, r1
	suble	r1, r1, r0
	b	.L2

main:

	push	{r0, r1, r4, lr}
	ldr	r4, .L9
	mov	r1, sp
	mov	r0, r4
	bl	scanf
	add	r1, sp, #4
	mov	r0, r4
	bl	scanf
	ldm	sp, {r0, r1}
	bl	gcd
	mov	r1, r0
	mov	r0, r4
	bl	printf
	mov	r0, #0
	add	sp, sp, #8
	pop	{r4, lr}
	bx	lr
.L9:
	.word	.LC0
.LC0:
	.ascii	"%d\000"
