/*
 * Get the current shadow stack configuration for the current thread,
 * this will be the value configured via PR_SET_SHADOW_STACK_STATUS.
 */
#define PR_GET_SHADOW_STACK_STATUS	74

/*
 * Set the current shadow stack configuration.  Enabling the shadow
 * stack will cause a shadow stack to be allocated for the thread.
 */
#define PR_SET_SHADOW_STACK_STATUS	75
# define PR_SHADOW_STACK_ENABLE		(1UL << 0)
# define PR_SHADOW_STACK_WRITE		(1UL << 1)
# define PR_SHADOW_STACK_PUSH		(1UL << 2)

/*
 * Prevent further changes to the specified shadow stack
 * configuration.  All bits may be locked via this call, including
 * undefined bits.
 */
#define PR_LOCK_SHADOW_STACK_STATUS	76

/*
 * Get or set the control flow integrity (CFI) configuration for the
 * current thread.
 *
 * Some per-thread control flow integrity settings are not yet
 * controlled through this prctl(); see for example
 * PR_{GET,SET,LOCK}_SHADOW_STACK_STATUS
 */
#define PR_GET_CFI	80
#define PR_SET_CFI	81

/*
 * Forward-edge CFI variants (excluding ARM64 BTI, which has its own
 * prctl()s).
 */
#define PR_CFI_BRANCH_LANDING_PADS	0

/* Return and control values for PR_{GET,SET}_CFI */
# define PR_CFI_ENABLE		(1UL << 0)
# define PR_CFI_DISABLE		(1UL << 1)
# define PR_CFI_LOCK		(1UL << 2)
