/* Support for GNU properties.  RISC-V version.
   Copyright (C) 2024 Free Software Foundation, Inc.
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

#ifndef _DL_PROP_H
#define _DL_PROP_H

extern void _dl_cfi_check (struct link_map *, const char *)
    attribute_hidden;

static inline void __attribute__ ((always_inline))
_rtld_main_check (struct link_map *m, const char *program)
{
#if defined(__riscv_zicfilp) || defined(__riscv_zicfiss)
  _dl_cfi_check(m, program);
#endif /* __riscv_zicfilp || __riscv_zicfiss */
}

static inline void __attribute__ ((always_inline))
_dl_open_check (struct link_map *m)
{
#if defined(__riscv_zicfilp) || defined(__riscv_zicfiss)
  _dl_cfi_check(m, NULL);
#endif /* __riscv_zicfilp || __riscv_zicfiss */
}

static inline void __attribute__ ((always_inline))
_dl_process_pt_note (struct link_map *l, int fd, const ElfW(Phdr) *ph)
{
}

static inline int
_dl_process_gnu_property (struct link_map *l, int fd, uint32_t type,
			  uint32_t datasz, void *data)
{
  /* FIXME: Detect cpu features after we have it implemented in glibc  */

  if (type == GNU_PROPERTY_RISCV_FEATURE_1_AND)
    {
      /* Stop if the property note is ill-formed.  */
      if (datasz != 4)
        return -1;

#if defined(__riscv_zicfilp) || defined(__riscv_zicfiss)
      unsigned int feature_1 = *(unsigned int *) data;
#endif
#ifdef __riscv_zicfilp
      if (feature_1 & GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED)
        l->l_riscv_feature_1_and |= GNU_PROPERTY_RISCV_FEATURE_1_CFI_LP_UNLABELED;
#endif
#ifdef __riscv_zicfiss
      if (feature_1 & GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS)
        l->l_riscv_feature_1_and |= GNU_PROPERTY_RISCV_FEATURE_1_CFI_SS;
#endif
    }
  /* Continue.  */
  return 1;
}

#endif /* _DL_PROP_H */
