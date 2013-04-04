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

#define OPTION_LONG (OPTION_MD_BASE + 0)

struct option md_longopts[] =
{
  {"mlong", no_argument, NULL, OPTION_LONG},
  {NULL, no_argument, NULL, 0}
};

size_t md_longopts_size = sizeof (md_longopts);

int option_long = 0;

/* Opcode hash table.  */
static struct hash_control *dcpu16_opcode_hash;

void
md_show_usage (FILE *stream ATTRIBUTE_UNUSED)
{
}

int
md_parse_option (int c, char *arg ATTRIBUTE_UNUSED)
{
  switch(c) {
  case OPTION_LONG:
    option_long = 1;
    break;
  default:
    return 0;
  }
  return 1;
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
  expressionS *expP = &operand->exp;

  char c;
  symbolS *symbolP;
  char *name;

  int indirect = 0;
  int pre_decrement = 0;
  int post_increment = 0;
  const struct dcpu16_register *reg = 0;

  /* check for indirect operand start */
  if (*input_line_pointer == '[')
    {
      indirect=1;
      input_line_pointer++;
    }

  /* check for -- op */
  if (*input_line_pointer == '-'
      && *(input_line_pointer+1) == '-')
    {
      pre_decrement = 1;
      input_line_pointer += 2;
    }

  /* Since I can't seem to persuade expr to handle register expressions
     the way I would like. Lets cheat for now.
     Assume registers always are the first symbol and handle them here
     before calling expr.
  */

  SKIP_WHITESPACE ();
  if (is_name_beginner (*input_line_pointer))
    {
      name = input_line_pointer;
      c = get_symbol_end ();
      symbolP = symbol_find (name);
      *input_line_pointer = c;

      if (symbolP && S_GET_SEGMENT (symbolP) == reg_section)
	{
	  reg = &dcpu16_register_table[S_GET_VALUE (symbolP)];

	  /* check for ++ op */
	  SKIP_WHITESPACE ();
	  if (*input_line_pointer == '+'
	      && *(input_line_pointer+1) == '+')
	    {
	      if (pre_decrement)
		{
		  as_bad (_("-- and ++ not allowed in the same expression"));
		  return;
		}

	      post_increment = 1;
	      input_line_pointer += 2;
	    }
	}
      else
	{
	  input_line_pointer = name;
	}
    }

  if ((pre_decrement || post_increment)
      && (!indirect || !reg || reg->opcode != REG_OP_SP))
    {
      as_bad (_("++ or -- only allowed with [SP]"));
      return;
    }

  SKIP_WHITESPACE ();
  if(*input_line_pointer != ',' && *input_line_pointer != ']')
    {
      expression (expP);

      if (expP->X_op == O_illegal)
	{
	  as_bad (_("illegal expression"));
	  return;
	}

      if (expP->X_op == O_absent 
	  || (expP->X_op == O_constant && expP->X_add_number == 0))
	{
	  /* discard zero offset */
	  expP = 0;
	}
    }
  else
    {
      expP = 0;
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
      operand->value = reg->opcode;

      if (indirect)
	{
	  if (reg->opcode <= REG_OP_J)
	    {
	      /* [reg] */
	      operand->value = 0x08 + reg->opcode;

	      if (expP)
		{
		  /* [reg+offset] */
		  operand->value = 0x10 + reg->opcode;
		  operand->is_long = 1;
		}
	    }
	  else if (reg->opcode == REG_OP_SP)
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
		}
	      else
		{
		  /* [SP] */
		  operand->value = 0x19;
		}
	    }
	}
      else if (reg->opcode == REG_OP_PICK)
	{
	  /* PICK n */

	  if (expP)
	    {
	      operand->value = 0x1a;
	      operand->is_long = 1;
	    }
	  else
	    {
	      /* zero offset */
	      operand->value = 0x19;
	    }
	}
    }
  else if (expP)
    {
      /* no register operand */

      /* sign-extend 16-bit quantity */
      if (expP->X_add_number & 0x8000)
        {
          expP->X_add_number = (expP->X_add_number&65535) - 65536;
        }

      if (!indirect && pos == 2 
	  && expP->X_op == O_constant && expP->X_add_number < 0x1f
	  && expP->X_add_number >= -1)
	{
	  /* short literal */
	  operand->value = 0x21 + expP->X_add_number;
	}
      else
	{
	  /* long literal */
	  operand->value = indirect ? 0x1e : 0x1f;
	  operand->is_long = 1;
	}
    }
  else
    {
	operand->value = 0x21;
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
  struct dcpu16_operand operand[2];

  char *p;
  int  nlen = 0;
  char *save_input_line_pointer = input_line_pointer;
  char *frag;
  int op = 0;
  int oplen = 2;

  memset(operand, 0, sizeof(operand));

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

  /* Operand 'a' should be handled first */
  if (operand[1].is_long)
    {
      frag = frag_more (2);
      md_number_to_chars (frag, operand[1].exp.X_add_number, 2);

      if (operand[1].exp.X_op != O_constant)
	{
	  fix_new_exp (frag_now, (frag - frag_now->fr_literal), 2, &operand[1].exp, 0, BFD_RELOC_16);
	}
    }

  /* then operand 'b' */
  if (operand[0].is_long)
    {
      frag = frag_more (2);
      md_number_to_chars (frag, operand[0].exp.X_add_number, 2);

      if (operand[0].exp.X_op != O_constant)
	{
	  fix_new_exp (frag_now, (frag - frag_now->fr_literal), 2, &operand[0].exp, 0, BFD_RELOC_16);
	}
    }

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
    case BFD_RELOC_32:
      bfd_put_32 (stdoutput, val, buf);
      *valP = val & 0xFFFFFFFF;
      break;
    case BFD_RELOC_64:
      bfd_put_64 (stdoutput, val, buf);
      //*valP = val & 0xFFFFFFFF;
      break;
    }

  if (fixP->fx_addsy == NULL && fixP->fx_pcrel == 0)
    fixP->fx_done = 1;
}

/* The target specific pseudo-ops which we support.  */
const pseudo_typeS md_pseudo_table[] =
{
  { "byte", cons, 2},
  { "long", cons_long, 4},
  { NULL, NULL, 0 }
};

void
cons_long (int size)
{
    if(option_long)
	cons (2*size);
    else
	cons (size);
}

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
