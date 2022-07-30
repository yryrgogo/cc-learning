#define __STDC_WANT_LIB_EXT1__ 1
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void error_at(char *loc, char *fmt, ...);

#define unreachable() error_at("internal error at %s:%d", __FILE__, __LINE__)
#define dbg(message)                                                           \
  error_at("### debug at %s, %s:%d\n", message, __FILE__, __LINE__)

typedef struct Token Token;
typedef struct Type Type;
typedef struct LVar LVar;
typedef struct GVar GVar;
typedef struct Node Node;
typedef struct HashEntry HashEntry;
typedef struct HashMap HashMap;
typedef struct Vector Vector;

// Kind of Token
typedef enum {
  TK_PUNCT,   // puncutuator
  TK_KEYWORD, // keyword
  TK_IDENT,   // 識別子
  TK_NUM,     // 整数トークン
  TK_EOF,     // 入力の終わりを表すトークン
  TK_RETURN,  // return
  TK_IF,      // if
  TK_ELSE,    // else
  TK_FOR,     // for
  TK_WHILE,   // while
  TK_TYPE,    // int
  TK_SIZEOF,  // sizeof
  TK_STR,     // str
} TokenKind;

// Kind of Type
typedef enum {
  TY_INT,   // int
  TY_PTR,   // pointer
  TY_ARRAY, // array
  TY_CHAR,  // char
  TY_BOOL,  // bool
} TypeKind;

// Kind of Operator
typedef enum {
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_EQ,     // ==
  ND_NE,     // !=
  ND_LT,     // <
  ND_LE,     // <=
  ND_ASSIGN, // =

  ND_ADDR,  // &
  ND_DEREF, // *

  ND_LVAR, // Local 変数
  ND_GVAR, // Global 変数

  ND_RETURN, // return
  ND_BLOCK,  // {}
  ND_IF,     // if
  ND_ELSE,   // else
  ND_FOR,    // for
  ND_WHILE,  // while NOTE: NF_FOR
            // にまとめられるらしいが今はわからんので分けている
  ND_FUNC, // function
  ND_CALL, // function call
  ND_NOP,  // no operation

  ND_NUM, // 整数
  ND_STR, // 文字列
} NodeKind;

struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kind が TK_NUM の場合、その数値
  char *str;      // トークン文字列
  int len;        // トークンの長さ

  char *loc;
};

struct Type {
  TypeKind kind;
  Type *ptr_to;
  size_t array_size;
};

// ローカル変数の型
struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;

  Type *ty;
};

struct GVar {
  GVar *next;
  char *name;
  int len;

  Type *ty;
};

// 抽象構文木のノードの型
struct Node {
  NodeKind kind; // ノードの型
  Node *next;    // 次のノード
  Node *lhs;     // 左辺
  Node *rhs;     // 右辺

  int val; // kind が ND_NUM の場合のみ使う

  char *str; // kind が ND_STR の場合のみ使う

  int offset; // kind が ND_LVAR の場合のみ使う

  int total_offset;

  bool is_declaration;
  bool is_derefernce;
  int has_index;

  char *name; // function, variable, parameter の名前
  int len;
  int args_num;
  Node *args;
  LVar *locals;

  // "if" or "for" statement
  Node *init;
  Node *cond;
  Node *inc;
  Node *then;
  Node *els;

  // block statement
  Node *body;

  // type
  Type *ty;
  Type *return_ty;
};

//
// tokenize.c
//
Token *tokenize(char *p);
bool equal(Token *tok, char *op);
bool equal_token(TokenKind tk);
bool consume(char *op);
bool consume_token(TokenKind tk);
Token *consume_ident();
Token *consume_type();
int consume_num();
void expect(char *op);
int expect_number();
char *expect_string();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Type *new_type(Token *tok, Type *ptr_to, size_t array_size);
bool startswith(char *p, char *q);
bool is_alnum(char c);
// static bool is_keyword(Token *tok);
// static int read_punct(char *p);

//
// parse.c
//

void program();
Node *toplevel();
Node *stmt(HashMap *lvar_map);
Node *expr(HashMap *lvar_map);
Node *assign(HashMap *lvar_map);
Node *equality(HashMap *lvar_map);
Node *relational(HashMap *lvar_map);
Node *array(HashMap *lvar_map);
Node *add(HashMap *lvar_map);
Node *mul(HashMap *lvar_map);
Node *unary(HashMap *lvar_map);
Node *primary(HashMap *lvar_map);
LVar *find_lvar(Token *tok);
GVar *find_gvar(Token *tok);
Node *func_args_definition(int *args_count, int *args_offset_total,
                           HashMap *lvar_map);
Node *func_call(Token *tok, HashMap *lvar_map);
Node *func_call_args(Node *node, HashMap *lvar_map);
Node *local_variable(Token *tok, Type *ty, HashMap *lvar_map);
Node *ident_declaration(HashMap *lvar_map);
Type *pointer_type(Type *ty, Type *cur);
int size_of_type(Type *ty);
void adjust_rsp();
Type *pointed_type(Type *ty);
Node *new_node(NodeKind kind);
Node *new_num(int val);
Node *new_str(char *str);
Node *new_unary(NodeKind kind, Node *lhs);
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);
Type *new_type(Token *tok, Type *ptr_to, size_t array_size);

//
// codegen.c
//

void gen(Node *node);
void gen_stmt(Node *node);
void gen_expr(Node *node, bool is_dereference);
void gen_func(Node *node);
void gen_func_call(Node *node);
void gen_func_call_arg(Node *node, char *name);
void gen_lvar_addr(Node *node);
void gen_gvar(Node *node);
void gen_calculator(Node *node, bool is_dereference);
void *gen_lhs_deref(Node *node);
void gen_gvar_value(Node *node);
void gen_gvar_assign(Node *node);
void gen_var_preprocess(Node *node, bool is_dereference);
void set_register_name(Type *ty, char **reg, char **prefix);
Type *deref_type(Node *node);

//
// vector.c
//
struct Vector {
  void **data;
  int capacity;
  int len;
};

Vector *new_vec(void);
void vec_push(Vector *v, void *elem);
void vec_pushi(Vector *v, int val);
void *vec_pop(Vector *v);
void *vec_last(Vector *v);
bool vec_contains(Vector *v, void *elem);
bool vec_union(Vector *v, void *elem);

//
// hashmap.c
//
struct HashEntry {
  char *key;
  int keylen;
  int val;
};

struct HashMap {
  HashEntry *buckets;
  int capacity;
  int used;
};

int hashmap_get(HashMap *map, char *key);
void hashmap_put(HashMap *map, char *key, int val);
void hashmap_delete(HashMap *map, char *key);
void hashmap_test(void);

//
// strings.c
//
char *format(char *fmt, ...);

//
// graphgen.c
//
void gen_graph(Node *node);
void gen_func_node(Node *node);
void gen_graph_stmt(Node *node, char *name);
void gen_graph_expr(Node *node, char *name);
void get_no(char *no);
void node_name(char *name);

//
// utils.c
//
int max(int a, int b);
char *node_kind_name(NodeKind kind);
void walk_nodes(Node *node);
void update_lvar_offset(Node *node, HashMap *lvar_map, int max_offset);
