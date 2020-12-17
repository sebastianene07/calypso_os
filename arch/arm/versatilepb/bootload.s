.globl _bootload
.align 4

.text
.code 32

.global vectors_start
.global vectors_end
.align 4

vectors_start:
  ldr pc, reset_handler_addr
  ldr pc, undef_handler_addr
  ldr pc, swi_handler_addr
  ldr pc, prefetch_abort_handler_addr
  ldr pc, data_abort_handler_addr
  B .
  ldr pc, irq_handler_addr
  ldr pc, fiq_handler_addr

reset_handler_addr: .word _bootload
undef_handler_addr: .word irq_generic_handler
swi_handler_addr: .word irq_generic_handler
prefetch_abort_handler_addr: .word irq_generic_handler
data_abort_handler_addr: .word irq_generic_handler
irq_handler_addr: .word irq_generic_handler
fiq_handler_addr: .word irq_generic_handler

vectors_end:_bootload:
  ldr r0,=_estack         /* Load the stack value from the linker _estack */
  mov sp, r0              /* Set the initial SP */
  bl copy_isr_vector
  mrs r0, cpsr            /* get Program Status Register */
  bic r1, r0, #0x1F       /* go in IRQ mode */
  orr r1, r1, #0x12
  msr cpsr, r1
  ldr sp, =_eirq_stack    /* set IRQ stack */
  bic r0, r0, #0x80       /* enable IRQs */

  msr cpsr, r0            /* go back in Supervisor mode */
  bl __start
  b .

