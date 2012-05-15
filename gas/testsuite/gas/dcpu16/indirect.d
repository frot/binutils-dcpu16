# objdump: -Drt

indirect.o:     filformat coff-dcpu16

SYMBOL TABLE:
[  0](sec -2)(fl 0x00)(ty   0)(scl 103) (nx 1) 0x00000000 fake
File 
[  2](sec  1)(fl 0x00)(ty   0)(scl   6) (nx 0) 0x00000006 label
[  3](sec  1)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .text
AUX scnlen 0xc nreloc 2 nlnno 0
[  5](sec  2)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .data
AUX scnlen 0x0 nreloc 0 nlnno 0
[  7](sec  3)(fl 0x00)(ty   0)(scl   3) (nx 1) 0x00000000 .bss
AUX scnlen 0x0 nreloc 0 nlnno 0
[  9](sec  0)(fl 0x00)(ty   0)(scl   2) (nx 0) 0x00000000 ext_label



Disassembly of section .text:

00000000 <.text>:
   0:	7801 0006      	set	A, [0x0006]
			1: r_imm16	.text
   2:	2401           	set	A, [B]
   3:	7801 0000      	set	A, [0x0000]
			4: r_imm16	ext_label
   5:	2401           	set	A, [B]
