.globl _bootload
.align 4

_bootload:
  ldr r0,=_estack     /* Load the stack value from the linker _estack */
  mov sp, r0              /* Set the initial SP */
  ldr r0, =isr_dispatch
  bl copy_isr_vector
  bl __start
  b .

isr_dispatch:
  ldr pc, irq_generic_address

irq_generic_address: .word irq_generic_handler
