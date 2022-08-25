#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Type Type;
typedef struct Node Node;

//
// tokenize.c
//

// Token
typedef enum {
  TK_IDENT,
  TK_PUNCT,
  TK_KEYWORD,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *loc;
  int len;
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_tok(Token *tok, char *fmt, ...);
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *s);
bool consume(Token **rest, Token *tok, char *str);
Token *tokenize(char *p);

//
// parse.c
//

// Local variable
typedef struct Obj Obj;
struct Obj {
  Obj *next;
  char *name;
  Type *ty;
  int offset;
};

// Function
typedef struct Function Function;
struct Function {
  Function *next;
  char *name;
  Node *body;
  Obj *locals;
  int stack_size;
};

// AST node
typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NEG, // unary -
  ND_EQ,
  ND_NE,
  ND_LT, // <
  ND_LE, // <=
  ND_ASSIGN,
  ND_ADDR,  // unary &
  ND_DEREF, // unary *
  ND_RETURN,
  ND_IF,
  ND_FOR, // "for" or "while"
  ND_BLOCK,
  ND_FUNCALL,
  ND_EXPR_STMT, // Expression statement
  ND_VAR,
  ND_NUM,
} NodeKind;

// AST node type
typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *next;
  Type *ty;   // Type, e.g. int or pointer to int
  Token *tok; // Representative token

  Node *lhs;
  Node *rhs;

  // "if" or "for" statement
  Node *cond;
  Node *then;
  Node *els;
  Node *init;
  Node *inc;

  // Block
  Node *body;

  // Function call
  char *funcname;
  Node *args;

  Obj *var;
  int val;
};

Function *parse(Token *tok);

//
// type.c
//

typedef enum {
  TY_INT,
  TY_PTR,
  TY_FUNC,
} TypeKind;

struct Type {
  TypeKind kind;

  // Pointer
  Type *base;

  // Declaration
  Token *name;

  // Function type
  Type *return_ty;
};

extern Type *ty_int;

bool is_integer(Type *ty);
Type *pointer_to(Type *base);
Type *func_type(Type *return_ty);
void add_type(Node *node);

//
// codegen.c
//

void codegen(Function *node);
