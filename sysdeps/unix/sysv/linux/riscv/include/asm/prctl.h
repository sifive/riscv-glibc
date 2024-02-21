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
 * Get the current indirect branch tracking configuration for the current
 * thread, this will be the value configured via PR_SET_INDIR_BR_LP_STATUS.
 */
#define PR_GET_INDIR_BR_LP_STATUS	79

/*
 * Set the indirect branch tracking configuration. PR_INDIR_BR_LP_ENABLE will
 * enable cpu feature for user thread, to track all indirect branches and ensure
 * they land on arch defined landing pad instruction.
 * x86 - If enabled, an indirect branch must land on `ENDBRANCH` instruction.
 * arch64 - If enabled, an indirect branch must land on `BTI` instruction.
 * riscv - If enabled, an indirect branch must land on `lpad` instruction.
 * PR_INDIR_BR_LP_DISABLE will disable feature for user thread and indirect
 * branches will no more be tracked by cpu to land on arch defined landing pad
 * instruction.
 */
#define PR_SET_INDIR_BR_LP_STATUS	80
# define PR_INDIR_BR_LP_ENABLE		(1UL << 0)

/*
 * Prevent further changes to the specified indirect branch tracking
 * configuration.  All bits may be locked via this call, including
 * undefined bits.
 */
#define PR_LOCK_INDIR_BR_LP_STATUS	81
