/* DCPU-16 opcode list.
   Copyright 2012 Fredrik Rothamel

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
    { "and",  0x0009, 2 },
    { "bor",  0x000a, 2 },
    { "xor",  0x000b, 2 },
    { "shr",  0x000c, 2 },
    { "asr",  0x000d, 2 },
    { "shl",  0x000e, 2 },
    { "mvi",  0x000f, 2 },
    { "ifb",  0x0010, 2 },
    { "ifc",  0x0011, 2 },
    { "ife",  0x0012, 2 },
    { "ifn",  0x0013, 2 },
    { "ifg",  0x0014, 2 },
    { "ifa",  0x0015, 2 },
    { "ifl",  0x0016, 2 },
    { "ifu",  0x0017, 2 },
    { "adx",  0x001a, 2 },
    { "sux",  0x001b, 2 },

    { "jsr",  0x0020, 1 },
    { "int",  0x0100, 1 },
    { "iag",  0x0120, 1 },
    { "ias",  0x0140, 1 },
    { "hwn",  0x0200, 1 },
    { "hwq",  0x0220, 1 },
    { "hwi",  0x0240, 1 },

    { "push", 0x0301, 1 },
    { "pop",  0x6001, 1 },
    { "jmp",  0x0381, 1 },
    { "bra",  0x0382, 1 },
    { "ret",  0x6381, 0 },
    { NULL, 0, 0 }
  };

const struct dcpu16_operand dcpu16_operand_table[] =
  {
    { "a",      0x00, 0, 0, 0 },
    { "b",      0x01, 0, 0, 0 },
    { "c",      0x02, 0, 0, 0 },
    { "x",      0x03, 0, 0, 0 },
    { "y",      0x04, 0, 0, 0 },
    { "z",      0x05, 0, 0, 0 },
    { "i",      0x06, 0, 0, 0 },
    { "j",      0x07, 0, 0, 0 },
    { "[a]",    0x08, 0, 0, 0 },
    { "[b]",    0x09, 0, 0, 0 },
    { "[c]",    0x0a, 0, 0, 0 },
    { "[x]",    0x0b, 0, 0, 0 },
    { "[y]",    0x0c, 0, 0, 0 },
    { "[z]",    0x0d, 0, 0, 0 },
    { "[i]",    0x0e, 0, 0, 0 },
    { "[j]",    0x0f, 0, 0, 0 },
    { "[sp++]", 0x18, 2, 0, 0 },
    { "pop",    0x18, 2, 0, 0 },
    { "[--sp]", 0x18, 1, 0, 0 },
    { "push",   0x18, 1, 0, 0 },
    { "[sp]",   0x19, 0, 0, 0 },
    { "peek",   0x19, 0, 0, 0 },
    { "sp",     0x1b, 0, 0, 0 },
    { "pc",     0x1c, 0, 0, 0 },
    { "ex",     0x1d, 0, 0, 0 },
    { NULL, 0, 0, 0, 0 }
  };

const struct dcpu16_register dcpu16_register_table[] = 
  {
    { "a", 0 },
    { "A", 0 },
    { "b", 1 },
    { "B", 1 },
    { "c", 2 },
    { "C", 2 },
    { "x", 3 },
    { "X", 3 },
    { "y", 4 },
    { "Y", 4 },
    { "z", 5 },
    { "Z", 5 },
    { "i", 6 },
    { "I", 6 },
    { "j", 7 },
    { "J", 7 },
    { "sp", 10 },
    { "SP", 10 },
    { "pc", -1 },
    { "PC", -1 },
    { "ex", -1 },
    { "EX", -1 },
    { "push", -1 },
    { "PUSH", -1 },
    { "pop", -1 },
    { "POP", -1 },
    { "peek", -1 },
    { "PEEK", -1 },
    { "pick", -1 },
    { "PICK", -1 },
    { NULL, 0 }
  };
