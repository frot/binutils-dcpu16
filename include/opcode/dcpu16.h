/* dcpu16.h -- Header file for DCPU-16 opcode table
   Copyright 2012 Fredrik Rothamel

   This file is part of GDB, GAS, and the GNU binutils.

   GDB, GAS, and the GNU binutils are free software; you can redistribute
   them and/or modify them under the terms of the GNU General Public
   License as published by the Free Software Foundation; either version 3,
   or (at your option) any later version.

   GDB, GAS, and the GNU binutils are distributed in the hope that they
   will be useful, but WITHOUT ANY WARRANTY; without even the implied
   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
   the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this file; see the file COPYING3.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#ifndef DCPU16_H
#define DCPU16_H

/* The opcode table is an array of struct dcpu16_opcode.  */
struct dcpu16_opcode
{
  /* The opcode name.  */
  const char *name;

  /* the opcode */
  unsigned short op;

  /* number of arguments */
  int args;
};

#define DCPU16_INSN_COUNT 33
extern const struct dcpu16_opcode dcpu16_opcode_table[];

/* The operands table is an array of struct dcpu16_operand.  */
struct dcpu16_operand
{
  /* The operand name */
  const char* name;

  /* value of instruction operand field (6 bits) */
  int value;

  /* only valid as source or destination operand */
  int op_pos;

  /* value is longer than 6 bits */
  int is_long;

  /* extended  operand value (16 bits) */
  int long_value;
};

#define REG_INDEX_PICK	0x1a
#define REG_INDEX_SP	0x1b
#define REG_INDEX_PC	0x1c
#define REG_INDEX_EX	0x1d

/* The registers table is an array of struct dcpu16_register.  */
struct dcpu16_register
{
  /* The register name */
  const char* name;

  /* Register index */
  int index;

  /* Indirect adress allowed */
  int indirect;

  /* Operand position */
  int pos;
};

extern const struct dcpu16_register dcpu16_register_table[];

#endif /* DCPU16_H */
