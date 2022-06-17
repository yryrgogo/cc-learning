.intel_syntax noprefix
.globl main
foo:
  push rbp
  mov rbp, rsp
  mov rax, rbp
  sub rax, 8
  mov [rax], rdi
  sub rsp, 16
  push 0
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .L.else.1
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .L.else.2
  push 33
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .L.end.2
.L.else.2:
.L.end.2:
  jmp .L.end.1
.L.else.1:
.L.end.1:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rax
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
main:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  push 15
  pop rax
  mov rdi, rax
  call foo
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
