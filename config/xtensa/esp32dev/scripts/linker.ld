/* Specify main memory areas */
MEMORY
{
  /* Use values from the ESP-IDF 'bootloader' component.
  /* TODO: Use human-readable lengths */
  /* TODO: Use the full memory map - this is just a test */
  iram_seg ( RX )       : ORIGIN = 0x40080400, len = 0xFC00
  dram_seg ( RW )       : ORIGIN = 0x3FFF0000, len = 0xFFFF
}

/* Define output sections */
SECTIONS {
  /* The program code and other data goes into Instruction RAM */
  .iram.text :
  {
    . = ALIGN(16);
    KEEP(*(.entry.text))
    *(.text)
    *(.text*)
    KEEP (*(.init))
    KEEP (*(.fini))
    *(.rodata)
    *(.rodata*)

    . = ALIGN(4);
    _etext = .;
  } >iram_seg

  /* Initialized data goes into Data RAM */
  _sidata = .;
  .data : AT(_sidata)
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)

    . = ALIGN(4);
    _edata = .;
  } >dram_seg

  /* Uninitialized data also goes into Data RAM */
  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;
  } >dram_seg

  .heap : {
    KEEP(*(.heap*))
    . = ALIGN(4);
    _sheap = .;
  } >dram_seg

  .stack : {
           KEEP(*(.stack*))
          . = ALIGN(4);
          _sstack = .;
          . = . + 1024;
          . = ALIGN(4);
          _estack = .;
 } >dram_seg

  _eheap = ORIGIN(dram_seg) + LENGTH(dram_seg) - 1024;

  . = ALIGN(4);
  PROVIDE ( end = . );
  PROVIDE ( _end = . );
}
