ENTRY(__start)

MEMORY {
  RAM(rwx) : ORIGIN =  0x20000000, LENGTH = 256k
}

SECTIONS {

	/* The second stage will always enter the image at the start of .text.
         * The debugger will use the ELF entry point, which is the _entry_point
         * symbol if present, otherwise defaults to start of .text.
         * This can be used to transfer control back to the bootrom on debugger
         * launches only, to perform proper flash setup.
         */
        .bootloader : {
                . = ALIGN(4);
        } > RAM

        .text :  {
                . = ALIGN(4);
                *(.text)                /* Program code */
                *(.text.*)
                . = ALIGN(4);
        } >RAM                        /* this section is code, hence gos to RAM */

        .rodata : {
          . = ALIGN(4);
          _srodata = .;
          *(.rodata)
          *(.rodata.*)
          . = ALIGN(4);
        } >RAM
        .ARM.extab :                                                                
        {                                                                           
            *(.ARM.extab* .gnu.linkonce.armextab.*)                                 
        } > RAM                                                                   
                                                                                    
        __exidx_start = .;                                                          
        .ARM.exidx :                                                                
        {                                                                           
            *(.ARM.exidx* .gnu.linkonce.armexidx.*)                                 
        } > RAM                                                                   
        __exidx_end = .;                                                            
                                                                                    
        /* Machine inspectable binary information */                                
        . = ALIGN(4);                                                               
        __binary_info_start = .;                                                    
        .binary_info :                                                              
        {                                                                           
            KEEP(*(.binary_info.keep.*))                                            
            *(.binary_info.*)                                                       
        } > RAM                                                                   
        __binary_info_end = .;                                                      
        . = ALIGN(4);                                                               
                                                                                    
        /* End of .text-like segments */                                            
        __etext = .;

        _etext = .;             /* _etext is end of program code */
        .data : {                       /* AT specifies the LMA (load memory address) */
                . = ALIGN(4);           /* also, this section is for initialized
                                             global/static variables */
		__data_start__ = .;
                _sdata = .;
                *(.data)
                *(.data.*)
                . = ALIGN(4);
                PROVIDE_HIDDEN (__mutex_array_start = .);
                KEEP(*(SORT(.mutex_array.*)))
                KEEP(*(.mutex_array))
                PROVIDE_HIDDEN (__mutex_array_end = .);
       
                . = ALIGN(4);
                /* preinit data */
                PROVIDE_HIDDEN (__preinit_array_start = .);
                KEEP(*(SORT(.preinit_array.*)))
                KEEP(*(.preinit_array))
                PROVIDE_HIDDEN (__preinit_array_end = .);
       
                . = ALIGN(4);
                /* init data */
                PROVIDE_HIDDEN (__init_array_start = .);
                KEEP(*(SORT(.init_array.*)))
                KEEP(*(.init_array))
                PROVIDE_HIDDEN (__init_array_end = .);
       
                . = ALIGN(4);
                /* finit data */
                PROVIDE_HIDDEN (__fini_array_start = .);
                *(SORT(.fini_array.*))
                *(.fini_array)
                PROVIDE_HIDDEN (__fini_array_end = .);
       
                *(.jcr)
                . = ALIGN(4);
		__data_end__ = .;
                _edata = .;
        } >RAM

        .bss : {                        /* this section is for uninitialized global/static
                                        variables, which we set to zero */
                . = ALIGN(4);
		__bss_start__ = .;
                _sbss = .;              /* the . sets the symbol to value of current counter */
                *(.bss)
                *(COMMON)
                . = ALIGN(4);
		__bss_end__ = .;
                _ebss = .;
        } >RAM

        .stack : {
                 KEEP(*(.stack*))
                . = ALIGN(8);
                _sstack = .;
                . = . + 4096;
                . = ALIGN(8);
		__StackLimit = .;
                _estack = .;
       } >RAM

       .irq_stack : {
                 KEEP(*(.irq_stack*))
                . = ALIGN(8);
                _sirq_stack = .;
                . = . + 2048;
                . = ALIGN(8);
                _eirq_stack = .;
       } >RAM

        .heap : {
		__end__ = .;
		end = __end__;
                KEEP(*(.heap*))
                . = ALIGN(4);
                _sheap = .;
        } >RAM

      _eheap = ORIGIN(RAM) + LENGTH(RAM);
}
