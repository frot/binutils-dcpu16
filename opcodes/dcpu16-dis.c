#include <stdio.h>
#include "sysdep.h"
#include "dis-asm.h"
#include "opintl.h"

const char* insn_names[] = {
  "ext",
  "set",
  "add",
  "sub",
  "mul",
  "div",
  "mod",
  "shl",
  "shr",
  "and",
  "bor",
  "xor",
  "ife",
  "ifn",
  "ifg",
  "ifb",
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
print_operand (bfd_vma memaddr, struct disassemble_info *info, int arg)
{
  int status;
  bfd_byte buffer[8];
  int w = 0;

  if ((arg >= 0x10 && arg <= 0x17) || arg == 0x1e || arg == 0x1f)
    {
      status = (*info->read_memory_func) (memaddr, buffer, 2, info);
      if (status != 0)
	{
	  (*info->memory_error_func) (status, memaddr, info);
	  return -1;
	}
      w = bfd_getb16 (buffer);
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
      (*info->fprintf_func) (info->stream, "[0x%04x+%s]", w, reg_names[arg&0x7]);
      return 1;
    }

  switch(arg)
    {
    case 0x18:
      (*info->fprintf_func) (info->stream, "POP");
      return 0;
    case 0x19:
      (*info->fprintf_func) (info->stream, "PEEK");
      return 0;
    case 0x1a:
      (*info->fprintf_func) (info->stream, "PUSH");
      return 0;
    case 0x1b:
      (*info->fprintf_func) (info->stream, "SP");
      return 0;
    case 0x1c:
      (*info->fprintf_func) (info->stream, "PC");
      return 0;
    case 0x1d:
      (*info->fprintf_func) (info->stream, "O");
      return 0;
    case 0x1e:
      (*info->fprintf_func) (info->stream, "[0x%04x]", w);
      return 1;
    case 0x1f:
      (*info->fprintf_func) (info->stream, "0x%04x", w);
      return 1;
    }

  (*info->fprintf_func) (info->stream, "0x%02x", arg&0x1f);
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
  info->display_endian = BFD_ENDIAN_BIG;

  status = (*info->read_memory_func) (memaddr, buffer, 2, info);
  if (status != 0)
    {
      (*info->memory_error_func) (status, memaddr, info);
      return -1;
    }
  op = bfd_getb16 (buffer);

  result = 1;

  if (op & 0x0f)
    {
      (*info->fprintf_func) (info->stream, "%s\t", insn_names[op&0x0f]);
      result += print_operand(memaddr+result, info, (op>>4)&0x3f);
      (*info->fprintf_func) (info->stream, ", ");
      result += print_operand(memaddr+result, info, op>>10);
    }
  else if ((op & 0x3ff) == 0x10)
    {
      (*info->fprintf_func) (info->stream, "jsr\t");
      result += print_operand(memaddr+result, info, op>>10);
    }
  else
    {
      (*info->fprintf_func) (info->stream, ".word\t0x%x", op);
    }

  return 2*result;
}
