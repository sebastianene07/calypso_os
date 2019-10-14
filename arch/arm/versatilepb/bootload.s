.globl __bootload
.align 4
.section .boot_startup

__bootload:
  ldr r0, =g_ram_vectors
  mov sp, r0  /* First address is the patched SP */
  bl os_startup
  b .
