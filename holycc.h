#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define unreachable() \
	error("internal error at %s:%d", __FILE__, __LINE__)

#define dbg(message) \
	error("### debug at %s, %s:%d\n", message, __FILE__, __LINE__)

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

bool equal(Token *tok, char *op);
bool consume(char *op);
bool consume_token(TokenKind tk);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
bool is_alnum(char c);
static bool is_keyword(Token *tok);
static int read_punct(char *p);

//
// parse.c
//

// Kind of Operator
typedef enum
{
	ND_ADD,		 // +
	ND_SUB,		 // -
	ND_MUL,		 // *
	ND_DIV,		 // /
	ND_EQ,		 // ==
	ND_NE,		 // !=
	ND_LT,		 // <
	ND_LE,		 // <=
	ND_ASSIGN, // =
	ND_LVAR,	 // ローカル変数
	ND_NUM,		 // 整数
	ND_RETURN, // return
	ND_IF,		 // if
	ND_ELSE,	 // else
	ND_FOR,		 // for
	ND_WHILE,	 // while NOTE: NF_FOR にまとめられるらしいが今はわからんので分けている
	ND_BLOCK,	 // {}
	ND_FUNC,	 // function
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

	// identifier
	Node *name;

	// "if" or "for" statement
	Node *init;
	Node *cond;
	Node *inc;
	Node *then;
	Node *els;

	// block statement
	Node *body;
};

typedef struct LVar LVar;
// ローカル変数の型
struct LVar
{
	LVar *next;
	char *name;
	int len;
	int offset;
};

Node *program();
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

//
// codegen.c
//

void gen(Node *node);

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
