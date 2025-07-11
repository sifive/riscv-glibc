/* Helper function to allocate shadow stack.
   Copyright (C) 2023-2026 Free Software Foundation, Inc.
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

#include <sys/ucontext.h>

#ifdef __riscv_shadow_stack
/* When shadow stack is enabled, derive the storage type from ucontext.  */
typedef __typeof (((ucontext_t *) 0)->__saved.__ssp) \
        shadow_stack_size_t;
#else
/* Without shadow stack support, use an unsigned long placeholder type.  */
typedef unsigned long int shadow_stack_size_t;
#endif

extern long int __allocate_shadow_stack (size_t, shadow_stack_size_t *)
  attribute_hidden;
