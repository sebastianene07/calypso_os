## How to add support for a new board ?

Every board should define and implement the following methods:

MANDATORY:

### Peripheral initialization function for the board

```board_init(void)```

Description:

	Initialize the board peripherals and start the clock sources.

### Task management functions

```int cpu_inittask(struct tcb_s *tcb, int argc, char **argv)```

Description:

	This function sets up the stack for a new task and configures the
	registers in the initial state before jumping to the new task.

```void cpu_destroytask(tcb_t *tcb)```

Description:

	Destroy the task and the associated resources. Free the TCB.

### CPU context management functions

```int cpu_savecontext(void **tcb_sp)```

Description:

	Save the current context on the stack and return "0". Update the
	task pointer from the TCB with the new value after register stacking.
	When we return from this function because of a cpu_restorecontext(..)
	call, the value "1" is returned.

```void cpu_restorecontext(void *tcb_sp)```

Description:

	Restore the task context from the stack and start executing from the last
	stacked PC. This function does not return.

### CPU interrupt management functions

```irq_state_t cpu_disableint(void)```

Description:

	This function disables the interrupts and returns the disabled status.

```void cpu_enableint(irq_state_t irq_state)```

Description:

	This function re-enables the interrupts based on the irq_state flag.

```int cpu_getirqnum(void)```

Description:

	This function returns the interrupt number that took place. It should never
	be called from a task context.

```cpu_attachint(int irq_num, irq_cb handler)```
```cpu_detachint(int irq_num)```

	The above functions are defined in a board specific header.
	(eg. arch/arm/nrf5x/include/board.h).
	The header should also define or include a define that contains the
	supported number of interrupts: ```NUM_IRQS```


OPTIONAL:

### Board power management functions

```void board_entersleep(void)```

Description:

	This function is for power saving and puts the board to sleep. If
	the functionality is implemented CONFIG_BOARD_SLEEP should be
	enabled for the board.

```board_powerdown(void)```

Description:

	This function puts the board in the lowest possible power state.
	If this is implemented CONFIG_BOARD_POWERDOWN should be enabled
	for the board.


### Debug utilities for the board

```board_assert(void)```

Description:

	This function triggers a register dump and unwinds the stack. It
	can be enabled when CONFIG_BOARD_ASSERT is defined.
