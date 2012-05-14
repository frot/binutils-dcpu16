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
  int i;

  /* Insert opcode names into a hash table.  */
  dcpu16_opcode_hash = hash_new ();

  for (i=0; dcpu16_opcode_table[i].name; i++)
    {
      hash_insert (dcpu16_opcode_hash, dcpu16_opcode_table[i].name, (char *)&dcpu16_opcode_table[i]);
    }

  /* Insert register names and reserved words in the symbol table */
  for (i=0; dcpu16_register_table[i].name; i++)
    {
      symbol_table_insert (symbol_create (dcpu16_register_table[i].name,
					  reg_section,
					  i,
					  &zero_address_frag));
    }
}

/* Words are stored big-endian, multi-word values stored little-endian */
void
dcpu16_number_to_chars (char *buf, valueT val, int n)
{
  if (n==1)
    {
      number_to_chars_bigendian (buf, val, 1);
    }
  else
    {
      while (n>0)
	{
	  number_to_chars_bigendian (buf, val & 0xffff, 2);
	  buf += 2;
	  val >>= 16;
	  n -= 2;
	}
    }
}

static void
parse_operand (int pos, struct dcpu16_operand *operand)
{
  expressionS expS;
  expressionS *expP = &expS;

  expressionS *tmp_exp1 = 0;
  expressionS *tmp_exp2 = 0;

  char *p;
  int indirect = 0;
  int pre_decrement = 0;
  int post_increment = 0;
  char *frag = 0;
  const struct dcpu16_register *reg = 0;

  /* check for indirect operand start */
  if (*input_line_pointer == '[')
    {
      indirect=1;
      input_line_pointer++;

      /* check for -- op */
      if (*input_line_pointer == '-'
	  && *(input_line_pointer+1) == '-')
	{
	  pre_decrement = 1;
	  input_line_pointer += 2;
	}

      /* check for ++ op */
      for (p=input_line_pointer; *p && *p != ']'; p++);
      if (*p == ']' && *(p-1) == '+' && *(p-2) == '+')
	{
	  if (pre_decrement)
	    {
	      as_bad (_("illegal expression"));
	      return;
	    }

	  post_increment = 1;
	  *(p-2) = ']';
	  *(p-1)= ' ';
	  *p = ' ';
	}
    }

  expression (expP);

  if (expP->X_op == O_absent)
    {
      as_bad (_("missing operand"));
      return;
    }

  if (expP->X_op == O_illegal)
    {
      as_bad (_("illegal expression"));
      return;
    }

  if (!(indirect
	&& expP->X_op == O_register 
	&& expP->X_add_number == REG_INDEX_SP)
      && (pre_decrement || post_increment))
    {
      as_bad (_("++ or -- operator only allowed with SP register."));
      return;
    }

  if (expP->X_op == O_register)
    {
      /* single register operand */
      reg = &dcpu16_register_table[expP->X_add_number];
      expP = 0;
    }
  else if (expP->X_op == O_add)
    {
      /* register + offset operand */

      if (!indirect)
	{
	  as_bad (_("offset only allowed in indirect mode"));
	  return;
	}

      tmp_exp1 = symbol_get_value_expression (expP->X_add_symbol);
      tmp_exp2 = symbol_get_value_expression (expP->X_op_symbol);

      if (tmp_exp1->X_op == O_register 
	  && (tmp_exp2->X_op == O_constant || tmp_exp2->X_op == O_symbol))
	{
	  reg = &dcpu16_register_table[tmp_exp1->X_add_number];
	  expP = tmp_exp2;
	}
      else if (tmp_exp2->X_op == O_register 
	  && (tmp_exp1->X_op == O_constant || tmp_exp1->X_op == O_symbol))
	{
	  reg = &dcpu16_register_table[tmp_exp2->X_add_number];
	  expP = tmp_exp1;
	}
      else 
	{
	  as_bad (_("illegal operand"));
	  return;
	}

      if (expP->X_op == O_constant && expP->X_add_number == 0)
	{
	  /* discard zero offset */
	  expP = 0;
	}
    }
  else if (expP->X_op != O_constant && expP->X_op != O_symbol)
    {
      as_bad (_("illegal operand"));
      return;
    }

  if (indirect)
    {
      /* check for indirect operand end */
      if (*input_line_pointer == ']')
	{
	  input_line_pointer++;
	}
      else
	{
	  as_bad (_("] expected"));
	  return;
	}
    }

  if (reg)
    {
      /* we've got a register */
      operand->value = reg->index;

      if (indirect)
	{
	  if (reg->index < 0x08)
	    {
	      /* [reg] */
	      operand->value = 0x08 + reg->index;

	      if (expP)
		{
		  /* [reg+offset] */
		  operand->value = 0x10 + reg->index;
		  operand->is_long = 1;
		  operand->long_value = expP->X_add_number;
		}
	    }
	  else if (reg->index == REG_INDEX_SP)
	    {
	      /* Something with SP in it */
	      if (pre_decrement || post_increment)
		{
		  if (expP)
		    {
		      /* ++ -- not allowed with offset */
		      as_bad (_("illegal expression"));
		      return;
		    }
		  /* [--SP] or [SP++] -> same code */
		  operand->value = 0x18;
		  pre_decrement = 0;
		  post_increment = 0;
		}
	      else if (expP)
		{
		  /* [SP+offset] */
		  operand->value = 0x1a;
		  operand->is_long = 1;
		  operand->long_value = expP->X_add_number;
		}
	      else
		{
		  /* [SP] */
		  operand->value = 0x19;
		}
	    }
	}
      else if (reg->index == REG_INDEX_PICK)
	{
	  /* PICK n */
	  expP = &expS;
	  expression (expP);

	  if (expP->X_op == O_constant || expP->X_op == O_symbol)
	    {
		  operand->value = 0x1a;
		  operand->is_long = 1;
		  operand->long_value = expP->X_add_number;
	    }
	  else
	    {
	      as_bad (_("illegal expression"));
	      return;
	    }
	}
    }
  else
    {
      /* no register operand */

      if (!indirect && pos == 2 
	  && expP->X_op == O_constant && expP->X_add_number < 0x1f)
	{
	  /* short literal */
	  operand->value = 0x21 + expP->X_add_number;
	}
      else
	{
	  /* long literal */
	  operand->value = indirect ? 0x1e : 0x1f;
	  operand->is_long = 1;
	  operand->long_value = expP->X_add_number;
	}
    }

  if (operand->is_long)
    {
      frag = frag_more (2);
      md_number_to_chars (frag, operand->long_value, 2);
    }
  
  if (frag && expP && expP->X_op == O_symbol)
    {
      fix_new_exp (frag_now, (frag - frag_now->fr_literal), 2, expP, 0, BFD_RELOC_16);
    }

  /* skip trailing whitespace */
  while (*input_line_pointer && *input_line_pointer == ' ')
      input_line_pointer++;
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
