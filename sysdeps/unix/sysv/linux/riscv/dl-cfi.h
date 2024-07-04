/* Linux/RISC-V CFI initializers function.
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

/* FIXME: Should be remove after they are included in the kernel header  */
#include <asm/prctl.h>
#include <sys/prctl.h>
#include <features-offsets.h>

#ifdef __riscv_shadow_stack
# define CHECK_AND_ENABLE_SHADOW_STACK \
"\
    andi  a0, s1, " STRINGXP (GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS) "\n\
    beqz  a0, 1f \n\
    li    a0, " STRINGXP (PR_SET_SHADOW_STACK_STATUS) "\n\
    li    a1, " STRINGXP (PR_SHADOW_STACK_ENABLE) "\n\
    li    a2, 0 \n\
    li    a3, 0 \n\
    li    a4, 0 \n\
    li    a7, " STRINGXP (__NR_prctl) "\n\
    ecall \n\
1: \n\
"
#else
# define CHECK_AND_ENABLE_SHADOW_STACK
#endif

#define RTLD_START_ENABLE_RISCV_CFI \
"\
    lw   s1, _rtld_local + " STRINGXP (RTLD_GLOBAL_DL_RISCV_FEATURE_1_OFFSET) " \n\
    # We need to enable shadow stack in the assembly code to avoid underflow \n\
    # Checking for landing pad is left to _dl_cfi_setup_features \n\
    " CHECK_AND_ENABLE_SHADOW_STACK "\n\
    mv   a0, s1 \n\
    jal  _dl_cfi_setup_features \n\
    \n\
"

static __always_inline int
dl_cfi_disable_cfi (unsigned int feature) {
  int res = 0;
#ifdef __riscv_landing_pad
  if (feature & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED)
    {
      res = prctl (PR_SET_CFI, PR_CFI_BRANCH_LANDING_PADS, PR_CFI_DISABLE, 0, 0);
      if (res)
        return res;
    }
#endif /* __riscv_landing_pad  */
#ifdef __riscv_shadow_stack
  if (feature & GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS)
    {
      res |= prctl (PR_SET_SHADOW_STACK_STATUS, 0, 0, 0, 0);
      if (res)
        return res;
    }
#endif /* __riscv_shadow_stack  */
  return 0;
}

static __always_inline int
dl_cfi_lock_cfi (unsigned int feature)
{
  int res = 0;
#ifdef __riscv_landing_pad
  if (feature & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED)
    res |= prctl (PR_SET_CFI, PR_CFI_BRANCH_LANDING_PADS,
                  PR_CFI_ENABLE | PR_CFI_LOCK, 0, 0);
#endif /* __riscv_landing_pad  */
#ifdef __riscv_shadow_stack
  if (feature & GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS)
    res |= prctl (PR_LOCK_SHADOW_STACK_STATUS, 0, 0, 0, 0);
#endif /* __riscv_shadow_stack  */
  return res;
}

static __always_inline int
dl_cfi_get_cfi_status (void) {
  int status = 0;
  unsigned long buf = 0;
  int ret = 0;
#ifdef __riscv_landing_pad
    ret = prctl (PR_GET_CFI, PR_CFI_BRANCH_LANDING_PADS, &buf, 0, 0);
    if (!ret && buf)
      status |= GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED;
#endif /* __riscv_landing_pad  */
#ifdef __riscv_shadow_stack
    ret = prctl (PR_GET_SHADOW_STACK_STATUS, &buf, 0, 0, 0);
    if (!ret && buf)
      status |= GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS;
#endif /* __riscv_shadow_stack  */
  return status;
}

#ifdef __riscv_landing_pad
static __always_inline int
dl_cfi_enable_lp (unsigned int feature) {
  if (!(feature & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED))
    return -1;
  return INTERNAL_SYSCALL_CALL (prctl, PR_SET_CFI, PR_CFI_BRANCH_LANDING_PADS,
                                PR_CFI_ENABLE, 0, 0);
}
#endif /* __riscv_landing_pad  */
