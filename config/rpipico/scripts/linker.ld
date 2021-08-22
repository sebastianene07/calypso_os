

MEMORY {
  FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 2048k
  RAM(rwx) : ORIGIN =  0x20000000, LENGTH = 256k
  SCRATCH_X(rwx) : ORIGIN = 0x20040000, LENGTH = 4k
  SCRATCH_Y(rwx) : ORIGIN = 0x20041000, LENGTH = 4k
}

SECTIONS {
        /* Second stage bootloader is prepended to the image. It must be 256 bytes big
         * and checksummed. It is usually built by the boot_stage2 target
         * in the Raspberry Pi Pico SDK
         */

        .boot2 : {
          __boot2_start__ = .;
          KEEP (*(.boot2))
          __boot2_end__ = .;
        } > FLASH

        ASSERT(__boot2_end__ - __boot2_start__ == 256,
             "ERROR: Pico second stage bootloader must be 256 bytes in size")

        /* The second stage will always enter the image at the start of .text.
         * The debugger will use the ELF entry point, which is the _entry_point
         * symbol if present, otherwise defaults to start of .text.
         * This can be used to transfer control back to the bootrom on debugger
         * launches only, to perform proper flash setup.
         */
	. = 0x10000100;
	.isr_vector : {
      		KEEP (*(.isr_vector))
	} > FLASH

        .text :  {
                . = ALIGN(4);
                *(.text)                /* Program code */
                *(.text.*)
                . = ALIGN(4);
        } > FLASH

        .rodata : {
          . = ALIGN(4);
          _srodata = .;
          *(.rodata)
          *(.rodata.*)
          . = ALIGN(4);
        } >FLASH
        .ARM.extab :                                                                
        {                                                                           
            *(.ARM.extab* .gnu.linkonce.armextab.*)                                 
        } > FLASH                                                                   
                                                                                    
        __exidx_start = .;                                                          
        .ARM.exidx :                                                                
        {                                                                           
            *(.ARM.exidx* .gnu.linkonce.armexidx.*)                                 
        } > FLASH                                                                   
        __exidx_end = .;                                                            
                                                                                    
        /* Machine inspectable binary information */                                
        . = ALIGN(4);                                                               
        __binary_info_start = .;                                                    
        .binary_info :                                                              
        {                                                                           
            KEEP(*(.binary_info.keep.*))                                            
            *(.binary_info.*)                                                       
        } > FLASH                                                                   
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
        } >RAM AT> FLASH                     /* this section go into this RAM */

       /* Start and end symbols must be word-aligned */                            
       .scratch_x : {                                                              
           __scratch_x_start__ = .;                                                
           *(.scratch_x.*)                                                         
           . = ALIGN(4);                                                           
           __scratch_x_end__ = .;                                                  
       } > SCRATCH_X AT > FLASH                                                    
       __scratch_x_source__ = LOADADDR(.scratch_x);                                
                                                                                   
       .scratch_y : {                                                              
           __scratch_y_start__ = .;                                                
           *(.scratch_y.*)                                                         
           . = ALIGN(4);                                                           
           __scratch_y_end__ = .;                                                  
       } > SCRATCH_Y AT > FLASH                                                    
       __scratch_y_source__ = LOADADDR(.scratch_y);

        .bss : {                        /* this section is for uninitialized global/static
                                        variables, which we set to zero */
                . = ALIGN(4);
		__bss_start__ = .;
                _sbss = .;              /* the . sets the symbol to value of current counter */
		. = ALIGN(4);
		__bss_end__ = .;
                _ebss = .;
        } >RAM
/*
        .stack : {
                 KEEP(*(.stack*))
                . = ALIGN(8);
                _sstack = .;
                . = . + 4096;
                . = ALIGN(8);
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
*/
		__StackLimit = .;
        .heap : {
/*
*/
                . = ALIGN(4);
		_sheap = .;
                KEEP(*(.heap*))
		__end__ = .;
		end = __end__;
        } >RAM
      _eheap = ORIGIN(RAM) + LENGTH(RAM) - 2048;
      __StackTop = ORIGIN(SCRATCH_Y) + LENGTH(SCRATCH_Y);
}
