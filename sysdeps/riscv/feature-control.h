/* RISC-V feature tuning.
   This file is part of the GNU C Library.
   Copyright (C) 2026 Free Software Foundation, Inc.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#ifndef _RISCV_FEATURE_CONTROL_H
#define _RISCV_FEATURE_CONTROL_H

/* For each CFI feature, LP and SS, valid control values.  */
enum dl_riscv_cfi_control
{
  /* Enable CFI features based on ELF property note.  */
  cfi_elf_property = 0,
  /* Always enable CFI features.  */
  cfi_always_on,
  /* Always disable CFI features.  */
  cfi_always_off,
  /* Enable CFI features permissively.  */
  cfi_permissive
};

struct dl_riscv_feature_control
{
  enum dl_riscv_cfi_control lp : 2;
  enum dl_riscv_cfi_control ss : 2;
};

#endif /* feature-control.h */

