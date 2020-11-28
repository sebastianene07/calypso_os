  .text
  .align 4
#ifdef __APPLE__
  .globl _cpu_savecontext
#else
  .globl cpu_savecontext
#endif

/**************************************************************************
 * Name:
 *  cpu_savecontext
 *
 * Description:
 *  Dump the registers in the argument received as buffer.
 *
 * Input Arguments:
 *  mcu_context - the pointer to the CPU context
 *
 * Return Value:
 *  If we return on the normal path (after we saved all the registers)
 *  we return 0. If we return from this function because cpu_contextrestore
 *  was called we return 1.
 *
 ************************************************************************/
#ifdef __APPLE__
_cpu_savecontext:
#else
cpu_savecontext:
#endif
  pop %rsi			/* Save the return value in RSI */
  push %rsi			/* Push back the return value to restore the calling stack */
  movq %rsi, (8 * 6)(%rdi)	/* Save the PC */
  push %rbp			/* Push the old frame pointer */
  movq %rbp, (8 * 1)(%rdi)	/* 1.RBP */ 
  movq %rsp, %rbp		/* Save the stack pointer on the RBP to create the stack frame for this function */
  movq %rbx, (%rdi) 		/* 0. RBX */
  xorl %eax,%eax		/* Zero out EAX for the normal return path */
  movq %r12, (8 * 2)(%rdi)	/* 2. R12 */
  movq %r13, (8 * 3)(%rdi)	/* 4. R13 */
  movq %r14, (8 * 4)(%rdi)	/* 5. R14 */
  movq %r15, (8 * 5)(%rdi)	/* 6. R15 */
  leaveq			/* Restore the old frame pointer */
  ret

  .align 4
#ifdef __APPLE__ 
  .globl _cpu_restorecontext
#else
  .globl cpu_restorecontext
#endif

/**************************************************************************
 * Name:
 *  cpu_restorecontext
 *
 * Description:
 *  Recreate the task context by copying the register contents from the
 *  parameter received as argument. 
 *
 * Input Arguments:
 *  mcu_context - the pointer to the CPU context)
 *
 ************************************************************************/
#ifdef __APPLE__
_cpu_restorecontext:
#else
cpu_restorecontext:
#endif
  mov $1, %eax		        /* Store an immediate value of 1 in EAX for the return value */
  movq (8 * 7)(%rdi), %rsp	/* Switch stacks to the one received as argument */
  movq (8 * 6)(%rdi), %rsi  /* 7. Get the return address in RSI */
  movq (8 * 5)(%rdi), %r15  /* 6. R15 */
  movq (8 * 4)(%rdi), %r14  /* 5. R14 */
  movq (8 * 3)(%rdi), %r13  /* 4. R13 */
  movq (8 * 2)(%rdi), %r12  /* 3. R12 */
  movq (8 * 1)(%rdi), %rbp  /* Restore the old frame pointer ( the function that initially called cpu_savecontext ) */
  movq (%rdi), %rbx		/* 1. RBX */
  jmp *%rsi		/* Jump to the return value from RSI */
