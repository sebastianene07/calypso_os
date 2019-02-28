
/*
 * context_switch.s
 *
 * Created: 3/31/2018 11:44:15 AM
 *  Author: sene
 */

.syntax unified

.global SysTick_Handler
.global sched_context_switch

.extern sched_get_next_task
.extern sched_preempt_task

.section .text

/* We are not using floating point stacking so here is the order of the registers saved:
 *   ______
 *	| xPSR | <---- Stack grows downwards so here are big address - Stack Base before
 *	|  PC  |       stacking.
 *	|  LR  |
 *	| R12  |
 *	|  R3  |
 *	|  R2  |
 *	|  R1  |
 *	|  R0  |
 *   ______	 <---- Stack ptr after stacking
 *
 *  32 bytes stored by the interrupt controller to save context.
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
#.thumb_func

SysTick_Handler:
	stmdb sp!, {lr}					    // Store the link register on the stack
	bl sched_get_current_task		// Get current TCB address in R0
	ldmia sp!, {lr}					    // Restore the link register from the stack

	cmp r0, #0						      // Verify NULL pointer TCB
	beq SysTick_Handler_ret			// Handle NULL pointer to return from handler
	ldr r1, [r0, #4]				    // Load the state of the task
  cmp r1, 2
  beq SysTick_Handler_ret

	cmp r1, 1						        // If task is not running then we must plan it
	bne SysTick_Handle_context_switch

/* Task is running so switch it to (not running) READY */

  mov r1, #0
  str r1, [r0, #4]

/* Save the R4-R11 registers on the stack (we are on the running TCB stack) */

  push {R4-R11}

/* Save the SP in the TCB */

  mov r1, sp
  str r1, [r0, #16]

/* Get the next task ptr in R0 */

	stmdb sp!, {lr}					    // Store the link register on the stack
	bl sched_get_next_task			// Get the next TCB address in R0
	ldmia sp!, {lr}					    // Restore the link register from the stack

	cmp r0, #0						      // Verify NULL pointer TCB
	beq SysTick_Handler_ret			// Handle NULL pointer to return from handler

SysTick_Handle_context_switch:
	mov r1, #1
	str r1, [r0, #4]				// Switch task state to running
	mov r5, r0						  // Save task TCB ptr in r5
	add r5, #16						  // Get the address of the sp from TCB struct in r5

/* The SP of the new task should have already some stacking values */

	ldr r0, [r5]							  // Set SP to point to the task SP
	mov sp, r0								  //
  pop {R4-R11}                // Pop R4-R11

SysTick_Handler_ret:
	bx lr							          // Return from handler

sched_context_switch:
  /* Stack all the registers */
  push {r0}
  mrs r0, xpsr
  push {r0}
  mov r0, pc
  add r0, #0x72
  push {r0}
  push {lr}
  push {r12}
  push {r3}
  push {r2}
  push {r1}
  ldr r0, [sp, #28]
  push {r0}

  /* Stack extra registers */

  push {R4-R11}

  /* Save the SP in the TCB structure */

  stmdb sp!, {lr}
  bl sched_preempt_task
  ldmia sp!, {lr}

  cmp r0, #0
  beq sched_context_switch_ret

  /* Save the SP in the TCB */

  mov r1, sp
  str r1, [r0, #16]

	stmdb sp!, {lr}					    // Store the link register on the stack
	bl sched_get_current_task		// Get the next TCB address in R0
	ldmia sp!, {lr}					    // Restore the link register from the stack

  cmp r0, #0
  beq sched_context_switch_ret

	mov r1, #1
	str r1, [r0, #4]				// Switch task state to running
	mov r5, r0						  // Save task TCB ptr in r5
	add r5, #16						  // Get the address of the sp from TCB struct in r5

  /* The SP of the new task should have already some stacking values */

	ldr r0, [r5]							  // Set SP to point to the task SP
	mov sp, r0								  //
  pop {R4-R11}                // Pop R4-R11

  pop {r0}
  pop {r1}
  pop {r2}
  pop {r3}
  pop {r12}
  pop {lr}

  /* Save the value of the R0 at SP + 0xC */

  str r0, [sp, #0x8]
  ldr r0, [sp, #0x4]  /* OLD XPSR */
  msr xpsr, r0        /* Restore XPSR */
  add sp, #0x4        /* SP points at XPSR on the stack */
  ldr r0, [sp, #0x4]  /* Restore R0 */
  cpsie i
  ldr pc, [sp, #-0x4]

sched_context_switch_ret:
  bx lr
