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
