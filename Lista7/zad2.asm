BITS 32
global _start
;nasm -f elf -o  zad2.o zad2.asm
;ld zad2.o -o zad2 -m elf_i386


section .data
newLineMsg db 0xA, 0xD
newLineLen equ $-newLineMsg
worker db "xd"

section .bss
inp_buf resb 10
var resb 32
n resb 10
act resb 10
size resb 10
section .text
_start:

call read_data
xor eax,eax ; zeruj eax
xor ebx, ebx
xor ecx, ecx
mov edx, inp_buf ; do edx przypisz wartość z inp_buf(czyli stringa ...)
call atoi
mov [n], eax ; w miejsce w pamięci, na które nakierowuje n daj wartość eax
xor ebx, ebx
mov ebx, 10
mov eax, [n]
call lets_loop_n
;call exec

;exit
mov eax, 1
xor ebx, ebx
int 0x80

lets_loop_n:
mov eax, 1
mov [act], eax
mov ecx, [n]

  whileLoop:
  push ecx
  call singleton
  mov eax, [act]
  inc eax
  mov [act], eax
  call printEnd
  pop ecx
  loop whileLoop
ret

singleton:
  mov ecx, [act]
  mov eax, 1
  mov [var], eax
  mov ebx, 10
loop1:
  push ecx
  xor eax, eax
  mov eax, [var]
  xor ecx, ecx
  call print
  ;call printEnd
  mov eax,[var]
  mov ebx, 10
  mov eax, [var]
  inc eax
  mov [var], eax
  pop ecx
  loop loop1
  ret








print:
mov ecx, esp
sub esp, 36   ; reserve space for the number string, for base-2 it takes 33 bytes with new line, aligned by 4 bytes it takes 36 bytes.

mov edi, 1
dec ecx
mov [ecx], byte 10
dec ecx
mov byte [ecx],  ' '
print_loop:

xor edx, edx
div ebx
add dl, 48
jmp after_use_letter

after_use_letter:
dec ecx
inc edi
mov [ecx],dl
test eax, eax
jnz print_loop

; system call to print, ecx is a pointer on the string
;mov byte [ecx], " "
;move[ecx], 13
mov eax, 4    ; system call number (sys_write)
mov ebx, 1    ; file descriptor (stdout)
mov edx, edi  ; length of the string
int 0x80
add esp, 36   ; release space for the number string
;call printSpace
ret

read_data:
mov eax, 3
mov ebx,2
mov ecx, inp_buf
mov edx, 10
int 80h
ret

; Input:
; ESI = pointer to the string to convert
; ECX = number of digits in the string (must be > 0)
; Output:
; EAX = integer value

  printEnd:
  mov edx, newLineLen
  mov ecx, newLineMsg
  mov ebx, 1
  mov eax, 4
  int 80h
      ret
atoi:
xor eax, eax ; zero a "result so far"
.top:
movzx ecx, byte [edx] ; get a character
inc edx ; ready for next one
cmp ecx, '0' ; valid?
jb .done
cmp ecx, '9'
ja .done
sub ecx, '0' ; "convert" character to number
imul eax, 10 ; multiply "result so far" by ten
add eax, ecx ; add in current digit
jmp .top ; until done
.done:
ret
