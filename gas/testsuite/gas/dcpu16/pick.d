# objdump: -Dr

.*:.*


Disassembly of section .text:

00000000 <.text>:
   0:	6401           	set	A, \[SP\]
   1:	6801 0001      	set	A, \[SP\+0x0001\]
   3:	6801 0005      	set	A, \[SP\+0x0005\]
			4: r_imm16	.text

00000005 <foo>:
   5:	6801 fffd      	set	A, \[SP\+0xfffd\]
