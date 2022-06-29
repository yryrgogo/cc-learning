#include "holycc.h"

extern Token *token;
static LVar *locals;
static LVar *locals_head;
Node *code[100];

int func_offset = 0;
int lvar_count = 0;

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

Node *new_unary(NodeKind kind, Node *lhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  if(!node->ty)
    node->ty = new_type(NULL, NULL, -1);
  if(!node->lhs->ty)
    node->lhs->ty = new_type(NULL, NULL, -1);
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

  if(startswith(token->str, "int")) {
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

void program() {
  int i = 0;
  while(!at_eof()) {
    Node *node = toplevel();
    code[i++] = node;
    // walk_nodes(node);
  }
  code[i] = NULL;
}

Node *toplevel() {
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
  node->ty = ty;

  if(tok && consume("(")) {
    int args_offset_total = 0;
    locals = NULL;
    node->args = func_args_definition(&args_offset_total);

    if(equal(token, "{")) {
      node->kind = ND_FUNC;
      func_offset = args_offset_total;
      node->body = stmt();
      func_offset = 0;
      node->locals = locals;
    } else {
      error_at(token->str, "toplevel に定義できる構文になっていません。");
    }
  }

  return node;
}

// stmt = expr ";"
//        | "{" stmt* "}"
//        | "if" "(" expr ")" stmt("else" stmt)?
//        | "while" "(" expr ")" stmt
//        | "for" "(" expr ? ";" expr ? ";" expr ? ")" stmt
Node *stmt() {
  Node *node;

  if(consume("{")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
    Node head;
    Node *cur = &head;

    bool has_stmt = false;
    while(!consume("}")) {
      cur->next = stmt();
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
    node->lhs = expr();
    expect(";");
  } else if(consume_token(TK_IF)) {
    expect("(");

    node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    node->cond = expr();

    expect(")");

    node->then = stmt();

    if(consume_token(TK_ELSE)) {
      node->els = stmt();
    }
  } else if(consume_token(TK_FOR)) {
    expect("(");

    node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;
    node->init = expr();
    expect(";");

    node->cond = expr();
    expect(";");

    node->inc = expr();
    expect(")");

    node->then = stmt();
  } else if(consume_token(TK_WHILE)) {
    expect("(");

    node = calloc(1, sizeof(Node));
    node->kind = ND_WHILE;
    node->cond = expr();

    expect(")");

    node->then = stmt();
  } else {
    node = expr();
    expect(";");
  }

  return node;
}

Node *expr() { return assign(); }

Node *assign() {
  Node *node = equality();

  if(consume("="))
    node = new_binary(ND_ASSIGN, node, equality());
  return node;
}

Node *equality() {
  Node *node = relational();

  for(;;) {
    if(consume("=="))
      node = new_binary(ND_EQ, node, relational());
    else if(consume("!="))
      node = new_binary(ND_NE, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for(;;) {
    if(consume("<="))
      node = new_binary(ND_LE, node, add());
    else if(consume(">="))
      node = new_binary(ND_LE, add(), node);
    else if(consume("<"))
      node = new_binary(ND_LT, node, add());
    else if(consume(">"))
      node = new_binary(ND_LT, add(), node);
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();

  for(;;) {
    if(consume("+")) {
      node = new_binary(ND_ADD, node, mul());
    } else if(consume("-")) {
      node = new_binary(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

Node *mul() {
  Node *node = unary();

  for(;;) {
    if(consume("*"))
      node = new_binary(ND_MUL, node, unary());
    else if(consume("/"))
      node = new_binary(ND_DIV, node, unary());
    else
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
Node *unary() {
  if(consume("+"))
    return primary();
  if(consume("-"))
    return new_binary(ND_SUB, new_num(0), unary());
  if(consume("&"))
    return new_unary(ND_ADDR, unary());
  if(consume("*"))
    return new_unary(ND_DEREF, unary());
  if(consume("sizeof")) {
    Node *node = unary();
    return new_num(size_of_type(node->ty));
  }

  return primary();
}

// primary = num
//         | type ident
//         | ident ("(" ")")?
//         | "(" expr ")"
Node *primary() {
  // 次のトークンが "(" なら、"(" expr ")" のはず
  if(consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  } else if(equal_token(TK_TYPE)) {
    Node *node = ident_declaration();
    return node;
  }

  Token *tok = consume_ident();
  // 関数呼び出し
  if(tok && consume("(")) {
    Node *node = func_call(tok);
    return node;
  } else if(tok) {
    Node *node = local_variable(tok, NULL);
    if(consume("[")) {
      int num = expect_number();
      node->offset = node->offset - (num * size_of_type(node->ty->ptr_to));
      expect("]");
      node->has_index = num;
    }
    return node;
  }

  return new_num(expect_number());
}

Node *ident_declaration() {
  Type *ty = calloc(1, sizeof(Type));
  Type *cur = ty;

  if(startswith(token->str, "int")) {
    ty->kind = TY_INT;
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

  Node *node = local_variable(tok, cur);
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

Node *local_variable(Token *tok, Type *ty) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;

  // LVar は今のところアクティベーションレコードの領域の計算と ND_LVAR Node
  // のオフセットの計算にしか使っていない Is it predefined local variable or
  // not?
  LVar *lvar = find_lvar(tok);

  if(!lvar) {
    lvar = calloc(1, sizeof(LVar));
    lvar->name = tok->str;
    lvar->len = tok->len;
    lvar->ty = ty;

    // array
    if(consume("[")) {
      Node *size = unary();
      expect("]");
      Type *array_ty = calloc(1, sizeof(Type));
      array_ty->kind = TY_ARRAY;
      array_ty->array_size = size->val;
      array_ty->ptr_to = ty;
      lvar->ty = array_ty;
    }

    if(!locals) {
      lvar->offset = size_of_type(lvar->ty) + func_offset;
      locals = lvar;
      locals_head = locals;
    } else {
      lvar->offset = locals_head->offset + size_of_type(lvar->ty);
      locals_head->next = lvar;
      locals_head = lvar;
    }
    lvar_count++;
  }

  node->offset = lvar->offset;
  node->ty = lvar->ty;

  return node;
}

Node *func_args_definition(int *args_offset_total) {
  Node head = {};
  Node *cur = &head;

  for(;;) {
    // arguments
    if(equal_token(TK_TYPE)) {
      Node *param = primary();
      cur = cur->next = param;
      args_offset_total += param->offset;
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

  return head.next;
}

Node *func_call(Token *tok) {
  Node *node = calloc(1, sizeof(Node));
  node->name = tok->str;
  node->len = tok->len;
  node->args = func_call_args(node);

  node->kind = ND_CALL;

  return node;
}

Node *func_call_args(Node *node) {
  Node head = {};
  Node *cur = &head;
  int count = 0;

  for(;;) {
    // arguments
    if(equal_token(TK_NUM) || equal_token(TK_IDENT) || equal(token, "&")) {
      Node *param = expr();
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

int size_of_type(Type *ty) {
  if(ty->kind == TY_INT) {
    return 4;
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
