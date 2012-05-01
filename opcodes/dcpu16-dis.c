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
lookup_instruction(unsigned short opcode)
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
print_operand (bfd_vma memaddr, struct disassemble_info *info, int arg, int is_dest)
{
  int status;
  bfd_byte buffer[8];
  int w = 0;

  if ((arg >= 0x10 && arg <= 0x17) || arg == 0x1a || arg == 0x1e || arg == 0x1f)
    {
      status = (*info->read_memory_func) (memaddr, buffer, 2, info);
      if (status != 0)
	{
	  (*info->memory_error_func) (status, memaddr, info);
	  return -1;
	}
      w = bfd_getl16 (buffer);
    }

  if (arg < 0x08)
    {
      (*info->fprintf_func) (info->stream, "%s", dcpu16_register_table[arg].name);
      return 0;
    }
  if (arg < 0x10)
    {
      (*info->fprintf_func) (info->stream, "[%s]", dcpu16_register_table[arg&0x7].name);
      return 0;
    }
  if (arg < 0x18)
    {
	(*info->fprintf_func) (info->stream, "[%s+0x%04x]", dcpu16_register_table[arg&0x7].name, w);
      return 1;
    }

  switch(arg)
    {
    case 0x18:
      if(is_dest)
	(*info->fprintf_func) (info->stream, "[--SP]");
      else
	(*info->fprintf_func) (info->stream, "[SP++]");
      return 0;
    case 0x19:
      (*info->fprintf_func) (info->stream, "[SP]");
      return 0;
    case 0x1a:
      (*info->fprintf_func) (info->stream, "[SP+0x%04x]", w);
      return 1;
    case 0x1b:
      (*info->fprintf_func) (info->stream, "SP");
      return 0;
    case 0x1c:
      (*info->fprintf_func) (info->stream, "PC");
      return 0;
    case 0x1d:
      (*info->fprintf_func) (info->stream, "EX");
      return 0;
    case 0x1e:
      (*info->fprintf_func) (info->stream, "[0x%04x]", w);
      return 1;
    case 0x1f:
      (*info->fprintf_func) (info->stream, "0x%04x", w);
      return 1;
    }

  (*info->fprintf_func) (info->stream, "0x%02x", (arg&0x1f)-1);
  return 0;
}

int
print_insn_dcpu16 (bfd_vma memaddr, struct disassemble_info *info)
{
  int status, result;
  bfd_byte buffer[8];
  unsigned short opcode;
  const struct dcpu16_opcode *op;

  info->bytes_per_line = 6;
  info->bytes_per_chunk = 2;
  info->display_endian = BFD_ENDIAN_LITTLE;

  status = (*info->read_memory_func) (memaddr, buffer, 2, info);
  if (status != 0)
    {
      (*info->memory_error_func) (status, memaddr, info);
      return -1;
    }
  opcode = bfd_getl16 (buffer);

  result = 1;

  if (opcode & 0x1f)
    {
      op = lookup_instruction (opcode&0x1f);
      if (op)
	(*info->fprintf_func) (info->stream, "%s\t", op->name);
      else
	(*info->fprintf_func) (info->stream, "**%02x**\t", opcode&0x1f);
      result += print_operand(memaddr+result, info, (opcode>>5)&0x1f, 1);
      (*info->fprintf_func) (info->stream, ", ");
      result += print_operand(memaddr+result, info, opcode>>10, 0);
    }
  else
    {
      op = lookup_instruction (opcode&0x3ff);
      if (op)
	(*info->fprintf_func) (info->stream, "%s\t", op->name);
      else
	(*info->fprintf_func) (info->stream, "**%02x**\t", opcode&0x1f);
      result += print_operand(memaddr+result, info, opcode>>10, 0);
    }

  return 2*result;
}
