.globl _bootload
.align 4

_bootload:
  mov r0, #0x20000
  mov sp, r0  /* First address is the patched SP */
  bl os_startup
  b .
