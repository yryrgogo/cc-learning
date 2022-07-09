#include "holycc.h"

int rsp_offset = 0;

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
  rsp_offset = 0;
  char s[node->len + 1];
  memcpy(s, node->name, node->len);
  s[node->len] = '\0';
  printf("%s:\n", s);

  // Prologue
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");

  printf("  sub rsp, %d\n", node->total_offset);

  void gen_arg(Node * argv, char *register_name) {
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", argv->offset);
    printf("  mov [rax], %s\n", register_name);
  }

  // 引数を Stack に書き出す
  int args_count = node->args_num;
  for(Node *argv = node->args; argv; argv = argv->next) {
    char *reg = "";

    switch(args_count) {
    case 1:
      if(argv->ty->kind == TY_INT) {
        reg = "edi";
      } else {
        reg = "rdi";
      }
      gen_arg(argv, reg);
      break;
    case 2:
      if(argv->ty->kind == TY_INT) {
        reg = "esi";
      } else {
        reg = "rsi";
      }
      gen_arg(argv, reg);
      break;
    case 3:
      if(argv->ty->kind == TY_INT) {
        reg = "edx";
      } else {
        reg = "rdx";
      }
      gen_arg(argv, reg);
      break;
    case 4:
      if(argv->ty->kind == TY_INT) {
        reg = "ecx";
      } else {
        reg = "rcx";
      }
      gen_arg(argv, reg);
      break;
    case 5:
      if(argv->ty->kind == TY_INT) {
        reg = "r8d";
      } else {
        reg = "r8";
      }
      gen_arg(argv, reg);
      break;
    case 6:
      if(argv->ty->kind == TY_INT) {
        reg = "r9d";
      } else {
        reg = "r9";
      }
      gen_arg(argv, reg);
      break;
    }
    args_count--;
  }

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
    gen_expr(node->lhs, false);
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

    gen_expr(node->init, false);
    printf("  pop rax\n");
    adjust_rsp();
    printf("  jmp .L.for.begin.%d\n", c);

    printf(".L.for.begin.%d:\n", c);
    gen_expr(node->cond, false);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    adjust_rsp();
    printf("  je .L.for.end.%d\n", c);

    gen_stmt(node->then);

    gen_expr(node->inc, false);
    printf("  pop rax\n");
    adjust_rsp();
    printf("  jmp .L.for.begin.%d\n", c);

    printf(".L.for.end.%d:\n", c);
    return;
  }
  case ND_WHILE: {
    int c = count();

    adjust_rsp();
    printf("  jmp .L.while.begin.%d\n", c);
    printf(".L.while.begin.%d:\n", c);
    gen_expr(node->cond, false);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    adjust_rsp();
    printf("  je .L.while.end.%d\n", c);

    gen_stmt(node->then);
    adjust_rsp();
    printf("  jmp .L.while.begin.%d\n", c);

    printf(".L.while.end.%d:\n", c);
    return;
  }
  case ND_IF: {
    int c = count();
    gen_expr(node->cond, false);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");

    adjust_rsp();
    printf("  je .L.else.%d\n", c);

    gen_stmt(node->then);

    adjust_rsp();
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
    gen_expr(node, false);
    printf("  pop rax\n");
    return;
  }

  return;
}

/**
 * @brief expression は式のため、値を stack に1つ残す
 *
 * @param node
 */
void gen_expr(Node *node, bool is_dereference) {
  char *reg = "";
  switch(node->kind) {
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lvar_addr(node);

    if(node->ty->kind == TY_ARRAY &&
       (is_dereference || node->is_derefernce || !(node->has_index >= 0))) {
      return;
    }

    if(node->ty->kind == TY_INT) {
      reg = "eax";
    } else {
      reg = "rax";
    }

    printf("  pop rax\n");
    printf("  mov %s, [rax]\n", reg);
    printf("  push rax\n");
    return;

  case ND_ADDR:
    // TODO: &(*a) のような式はコンパイルできない
    gen_lvar_addr(node->lhs);
    return;
  case ND_DEREF:
    gen_expr(node->lhs, true);

    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN: {
    Type *ty = calloc(1, sizeof(Type));

    // TODO: switch のネストをリファクタ
    switch(node->lhs->kind) {
    case ND_LVAR:
      gen_lvar_addr(node->lhs);
      break;
    case ND_DEREF: {
      gen_lhs_deref(node->lhs);
      break;
    }
    }
    gen_expr(node->rhs, is_dereference);

    void set_reg(Type * ty, char **reg, char **prefix) {
      switch(ty->kind) {
      case TY_INT:
        *prefix = "DWORD PTR";
        *reg = "edi";
        break;
      case TY_PTR:
        *prefix = "";
        *reg = "rdi";
        break;
      case TY_ARRAY:
        set_reg(pointed_type(ty), reg, prefix);
        break;
      }
    }

    char *prefix = "";
    if(node->lhs->kind == ND_DEREF) {
      set_reg(ty, &reg, &prefix);
    } else {
      set_reg(node->lhs->ty, &reg, &prefix);
    }

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov %s [rax], %s\n", prefix, reg);

    printf("  push rax\n");
    return;
  }
  case ND_CALL:
    gen_func_call(node);
    printf("  push rax\n");
    return;
  default:
    if(node->lhs && node->rhs) {
      gen_calculator(node, is_dereference);
      printf("  push rax\n");
    }
    return;
  }
}

/**
 * @brief rax, rdi を使って演算を行う
 *
 * @param node
 */
void gen_calculator(Node *node, bool is_dereference) {
  // TODO: このやり方だと () でネストされた式をポインタと判定しないため要修正
  bool is_lhs_ptr = false;
  if(node->lhs->kind == ND_LVAR) {
    if(node->lhs->ty->kind == TY_PTR) {
      is_lhs_ptr = true;
    }
    if(node->lhs->ty->kind == TY_ARRAY && !node->lhs->has_index) {
      is_lhs_ptr = true;
    }
  }

  gen_expr(node->lhs, is_dereference);

  if((is_lhs_ptr) && node->rhs->kind == ND_NUM) {
    node->rhs->val = node->rhs->val * 4;
  }
  gen_expr(node->rhs, is_dereference);

  printf("  pop rdi\n");
  printf("  pop rax\n");

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
void gen_lvar_addr(Node *node) {
  if(node->kind != ND_LVAR) {
    printf("%s 代入の左辺値が変数ではありません。", __FILE__);
  }
  if(node->ty->kind == TY_INT && node->is_declaration) {
    rsp_offset += 4;
  }

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");

  if(node->ty->kind == TY_ARRAY && node->is_declaration) {
    printf("  pop rdi\n");
    printf("  mov [rax], rdi\n");
    printf("  push rax\n");
  }
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
  gen_expr(node, false);
  printf("  pop rax\n");
  printf("  mov %s, rax\n", register_name);
}

/**
 * @brief ポインタ型への代入式における左辺のコード生成（右辺とは異なる）
 *
 * @param node
 */
Type *gen_lhs_deref(Node *node) {
  switch(node->kind) {
  case ND_LVAR:
    gen_lvar_addr(node);
    if(node->ty->kind == TY_ARRAY) {
      return NULL;
    }
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    break;
  case ND_DEREF:
    gen_lhs_deref(node->lhs);
    break;
  case ND_ADD:
    gen_calculator(node, true);
    printf("  push rax\n");
    break;
  }

  return node->ty;
}

void adjust_rsp() {
  // rsp を8の倍数に合わせないと、je, jmp が50%くらいで失敗する
  if(rsp_offset % 8 == 4) {
    printf("  sub rsp, 4\n");
    rsp_offset += 4;
  }
}
