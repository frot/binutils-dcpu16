/* Disassemble DCPU-16 instructions.
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
#include "dis-asm.h"
#include "opintl.h"
#include "opcode/dcpu16.h"

static const struct dcpu16_opcode *
lookup_instruction(u16 opcode)
{
  int start = 0;
  int end = DCPU16_INSN_COUNT;
  int mid;

  /* Optimize the common case */
  if (opcode == 0x0001)
    return &dcpu16_opcode_table[0];

  /* Search the opcode table */
  while (start < end)
    {
      mid = (start+end) / 2;

      if (opcode < dcpu16_opcode_table[mid].op)
	{
	  end = mid;
	}
      else if (opcode > dcpu16_opcode_table[mid].op)
	{
	  start = mid + 1;
	}
      else
	return &dcpu16_opcode_table[mid];
    }

  return 0;
}

static int
read_word (bfd_vma memaddr, struct disassemble_info *info, u16 arg, u16 *val)
{
  int status;
  bfd_byte buffer[8];

  if ((arg >= 0x10 && arg <= 0x17) || arg == 0x1a || arg == 0x1e || arg == 0x1f)
    {
      status = (*info->read_memory_func) (memaddr, buffer, 2, info);
      if (status != 0)
	{
	  (*info->memory_error_func) (status, memaddr, info);
	  return -1;
	}
      *val = bfd_getb16 (buffer);

      return 1;
    }

  return 0;
}

static void
print_operand (struct disassemble_info *info, u16 arg, u16 val, int is_dest)
{
  if (arg < 0x08)
    {
      (*info->fprintf_func) (info->stream, "%s", dcpu16_register_table[arg].name);
      return;
    }
  if (arg < 0x10)
    {
      (*info->fprintf_func) (info->stream, "[%s]", dcpu16_register_table[arg&0x7].name);
      return;
    }
  if (arg < 0x18)
    {
	(*info->fprintf_func) (info->stream, "[%s+0x%04x]", dcpu16_register_table[arg&0x7].name, val);
      return;
    }

  switch(arg)
    {
    case 0x18:
      if(is_dest)
	(*info->fprintf_func) (info->stream, "[--SP]");
      else
	(*info->fprintf_func) (info->stream, "[SP++]");
      return;
    case 0x19:
      (*info->fprintf_func) (info->stream, "[SP]");
      return;
    case 0x1a:
      (*info->fprintf_func) (info->stream, "[SP+0x%04x]", val);
      return;
    case 0x1b:
      (*info->fprintf_func) (info->stream, "SP");
      return;
    case 0x1c:
      (*info->fprintf_func) (info->stream, "PC");
      return;
    case 0x1d:
      (*info->fprintf_func) (info->stream, "EX");
      return;
    case 0x1e:
      (*info->fprintf_func) (info->stream, "[0x%04x]", val);
      return;
    case 0x1f:
      (*info->fprintf_func) (info->stream, "0x%04x", val);
      return;
    }

  (*info->fprintf_func) (info->stream, "0x%02x", (arg&0x1f)-1);
}

int
print_insn_dcpu16 (bfd_vma memaddr, struct disassemble_info *info)
{
  int status, result;
  bfd_byte buffer[8];
  u16 opcode, aword, bword;
  const struct dcpu16_opcode *op;

  info->bytes_per_line = 6;
  info->bytes_per_chunk = 2;
  info->display_endian = BFD_ENDIAN_BIG;

  status = (*info->read_memory_func) (memaddr, buffer, 2, info);
  if (status != 0)
    {
      (*info->memory_error_func) (status, memaddr, info);
      return -1;
    }
  opcode = bfd_getb16 (buffer);

  result = 1;

  if (opcode & 0x1f)
    {
      op = lookup_instruction (opcode&0x1f);
      if (op)
	{
	  (*info->fprintf_func) (info->stream, "%s\t", op->name);

	  result += read_word(memaddr+result, info, opcode>>10, &aword);
	  result += read_word(memaddr+result, info, (opcode>>5)&0x1f, &bword);

	  print_operand(info, (opcode>>5)&0x1f, bword, 1);
	  (*info->fprintf_func) (info->stream, ", ");
	  print_operand(info, opcode>>10, aword, 0);
	}
    }
  else
    {
      op = lookup_instruction (opcode&0x3ff);
      if (op)
	{
	  (*info->fprintf_func) (info->stream, "%s\t", op->name);
	  result += read_word(memaddr+result, info, opcode>>10, &aword);
	  print_operand(info, opcode>>10, aword, 0);
	}
    }

  return 2*result;
}
