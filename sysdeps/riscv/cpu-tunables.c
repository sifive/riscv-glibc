/* RISC-V CPU feature tuning.
   This file is part of the GNU C Library.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

#define TUNABLE_NAMESPACE cpu
#include <elf/dl-tunables.h>
#include <dl-tunables-parse.h>
#include <ldsodefs.h>

#ifdef __riscv_landing_pad
attribute_hidden
void
TUNABLE_CALLBACK (set_riscv_cfi_lp) (tunable_val_t *valp)
{
  if (tunable_strcmp_cte (valp, "permissive"))
    GL(dl_riscv_feature_control).lp = cfi_permissive;
  else if (tunable_strcmp_cte (valp, "off"))
    GL(dl_riscv_feature_control).lp = cfi_always_off;
  else
    GL(dl_riscv_feature_control).lp = cfi_always_on;
}
#endif

#ifdef __riscv_shadow_stack
attribute_hidden
void
TUNABLE_CALLBACK (set_riscv_cfi_ss) (tunable_val_t *valp)
{
  if (tunable_strcmp_cte (valp, "permissive"))
    GL(dl_riscv_feature_control).ss = cfi_permissive;
  else if (tunable_strcmp_cte (valp, "off"))
    GL(dl_riscv_feature_control).ss = cfi_always_off;
  else
    GL(dl_riscv_feature_control).ss = cfi_always_on;
}
#endif
