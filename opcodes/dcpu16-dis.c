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

const char* insn_names[] = {
  "extended",	/* 0x00 */
  "set",	/* 0x01 */
  "add",	/* 0x02 */
  "sub",	/* 0x03 */
  "mul",	/* 0x04 */
  "mli",	/* 0x05 */
  "div",	/* 0x06 */
  "dvi",	/* 0x07 */
  "mod",	/* 0x08 */
  "and",	/* 0x09 */
  "bor",	/* 0x0a */
  "xor",	/* 0x0b */
  "shr",	/* 0x0c */
  "asr",	/* 0x0d */
  "shl",	/* 0x0e */
  "**0x0f**",	/* 0x0f */
  "ifb",	/* 0x10 */
  "ifc",	/* 0x11 */
  "ife",	/* 0x12 */
  "ifn",	/* 0x13 */
  "ifg",	/* 0x14 */
  "ifa",	/* 0x15 */
  "ifl",	/* 0x16 */
  "ifu"		/* 0x17 */
  "**0x18**",	/* 0x18 */
  "**0x19**",	/* 0x19 */
  "**0x1a**",	/* 0x1a */
  "**0x1b**",	/* 0x1b */
  "**0x1c**",	/* 0x1c */
  "**0x1d**",	/* 0x1d */
  "**0x1e**",	/* 0x1e */
  "**0x1f**"	/* 0x1f */
};

const char* ext_insn_names[] = { 
  "**0x00**",	/* 0x00 */
  "jsr",	/* 0x01 */
  "**0x02**",	/* 0x02 */
  "**0x03**",	/* 0x03 */
  "**0x04**",	/* 0x04 */
  "**0x05**",	/* 0x05 */
  "**0x06**",	/* 0x06 */
  "**0x07**",	/* 0x07 */
  "int",	/* 0x08 */
  "ing",	/* 0x09 */
  "ins",	/* 0x0a */
  "**0x0b**",	/* 0x0b */
  "**0x0c**",	/* 0x0c */
  "**0x0d**",	/* 0x0d */
  "**0x0e**",	/* 0x0e */
  "**0x0f**",	/* 0x0f */
  "hwn",	/* 0x10 */
  "hwq",	/* 0x11 */
  "hwi",	/* 0x12 */
  "**0x13**",	/* 0x13 */
  "**0x14**",	/* 0x14 */
  "**0x15**",	/* 0x15 */
  "**0x16**",	/* 0x16 */
  "**0x17**",	/* 0x17 */
  "**0x18**",	/* 0x18 */
  "**0x19**",	/* 0x19 */
  "**0x1a**",	/* 0x1a */
  "**0x1b**",	/* 0x1b */
  "**0x1c**",	/* 0x1c */
  "**0x1d**",	/* 0x1d */
  "**0x1e**",	/* 0x1e */
  "**0x1f**",	/* 0x1f */
};

const char* reg_names[] = {
    "A",
    "B",
    "C",
    "X",
    "Y",
    "Z",
    "I",
    "J"
};

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
      (*info->fprintf_func) (info->stream, "%s", reg_names[arg]);
      return 0;
    }
  if (arg < 0x10)
    {
      (*info->fprintf_func) (info->stream, "[%s]", reg_names[arg&0x7]);
      return 0;
    }
  if (arg < 0x18)
    {
	(*info->fprintf_func) (info->stream, "[%s+0x%04x]", reg_names[arg&0x7], w);
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
  unsigned short op;

  info->bytes_per_line = 6;
  info->bytes_per_chunk = 2;
  info->display_endian = BFD_ENDIAN_LITTLE;

  status = (*info->read_memory_func) (memaddr, buffer, 2, info);
  if (status != 0)
    {
      (*info->memory_error_func) (status, memaddr, info);
      return -1;
    }
  op = bfd_getl16 (buffer);

  result = 1;

  if (op & 0x1f)
    {
      (*info->fprintf_func) (info->stream, "%s\t", insn_names[op&0x1f]);
      result += print_operand(memaddr+result, info, (op>>5)&0x1f, 1);
      (*info->fprintf_func) (info->stream, ", ");
      result += print_operand(memaddr+result, info, op>>10, 0);
    }
  else
    {
      (*info->fprintf_func) (info->stream, "%s\t", ext_insn_names[(op>>5)&0x1f]);
      result += print_operand(memaddr+result, info, op>>10, 0);
    }

  return 2*result;
}
