
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

/* We are using floating point stacking so here is the order of the registers saved:
 *
 *      | FPCSR | <---- Stack grows downwards so here are big address - 
 *      | S15   |       before stacking
 *      | S14   |
 *      | S13   |
 *      | S12   |
 *      | S11   |
 *      | S10   |
 *      | S9    |
 *      | S8    |
 *      | S7    |
 *      | S6    |
 *      | S5    |
 *      | S4    |
 *      | S3    |
 *      | S2    |
 *      | S1    |
 *      | S0    |
 *      | xPSR  |
 *      |  PC   |
 *      |  LR   |
 *      | R12   |
 *      |  R3   |
 *      |  R2   |
 *      |  R1   |
 *      |  R0   |
 *                <---- Stack ptr after stacking
 *
 *  100 bytes stored by the interrupt controller to save context.
 */

/* We also need to store :
 *
 * This will be placed below the R0 from the ISR stacking
 *
 * R4
 * R5
 * R6
 * R7
 * R8
 * R9
 * R10
 * R11
 */


.align 4
.thumb_func

/**************************************************************************
 * Name:
 *  cpu_savecontextt
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
