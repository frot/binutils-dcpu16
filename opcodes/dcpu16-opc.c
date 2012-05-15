/* DCPU-16 opcode list.
   Copyright 2012 Fredrik Rothamel

   Implements v1.7 of the DCPU-16 specification.

   This file is part of the GNU opcodes library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include <stdio.h>
#include "sysdep.h"
#include "opcode/dcpu16.h"

const struct dcpu16_opcode dcpu16_opcode_table[] = 
  {
    { "set",  0x0001, 2 },
    { "add",  0x0002, 2 },
    { "sub",  0x0003, 2 },
    { "mul",  0x0004, 2 },
    { "mli",  0x0005, 2 },
    { "div",  0x0006, 2 },
    { "dvi",  0x0007, 2 },
    { "mod",  0x0008, 2 },
    { "mdi",  0x0009, 2 },
    { "and",  0x000a, 2 },
    { "bor",  0x000b, 2 },
    { "xor",  0x000c, 2 },
    { "shr",  0x000d, 2 },
    { "asr",  0x000e, 2 },
    { "shl",  0x000f, 2 },
    { "ifb",  0x0010, 2 },
    { "ifc",  0x0011, 2 },
    { "ife",  0x0012, 2 },
    { "ifn",  0x0013, 2 },
    { "ifg",  0x0014, 2 },
    { "ifa",  0x0015, 2 },
    { "ifl",  0x0016, 2 },
    { "ifu",  0x0017, 2 },
    { "adx",  0x001a, 2 },
    { "sbx",  0x001b, 2 },
    { "sti",  0x001e, 2 },
    { "std",  0x001f, 2 },

    { "jsr",  0x01<<5, 1 },
    { "hcf",  0x07<<5, 1 },
    { "int",  0x08<<5, 1 },
    { "iag",  0x09<<5, 1 },
    { "ias",  0x0a<<5, 1 },
    { "rfi",  0x0b<<5, 1 },
    { "iaq",  0x0c<<5, 1 },
    { "hwn",  0x10<<5, 1 },
    { "hwq",  0x11<<5, 1 },
    { "hwi",  0x12<<5, 1 },

    { "jmp",  0x0381, 1 },
    { "bra",  0x0382, 1 },
    { "ret",  0x6381, 0 },
    { NULL, 0, 0 }
  };

const struct dcpu16_register dcpu16_register_table[] = 
  {
    { "A",    0x00, 1, 0 },
    { "B",    0x01, 1, 0 },
    { "C",    0x02, 1, 0 },
    { "X",    0x03, 1, 0 },
    { "Y",    0x04, 1, 0 },
    { "Z",    0x05, 1, 0 },
    { "I",    0x06, 1, 0 },
    { "J",    0x07, 1, 0 },
    { "PUSH", 0x18, 0, 1 },
    { "POP",  0x18, 0, 2 },
    { "PEEK", 0x19, 0, 0 },
    { "PICK", 0x1a, 0, 0 },
    { "SP",   0x1b, 1, 0 },
    { "PC",   0x1c, 0, 0 },
    { "EX",   0x1d, 0, 0 },
    { NULL, 0, 0, 0 }
  };
