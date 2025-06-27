/* Architecture-specific __sigset_t definition.  ARC version.  */
#ifndef ____sigset_t_defined
#define ____sigset_t_defined

/* We shrink it a bit to make space for storing shadow stack pointer.  */

#define _SIGSET_NWORDS (896 / (8 * sizeof (unsigned long int)))
typedef struct
{
  unsigned long int __val[_SIGSET_NWORDS];
} __sigset_t;

#endif
