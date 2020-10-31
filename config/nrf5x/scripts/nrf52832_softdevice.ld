/* details on syntax can be found in GCC linker manual */

MEMORY {
  RAM   (xrw): ORIGIN = 0x20002800, LENGTH = 0xd800
  FLASH (rx) : ORIGIN = 0x1f000, LENGTH = 0x61000
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
		KEEP(*(.init))
		KEEP(*(.fini))

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)

		/* .dtors */
 		*crtbegin.o(.dtors)
 		*crtbegin?.o(.dtors)
 		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
 		*(SORT(.dtors.*))
 		*(.dtors)

		. = ALIGN(4);
                _etext = .;             /* _etext is end of program code */
        } >FLASH                        /* this section is code, hence gos to FLASH */

        .data : AT ( _etext ) {         /* AT specifies the LMA (load memory address) */
                . = ALIGN(4);           /* also, this section is for initialized
                                             global/static variables */
                _sdata = .;
		__data_start__ = .;
		*(vtable)
                *(.data)
                *(.data.*)
                . = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		*(.preinit_array)
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		/* init data */
		PROVIDE_HIDDEN (__init_array_start = .);
		*(SORT(.init_array.*))
		*(.init_array)
		PROVIDE_HIDDEN (__init_array_end = .);


		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		*(SORT(.fini_array.*))
		*(.fini_array)
		PROVIDE_HIDDEN (__fini_array_end = .);

		*(.jcr)
		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;
                _edata = .;
        } >RAM                          /* this section go into this RAM */

        .bss : {                        /* this section is for uninitialized global/static
                                        variables, which we set to zero */
                . = ALIGN(4);
                _sbss = .;              /* the . sets the symbol to value of current counter */
		__bss_start__ = .;
		*(.bss)
		*(.bss.*)
                *(COMMON)
                . = ALIGN(4);
		__bss_end__ = .;
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
      end = _eheap;
}
