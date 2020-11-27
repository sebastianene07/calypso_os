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
 *  Dump the registers on the stack and update the stack pointer from the TCB. 
 *
 * Input Arguments:
 *  sp - the pointer to the location of the 'sp' member from the tcb_t
 *       (in RDI register)
 *
 * Return Value:
 *  If we return on the normal path (after we stacked all the registers)
 *  we return 0. If we return from this function because cpu_contextrestore
 *  was called we return 1.
 *
 ************************************************************************/
#ifdef __APPLE__
_cpu_savecontext:
#else
cpu_savecontext:
#endif
  pop %rsi		/* Save the return value in RSI */
  push %rsi		/* Push back the return value to restore the calling stack */
  push %rbp		/* Push the old frame pointer */
  movq %rbp, %rax	/* Save the old frame pointer in RAX to push it on the stack for context switch save */
  movq %rsp, %rbp	/* Save the stack pointer on the RBP to create the stack frame for this function */
  push %rbx		/* 1. RBX */
  push %rax		/* 2. in RAX we have the old frame pointer RBP */
  xorl %eax,%eax	/* Zero out EAX for the normal return path */
  push %r12		/* 3. R12 */
  push %r13		/* 4. R13 */
  push %r14		/* 5. R14 */
  push %r15		/* 6. R15 */
  push %rsi		/* 7. in RSI we keep the return address, push it on the stack for context switch save */
  movq %rsp, (%rdi)	/* Save the address of the stack pointer (after stacking) in the paramter received as argument */
  leaveq		/* Restore the old frame pointer */
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
 *  Recreate the task context by popping the registers from the stack. 
 *
 * Input Arguments:
 *  sp - the pointer to the stack (in R0 register)
 *
 ************************************************************************/
#ifdef __APPLE__
_cpu_restorecontext:
#else
cpu_restorecontext:
#endif
  mov $1, %eax		/* Store an immediate value of 1 in EAX for the return value */
  movq %rdi, %rsp	/* Switch stacks to the one received as argument */
  pop %rsi		/* 7. Get the return address in RSI */
  pop %r15		/* 6. R15 */
  pop %r14		/* 5. R14 */
  pop %r13		/* 4. R13 */
  pop %r12		/* 3. R12 */
  pop %rbp		/* Restore the old frame pointer ( the function that initially called cpu_savecontext ) */
  pop %rbx		/* 1. RBX */
  jmp *%rsi		/* Jump to the return value from RSI */
