.syntax unified

.global cpu_restorecontext
.global cpu_savecontext

/* The MCU context registers */

.section .text
.align 4
.thumb_func

/*
 * REG_R0                (0)
 * REG_R1                (1)
 * REG_R2                (2)
 * REG_R3                (3)
 * REG_R12               (4)
 * REG_LR                (5)
 * REG_PC                (6)
 * REG_XPSR              (7)
 * REG_R4                (8)
 * REG_R5                (9)
 * REG_R6                (10)
 * REG_R7                (11)
 * REG_R8                (12)
 * REG_R9                (13)
 * REG_R10               (14)
 * REG_R11               (15)
 * REG_SP                (16)
 * REG_NUMS              (17)
 */

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

  push {r4}                         // Save R4
  mrs r4, xpsr                      // Move the content of the XPSR in R4
  str r4, [r0, #64]                 // Save XPSR in (16)
  ldr r4, =cpu_savecontext_ret      // Load PC in R4
  str r4, [r0, #60]                 // Save PC in (15)
  pop {r4}                          // Restore R4

  str lr, [r0, #56]                 // Save LR in  (14)
  str r12, [r0, #48]                // Save R12 in (12)
  str r3, [r0, #12]                 // Save R3 in  (3)
  str r2, [r0, #8]                  // Save R2 in  (2)
  str r1, [r0, #4]                  // Save R1 in  (1)
  str sp, [r0, #52]                 // Save SP in  (13)
  str r4, [r0, #16]                 // Save R4 in  (4)
  str r5, [r0, #20]                 // Save R5 in  (5)
  str r6, [r0, #24]                 // Save R6 in  (6)
  str r7, [r0, #28]                 // Save R7 in  (7)
  str r8, [r0, #32]                 // Save R8 in  (8)
  str r9, [r0, #36]                 // Save R9 in  (9)
  str r10, [r0, #40]                // Save R10 in  (10)
  str r11, [r0, #44]                // Save R11 in  (11)
  mov r0, #0                        // Return "0" if we saved the data on the stack
  bx lr
cpu_savecontext_ret:
  mov r0, #1
  bx lr

.align 4
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
  ldr r1, [r0, #4]                   // Retrieve the R1 (1)
  ldr r2, [r0, #8]                   // Retrieve the R2 (2)
  ldr r3, [r0, #12]                  // Retrieve the R3 (3)
  ldr r4, [r0, #64]                  // Get the XPSR contents in R4
  msr APSR_nzcvq, r4                 // Restore the XPSR from R4
  ldr r4, [r0, #16]                  // Get the R4 value
  ldr r5, [r0, #20]                  // Restore the R5 (9)
  ldr r6, [r0, #24]                  // Restore the R6 (10)
  ldr r7, [r0, #28]                  // Restore the R7 (11)
  ldr r8, [r0, #32]                  // Restore the R8 (12)
  ldr r9, [r0, #36]                  // Restore the R9 (13)
  ldr r10, [r0, #40]                 // Restore the R10 (14)
  ldr r11, [r0, #44]                 // Restore the R11 (15)
  ldr r12, [r0, #48]                 // Retrieve the R12 (4)
  ldr sp, [r0, #52]                  // Retrieve the SP and switch stacks
  ldr lr, [r0, #56]                  // Retrieve the LR
  ldr pc, [r0, #60]
