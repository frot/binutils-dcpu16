/* tc-dcpu16.c -- Assembler code for the Mojang DCPU16
   Copyright 2012 Fredrik Rothamel

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
  struct dcpu16_operand *operand;
  struct dcpu16_register *reg;

  /* Insert opcode names into a hash table.  */
  dcpu16_opcode_hash = hash_new ();

  for (opcode = (struct dcpu16_opcode *) dcpu16_opcode_table; opcode->name; opcode++)
    {
      hash_insert (dcpu16_opcode_hash, opcode->name, (char *) opcode);
    }

  /* Insert known operand strings into a hash table.  */
  dcpu16_operand_hash = hash_new ();

  for (operand = (struct dcpu16_operand *) dcpu16_operand_table; operand->name; operand++)
    {
      hash_insert (dcpu16_operand_hash, operand->name, (char *) operand);
    }

  /* insert register names and reserved words in the symbol table */
  for (reg = (struct dcpu16_register *) dcpu16_register_table; reg->name; reg++)
    {
      symbol_table_insert (symbol_create (reg->name,
					  reg_section,
					  reg->index,
					  &zero_address_frag));
    }

  symbol_print_statistics (stderr);
}

#define NAME_BUF_LEN 10

static void
parse_operand (int pos, struct dcpu16_operand *operand)
{
  expressionS expS;
  expressionS *expP = &expS;

  expressionS *tmp_exp1 = 0;
  expressionS *tmp_exp2 = 0;

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
	  if(op->op_pos && op->op_pos != pos)
	    {
	      as_bad (_("illegal operand"));
	      return;
	    }
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
    }

  expression (expP);

  if (expP->X_op == O_illegal)
    {
      as_bad (_("illegal operand"));
      return;
    }
  else if (expP->X_op == O_absent)
    {
      as_bad (_("missing operand"));
      return;
    }
  else if (expP->X_op == O_add)
    {
      tmp_exp1 = symbol_get_value_expression (expP->X_add_symbol);
      tmp_exp2 = symbol_get_value_expression (expP->X_op_symbol);

      if (tmp_exp1->X_op == O_register 
	  && (tmp_exp2->X_op == O_constant || tmp_exp2->X_op == O_symbol))
	{
	  regnum = tmp_exp1->X_add_number;
	  expP = tmp_exp2;
	}
      else if (tmp_exp2->X_op == O_register 
	  && (tmp_exp1->X_op == O_constant || tmp_exp1->X_op == O_symbol))
	{
	  regnum = tmp_exp2->X_add_number;
	  expP = tmp_exp1;
	}
      else 
	{
	  as_bad (_("illegal operand"));
	}
    }

  if (indirect)
    {
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
      if (regnum == 0x18)
	{
	  operand->value = 0x18;
	  operand->is_long = 1;
	  operand->long_value = expP->X_add_number;
	  frag = frag_more (2);
	  md_number_to_chars (frag, operand->long_value, 2);
	}
      else if (regnum >= 0)
	{
	  operand->value = 0x10 | regnum;
	  operand->is_long = 1;
	  operand->long_value = expP->X_add_number;
	  frag = frag_more (2);
	  md_number_to_chars (frag, operand->long_value, 2);
	}
      else
	{
	  operand->value = 0x1e;
	  operand->is_long = 1;
	  operand->long_value = expP->X_add_number;
	  frag = frag_more (2);
	  md_number_to_chars (frag, operand->long_value, 2);
	}
    }
  else
    {
      if (pos == 2 && expP->X_op == O_constant && expP->X_add_number < 0x1f)
	{
	  operand->value = 0x21 + expP->X_add_number;
	}
      else
	{
	  operand->value = 0x1f;
	  operand->is_long = 1;
	  operand->long_value = expP->X_add_number;
	  frag = frag_more (2);
	  md_number_to_chars (frag, operand->long_value, 2);
	}
    }
  
  if (frag && expP->X_op != O_constant)
    {
      fix_new_exp (frag_now, (frag - frag_now->fr_literal), 2, expP, 0, BFD_RELOC_16);
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
  struct dcpu16_operand operand[2] = { { 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0 } };

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
      parse_operand (1, &operand[0]);

      if (*input_line_pointer == ',')
	{
	  if (opcode->args == 1)
	    {
	      as_bad (_("too many operands"));
	      return;
	    }

	  input_line_pointer++;
	  parse_operand (2, &operand[1]);
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
      op |= (operand[0].value << 5);
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
