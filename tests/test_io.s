.intel_syntax noprefix
.text
.global _main
.extern _printf
.extern _scanf
_main:
  push ebp
  mov ebp, esp
  sub esp, 64
  mov dword ptr [ebp-8], 0
  lea eax, [LC0]
  push eax
  call _printf
  add esp, 4
  lea eax, [ebp-8]
  push eax
  lea eax, [LC1]
  push eax
  call _scanf
  add esp, 8
  mov eax, 2
  push eax
  mov eax, dword ptr [ebp-8]
  pop ecx
  imul eax, ecx
  mov dword ptr [ebp-16], eax
  mov eax, dword ptr [ebp-16]
  push eax
  lea eax, [LC2]
  push eax
  call _printf
  add esp, 8
  mov eax, 0
  leave
  ret
  mov eax, 0
  leave
  ret

.data
LC2:
  .asciz "Double your number is: %d\n"
LC1:
  .asciz "%d"
LC0:
  .asciz "Enter a number: "
