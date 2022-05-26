.intel_syntax noprefix
.globl main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  push 0
  push 0
  push 10
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
