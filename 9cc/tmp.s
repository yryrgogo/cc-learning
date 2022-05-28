.intel_syntax noprefix
.globl main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  push 1
  pop rax
  cmp rax, 0
  je .L.else.1
  push 11
  jmp .L.end.1
.L.else.1:  push 5
.L.end.1:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
