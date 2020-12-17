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
  mrs r4, apsr              // Move the content of the XPSR in R4
  push {r4}                 // Push the XPSR
  ldr r4, =cpu_savecontext_ret
  push {r4}                 // Push the PC to the cpu_savecontext_ret
  push {lr}                 // Push the LR
  push {r12}                // Push the R12
  push {r3}                 // Push the R3
  push {r2}                 // Push the R2
  push {r1}                 // Push the R1
  push {r0}                 // Push the R0
  push {fp}
  str sp, [r0, #52]              // Update the SP from the task
  mov r0, #0                        // Return "0" if we saved the data on the stack
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
    ldr sp, [r0, #52]
    pop {fp}
    pop {r0}                  // Pop the R0
    pop {r1}                  // Pop the R1
    pop {r2}                  // Pop the R2
    pop {r3}                  // Pop the R3
    pop {r12}                 // Pop the R12
    pop {lr}                  // Pop the LR
    pop {r4}                  // Pop the PC address in R4
    pop {r5}                  // Pop the XPSR address in R5
    msr apsr, r5              // Restore the XPSR from R5
    mov pc, r4                // Jump to the last PC from R4

.size cpu_restorecontext, .-cpu_restorecontext
.end
