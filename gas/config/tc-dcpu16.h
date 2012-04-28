/* tc-dcpu16.h -- Header file for tc-dcpu16.c.
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
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#ifndef _TC_DCPU16_H_
#define _TC_DCPU16_H_

#define TC_DCPU16

/* The target BFD architecture.  */
#define TARGET_ARCH              bfd_arch_dcpu16

#define TARGET_BYTES_BIG_ENDIAN  0
#define OCTETS_PER_BYTE_POWER 1

#define md_operand(x)
#define md_convert_frag(b,s,f)   as_fatal ("convert_frag called\n")
#define md_estimate_size_before_relax(f,s) \
  (as_fatal (_("estimate_size_before_relax called")), 1)

# define md_register_arithmetic 0

#define LABELS_START_WITH_COLON 1
#define TC_START_LABEL_DELIMITER(c) (c == ':')

/* Permit temporary numeric labels.  */
#define LOCAL_LABELS_FB 1

/* .-foo gets turned into PC relative relocs.  */
#define DIFF_EXPR_OK

/* We don't need to handle .word strangely.  */
#define WORKING_DOT_WORD

#define md_number_to_chars           number_to_chars_littleendian

/* No shared lib support, so we don't need to ensure externally
   visible symbols can be overridden.  */
#define EXTERN_FORCE_RELOC 0

#define LISTING_WORD_SIZE 2

#endif
