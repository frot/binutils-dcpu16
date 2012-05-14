
byte.o:     filformat coff-dcpu16


Disassembly of section .text:

00000000 <.text>:
   0:	0000           	**00**	A
   1:	0001           	set	A, A
   2:	0002           	add	A, A
   3:	0003           	sub	A, A
   4:	1234 3003      	ifg	[B+0x3003], Y
   6:	4004 5005      	mul	A, [A+0x5005]
   8:	ffff fffe      	**1f**	0xfffe, 0x1e
   a:	fffd fffc      	**1d**	0xfffc, 0x1e
   c:	0000           	**00**	A
   d:	0001           	set	A, A
   e:	0002           	add	A, A
   f:	0003           	sub	A, A
  10:	1234 3003      	ifg	[B+0x3003], Y
  12:	4004 5005      	mul	A, [A+0x5005]
  14:	ffff fffe      	**1f**	0xfffe, 0x1e
  16:	fffd fffc      	**1d**	0xfffc, 0x1e
