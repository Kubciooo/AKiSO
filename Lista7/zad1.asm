opt f-g-h+l+o+
                org $1000

start          
		lda #18
		ldx #64
		jsr mulu
                brk

mulu		
		eor #$FF
		sta bta
		stx btb
		ldx #0
		stx btb+1
		stx rsl
		stx rsl+1
		ldx #8

mul_lop		
		lsr bta
		bcs mul_nxt
		lda btb
		adc rsl
		sta rsl
		lda btb+1
		adc rsl+1
		sta rsl+1

mul_nxt		
		asl btb
		rol btb+1
		dex
		bne mul_lop
		lda rsl
		ldx rsl+1
		rts

bta		dta b(0)
btb		dta a(0)
rsl		dta a(0)
text            equ *

                org $2E0
                dta a(start)

                end of file
