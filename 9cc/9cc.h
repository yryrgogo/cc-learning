#include <stdbool.h>

// Kind of Token
typedef enum
{
	TK_RESERVED, // 記号
	TK_IDENT,		 // 識別子
	TK_NUM,			 // 整数トークン
	TK_EOF,			 // 入力の終わりを表すトークン
	TK_RETURN,	 //  return
} TokenKind;

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
} NodeKind;

typedef struct Token Token;

// トークン型
struct Token
{
	TokenKind kind; // トークンの型
	Token *next;		// 次の入力トークン
	int val;				// kind が TK_NUM の場合、その数値
	char *str;			// トークン文字列
	int len;				// トークンの長さ
};

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node
{
	NodeKind kind; // ノードの型
	Node *lhs;		 // 左辺
	Node *rhs;		 // 右辺
	int val;			 // kind が ND_NUM の場合のみ使う
	int offset;		 // kind が ND_LVAR の場合のみ使う
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

void gen(Node *node);

bool consume(char *op);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
