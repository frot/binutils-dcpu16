/* tc-dcpu16.c -- Assembler code for the Mojang DCPU16

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 51 Franklin Street - Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "as.h"
#include "safe-ctype.h"
#include "subsegs.h"
#include "opcode/dcpu16.h"

const char comment_chars[]        = ";";
const char line_comment_chars[]   = "#";
const char line_separator_chars[] = "";
const char *md_shortopts          = "";
const char EXP_CHARS[]            = "eE";
const char FLT_CHARS[]            = "dD";

#if HAVE_LIMITS_H
#include <limits.h>
#endif

#ifndef CHAR_BIT
#define CHAR_BIT 16
#endif

struct option md_longopts[] =
{
  {NULL, no_argument, NULL, 0}
};

size_t md_longopts_size = sizeof (md_longopts);

/* Opcode hash table.  */
static struct hash_control *dcpu16_opcode_hash;

/* Simple operands hash table.  */
static struct hash_control *dcpu16_operand_hash;

void
md_show_usage (FILE *stream ATTRIBUTE_UNUSED)
{
}

int
md_parse_option (int c ATTRIBUTE_UNUSED, char *arg ATTRIBUTE_UNUSED)
{
  return 0;
}

symbolS *
md_undefined_symbol (char *name ATTRIBUTE_UNUSED)
{
  return 0;
}

char *
md_atof (int type, char *litP, int *sizeP)
{
  return ieee_md_atof (type, litP, sizeP, TRUE);
}

valueT
md_section_align (segT segment ATTRIBUTE_UNUSED, valueT section_size)
{
  return section_size;
}

long
md_pcrel_from (fixS *fixP ATTRIBUTE_UNUSED)
{
  return 0;
}

void
md_begin (void)
{
  struct dcpu16_opcode *opcode;
  dcpu16_opcode_hash = hash_new ();

  struct dcpu16_operand *operand;
  dcpu16_operand_hash = hash_new ();

  /* Insert opcode names into a hash table.  */
  for (opcode = (struct dcpu16_opcode *) dcpu16_opcode_table; opcode->name; opcode++)
      hash_insert (dcpu16_opcode_hash, opcode->name, (char *) opcode);

  /* Insert known operand names into a hash table.  */
  for (operand = (struct dcpu16_operand *) dcpu16_operand_table; operand->name; operand++)
      hash_insert (dcpu16_operand_hash, operand->name, (char *) operand);
}

#define NAME_BUF_LEN 10

static void
parse_operand (struct dcpu16_operand *operand)
{
  static expressionS *exp = 0;
  int indirect = 0;
  int regnum = -1;
  char *p;
  char opname[NAME_BUF_LEN];
  int oplen = 0;
  struct dcpu16_operand *op;
  char *frag = 0;

  /* First try to match against list of known simple operands */
  for (p = input_line_pointer; *p && oplen < NAME_BUF_LEN && *p != ','; p++)
    {
      opname[oplen] = TOLOWER (*p);
      oplen++;
    }

  if (oplen == 0)
    {
      as_bad (_("operand expected"));
      return;
    }
    
  if (oplen < NAME_BUF_LEN)
    {
      op = (struct dcpu16_operand *) hash_find_n (dcpu16_operand_hash, opname, oplen);

      if (op)
	{
	  input_line_pointer = p;
	  operand->value = op->value;
	  return;
	}
    }

  /* No simple match, evaluate expression */
  if (*input_line_pointer == '[')
    {
      indirect=1;
      input_line_pointer++;

      /* check for [addr + reg] expressions */
      for (p = input_line_pointer; *p && *p != ']' && *p != ','; p++);

      if (*p == ']' && ISALPHA(*(p-1)) && *(p-2) == '+')
	{
	  *(p-2) = ',';
	}
    }

  if (!exp)
    exp = xmalloc(sizeof(expressionS));

  expression (exp);

  if (indirect)
    {
      if (*input_line_pointer == ',')
	{
	  input_line_pointer++;
	  opname[0] = TOLOWER (*input_line_pointer);
	  op = (struct dcpu16_operand *) hash_find_n (dcpu16_operand_hash, opname, 1);

	  if (op)
	    {
	      input_line_pointer++;
	      regnum = op->value;
	    }
	}

      if (*input_line_pointer==']')
	{
	  input_line_pointer++;
	}
      else
	{
	  as_bad (_("] expected"));
	}
    }

  if (indirect)
    {
      if (regnum >= 0)
	{
	  operand->value = 0x10 | regnum;
	  operand->is_long = 1;
	  operand->long_value = exp->X_add_number;
	  frag = frag_more (2);
	  md_number_to_chars (frag, operand->long_value, 2);
	}
      else
	{
	  operand->value = 0x1e;
	  operand->is_long = 1;
	  operand->long_value = exp->X_add_number;
	  frag = frag_more (2);
	  md_number_to_chars (frag, operand->long_value, 2);
	}
    }
  else
    {
      if (exp->X_op == O_constant && exp->X_add_number < 0x20)
	{
	  operand->value = 0x20 | exp->X_add_number;
	}
      else
	{
	  operand->value = 0x1f;
	  operand->is_long = 1;
	  operand->long_value = exp->X_add_number;
	  frag = frag_more (2);
	  md_number_to_chars (frag, operand->long_value, 2);
	}
    }

  if (frag && exp->X_op != O_constant)
    {
      fix_new_exp (frag_now, (frag - frag_now->fr_literal), 2, exp, 0, BFD_RELOC_16);
      exp=0;
    }
}

