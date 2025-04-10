/* RISC-V CFI extensions (zicfilp/zicfiss) functions.
   Copyright (C) 2024 Free Software Foundation, Inc.

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

#include "feature-control.h"
#include <asm-generic/errno-base.h>
#include <unistd.h>
#include <libintl.h>
#include <ldsodefs.h>
#include <dl-cfi.h>
#include <sys/mman.h>

struct dl_cfi_info
{
  const char *program;

  /* Check how lp and ss should be enabled.  */
#ifdef __riscv_zicfilp
  enum dl_riscv_cfi_control enable_lp_type;
#endif
#ifdef __riscv_zicfiss
  enum dl_riscv_cfi_control enable_ss_type;
#endif

  /* Previously enabled features.  */
  unsigned int feature_1_enabled;

  /* Featuresshould be enabled.  */
  unsigned int enable_feature_1;

  /* If there are any legacy shared object.  */
  unsigned int feature_1_legacy;

  /* Which shared object is the first legacy shared object.  */
#ifdef __riscv_zicfilp
  unsigned int feature_1_legacy_lp;
#endif
#ifdef __riscv_zicfiss
  unsigned int feature_1_legacy_ss;
#endif
};


static void
dl_check_legacy_object (struct link_map *m, struct dl_cfi_info *info)
{
  /* Iterate through the dependencies and record legacy objects  */
  struct link_map *l = NULL;
  unsigned int i;
  i = m->l_searchlist.r_nlist;
  while (i-- > 0)
    {
      /* Check each shared object to see if shadow stack and landing pad
         are enabled.  */
      l = m->l_initfini[i];

      if (l->l_init_called)
        continue;

#ifdef SHARED
      /* Skip check for ld.so since it has the features enabled. The
         features will be disabled later if they are not enabled in
	 executable.  */
      if (l == &GL(dl_rtld_map)
          || l->l_real == &GL(dl_rtld_map)
          || (info->program != NULL && l == m))
        continue;
#endif /* SHARED */

      info->enable_feature_1 &= ((l->l_riscv_feature_1_and
                                  & (GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED
				     | GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_FUNC_SIG
                                     | GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS))
                                  | ~(GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED
                                     | GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS
				     | GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_FUNC_SIG));

      /* Bookkeeping first found mismatch object for both lp/ss.
         These information would only be used by dlopen check for now.
         A dependency with a feature on will be record as legacy if the task
         did not enable the feature, however it is safe because the following
         check will only be performed if the task has the feature on.  */
#ifdef __riscv_zicfilp
      if ((info->feature_1_legacy & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED) == 0
          && ((info->enable_feature_1 & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED)
              != (info->feature_1_enabled & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED))
         )
        {
          info->feature_1_legacy_lp = i;
          info->feature_1_legacy |= GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED;
        }
      if ((info->feature_1_legacy & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_FUNC_SIG) == 0
          && ((info->enable_feature_1 & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_FUNC_SIG)
              != (info->feature_1_enabled & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_FUNC_SIG))
         )
        {
          info->feature_1_legacy_lp = i;
          info->feature_1_legacy |= GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED;
        }

#endif
#ifdef __riscv_zicfiss
      if ((info->feature_1_legacy & GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS) == 0
          && ((info->enable_feature_1 & GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS)
              != (info->feature_1_enabled & GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS))
         )
        {
          info->feature_1_legacy_ss = i;
          info->feature_1_legacy |= GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS;
        }
#endif
    }

  /* Keep bits set if cfi_always_on */
#ifdef __riscv_zicfilp
  if ((info->feature_1_enabled & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED) != 0
      && info->enable_lp_type == cfi_always_on)
    {
      info->enable_feature_1 |= GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED;
    }
  if ((info->feature_1_enabled & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_FUNC_SIG) != 0
      && info->enable_lp_type == cfi_always_on)
    {
      info->enable_feature_1 |= GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_FUNC_SIG;
    }
#endif
#ifdef __riscv_zicfiss
  if ((info->feature_1_enabled & GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS) != 0
      && info->enable_ss_type == cfi_always_on)
    {
      info->enable_feature_1 |= GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS;
    }
#endif
}

#ifdef SHARED
static void
dl_cfi_check_startup (struct link_map *m, struct dl_cfi_info *info)
{
#ifdef __riscv_zicfilp
  if (info->enable_lp_type == cfi_always_on)
    info->enable_feature_1 |= GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED;
  else if (info->enable_lp_type == cfi_always_off)
    info->enable_feature_1 &= ~GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED;
  else
    info->enable_feature_1 &= ((m->l_riscv_feature_1_and
                                & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED)
                               | ~GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED);
  // TODO: func sig
# endif
# ifdef __riscv_zicfiss
  if (info->enable_ss_type == cfi_always_on)
    info->enable_feature_1 |= GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS;
  else if (info->enable_ss_type == cfi_always_off)
    info->enable_feature_1 &= ~GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS;
  else
    info->enable_feature_1 &= ((m->l_riscv_feature_1_and
                                & GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS)
                               | ~GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS);
# endif

  if (info->enable_feature_1 != 0)
    dl_check_legacy_object (m, info);

  /* Update GL(dl_riscv_feature_1)  */
  if (info->enable_feature_1 ^ info->feature_1_enabled) {
    info->feature_1_enabled = info->enable_feature_1;
    GL(dl_riscv_feature_1) = info->enable_feature_1;
  }
}
#endif /* SHARED */

