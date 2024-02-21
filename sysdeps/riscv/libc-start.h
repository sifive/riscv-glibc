/* RISC-V libc main startup.
   Copyright (C) 2025 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

#ifndef SHARED
# define ARCH_SETUP_IREL() apply_irel ()
# define ARCH_APPLY_IREL()

# if defined(__riscv_landing_pad) || defined(__riscv_shadow_stack)
/* Get shadow stack features enabled in the static executable.  */
#  include <asm/prctl.h>
#  include <sys/prctl.h>
extern void _dl_cfi_setup_features (unsigned int);

static inline unsigned int
get_cfi_feature (void)
{
  unsigned int cfi_feature = 0;
  /* FIXME: check if cfi feature is supported by CPU  */
  struct link_map *main_map = _dl_get_dl_main_map ();

  /* Scan program headers backward to check PT_GNU_PROPERTY early for
     feature bits on static executable.  */
  const ElfW(Phdr) *phdr = GL(dl_phdr);
  const ElfW(Phdr) *ph;
  for (ph = phdr + GL(dl_phnum); ph != phdr; ph--)
    if (ph[-1].p_type == PT_GNU_PROPERTY)
      {
        _dl_process_pt_gnu_property (main_map, -1, &ph[-1]);
        /* Enable landing pad and shstk only if they are enabled on a static
           executable.  */
        /* FIXME: change to &= to mask off other features after cpu_feature
           is implemented  */
        cfi_feature = (main_map->l_riscv_feature_1_and
                       & (GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED
                          | GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS));

        GL(dl_riscv_feature_1) = cfi_feature;
        return cfi_feature;
      }
  GL(dl_riscv_feature_1) = 0;
  return 0;
}

/* The function using this macro to enable shadow stack must not return
   to avoid shadow stack underflow.  */
#  ifdef __riscv_shadow_stack
#   define ENABLE_RISCV_SHADOW_STACK                                    \
  do                                                                    \
    {                                                                   \
      if (feature & GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS)                \
        {                                                               \
          INTERNAL_SYSCALL_CALL (prctl, PR_SET_SHADOW_STACK_STATUS,     \
                                 PR_SHADOW_STACK_ENABLE, 0, 0, 0);      \
        }                                                               \
    }                                                                   \
  while (0)
#  else
#   define ENABLE_RISCV_SHADOW_STACK
#  endif

#  define ARCH_SETUP_TLS()                                              \
  {                                                                     \
    __libc_setup_tls ();                                                \
                                                                        \
    unsigned int feature = get_cfi_feature ();                          \
    ENABLE_RISCV_SHADOW_STACK;                                          \
    /* Landing pad will be enabled in _dl_cfi_setup_features  */        \
    _dl_cfi_setup_features(feature);                                    \
  }
# else
#  define ARCH_SETUP_TLS()      __libc_setup_tls ()
# endif /* __riscv_landing_pad || __riscv_shadow_stack */
#endif /* !SHARED */
