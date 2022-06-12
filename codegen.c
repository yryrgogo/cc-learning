#include "holycc.h"

static int count(void) {
  static int i = 1;
  return i++;
}

/**
 * @brief コードの top level にある Node を処理する
 *
 * @param node
 */
void gen(Node *node) {
  switch(node->kind) {
  case ND_FUNC: {
    gen_func(node);
    break;
  }
  default: {
    error_at(NULL, "not implemented.");
  }
  }

  return;
}

void gen_func(Node *node) {
  char s[node->len + 1];
  memcpy(s, node->name, node->len);
  s[node->len] = '\0';
  printf("%s:\n", s);

  // Prologue
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");

  void gen_arg(Node * argv, char *register_name) {
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", argv->offset);
    printf("  mov [rax], %s\n", register_name);
  }

  // 引数を Stack に書き出す
  int args_count = 0;
  for(Node *argv = node->args; argv; argv = argv->next) {
    switch(args_count) {
    case 0:
      gen_arg(argv, "rdi");
      break;
    case 1:
      gen_arg(argv, "rsi");
      break;
    case 2:
      gen_arg(argv, "rdx");
      break;
    case 3:
      gen_arg(argv, "rcx");
      break;
    case 4:
      gen_arg(argv, "r8");
      break;
    case 5:
      gen_arg(argv, "r9");
      break;
    }
    args_count++;
  }

  int locals_count = 0;
  for(LVar *var = node->locals; var; var = var->next) {
    locals_count++;
  }
  if(locals_count > 0)
    printf("  sub rsp, %d\n", locals_count * 8);

  gen_stmt(node->body);

  // Epilogue
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");

  return;
}

/**
 * @brief statement は expression が stack
 * に残した値を使って処理を組み立てる。gen_expr() のあとは pop rax で式の結果を
 * rax にセットし後続処理を行うことが多い
 *
 * @param node
 */
void gen_stmt(Node *node) {
  switch(node->kind) {
  case ND_RETURN:
    gen_expr(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_BLOCK: {
    Node *cur = node->body;
    while(cur) {
      gen_stmt(cur);
      cur = cur->next;
    }
    return;
  }
  case ND_FOR: {
    int c = count();

    gen_expr(node->init);
    printf("  pop rax\n");
    printf("  jmp .L.for.begin.%d\n", c);

    printf(".L.for.begin.%d:\n", c);
    gen_expr(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .L.for.end.%d\n", c);

    gen_stmt(node->then);

    gen_expr(node->inc);
    printf("  pop rax\n");
    printf("  jmp .L.for.begin.%d\n", c);

    printf(".L.for.end.%d:\n", c);
    return;
  }
  case ND_WHILE: {
    int c = count();

    printf("  jmp .L.while.begin.%d\n", c);
    printf(".L.while.begin.%d:\n", c);
    gen_expr(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .L.while.end.%d\n", c);

    gen_stmt(node->then);
    printf("  jmp .L.while.begin.%d\n", c);

    printf(".L.while.end.%d:\n", c);
    return;
  }
  case ND_IF: {
    int c = count();
    gen_expr(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .L.else.%d\n", c);

    gen_stmt(node->then);

    printf("  jmp .L.end.%d\n", c);
    printf(".L.else.%d:\n", c);
    if(node->els) {
      gen_stmt(node->els);
    }

    printf(".L.end.%d:\n", c);
    return;
  }
  case ND_NOP: {
    printf("  nop\n");
    return;
  }
  default:
    gen_expr(node);
    printf("  pop rax\n");
  }

  return;
}

/**
 * @brief expression は式のため、値を stack に1つ残す
 *
 * @param node
 */
void gen_expr(Node *node) {
  switch(node->kind) {
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen_expr(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rax\n");
    return;
  case ND_FUNC_CALL:
    gen_func_call(node);
    printf("  push rax\n");
    return;
  default:
    gen_calculator(node);
    printf("  push rax\n");
    return;
  }
}

/**
 * @brief rax, rdi を使って演算を行う
 *
 * @param node
 */
void gen_calculator(Node *node) {
  if(node->lhs && node->rhs) {
    gen_expr(node->lhs);
    gen_expr(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
  }

  switch(node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NE:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LE:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  }
}

/**
 * @brief ベースポインタからのオフセットでローカル変数のアドレスを算出し、stack
 * push する
 *
 * @param node
 */
void gen_lval(Node *node) {
  if(node->kind != ND_LVAR)
    error_at(__FILE__, "代入の左辺値が変数ではありません。");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen_func_call(Node *node) {
  if(node->args_num > 0) {
    int arg_no = node->args_num;
    for(Node *argv = node->args; argv; argv = argv->next) {
      switch(arg_no) {
      case 1:
        gen_func_call_arg(argv, "rdi");
        break;
      case 2:
        gen_func_call_arg(argv, "rsi");
        break;
      case 3:
        gen_func_call_arg(argv, "rdx");
        break;
      case 4:
        gen_func_call_arg(argv, "rcx");
        break;
      case 5:
        gen_func_call_arg(argv, "r8");
        break;
      case 6:
        gen_func_call_arg(argv, "r9");
        break;
      }
      arg_no--;
    }
  }
  char s[node->len + 1];
  memcpy(s, node->name, node->len);
  s[node->len] = '\0';
  printf("  call %s\n", s);
  return;
}

/**
 * @brief 関数呼び出しにおける引数に与えられた式をアセンブラに変換
 *
 * @param node
 */
void gen_func_call_arg(Node *node, char *register_name) {
  gen_expr(node);
  printf("  pop rax\n");
  printf("  mov %s, rax\n", register_name);
}
