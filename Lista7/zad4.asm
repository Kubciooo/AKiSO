;nasm -f bin zad4.asm -o zad4.exe
; otwierać za pomocą DOSBOX

section	.data
Mode:		db 0
x			dd 1100h
y			dd 1104h
i			dd 1108h
imax		dd 1112h
tmp			dd 1116h
x0			dd 1120h
y0			dd 1124h
px			dd 1128h
py			dd 1132h
xmin		dd 1136h
ymin		dd 1140h
xmax		dd 1144h
ymax		dd 1148h
e			dd 1152h
w			dd 1156h
h			dd 1160h

org 100h
section	.text

start:		mov al,13h
  mov	dl,alasm -f bin zad4.asm -o zad4.exe
; otwierać za pomocą DOSBOX

org 100h
section	.text

start:		mov al,13h
  mov	dl,al
  mov	ah,0Fh
  int	10h
  mov	byte [Mode],al
  xor	ah,ah
  mov	al,dl
  int	10h
  mov	ah,0Fh
  int	10h

  call draw_mandelbrot

  xor	ah,ah
  int 16h

  call exit

draw_mandelbrot:
  mov dword[xmin], 2
  sal dword[xmin], 16
  neg dword[xmin]

  mov dword[xmax], 1
  sal dword[xmax], 16

  mov dword[ymin], 1
  sal dword[ymin], 16
  neg dword[ymin]

  mov dword[ymax], 1
  sal dword[ymax], 16

  mov dword[px], 0
  mov dword[py], 0
  mov dword[i], 0
  mov dword[e], 0
  mov dword[imax], 10

  mov dword[w], 320
  mov dword[h], 200

forPX:
  mov dword[py], 0
forPY:
  mov eax, dword[xmax]
  sub eax, dword[xmin]
  mov ebx, dword[w]
  xor edx, edx
  div ebx
  xor edx, edx
  mul dword[px]
  add eax, dword[xmin]
  mov dword[x0], eax

  mov eax, dword[ymax]
  sub eax, dword[ymin]
  mov ebx, dword[h]
  xor edx, edx
  div ebx
  xor edx, edx
  mul dword[py]
  add eax, dword[ymin]
  mov dword[y0], eax

  mov dword[x], 0
  mov dword[y], 0
  mov dword[i], 0
  mov dword[e], 0

whale:
  sar dword[x], 8
  sar dword[y], 8
  mov eax, dword[x]
  xor edx, edx
  mul eax
  mov ebx, eax
  mov eax, dword[y]
  xor edx, edx
  mul eax
  sub ebx, eax
  add ebx, dword[x0]
  mov dword[tmp], ebx

  mov eax, 2
  xor edx, edx
  mul dword[x]
  xor edx, edx
  mul dword[y]
  add eax, dword[y0]
  mov dword[y], eax

  mov eax, dword[tmp]
  mov dword[x], eax
  inc dword[i]

  ;loop comparisons:
  sar dword[x], 8
  sar dword[y], 8
  mov eax, dword[x]	;eax = x
  xor edx, edx
  mul dword[x]		;eax = x^2
  mov ebx, eax		;ebx = x^2
  mov eax, dword[y]	;eax = y
  xor edx, edx
  mul dword[y]		;eax = y^2
  add eax, ebx		;eax = x^2 + y^2
  sal dword[x], 8
  sal dword[y], 8

  mov ebx, 4
  sal ebx, 16
  cmp eax, ebx
  jl ok1
  mov dword[e], 1
ok1:
  mov eax, dword[i]
  sub eax, 1d
  cmp eax, dword[imax]
  jle ok2
  mov dword[e], 1
ok2:
  cmp dword[e], 1
  jne whale

  mov eax, dword[imax]
  cmp dword[i], eax
  jg nodraw

  xor ecx, ecx
  xor edx, edx
  mov cx, word[px]
  mov dx, word[py]
  mov al, byte[i]
  call draw_pixel
nodraw:
  inc dword[py]
  mov eax, dword[h]
  cmp dword[py], eax
  jl forPY

  inc dword[px]
  mov eax, dword[w]
  cmp dword[px], eax
  jl forPX

  ret


