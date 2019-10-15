.globl _bootload
.align 4

_bootload:
  ldr r0,=g_vectors
  ldr r1, [r0]
  mov sp, r1  /* First address is the patched SP */
  bl os_startup
  b .
