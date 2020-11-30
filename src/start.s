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
.global exit
exit:
  push %ebx
  mov 8(%esp), %ebx  # exit code
  mov $1, %eax
  int $0x80
  pop %ebx
  ret
.global read
read:
  push %ebx
  mov 8(%esp), %ebx   # fd
  mov 12(%esp), %ecx  # buffer
  mov 16(%esp), %edx  # size
  mov $3, %eax
  int $0x80
  pop %ebx
  ret
.global write
write:
  push %ebx
  mov 8(%esp), %ebx   # fd
  mov 12(%esp), %ecx  # buffer
  mov 16(%esp), %edx  # size
  mov $4, %eax
  int $0x80
  pop %ebx
  ret
