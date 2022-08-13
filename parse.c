#include "holycc.h"

static LVar *locals;
static LVar *locals_head;
static GVar *globals;
static GVar *globals_head;

Node *code[100];
Token *token;
extern HashMap *str_literal_map;
HashMap *str_literal_map;

int local_offset = 0;
int lvar_count = 0;

void program() {
  str_literal_map = calloc(1, sizeof(HashMap));
  globals = NULL;
  int i = 0;

  while(!at_eof()) {
    Node *node = toplevel();
    code[i++] = node;
  }
  code[i] = NULL;
}

Node *toplevel() {
  HashMap *lvar_map = calloc(1, sizeof(HashMap));
  Token *tok;
  tok = consume_type();
  if(!tok) {
    error_at(NULL, "TK_TYPE が必須です。");
  }
  Type *ty = new_type(tok, NULL, 0);

  tok = consume_ident();
  Node *node = calloc(1, sizeof(Node));

  node->name = tok->str;
  node->len = tok->len;

  if(consume("*")) {
    Type *ptr = calloc(1, sizeof(Type));
    ty = pointer_type(ty, ptr);
  }
  node->ty = ty;

  if(consume("(")) {
    int args_offset_total = 0;
    int args_count = 0;
    locals = NULL;
    node->args =
        func_args_definition(&args_count, &args_offset_total, lvar_map);
    node->args_num = args_count;

    if(equal(token, "{")) {
      node->kind = ND_FUNC;
      local_offset = args_offset_total;
      node->body = stmt(lvar_map);
      node->locals = locals;
    } else {
      error_at(token->str, "toplevel に定義できる構文になっていません。");
    }

    int max_offset = 0;
    for(LVar *var = locals; var; var = var->next) {
      max_offset = max(max_offset, var->offset);
    }
    update_lvar_offset(node, lvar_map, local_offset);
    node->total_offset = local_offset;
    local_offset = 0;
  }
  // global array
  else if(consume("[")) {
    node->kind = ND_LVAR;
    // FIXME:
    exit(1);
  }
  // global var
  else {
    node->kind = ND_GVAR;
    // char gvar_key[node->len + 1];
    // memcpy(gvar_key, node->name, node->len);
    // gvar_key[node->len] = '\0';

    GVar *gvar = find_gvar(tok);
    if(!gvar) {
      gvar = calloc(1, sizeof(GVar));
      gvar->name = tok->str;
      gvar->len = tok->len;
      gvar->ty = ty;
    }
    if(!globals) {
      globals = gvar;
      globals_head = gvar;
    } else {
      globals_head->next = gvar;
      globals_head = gvar;
    }
    if(consume("="))
      node = new_binary(ND_ASSIGN, node, equality(NULL));
    expect(";");
  }

  return node;
}

// stmt = expr ";"
//        | "{" stmt* "}"
//        | "if" "(" expr ")" stmt("else" stmt)?
//        | "while" "(" expr ")" stmt
//        | "for" "(" expr ? ";" expr ? ";" expr ? ")" stmt
Node *stmt(HashMap *lvar_map) {
  Node *node;

  if(consume("{")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
    Node head;
    Node *cur = &head;

    bool has_stmt = false;
    while(!consume("}")) {
      cur->next = stmt(lvar_map);
      cur = cur->next;
      has_stmt = true;
    }

    if(has_stmt) {
      node->body = head.next;
    } else {
      Node *body = calloc(1, sizeof(Node));
      body->kind = ND_NOP;
      node->body = body;
    }
  } else if(consume_token(TK_RETURN)) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr(lvar_map);
    expect(";");
  } else if(consume_token(TK_IF)) {
    expect("(");

    node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    node->cond = expr(lvar_map);

    expect(")");

    node->then = stmt(lvar_map);

    if(consume_token(TK_ELSE)) {
      node->els = stmt(lvar_map);
    }
  } else if(consume_token(TK_FOR)) {
    expect("(");

    node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;
    node->init = expr(lvar_map);
    expect(";");

    node->cond = expr(lvar_map);
    expect(";");

    node->inc = expr(lvar_map);
    expect(")");

    node->then = stmt(lvar_map);
  } else if(consume_token(TK_WHILE)) {
    expect("(");

    node = calloc(1, sizeof(Node));
    node->kind = ND_WHILE;
    node->cond = expr(lvar_map);

    expect(")");

    node->then = stmt(lvar_map);
  } else {
    node = expr(lvar_map);
    expect(";");
  }

  return node;
}

