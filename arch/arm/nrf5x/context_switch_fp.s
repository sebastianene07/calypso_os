
/*
 * context_switch.s
 *
 * Created: 3/31/2018 11:44:15 AM
 *  Author: sene
 */

.syntax unified

.global cpu_restorecontext
.global cpu_savecontext

.section .text
.align 4
.thumb_func

/**************************************************************************
 * Name:
 *  cpu_savecontext
 *
 * Description:
 *  Dump the registers on the stack and update the stack pointer from the TCB. 
 *
 * Input Arguments:
 *  sp - the pointer to the location of the 'sp' member from the tcb_t
 *       (in R0 register)
 *
 * Return Value:
 *  If we return on the normal path (after we stacked all the registers)
 *  we return 0. If we return from this function because cpu_contextrestore
 *  was called we return 1.
 *
 ************************************************************************/

 cpu_savecontext:
  mrs r4, xpsr              // Move the content of the XPSR in R4
  push {r4}                 // Push the XPSR
  ldr r4, =cpu_savecontext_ret
  push {r4}                 // Push the PC to the cpu_savecontext_ret
  push {lr}                 // Push the LR
  push {r12}                // Push the R12
  push {r3}                 // Push the R3
  push {r2}                 // Push the R2
  push {r1}                 // Push the R1
  push {r0}                 // Push the R0
  str sp, [r0]              // Update the SP from the task
  mov r0, #0                // Return "0" if we saved the data on the stack
  bx lr
cpu_savecontext_ret:
  mov r0, #1
  bx lr

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

cpu_restorecontext:
  mov sp, r0                // Switch the stack to the SP received as argument
  pop {r0}                  // Pop the R0
  pop {r1}                  // Pop the R1
  pop {r2}                  // Pop the R2
  pop {r3}                  // Pop the R3
  pop {r12}                 // Pop the R12
  pop {lr}                  // Pop the LR
  pop {r4}                  // Pop the PC address in R4
  pop {r5}                  // Pop the XPSR address in R5
  msr xpsr, r5              // Restore the XPSR from R5
  mov pc, r4                // Jump to the last PC from R4
