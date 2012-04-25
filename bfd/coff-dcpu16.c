/* BFD back-end for Mojang DCPU-16 COFF binaries.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify 
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "bfdlink.h"
#include "coff/dcpu16.h"
#include "coff/internal.h"
#include "libcoff.h"

static void reloc_processing
  PARAMS ((arelent *, struct internal_reloc *, asymbol **, bfd *, asection *));

static reloc_howto_type r_imm16 =
    HOWTO (BFD_RELOC_16,0,1,16,FALSE,0,complain_overflow_dont,
	   NULL,"r_imm16",FALSE,0xFFFF,0xFFFF,FALSE);

/* Code to turn a r_type into a howto ptr, uses the above howto table.  */

static void
rtype2howto (arelent *internal, struct internal_reloc *dst)
{
  switch (dst->r_type)
    {
    default:
      abort ();
      break;
    case BFD_RELOC_16:
      internal->howto = &r_imm16;
      break;
    }
}

#define RTYPE2HOWTO(internal, relocentry) rtype2howto (internal, relocentry)

static reloc_howto_type *
coff_dcpu16_reloc_type_lookup (bfd *abfd ATTRIBUTE_UNUSED,
			    bfd_reloc_code_real_type code)
{
  switch (code)
    {
    case BFD_RELOC_16:
      return & r_imm16;
    default:
      BFD_FAIL ();
      return NULL;
    }
}

static reloc_howto_type *
coff_dcpu16_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
			    const char *r_name)
{
  if (strcasecmp (r_imm16.name, r_name) == 0)
    return &r_imm16;

  return NULL;
}

#define RELOC_PROCESSING(relent,reloc,symbols,abfd,section) \
 reloc_processing(relent, reloc, symbols, abfd, section)

#define coff_bfd_reloc_type_lookup  coff_dcpu16_reloc_type_lookup
#define coff_bfd_reloc_name_lookup coff_dcpu16_reloc_name_lookup

#ifndef bfd_pe_print_pdata
#define bfd_pe_print_pdata	NULL
#endif

#define BADMAG(x) DCPU16_BADMAG(x)

#include "coffcode.h"

static void
reloc_processing (arelent *relent,
                  struct internal_reloc *reloc,
                  asymbol **symbols,
                  bfd *abfd,
                  asection *section)
{
  asymbol *ptr;

  relent->address = reloc->r_vaddr;

  if (reloc->r_symndx != -1)
    {
      if (reloc->r_symndx < 0 || reloc->r_symndx >= obj_conv_table_size (abfd))
        {
          (*_bfd_error_handler)
            (_("%B: warning: illegal symbol index %ld in relocs"),
             abfd, reloc->r_symndx);
          relent->sym_ptr_ptr = bfd_abs_section_ptr->symbol_ptr_ptr;
          ptr = NULL;
        }
      else
        {
          relent->sym_ptr_ptr = (symbols
                                 + obj_convert (abfd)[reloc->r_symndx]);
          ptr = *(relent->sym_ptr_ptr);
        }
    }
  else
    {
      relent->sym_ptr_ptr = section->symbol_ptr_ptr;
      ptr = *(relent->sym_ptr_ptr);
    }

  /* The symbols definitions that we have read in have been
     relocated as if their sections started at 0. But the offsets
     refering to the symbols in the raw data have not been
     modified, so we have to have a negative addend to compensate.

     Note that symbols which used to be common must be left alone.  */

  /* Calculate any reloc addend by looking at the symbol.  */
  CALC_ADDEND (abfd, ptr, *reloc, relent);

  relent->address -= section->vma;
  /* !!     relent->section = (asection *) NULL;*/

  /* Fill in the relent->howto field from reloc->r_type.  */
  rtype2howto (relent, reloc);
}

CREATE_BIG_COFF_TARGET_VEC (dcpu16_coff_vec, "coff-dcpu16", 0,
			    SEC_CODE | SEC_DATA, '\0', NULL, 
			    COFF_SWAP_TABLE)
