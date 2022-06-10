#define __STDC_WANT_LIB_EXT1__ 1
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void error_at(char *loc, char *fmt, ...);

#define unreachable() \
	error_at("internal error at %s:%d", __FILE__, __LINE__)

#define dbg(message) \
	error_at("### debug at %s, %s:%d\n", message, __FILE__, __LINE__)

//
// tokenize.c
//

// Kind of Token
typedef enum
{
	TK_PUNCT,		// puncutuator
	TK_KEYWORD, // keyword
	TK_IDENT,		// 識別子
	TK_NUM,			// 整数トークン
	TK_EOF,			// 入力の終わりを表すトークン
	TK_RETURN,	// return
	TK_IF,			// if
	TK_ELSE,		// else
	TK_FOR,			// for
	TK_WHILE,		// while
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token
{
	TokenKind kind; // トークンの型
	Token *next;		// 次の入力トークン
	int val;				// kind が TK_NUM の場合、その数値
	char *str;			// トークン文字列
	int len;				// トークンの長さ

	char *loc;
};

Token *tokenize(char *p);
bool equal(Token *tok, char *op);
bool equal_token(TokenKind tk);
bool consume(char *op);
bool consume_token(TokenKind tk);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
bool is_alnum(char c);
// static bool is_keyword(Token *tok);
// static int read_punct(char *p);

//
// parse.c
//

typedef struct LVar LVar;
// ローカル変数の型
struct LVar
{
	LVar *next;
	char *name;
	int len;
	int offset;
};

// Kind of Operator
typedef enum
{
	ND_ADD,				// +
	ND_SUB,				// -
	ND_MUL,				// *
	ND_DIV,				// /
	ND_EQ,				// ==
	ND_NE,				// !=
	ND_LT,				// <
	ND_LE,				// <=
	ND_ASSIGN,		// =
	ND_LVAR,			// ローカル変数
	ND_NUM,				// 整数
	ND_RETURN,		// return
	ND_IF,				// if
	ND_ELSE,			// else
	ND_FOR,				// for
	ND_WHILE,			// while NOTE: NF_FOR にまとめられるらしいが今はわからんので分けている
	ND_BLOCK,			// {}
	ND_FUNC,			// function
	ND_FUNC_CALL, // function call
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node
{
	NodeKind kind; // ノードの型
	Node *next;		 // 次のノード
	Node *lhs;		 // 左辺
	Node *rhs;		 // 右辺
	int val;			 // kind が ND_NUM の場合のみ使う
	int offset;		 // kind が ND_LVAR の場合のみ使う

	// function
	char *name;
	int len;
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
};

void program();
Node *toplevel();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
LVar *find_lvar(Token *tok);
Node *func_args_definition(int *arg_count);
Node *func_call_args();

//
// codegen.c
//

void gen(Node *node);
void gen_stmt(Node *node);
void gen_expr(Node *node);
void gen_func(Node *node);
void gen_func_call(Node *node);
void gen_calculator(Node *node);

//
// hashmap.c
//

typedef struct
{
	char *key;
	int keylen;
	void *val;
} HashEntry;

typedef struct
{
	HashEntry *buckets;
	int capacity;
	int used;
} HashMap;

void *hashmap_get(HashMap *map, char *key);
void hashmap_put(HashMap *map, char *key, void *val);
void hashmap_delete(HashMap *map, char *key);
void hashmap_test(void);

//
// strings.c
//
char *format(char *fmt, ...);
