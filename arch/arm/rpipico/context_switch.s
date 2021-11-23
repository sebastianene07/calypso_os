.cpu cortex-m0
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

  mrs r4, psr              // Move the content of the XPSR in R4
  str r4, [r0, #64]         // Save the XPSR
  ldr r4, =cpu_savecontext_ret
  str r4, [r0, #60]         // Place the PC to the cpu_savecontext_ret
  push {lr}
  pop  {r4}
  str r4, [r0, #56]         // Place the LR
  mov r4, r12
  str r4, [r0, #48]        // Place the R12
  str r3, [r0, #12]         // Place the R3
  str r2, [r0, #8]          // Place the R2
  str r1, [r0, #4]          // Place the R1
  mrs r4, msp
  str r4, [r0, #52]              // Update the SP from the task
  movs r0, #0                        // Return "0" if we saved the data on the stack
  bx lr
cpu_savecontext_ret:
  movs r0, #1
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
    ldr r4, [r0, #52]
    msr msp, r4
                  
    ldr r1, [r0, #4]          // Get the R1
    ldr r2, [r0, #8]          // Get the R2
    ldr r3, [r0, #12]         // Get the R3
    ldr r4, [r0, #48]        // Get the R12
    mov r12, r4
    ldr r4, [r0, #56]         // Get the LR
    mov lr, r4
    ldr r4, [r0, #60]         // Get the PC address in R4
    ldr r5, [r0, #64]         // Get the XPSR address in R5
    msr psr, r5              // Restore the XPSR from R5
    mov pc, r4                // Jump to the last PC from R4

.size cpu_restorecontext, .-cpu_restorecontext
.end