draw_pixel:				;will draw pixel [cx, dx]
  mov ah, 0Ch ;ah -> mode, al -> color, bh -> page, cx -> x-axis, dx -> y-axis
  mov bh, 0
  ;mov al, 1
  int 10h
  ret

  mov	ah,0Fh
  int	10h
  mov	byte [Mode],al
  xor	ah,ah
  mov	al,dl
  int	10h
  mov	ah,0Fh
  int	10h

  call draw_mandelbrot

  xor	ah,ah
  int 16h

  call exit

draw_mandelbrot:
  mov dword[xmin], 2
  sal dword[xmin], 16
  neg dword[xmin]

  mov dword[xmax], 1
  sal dword[xmax], 16

  mov dword[ymin], 1
  sal dword[ymin], 16
  neg dword[ymin]

  mov dword[ymax], 1
  sal dword[ymax], 16

  mov dword[px], 0
  mov dword[py], 0
  mov dword[i], 0
  mov dword[e], 0
  mov dword[imax], 10

  mov dword[w], 320
  mov dword[h], 200

forPX:
  mov dword[py], 0
forPY:
  mov eax, dword[xmax]
  sub eax, dword[xmin]
  mov ebx, dword[w]
  xor edx, edx
  div ebx
  xor edx, edx
  mul dword[px]
  add eax, dword[xmin]
  mov dword[x0], eax

  mov eax, dword[ymax]
  sub eax, dword[ymin]
  mov ebx, dword[h]
  xor edx, edx
  div ebx
  xor edx, edx
  mul dword[py]
  add eax, dword[ymin]
  mov dword[y0], eax

  mov dword[x], 0
  mov dword[y], 0
  mov dword[i], 0
  mov dword[e], 0

whale:
  sar dword[x], 8
  sar dword[y], 8
  mov eax, dword[x]
  xor edx, edx
  mul eax
  mov ebx, eax
  mov eax, dword[y]
  xor edx, edx
  mul eax
  sub ebx, eax
  add ebx, dword[x0]
  mov dword[tmp], ebx

  mov eax, 2
  xor edx, edx
  mul dword[x]
  xor edx, edx
  mul dword[y]
  add eax, dword[y0]
  mov dword[y], eax

  mov eax, dword[tmp]
  mov dword[x], eax
  inc dword[i]

  ;loop comparisons:
  sar dword[x], 8
  sar dword[y], 8
  mov eax, dword[x]	;eax = x
  xor edx, edx
  mul dword[x]		;eax = x^2
  mov ebx, eax		;ebx = x^2
  mov eax, dword[y]	;eax = y
  xor edx, edx
  mul dword[y]		;eax = y^2
  add eax, ebx		;eax = x^2 + y^2
  sal dword[x], 8
  sal dword[y], 8

  mov ebx, 4
  sal ebx, 16
  cmp eax, ebx
  jl ok1
  mov dword[e], 1
ok1:
  mov eax, dword[i]
  sub eax, 1d
  cmp eax, dword[imax]
  jle ok2
  mov dword[e], 1
ok2:
  cmp dword[e], 1
  jne whale

  mov eax, dword[imax]
  cmp dword[i], eax
  jg nodraw

  xor ecx, ecx
  xor edx, edx
  mov cx, word[px]
  mov dx, word[py]
  mov al, byte[i]
  call draw_pixel
nodraw:
  inc dword[py]
  mov eax, dword[h]
  cmp dword[py], eax
  jl forPY

  inc dword[px]
  mov eax, dword[w]
  cmp dword[px], eax
  jl forPX

  ret


draw_pixel:				;will draw pixel [cx, dx]
  mov ah, 0Ch ;ah -> mode, al -> color, bh -> page, cx -> x-axis, dx -> y-axis
  mov bh, 0
  ;mov al, 1
  int 10h
  ret

exit:		xor	ah,ah
  mov	al,3
  int	10h
  mov	ax,4C00h
  int	21h
