# objdump: -Dr

.*:.*


Disassembly of section .text:

00000000 <.text>:
   0:	7801 0012      	set	A, \[0x0012\]
   2:	4001 0034      	set	A, \[A\+0x0034\]

00000004 <defined>:
   4:	7801 0004      	set	A, \[0x0004\]
			5: r_imm16	.text
   6:	4001 0004      	set	A, \[A\+0x0004\]
			7: r_imm16	.text
   8:	7801 0016      	set	A, \[0x0016\]
			9: r_imm16	.text
   a:	4001 0016      	set	A, \[A\+0x0016\]
			b: r_imm16	.text
   c:	7801 0000      	set	A, \[0x0000\]
			d: r_imm16	ext_label
   e:	4001 0000      	set	A, \[A\+0x0000\]
			f: r_imm16	ext_label
  10:	7801 0012      	set	A, \[0x0012\]
  12:	7801 0020      	set	A, \[0x0020\]
			13: r_imm16	.text
  14:	4001 0020      	set	A, \[A\+0x0020\]
			15: r_imm16	.text
