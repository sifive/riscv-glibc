/* Internal header file for <setjmp.h>.  Linux/risc-v version.
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

#ifndef	_SETJMPP_H
#define	_SETJMPP_H	1

#include <bits/types/__sigset_t.h>
#include <libc-pointer-arith.h>
#include <sigsetops.h>

/* <setjmp/setjmp.h> has

   NB: We use setjmp in thread cancellation and this saves the shadow
   stack register, but __libc_unwind_longjmp doesn't restore the shadow
   stack register since cancellation never returns after longjmp.  */
#undef __sigset_t
#define __sigset_t __ssp_sigset_arch_t
#include <setjmp.h>
#undef __saved_mask
#define __saved_mask __saved_mask.__saved.__saved_mask

#include <signal.h>

typedef struct
  {
    unsigned long int __val[__NSIG_WORDS];
  } __sigprocmask_sigset_t;

extern jmp_buf ___buf;
extern  __typeof (___buf[0].__saved_mask) ___saved_mask;
_Static_assert (sizeof (___saved_mask) >= sizeof (__sigprocmask_sigset_t),
		"size of ___saved_mask < size of __sigprocmask_sigset_t");

#endif /* setjmpP.h  */
