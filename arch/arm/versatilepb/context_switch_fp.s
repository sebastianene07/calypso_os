.text
.syntax unified

.global cpu_savecontext
.type cpu_savecontext, %function

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
  mrs r1, apsr              // Move the content of the XPSR in R4
  str r1, [r0, #64]         // Save the XPSR
  ldr r1, =cpu_savecontext_ret
  stm r0, {r1, r4-r12, sp, lr} // Store registers R1(new PC) then  R4 to R12, SP(R13) and the LR(R14)
  mov r0, #0                   // Return "0" if we saved the data on the stack
  bx lr
cpu_savecontext_ret:
  mov r0, #1
  bx lr

.size cpu_savecontext, .-cpu_savecontext

.global cpu_restorecontext
.type cpu_restorecontext, %function

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
    ldm r0, {r2, r4-r12, sp, lr}
    ldr r5, [r0, #64]         // Get the XPSR address in R5
    ldr r1, [r0, #56]
    ldr r0, [r0, #52]
    msr apsr, r5              // Restore the XPSR from R5
    mov pc, r2               // Jump to the last PC from R4
.size cpu_restorecontext, .-cpu_restorecontext
.end
