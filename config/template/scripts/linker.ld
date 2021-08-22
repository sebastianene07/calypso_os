/* On Template the kernel is load at 0x10000 address
 * We only use 16Mb of memory
 */
MEMORY {
  RAM_CODE   (xrw): ORIGIN = 0x10000,  LENGTH = 0x100000  /* 1Mb text */
  RAM_DATA   (xrw): ORIGIN = 0x110000, LENGTH = 0x400000  /* 4Mb stack + heap */
}

SECTIONS {
        .boot_startup : {
          . = ALIGN(4);
          bootload.o(.text)
          . = ALIGN(4);
        } >RAM_CODE

        .rodata : {
          . = ALIGN(4);
          _srodata = .;
          *(.rodata)
          *(.rodata.*)
          . = ALIGN(4);
        } >RAM_CODE

        .text :  {
                . = ALIGN(4);
                *(.text)                /* Program code */
                *(.text.*)
                . = ALIGN(4);
                _etext = .;             /* _etext is end of program code */
        } >RAM_CODE                     /* this section is code, hence gos to FLASH */

        .data : {                       /* AT specifies the LMA (load memory address) */
                . = ALIGN(4);           /* also, this section is for initialized
                                             global/static variables */
                _sdata = .;
                *(.data)
                *(.data.*)
                . = ALIGN(4);
                _edata = .;
        } >RAM_CODE                     /* this section go into this RAM */

        .bss : {                        /* this section is for uninitialized global/static
                                        variables, which we set to zero */
                . = ALIGN(4);
                _sbss = .;              /* the . sets the symbol to value of current counter */
                *(.bss)
                *(COMMON)
                . = ALIGN(4);
                _ebss = .;
        } >RAM_CODE

        .stack : {
                 KEEP(*(.stack*))
                . = ALIGN(8);
                _sstack = .;
                . = . + 4096;
                . = ALIGN(8);
                _estack = .;
       } >RAM_DATA

       .irq_stack : {
                 KEEP(*(.irq_stack*))
                . = ALIGN(8);
                _sirq_stack = .;
                . = . + 2048;
                . = ALIGN(8);
                _eirq_stack = .;
       } >RAM_DATA

        .heap : {
                KEEP(*(.heap*))
                . = ALIGN(4);
                _sheap = .;
        } >RAM_DATA

      _eheap = ORIGIN(RAM_DATA) + LENGTH(RAM_DATA);
}
