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
  mrs r1, psr              // Move the content of the XPSR in R4
  str r1, [r0, #64]         // Save the XPSR
  ldr r1, =cpu_savecontext_ret
  str r1, [r0, #60]         // Place the PC to the cpu_savecontext_ret
  push {lr}
  pop  {r1}
  str r1, [r0, #56]         // Place the LR
  mov r1, r12
  str r1, [r0, #48]         // Place the R12
  mov r1, r11
  str r1, [r0, #32]        // Place R11
  mov r1, r10
  str r1, [r0, #28]	    // Place R10
  mov r1, r9
  str r1, [r0, #24]         // Place R9
  mov r1, r8
  str r1, [r0, #20]         // Place R8
  str r5, [r0, #16]	    // Place R5
  str r4, [r0, #12]         // Place R4
  str r6, [r0, #8]          // Place the R6
  str r7, [r0, #4]          // Place the R7
  mrs r4, msp
  str r4, [r0, #52]              // Update the SP from the task
  movs r0, #0                    // Return "0" if we saved the data on the stack
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
    ldr r1, [r0, #52]
    msr msp, r1
                  
    ldr r7, [r0, #4]          // Get the R7
    ldr r6, [r0, #8]          // Get the R6
    ldr r4, [r0, #12]         // Get the R4
    ldr r5, [r0, #16]         // Get the R5

    ldr r1, [r0, #48]        // Get the R12
    mov r12, r1

    ldr r1, [r0, #20]         // Get the R8
    mov r8, r1

    ldr r1, [r0, #24]         // Get the R9
    mov r9, r1

    ldr r1, [r0, #28]         // Get the R10
    mov r10, r1

    ldr r1, [r0, #32]         // Get the R11
    mov r11, r1

    ldr r1, [r0, #56]         // Get the LR
    mov lr, r1
    ldr r1, [r0, #60]         // Get the PC address in R1
    ldr r2, [r0, #64]         // Get the XPSR address in R2
    msr psr, r2              // Restore the XPSR from R2
    mov pc, r1                // Jump to the last PC from R1

.size cpu_restorecontext, .-cpu_restorecontext
.end
