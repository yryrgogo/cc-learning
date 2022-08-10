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
  case ND_GVAR: {
    // TODO: data section が上部に集まらないとこれではエラーになる
    printf(".data\n");
    gen_gvar(node);
    break;
  }
  default: {
    error_at(NULL, "not implemented.");
  }
  }

  return;
}

// generate data section for global variables from node
void gen_gvar(Node *node) {
  char name[node->len + 1];
  memcpy(name, node->name, node->len);
  name[node->len] = '\0';
  printf("%s:\n", name);

  switch(node->ty->kind) {
  case TY_CHAR: {
    printf("  .byte %d\n", 0);
    break;
  }
  case TY_INT: {
    printf("  .int %d\n", size_of_type(node->ty));
    break;
  }
  case TY_PTR: {
    printf("  .zero %d\n", size_of_type(node->ty));
    break;
  }
  case TY_ARRAY: {
    printf("  .zero %d\n", size_of_type(node->ty));
    break;
  }
  default: {
    printf("  .zero %d\n", 0);
    break;
  }
  }
  printf("  .text\n");
}

void gen_func(Node *node) {
  char *reg = "";
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

  return;
}

/**
 * @brief statement は expression が stack
 * に残した値を使って処理を組み立てる。gen_expr() のあとは pop rax
 * で式の結果を rax にセットし後続処理を行うことが多い
 *
 * @param node
 */
