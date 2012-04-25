/* dcpu16.h -- Header file for DCPU-16 opcode table

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
  int op;

  /* number of arguments */
  int args;
};

extern const struct dcpu16_opcode dcpu16_opcode_table[];
//extern const int dcpu16_num_opcodes;

/* The operands table is an array of struct dcpu16_operand.  */
struct dcpu16_operand
{
  /* The operand name */
  const char* name;

  /* value of instruction operand field (6 bits) */
  int value;

  /* value is longer than 6 bits */
  int is_long;

  /*extended  operand value (16 bits) */
  int long_value;
};
extern const struct dcpu16_operand dcpu16_operand_table[];

#endif /* DCPU16_H */
