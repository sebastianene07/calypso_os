
/*
 * context_switch.s
 *
 * Created: 3/31/2018 11:44:15 AM
 *  Author: sene
 */

.syntax unified

.global SysTick_Handler
.extern sched_get_next_task
.extern sched_run
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
 *   ______	 <---- Stack top after stacking
 *
 *  32 bytes stored by the interrupt controller to save context.
 */

.align 4
.thumb_func

 SysTick_Handler:
	stmdb sp!, {lr}					    // Store the link register on the stack
	bl sched_get_current_task		// Get current TCB address in R0
	ldmia sp!, {lr}					    // Restore the link register from the stack

	cmp r0, #0						      // Verify NULL pointer TCB
	beq SysTick_Handler_ret			// Handle NULL pointer to return from handler
	ldr r1, [r0, #4]				    // Load the state of the task
	cmp r1, 1						        // If task is not running then we must plan it
	bne SysTick_Handle_context_switch

/* Task is running so switch it to READY and save it's SP */

  mov r1, #0
  str r1, [r0, #4]

/* Source is the SP address */

  mov r1, sp
  str r1, [r0, #16]

/* Save context in struct tcb from stack */

#SysTick_Handler_copy_from_stack:
#	ldr r4, [r1], #4						// Load one word in r4
#	str r4, [r0], #4						// Store one word in mcu_context
#	subs r3, r3, #1							// Decrement counter as we copied 1 word
#	bne SysTick_Handler_copy_from_stack		// Repeat word copy

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

/* Restore context from struct tcb and set stack */

#SysTick_Handler_copy_to_stack:
#	ldr r4, [r1], #4						// Load one word in r4
#	str r4, [r0], #4						// Store one word in mcu_context
#	subs r3, r3, #1							// Decrement counter as we copied 1 word
#	bne SysTick_Handler_copy_to_stack		// Repeat word copy

/* The SP of the new task should have already some stacking values */

	ldr r0, [r5]							  // Set SP to point to the task SP
	mov sp, r0								  //

SysTick_Handler_ret:
	bx lr							          // Return from handler

