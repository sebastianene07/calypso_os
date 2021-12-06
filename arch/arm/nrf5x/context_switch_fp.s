.text
.syntax unified

.global cpu_savecontext
.type cpu_savecontext, %function
.thumb_func

/**************************************************************************
 * Name:
 *  cpu_savecontext
 *
 * Description:
 *  Save the CPU context in the mcu_context received as argument.
 *
 * Input Arguments:
 *  mcu_context - the pointer to mcu_context
 *
 * Return Value:
 *  If we return on the normal path (after we saved all the registers)
 *  we return 0. If we return from this function because cpu_contextrestore
 *  was called we return 1.
 *
 ************************************************************************/

 cpu_savecontext:

  mrs r1, xpsr              // Move the content of the XPSR in R1
  str r1, [r0, #64]         // Save the XPSR
  ldr r1, =cpu_savecontext_ret 
  mov r2, sp                // Place the SP in R2
  stm r0, {r1, r2, r4-r12, lr} // Store PC[0], SP[1], R4-R12, LR[10]
  mov r0, #0                   // Return "0" if we saved the data on the stack
  bx lr
cpu_savecontext_ret:
  mov r0, #1
  bx lr

.size cpu_savecontext, .-cpu_savecontext

.global cpu_restorecontext
.type cpu_restorecontext, %function
.thumb_func

/**************************************************************************
 * Name:
 *  cpu_restorecontext
 *
 * Description:
 *  Recreate the CPU context from the mcu_context argument.
 *
 * Input Arguments:
 *  mcu_context - the pointer to mcu_context (in R0)
 *
 ************************************************************************/
cpu_restorecontext:
    ldm r0, {r2-r12, lr}      // Get the PC and SP in R2 and R3 and the rest
    mov sp, r3                // Update the stack pointer
    ldr r3, [r0, #64]         // Get the XPSR address in R5
    ldr r1, [r0, #52]         // These are used (r0, r1) only when we create
    ldr r0, [r0, #48]         // the initial context and pass the args in the
			      // entry point
    msr apsr, r3              // Restore the XPSR from R5
    mov pc, r2                // Jump to the last PC from R4

.size cpu_restorecontext, .-cpu_restorecontext
.end
