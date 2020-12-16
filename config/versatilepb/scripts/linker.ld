/* On VersatilePB the kernel is load at 0x10000 address
 * We only use 16Mb of memory
 */
MEMORY {
  RAM   (xrw): ORIGIN = 0x10000, LENGTH = 0xFF0000
}

__RAM_START   = ORIGIN(RAM);
__RAM_LENGTH  = LENGTH(RAM);
__RAM_END     = (ORIGIN(RAM) + LENGTH(RAM)) - 1;

SECTIONS {
        .boot_startup : {
          . = ALIGN(4);
          bootload.o(.text)
          . = ALIGN(4);
        } >RAM
        .isr_vector : {
                . = ALIGN(4);
                KEEP(*(.isr_vector))    /* vector table */
                . = ALIGN(4);
        } >RAM

        .rodata : {
          . = ALIGN(4);
          _srodata = .;
          *(.rodata)
          *(.rodata.*)
          . = ALIGN(4);
        } >RAM

        .text :  {
                . = ALIGN(4);
                *(.text)                /* Program code */
                *(.text.*)
                . = ALIGN(4);
                _etext = .;             /* _etext is end of program code */
        } >RAM                        /* this section is code, hence gos to FLASH */

        .data : {         /* AT specifies the LMA (load memory address) */
                . = ALIGN(4);           /* also, this section is for initialized
                                             global/static variables */
                _sdata = .;
                *(.data)
                *(.data.*)
                . = ALIGN(4);
                _edata = .;
        } >RAM                          /* this section go into this RAM */

        .bss : {                        /* this section is for uninitialized global/static
                                        variables, which we set to zero */
                . = ALIGN(4);
                _sbss = .;              /* the . sets the symbol to value of current counter */
                *(.bss)
                *(COMMON)
                . = ALIGN(4);
                _ebss = .;
        } >RAM

        .stack : {
                 KEEP(*(.stack*))
                . = ALIGN(4);
                _sstack = .;
                . = . + 4096;
                . = ALIGN(4);
                _estack = .;
       } >RAM

        .heap : {
                KEEP(*(.heap*))
                . = ALIGN(4);
                _sheap = .;
        } >RAM

      _eheap = ORIGIN(RAM) + LENGTH(RAM);
}