Node *expr(HashMap *lvar_map) { return assign(lvar_map); }

Node *assign(HashMap *lvar_map) {
  Node *node = equality(lvar_map);

  if(consume("="))
    node = new_binary(ND_ASSIGN, node, equality(lvar_map));
  return node;
}

Node *equality(HashMap *lvar_map) {
  Node *node = relational(lvar_map);

  for(;;) {
    if(consume("=="))
      node = new_binary(ND_EQ, node, relational(lvar_map));
    else if(consume("!="))
      node = new_binary(ND_NE, node, relational(lvar_map));
    else
      return node;
  }
}

Node *relational(HashMap *lvar_map) {
  Node *node = add(lvar_map);

  for(;;) {
    if(consume("<="))
      node = new_binary(ND_LE, node, add(lvar_map));
    else if(consume(">="))
      node = new_binary(ND_LE, add(lvar_map), node);
    else if(consume("<"))
      node = new_binary(ND_LT, node, add(lvar_map));
    else if(consume(">"))
      node = new_binary(ND_LT, add(lvar_map), node);
    else
      return node;
  }
}

Node *add(HashMap *lvar_map) {
  Node *node = mul(lvar_map);

  for(;;) {
    if(consume("+")) {
      node = new_binary(ND_ADD, node, mul(lvar_map));
    } else if(consume("-")) {
      node = new_binary(ND_SUB, node, mul(lvar_map));
    } else {
      return node;
    }
  }
}

Node *mul(HashMap *lvar_map) {
  Node *node = array(lvar_map);

  for(;;) {
    if(consume("*"))
      node = new_binary(ND_MUL, node, array(lvar_map));
    else if(consume("/"))
      node = new_binary(ND_DIV, node, array(lvar_map));
    else
      return node;
  }
}

/**
 * @brief a[1] といった添字による配列アクセスを *(a + 1) に置換する
 *
 * @param lvar_map
 * @return Node*
 */
Node *array(HashMap *lvar_map) {
  Node *node = unary(lvar_map);

  for(;;) {
    if(consume("[")) {
      node = new_unary(ND_DEREF, new_binary(ND_ADD, node, expr(lvar_map)));
      expect("]");
    } else
      return node;
  }
}

/**
 * @brief unary =
 * | "+"? primary
 * | "-"? primary
 * | "*" unary
 * | "&" unary
 * | sizeof unary
 * @return Node*
 */
Node *unary(HashMap *lvar_map) {
  if(consume("+"))
    return primary(lvar_map);
  if(consume("-"))
    return new_binary(ND_SUB, new_num(0), unary(lvar_map));
  if(consume("&"))
    return new_unary(ND_ADDR, unary(lvar_map));
  if(consume("*"))
    return new_unary(ND_DEREF, unary(lvar_map));
  if(consume("sizeof")) {
    Node *node = unary(lvar_map);
    return new_num(size_of_type(node->ty));
  }

  return primary(lvar_map);
}

// primary = num
//         | str
//         | type ident
//         | ident ("(" ")")?
//         | "(" expr ")"
Node *primary(HashMap *lvar_map) {
  // 次のトークンが "(" なら、"(" expr ")" のはず
  if(consume("(")) {
    Node *node = expr(lvar_map);
    expect(")");
    return node;
  } else if(equal_token(TK_TYPE)) {
    Node *node = ident_declaration(lvar_map);
    if(consume("=")) {
      node = new_binary(ND_ASSIGN, node, expr(lvar_map));
    }
    return node;
  }

  Token *tok = consume_ident();
  // 関数呼び出し
  if(tok && consume("(")) {
    Node *node = func_call(tok, lvar_map);
    return node;
  } else if(tok) {
    Node *node = local_variable(tok, NULL, lvar_map);
    return node;
  }

  if(consume("\"")) {
    Node *node = new_str(expect_string());
    expect("\"");
    return node;
  } else if(consume("\'")) {
    Node *node = new_char(expect_char());
    expect("\'");
    return node;
  }

  return new_num(expect_number());
}

