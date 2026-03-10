.intel_syntax noprefix
.text
.global _main
.extern _printf
.extern _scanf
_main:
  push ebp
  mov ebp, esp
  sub esp, 64
  mov eax, 0
  mov dword ptr [ebp-8], eax
  mov eax, 0
  mov dword ptr [ebp-16], eax
.L_while_start0:
  mov eax, 5
  push eax
  mov eax, dword ptr [ebp-8]
  pop ecx
  cmp eax, ecx
  setl al
  movzx eax, al
  cmp eax, 0
  je .L_while_end1
  mov eax, dword ptr [ebp-8]
  push eax
  mov eax, dword ptr [ebp-16]
  pop ecx
  add eax, ecx
  mov dword ptr [ebp-16], eax
  mov eax, 1
  push eax
  mov eax, dword ptr [ebp-8]
  pop ecx
  add eax, ecx
  mov dword ptr [ebp-8], eax
  jmp .L_while_start0
.L_while_end1:
  mov eax, dword ptr [ebp-16]
  push eax
  lea eax, [LC0]
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
LC0:
  .asciz "The loop computed: %d\n"
