/* RISC-V CFI extensions (zicfilp/zicfiss) functions.
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

#include <asm-generic/errno-base.h>
#include <unistd.h>
#include <libintl.h>
#include <ldsodefs.h>
#include <dl-cfi.h>
#include <sys/mman.h>

attribute_hidden void
_dl_cfi_setup_features (unsigned int feature_1)
{
  /* Since prctl could fail to enable some features
     use prctl to get enabled features again and sync it back.  */
#ifdef __riscv_landing_pad
  if (feature_1 & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED)
    INTERNAL_SYSCALL_CALL (prctl, PR_SET_INDIR_BR_LP_STATUS,
                           PR_INDIR_BR_LP_ENABLE, 0, 0, 0);
#endif /* __riscv_landing_pad  */
  /* FIXME: Read enabled features from kernel and re-sync  */
}
