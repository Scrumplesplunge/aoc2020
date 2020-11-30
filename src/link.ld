SECTIONS {
  . = 0x10000;
  .text ALIGN(0x1) : { *(.text*) }
  .rodata ALIGN(0x4) : { *(.rodata*) }
  .data ALIGN(0x4) : { *(.data*) }
  __file_size = . - 0x10000;
  .bss ALIGN(0x4) : { *(.bss*) }
  __memory_size = . - 0x10000;
  /DISCARD/ : {
    *(*)
  }
}

OUTPUT_FORMAT(binary)
