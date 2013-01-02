# objdump: -Dr

.*:.*


Disassembly of section .text:

00000000 <.text>:
   0:	7c01 0030      	set	A, 0x0030
   2:	7fc1 0020 1000 	set	\[0x1000\], 0x0020
   5:	7803 1000      	sub	A, \[0x1000\]
   7:	c413           	ifn	A, 0x10
   8:	7f81 001a      	set	PC, 0x001a
			9: r_imm16	.text
   a:	acc1           	set	I, 0x0a
   b:	7c01 2000      	set	A, 0x2000

0000000d <loop>:
   d:	22c1 2000      	set	\[I\+0x2000\], \[A\]
   f:	88c3           	sub	I, 0x01
  10:	84d3           	ifn	I, 0x00
  11:	7f81 000d      	set	PC, 0x000d
			12: r_imm16	.text
  13:	9461           	set	X, 0x04
  14:	7c20 0018      	jsr	0x0018
			15: r_imm16	.text
  16:	7f81 001a      	set	PC, 0x001a
			17: r_imm16	.text

00000018 <testsub>:
  18:	946f           	shl	X, 0x04
  19:	6381           	set	PC, \[SP\+\+\]

0000001a <crash>:
  1a:	7f81 001a      	set	PC, 0x001a
			1b: r_imm16	.text
