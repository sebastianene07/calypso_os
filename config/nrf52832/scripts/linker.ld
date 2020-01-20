/* details on syntax can be found in GCC linker manual */

MEMORY {
  RAM   (xrw): ORIGIN = 0x20000000, LENGTH = 0x10000
  FLASH (rx) : ORIGIN = 0x0, LENGTH = 512K
}

__RAM_START   = ORIGIN(RAM);
__RAM_LENGTH  = LENGTH(RAM);
__RAM_END     = (ORIGIN(RAM) + LENGTH(RAM)) - 1;

SECTIONS {
        .isr_vector : {
                . = ALIGN(4);
                KEEP(*(.isr_vector))    /* vector table */
                . = ALIGN(4);
        } >FLASH

        .rodata : {
          . = ALIGN(4);
          _srodata = .;
          *(.rodata)
          *(.rodata.*)
          . = ALIGN(4);
        } > FLASH

        .text : {
                . = ALIGN(4);
                *(.text)                /* Program code */
                *(.text.*)
                . = ALIGN(4);
                _etext = .;             /* _etext is end of program code */
        } >FLASH                        /* this section is code, hence gos to FLASH */

        .data : AT ( _etext ) {         /* AT specifies the LMA (load memory address) */
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

        .heap : {
                KEEP(*(.heap*))
                . = ALIGN(4);
                _sheap = .;
        } >RAM

        .stack : {
                 KEEP(*(.stack*))
                . = ALIGN(4);
                _sstack = .;
                . = . + 1024;
                . = ALIGN(4);
                _estack = .;
       } >RAM


      _eheap = ORIGIN(RAM) + LENGTH(RAM) - 1024;
}
