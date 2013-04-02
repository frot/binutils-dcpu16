# objdump: -Dr

.*:.*


Disassembly of section .text:

00000000 <.text>:
   0:	2001           	set	A, \[A\]
   1:	2121           	set	\[B\], \[A\]
   2:	0501           	set	\[A\], B

00000003 <crash>:
   3:	7f81 0003      	set	PC, 0x0003
			4: r_imm16	.text