void gen_stmt(Node *node) {
  switch(node->kind) {
  case ND_RETURN:
    if(node->lhs->kind == ND_GVAR) {
      Node *lhs = node->lhs;
      char name[lhs->len + 1];
      memcpy(name, lhs->name, lhs->len);
      name[lhs->len] = '\0';

      printf("  mov rax, [rip+%s]\n", name);
      printf("  push rax\n");
    } else {
      gen_expr(node->lhs, false);
    }
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
  switch(node->kind) {
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_STR:
    printf("  push %d\n", 1);
    return;
  case ND_GVAR:
    gen_gvar_value(node);
    gen_var_preprocess(node, is_dereference);
    return;
  case ND_LVAR:
    gen_lvar_addr(node);
    gen_var_preprocess(node, is_dereference);
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
    // グローバル変数のアドレスをraxにセットして取り出す方法がわからないため分けている
    if(node->lhs->kind == ND_GVAR) {
      gen_gvar_assign(node);
      return;
    }

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

    if(node->rhs->ty && node->rhs->ty->kind == TY_ARRAY &&
       node->lhs->ty->kind == TY_PTR) {
      gen_lvar_addr(node->rhs);
    } else {
      gen_expr(node->rhs, is_dereference);
    }

    char *reg = "";
    char *prefix = "";
    if(node->lhs->kind == ND_DEREF) {
      set_register_name(deref_type(node->lhs), &reg, &prefix);
    } else {
      set_register_name(node->lhs->ty, &reg, &prefix);
    }

    printf("  pop rdi\n");
    printf("  pop rax\n");

    char *acc_reg = "";
    switch(node->lhs->kind) {
    case ND_GVAR:
      if(node->lhs->ty->kind == TY_INT) {
        acc_reg = "eax";
      } else {
        acc_reg = "rax";
      }
      printf("  mov %s, %s\n", acc_reg, reg);
      break;
    default: {
      printf("  mov %s [rax], %s\n", prefix, reg);
      break;
    }
    }

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

void gen_gvar_assign(Node *node) {
  gen_expr(node->rhs, false);
  printf("  pop rax\n");

  Node *lhs = node->lhs;
  char name[lhs->len + 1];
  memcpy(name, lhs->name, lhs->len);
  name[lhs->len] = '\0';

  char *prefix = "";
  char *register_name = "";

  if(lhs->ty->kind == TY_INT) {
    prefix = "DWORD PTR ";
    register_name = "eax";
  } else if(lhs->ty->kind == TY_CHAR) {
    prefix = "BYTE PTR ";
    register_name = "al";
  } else {
    prefix = "";
    register_name = "rax";
  }

  printf("  mov %s[rip+%s], %s\n", prefix, name, register_name);
  printf("  push [rip+%s]\n", name);
}

void gen_var_preprocess(Node *node, bool is_dereference) {
  char *reg = "";
  // TODO: 配列へのポインタの場合も後続処理をスキップする
  if(node->ty->kind == TY_ARRAY &&
     (is_dereference || node->is_derefernce || !(node->has_index >= 0))) {
    return;
  }
  if(node->ty->ptr_to && node->ty->ptr_to->kind == TY_ARRAY) {
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
}

void set_register_name(Type *ty, char **reg, char **prefix) {
  switch(ty->kind) {
  case TY_INT:
    *prefix = "DWORD PTR";
    *reg = "edi";
    break;
  case TY_PTR:
    *prefix = "";
    *reg = "rdi";
    break;
  case TY_CHAR:
    *prefix = "BYTE PTR";
    *reg = "dil";
    break;
  case TY_ARRAY:
    set_register_name(pointed_type(ty), reg, prefix);
    break;
  }
}

Type *deref_type(Node *node) {
  if(node->kind == ND_DEREF) {
    return deref_type(node->lhs);
  } else if(node->kind == ND_ADD) {
    return deref_type(node->lhs);
  } else if(node->ty->kind == TY_PTR) {
    return pointed_type(node->ty);
  } else {
    return node->ty;
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
  if((is_lhs_ptr) && node->rhs->kind == ND_NUM) {
    node->rhs->val = node->rhs->val * 4;
  }

  bool is_rhs_ptr = false;
  if(node->rhs->kind == ND_LVAR) {
    if(node->rhs->ty->kind == TY_PTR) {
      is_rhs_ptr = true;
    }
    if(node->rhs->ty->kind == TY_ARRAY && !node->rhs->has_index) {
      is_rhs_ptr = true;
    }
  }
  if((is_rhs_ptr) && node->lhs->kind == ND_NUM) {
    node->lhs->val = node->lhs->val * 4;
  }

  gen_expr(node->lhs, is_dereference);
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
 * @brief
 * ベースポインタからのオフセットでローカル変数のアドレスを算出し、stack push
 * する
 *
 * @param node
 */
void gen_lvar_addr(Node *node) {
  if(node->kind != ND_LVAR) {
    printf("%s 代入の左辺値が変数ではありません。", __FILE__);
  }
  if(node->ty && node->ty->kind == TY_INT && node->is_declaration) {
    rsp_offset += 4;
  }

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");

  // TODO: ARRAY の変数宣言では、この命令が追加で必要だが理由を忘れた
  // addressをpushするだけでなく、そのaddressにはaddressそのものをセットしておく
  if(node->ty && node->ty->kind == TY_ARRAY && node->is_declaration) {
    char *prefix = "";
    char *reg = "";
    if(node->ty->ptr_to->kind != TY_PTR) {
      set_register_name(node->ty->ptr_to, &reg, &prefix);
    } else {
      set_register_name(node->ty->ptr_to->ptr_to, &reg, &prefix);
    }
    
    printf("  mov %s [rax], %s\n", prefix, reg);
    printf("  push rax\n");
  }
}

void gen_gvar_value(Node *node) {
  char name[node->len + 1];
  memcpy(name, node->name, node->len);
  name[node->len] = '\0';

  if(node->ty->kind == TY_PTR) {
    printf("  lea rax, [rip + %s]\n", name);
    printf("  push rax\n");
  } else {
    printf("  mov rax, [rip + %s]\n", name);
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
void *gen_lhs_deref(Node *node) {
  switch(node->kind) {
  case ND_LVAR:
    gen_lvar_addr(node);
    if(node->ty && node->ty->kind == TY_ARRAY) {
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
