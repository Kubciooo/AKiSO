;nasm -o zad4 zad4.asm
;qemu-system-i386  -fda zad4 -net none
[bits 16]
[org 7c00h]
mov ax,0fh
int 10h

mov dx,-128 ;start on the first line
yloop:
mov di,-256 ;go to the beginning of the line
xloop:
;set z=c initially
mov si,di
mov bp,dx
xor ax,ax ; set colour to black (al=0) and iteration counter to zero (ah=0)

iterate:
;calculate Im(z^2)=2xy
mov cx,bp
imul cx,si
jo overflow
sar cx,6 ;maybe mov 6 into a register first?
;cx contains Im(z^2)

;calculate Re(z^2)=x^2-y^2
mov bx,si ;we will work in the DX register for now
add bx,bp ;bx contains x+y
sub si,bp ;si contains x-y
imul si,bx
jo overflow
sar si,7 ;si contains Re(z^2)

;calculate z'=z^2+c
add si,di
add cx,dx
mov bp,cx

;do another iteration
inc ah
jno iterate

;iterations are over
inc al ; if we've gotten all this way, set the colour to white
overflow:

;now write a pixel
mov ah,0ch ; write pixel interrupt
xor bh,bh
mov cx,di
add cx,320
add dx,175
int 10h
sub dx,175

;loop around, do the next pixel
inc di
cmp di,255
jne xloop
;or if we've gotten to here, draw the next row
inc dx
cmp dx,127
jne yloop

cli
hlt
db $-$$ ;tell us the length of the code

times 510 - ($ - $$) db 0
dw 0xAA55
