# objdump: -Dr

.*:.*


Disassembly of section .text:

00000000 <.text>:
   0:	0000           	
   1:	0001           	set	A, A
   2:	0002           	add	A, A
   3:	0003           	sub	A, A
   4:	1234 3003      	ifg	\[B\+0x3003\], Y
   6:	4004 5005      	mul	A, \[A\+0x5005\]
   8:	ffff fffe      	std	0xfffe, 0x1e
   a:	fffd           	
   b:	fffc           	
   c:	0000           	
   d:	0001           	set	A, A
   e:	0002           	add	A, A
   f:	0003           	sub	A, A
  10:	1234 3003      	ifg	\[B\+0x3003\], Y
  12:	4004 5005      	mul	A, \[A\+0x5005\]
  14:	ffff fffe      	std	0xfffe, 0x1e
  16:	fffd           	
  17:	fffc           	
  18:	0048           	mod	C, A
  19:	0065           	mli	X, A
  1a:	006c           	xor	X, A
  1b:	006c           	xor	X, A
  1c:	006f           	shl	X, A
  1d:	002c           	xor	B, A
  1e:	0020           	jsr	A
  1f:	0057           	ifu	C, A
  20:	006f           	shl	X, A
  21:	0072           	ife	X, A
  22:	006c           	xor	X, A
  23:	0064           	mul	X, A
  24:	0021           	set	B, A
  25:	0000           	
  26:	4661 696c 2100 	set	\[X\+0x2100\], \[B\+0x696c\]
