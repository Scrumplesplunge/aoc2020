.section .text
.global _start
_start:
  pop %esi
  push %esp  # argv
  push %esi  # argc
  call main
  mov %eax, %ebx
  mov $1, %eax
  int $0x80
