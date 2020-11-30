.section .text
elf_header:
  .byte 0x7f, 'E', 'L', 'F'          # Magic constant
  .byte 1, 1, 1                      # 32-bit, little-endian, ELF
  .fill 9, 1, 0                      # Unused ABI bytes, padding
  .short 2                           # Executable file
  .short 3                           # x86
  .long 1                            # ELF version
  .long _start                       # Entry point
  .long program_header - elf_header  # Program header offset
  .long 0                            # Section header offset.
  .long 0                            # Flags
  .short 52                          # ELF header size
  .short 32                          # Program header entry size
  .short 1                           # Program header entry count
  .short 0                           # Section header entry size
  .short 0                           # Section header entry count
  .short 0                           # Section name section

program_header:
  .long 1                            # Loadable section
  .long 0                            # Segment offset
  .long 0x10000                      # Virtual address
  .long 0x10000                      # Physical address
  .long __file_size                  # Size of segment in the file
  .long __memory_size                # Size of segment in memory
  .long 7                            # 1 (exec) | 2 (writable) | 4 (readable)
  .long 0x1000                       # Alignment
