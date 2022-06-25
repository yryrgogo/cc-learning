#include "holycc.h"

extern Token *token;
static LVar *locals;
static LVar *locals_head;
Node *code[100];

int func_offset = 0;

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
  while(!at_eof())
    code[i++] = toplevel();
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
    int arg_count = 0;
    locals = NULL;
    node->args = func_args_definition(&arg_count);

    if(equal(token, "{")) {
      node->kind = ND_FUNC;
      func_offset = arg_count * 8;
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
    if(node->ty->kind == TY_INT) {
      return new_num(4);
    } else if(node->ty->kind == TY_PTR && !node->ty->array_size) {
      return new_num(8);
    } else if(node->ty->kind == TY_PTR && node->ty->array_size) {
      return new_num(node->ty->array_size * 8);
    } else if(node->ty->kind == TY_ARRAY) {
      return new_num(node->ty->array_size *
                     (node->ty->ptr_to->kind == TY_INT ? 4 : 8));
    } else {
      error_at(token->str,
               "sizeof は int と ptr のサイズを返すことができます。");
      return node;
    }
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
      node->offset = node->offset - (num * 8);
      expect("]");
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

  // array
  if(consume("[")) {
    Node *size = unary();
    expect("]");
    Type *array_ty = calloc(1, sizeof(Type));
    if(cur->kind == TY_INT) {
      array_ty->kind = TY_INT;
    } else if(cur->kind == TY_PTR) {
      array_ty->kind = TY_PTR;
    }
    ty->kind = TY_ARRAY;
    ty->array_size = size->val;
    cur->array_size = size->val;
    ty->ptr_to = array_ty;
    node->offset = node->offset + (ty->array_size - 1) * 8;
    locals_head->offset = node->offset;
  }

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

    if(!locals) {
      lvar->offset = 8 + func_offset;
      locals = lvar;
      locals_head = locals;
    } else {
      lvar->offset = locals_head->offset + 8;
      locals_head->next = lvar;
      locals_head = lvar;
    }
  }

  node->offset = lvar->offset;
  node->ty = lvar->ty;

  return node;
}

Node *func_args_definition(int *arg_count) {
  Node head = {};
  Node *cur = &head;

  for(;;) {
    // arguments
    if(equal_token(TK_TYPE)) {
      Node *param = primary();
      cur = cur->next = param;
      (*arg_count)++;
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

  node->kind = ND_FUNC_CALL;

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