/* This is the main entry point for the machine-dependent assembler.
   STR points to a machine-dependent instruction.  This function is
   supposed to emit the frags/bytes it assembles to.
*/

void
md_assemble (char *str)
{
  struct dcpu16_opcode *opcode;
  struct dcpu16_operand operand[2] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

  char *p;
  int  nlen = 0;
  char *save_input_line_pointer = input_line_pointer;
  char *frag;
  int op = 0;
  int oplen = 2;

  /* Find the opcode end.  */
  for (p = str; *p && *p != ' '; p++, nlen++);

  if (nlen == 0)
    return;

  /* Find the first opcode with the proper name.  */
  opcode = (struct dcpu16_opcode *) hash_find_n (dcpu16_opcode_hash, str, nlen);
  if (opcode == NULL)
    {
      as_bad (_("unknown opcode: %s"), str);
      return;
    }

  frag = frag_more (oplen);

  if (*p == ' ')
    {
      if (opcode->args == 0)
	{
	  as_bad (_("too many operands"));
	  return;
	}

      input_line_pointer = p+1;
      parse_operand (&operand[0]);

      if (*input_line_pointer == ',')
	{
	  if (opcode->args == 1)
	    {
	      as_bad (_("too many operands"));
	      return;
	    }

	  input_line_pointer++;
	  parse_operand (&operand[1]);
	}
    }

  if (*input_line_pointer)
    {
      as_bad (_("junk at end of line"));
    }

  if (operand[0].is_long) oplen+=2;
  if (operand[1].is_long) oplen+=2;

  op = opcode->op;
  if (opcode->args == 1)
    {
      op |= (operand[0].value << 10);
    }
  else if (opcode->args == 2)
    {
      op |= (operand[0].value << 4);
      op |= (operand[1].value << 10);
    }
  
  md_number_to_chars (frag, op, 2);

  input_line_pointer = save_input_line_pointer;
}

void
md_apply_fix (fixS *fixP, valueT *valP, segT seg ATTRIBUTE_UNUSED)
{
  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;
  valueT val = * valP;

  switch (fixP->fx_r_type)
    {
    default:
      as_fatal ("Bad relocation type: 0x%02x", fixP->fx_r_type);
      return;
    case BFD_RELOC_16:
      bfd_put_16 (stdoutput, val, buf);
      *valP = val & 0xFFFF;
      break;
    }

  if (fixP->fx_addsy == NULL && fixP->fx_pcrel == 0)
    fixP->fx_done = 1;
}

/* The target specific pseudo-ops which we support.  */
const pseudo_typeS md_pseudo_table[] =
{
  { NULL, NULL, 0 }
};

arelent *
tc_gen_reloc (asection *seg ATTRIBUTE_UNUSED , fixS *fixp)
{
  arelent *reloc;

  if (! bfd_reloc_type_lookup (stdoutput, fixp->fx_r_type))
    {
      as_bad_where (fixp->fx_file, fixp->fx_line,
		    _("reloc %d not supported by object file format"),
		    (int) fixp->fx_r_type);
      return NULL;
    }

  reloc               = xmalloc (sizeof (arelent));
  reloc->sym_ptr_ptr  = xmalloc (sizeof (asymbol *));
  *reloc->sym_ptr_ptr = symbol_get_bfdsym (fixp->fx_addsy);
  reloc->address      = fixp->fx_frag->fr_address + fixp->fx_where;
  reloc->address /= OCTETS_PER_BYTE;
  reloc->howto        = bfd_reloc_type_lookup (stdoutput, fixp->fx_r_type);
  reloc->addend       = fixp->fx_offset;

  return reloc;
}
