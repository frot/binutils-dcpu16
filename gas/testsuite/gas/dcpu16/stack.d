# objdump: -Dr

.*:.*


Disassembly of section .text:

00000000 <.text>:
   0:	0301           	set	\[--SP\], A
   1:	0301           	set	\[--SP\], A
   2:	6001           	set	A, \[SP\+\+\]
   3:	6001           	set	A, \[SP\+\+\]
   4:	6401           	set	A, \[SP\]
   5:	6401           	set	A, \[SP\]
   6:	0321           	set	\[SP\], A
   7:	0321           	set	\[SP\], A
   8:	6401           	set	A, \[SP\]
   9:	6401           	set	A, \[SP\]
   a:	6801 0001      	set	A, \[SP\+0x0001\]
   c:	6801 0001      	set	A, \[SP\+0x0001\]
   e:	6801 0016      	set	A, \[SP\+0x0016\]
			f: r_imm16	.text
  10:	6801 0016      	set	A, \[SP\+0x0016\]
			11: r_imm16	.text
  12:	6801 fffd      	set	A, \[SP\+0xfffd\]
  14:	6801 fffd      	set	A, \[SP\+0xfffd\]

00000016 <foo>:
  16:	6c01           	set	A, SP
  17:	0361           	set	SP, A