static void
dl_cfi_check_dlopen (struct link_map *m, struct dl_cfi_info *info)
{
  if (info->enable_feature_1 != 0) {
    dl_check_legacy_object(m, info);

    if (info->feature_1_legacy == 0)
      return;
  }

  unsigned int disable_feature_1 = 0;
  unsigned int legacy_obj = 0;
  const char *msg = NULL;

#ifdef __riscv_zicfilp
  if ((info->feature_1_enabled & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED) != 0
      && (info->feature_1_legacy & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED) != 0)
    {
      if (info->enable_lp_type != cfi_permissive || !SINGLE_THREAD_P)
        {
          legacy_obj = info->feature_1_legacy_lp;
          msg = N_("rebuild shared object with landing pad support");
        }
      else
        disable_feature_1 |= GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED;
    }
  if ((info->feature_1_enabled & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_FUNC_SIG) != 0
      && (info->feature_1_legacy & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_FUNC_SIG) != 0)
    {
      if (info->enable_lp_type != cfi_permissive || !SINGLE_THREAD_P)
        {
          legacy_obj = info->feature_1_legacy_lp;
          msg = N_("rebuild shared object with landing pad support");
        }
      else
        disable_feature_1 |= GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_FUNC_SIG;
    }
#endif

#ifdef __riscv_zicfiss
  if ((info->feature_1_enabled & GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS) != 0
      && (info->feature_1_legacy & GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS) != 0)
    {
      if (info->enable_ss_type != cfi_permissive || !SINGLE_THREAD_P)
        {
          legacy_obj = info->feature_1_legacy_ss;
          msg = N_("rebuild shared object with shadow stack support");
        }
      else
        disable_feature_1 |= GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS;
    }
#endif

  if (msg != NULL)
    _dl_signal_error (0, m->l_initfini[legacy_obj]->l_name, "dlopen", msg);

  if (disable_feature_1 != 0)
    {
      int res = dl_cfi_disable_cfi (disable_feature_1);
      if (res)
        {
          msg = N_("can't disable CFI feature");
          _dl_signal_error (-res, m->l_initfini[legacy_obj]->l_name,
                            "dlopen", msg);
        }
      GL(dl_riscv_feature_1) &= ~disable_feature_1;
    }
}

attribute_hidden void
_dl_cfi_setup_features (unsigned int feature_1)
{
  /* Enable features. Shadow stack is enabled earlier as it should
   * be enabled in a function that never returns.  */
#ifdef __riscv_zicfilp
  dl_cfi_enable_lp (feature_1);
#endif /* __riscv_zicfilp  */

  /* Since we could failed to enable some features,
     get enabled features from system again and sync it back.  */
  int status = dl_cfi_get_cfi_status ();
  GL(dl_riscv_feature_1) = status | (GL(dl_riscv_feature_1) &
                           ~(GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS
			     | GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_FUNC_SIG
                             | GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED));
  
  /* Lock features if set to always_on  */
#ifdef __riscv_zicfilp
  if (GL(dl_riscv_feature_control).lp == cfi_always_on)
      dl_cfi_lock_cfi (GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED);
  // TODO lock func sig
#endif /* __riscv_zicfilp  */
#ifdef __riscv_zicfiss
  if (GL(dl_riscv_feature_control).ss == cfi_always_on)
      dl_cfi_lock_cfi (GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS);
#endif /* __riscv_zicfiss  */
  /* FIXME: Should we terminate if failed to lock under always on mode? */
}

/* Enable CFI for l and its dependencies.  */
void
_dl_cfi_check (struct link_map *l, const char *program)
{
    /* As this point we have parsed the gnu properties.
       For dynamic binary we should verify the dependencies here.  */
  struct dl_cfi_info info;
#if defined SHARED && defined RTLD_START_ENABLE_RISCV_CFI
  if (program)
    {
      GL(dl_riscv_feature_1) = l->l_riscv_feature_1_and;
    }
#endif /* SHARED  */

  unsigned int supported_exts = 0;
  unsigned int always_on_exts = 0;

#ifdef __riscv_zicfilp
  info.enable_lp_type = GL(dl_riscv_feature_control).lp;
  supported_exts += 1;
  always_on_exts += (info.enable_lp_type == cfi_always_on);
#endif
#ifdef __riscv_zicfiss
  info.enable_ss_type = GL(dl_riscv_feature_control).ss;
  supported_exts += 1;
  always_on_exts += (info.enable_ss_type == cfi_always_on);
#endif

  info.feature_1_enabled = GL(dl_riscv_feature_1);

  /* No legacy check needed if all cfi exts are always on in main  */
  if (program && (supported_exts == always_on_exts))
    return;

  /* No legacy check needed if all cfi exts are off  */
  if (info.feature_1_enabled == 0)
    return;

  info.program = program;

  info.enable_feature_1 = 0;
#ifdef __riscv_zicfilp
  if (info.enable_lp_type != cfi_always_off)
    info.enable_feature_1 |= (info.feature_1_enabled
            & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED);
  info.feature_1_legacy_lp = 0;
  // TODO: func sig
#endif
#ifdef __riscv_zicfiss
  if (info.enable_ss_type != cfi_always_off)
    info.enable_feature_1 |= (info.feature_1_enabled
            & GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS);
  info.feature_1_legacy_ss = 0;
#endif

  info.feature_1_legacy = 0;

#ifdef SHARED
  if (program)
    dl_cfi_check_startup (l, &info);
  else
#endif /* SHARED  */
    dl_cfi_check_dlopen (l, &info);
}