Node *ident_declaration(HashMap *lvar_map) {
  Type *ty = calloc(1, sizeof(Type));
  Type *cur = ty;

  if(startswith(token->str, "int")) {
    ty->kind = TY_INT;
  } else if(startswith(token->str, "char")) {
    ty->kind = TY_CHAR;
  } else {
    error_at(NULL, "not implemented.");
  }
  consume_token(TK_TYPE);

  while(consume("*")) {
    cur = pointer_type(ty, cur);
  }

  Token *tok = consume_ident();
  if(!tok) {
    error_at(NULL, "TK_TYPE の後には TK_IDENT が必須です。");
  }

  Node *node = local_variable(tok, cur, lvar_map);
  node->is_declaration = true;

  return node;
}

Type *pointer_type(Type *ty, Type *cur) {
  Type *ptr_ty = calloc(1, sizeof(Type));
  ptr_ty->kind = TY_PTR;
  if(!cur) {
    ptr_ty->ptr_to = ty;
    cur = ptr_ty;
  } else {
    ptr_ty->ptr_to = cur;
    cur = ptr_ty;
  }
  return cur;
}

Node *local_variable(Token *tok, Type *ty, HashMap *lvar_map) {
  Node *node = calloc(1, sizeof(Node));

  // LVar は今のところアクティベーションレコードの領域の計算と ND_LVAR Node
  // のオフセットの計算にしか使っていない Is it predefined local variable or
  // not?
  LVar *lvar = find_lvar(tok);

  char *var_key = calloc(1, sizeof(char) * (tok->len + 1));
  memcpy(var_key, tok->str, tok->len);
  var_key[tok->len] = '\0';

  GVar *gvar = find_gvar(tok);

  if(!lvar && !gvar) {
    lvar = calloc(1, sizeof(LVar));
    lvar->name = tok->str;
    lvar->len = tok->len;
    lvar->ty = ty;

    // array
    if(consume("[")) {
      Node *size = unary(lvar_map);
      expect("]");
      Type *array_ty = calloc(1, sizeof(Type));
      array_ty->kind = TY_ARRAY;
      array_ty->array_size = size->val;
      array_ty->ptr_to = ty;
      lvar->ty = array_ty;
    }

    if(!locals) {
      if(lvar->ty->kind == TY_ARRAY) {
        // 先頭は配列の型のサイズ
        lvar->offset = size_of_type(lvar->ty->ptr_to) + local_offset;
        // オフセットは配列のサイズ分足す
        local_offset += size_of_type(lvar->ty);
      } else {
        lvar->offset = size_of_type(lvar->ty) + local_offset;
        local_offset = lvar->offset;
      }
      locals = lvar;
      locals_head = locals;
    } else if(lvar->ty) {
      if(lvar->ty->kind == TY_ARRAY) {
        lvar->offset = local_offset + size_of_type(lvar->ty->ptr_to);
        local_offset += size_of_type(lvar->ty);
      } else {
        lvar->offset = local_offset + size_of_type(lvar->ty);
        local_offset = lvar->offset;
      }
      locals_head->next = lvar;
      locals_head = lvar;
    } else {
      error_at(tok->str, "ローカル変数の型が存在しません。");
    }
    lvar_count++;
    node->kind = ND_LVAR;
    hashmap_put(lvar_map, var_key, size_of_type(lvar->ty));
  }

  if(lvar) {
    node->offset = lvar->offset;
    node->ty = lvar->ty;
    node->kind = ND_LVAR;
  } else if(gvar) {
    node->ty = gvar->ty;
    node->kind = ND_GVAR;
  }
  node->name = var_key;
  node->len = tok->len;

  return node;
}

