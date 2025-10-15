/* Architecture-specific __sigset_t definition.  RISC-V version.  */
#ifndef ____sigset_t_defined
#define ____sigset_t_defined

#define _SIGSET_NWORDS (1024 / (8 * sizeof (unsigned long int)))
typedef struct
{
  unsigned long int __val[_SIGSET_NWORDS];
} __sigset_t;

#define __ALIGN_DOWN(base, size)	((base) & -((__typeof__ (base)) (size)))
#define __ALIGN_UP(base, size)	__ALIGN_DOWN ((base) + (size) - 1, (size))

/* Number of bits per long.  */
#define _SSP_SIGSET_BITS_PER_WORD (8 * sizeof (unsigned long int))
/* This holds the number of signals, 512 should be sufficient for future.
   expansion  */
#define _SSP_SIGSET_NSIG	512
/* Number of longs to hold all signals.  */
#define _SSP_SIGSET_NWORDS \
  (__ALIGN_UP (_SSP_SIGSET_NSIG, _SSP_SIGSET_BITS_PER_WORD) \
   / _SSP_SIGSET_BITS_PER_WORD)

typedef struct
  {
    unsigned long int __val[_SSP_SIGSET_NWORDS];
  } __ssp_sigset_t;

typedef union
  {
    __sigset_t __saved_mask_compat;
    struct
      {
	__ssp_sigset_t __saved_mask;
	/* Used for shadow stack pointer.  NB: Shadow stack pointer
	   must have the same alignment as __saved_mask.  Otherwise
	   offset of __saved_mask will be changed.  */
	unsigned long int __ssp;
	unsigned long int __ssp_base;
      } __saved;
  } __ssp_sigset_arch_t;

#endif