Node *func_args_definition(int *args_count, int *args_offset_total,
                           HashMap *lvar_map) {
  Node *cur = calloc(1, sizeof(Node));

  for(;;) {
    // arguments
    if(equal_token(TK_TYPE)) {
      (*args_count)++;
      Node *param = primary(lvar_map);
      param->next = cur;
      *args_offset_total += param->offset;
      cur = param;
      consume(",");
    } else if(consume(")")) {
      break;
    } else {
      error_at(NULL, "TK_TYPE が必須です。");
    }

    if(consume(")")) {
      break;
    }
  }

  return cur;
}

Node *func_call(Token *tok, HashMap *lvar_map) {
  Node *node = calloc(1, sizeof(Node));
  node->name = tok->str;
  node->len = tok->len;
  node->args = func_call_args(node, lvar_map);

  node->kind = ND_CALL;

  return node;
}

Node *func_call_args(Node *node, HashMap *lvar_map) {
  Node *cur = calloc(1, sizeof(Node));
  int count = 0;

  for(;;) {
    // arguments
    if(equal_token(TK_NUM) || equal_token(TK_IDENT) || equal(token, "&")) {
      Node *param = expr(lvar_map);
      if(count > 0) {
        // この繋ぎ方が肝で、1->2->3,,,でなく3->2->1のように前に繋がれていく
        param->next = cur;
      }
      cur = param;
      count++;

      consume(",");
    } else if(consume(")")) {
      break;
    } else {
      break;
    }
  }
  node->args_num = count;

  return cur;
}

LVar *find_lvar(Token *tok) {
  for(LVar *var = locals; var; var = var->next) {
    if(var->len == tok->len && !memcmp(tok->loc, var->name, var->len))
      return var;
  }
  return NULL;
}

GVar *find_gvar(Token *tok) {
  for(GVar *var = globals; var; var = var->next) {
    if(var->len == tok->len && !memcmp(tok->loc, var->name, var->len))
      return var;
  }
  return NULL;
}

int size_of_type(Type *ty) {
  if(ty->kind == TY_INT) {
    return 4;
  } else if(ty->kind == TY_CHAR) {
    return 1;
  } else if(ty->kind == TY_STR) {
    return 8;
  } else if(ty->kind == TY_PTR && !ty->array_size) {
    return 8;
  } else if(ty->kind == TY_PTR && ty->array_size) {
    return ty->array_size * 8;
  } else if(ty->kind == TY_ARRAY) {
    return ty->array_size * size_of_type(ty->ptr_to);
  } else {
    error_at(NULL, "sizeof は int, ptr, array のサイズを返すことができます。");
    exit(1);

    return -1;
  }
}

Type *pointed_type(Type *ty) {
  if(ty->ptr_to) {
    Type *last_ty = pointed_type(ty->ptr_to);

    return last_ty;
  } else {
    return ty;
  }
}

Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;

  return node;
}

Node *new_num(int val) {
  Node *node = new_node(ND_NUM);
  Type *ty = calloc(1, sizeof(Type));
  ty->kind = TY_INT;

  node->val = val;
  node->ty = ty;

  return node;
}

Node *new_char(Token *tok) {
  Node *node = new_node(ND_CHAR);
  Type *ty = calloc(1, sizeof(Type));
  ty->kind = TY_CHAR;

  char ch = tok->str[0];
  node->ch = ch;
  node->ty = ty;

  return node;
}

Node *new_str(Token *tok) {
  Node *node = new_node(ND_STR);
  Type *ty = calloc(1, sizeof(Type));
  ty->kind = TY_STR;

  char *str = malloc(sizeof(char) * tok->len + 1);
  memcpy(str, tok->str, tok->len);
  str[tok->len] = '\0';

  node->str = str;
  node->ty = ty;

  static int label_count = 0;
  hashmap_put(str_literal_map, str, label_count);
  label_count++;

  return node;
}

Node *new_unary(NodeKind kind, Node *lhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;

  if(kind == ND_DEREF) {
    node->lhs->is_derefernce = true;
  }
  return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Type *new_type(Token *tok, Type *ptr_to, size_t array_size) {
  Type *ty = calloc(1, sizeof(Type));

  if(startswith(tok->str, "int")) {
    ty->kind = TY_INT;
  }
  if(ptr_to) {
    ty->ptr_to = ptr_to;
  }
  if(array_size) {
    ty->array_size = array_size;
  }
  return ty;
}
