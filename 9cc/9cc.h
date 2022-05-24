#include <stdbool.h>

typedef struct Token Token;
typedef struct Node Node;

// Kind of Token
typedef enum
{
	TK_RESERVED, // 記号
	TK_IDENT,		 // 識別子
	TK_NUM,			 // 整数トークン
	TK_EOF,			 // 入力の終わりを表すトークン
} TokenKind;

// Kind of Operator
typedef enum
{
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_EQ,	// ==
	ND_NE,	// !=
	ND_LT,	// <
	ND_LE,	// <=
	ND_NUM, // 整数
} NodeKind;

// トークン型
struct Token
{
	TokenKind kind; // トークンの型
	Token *next;		// 次の入力トークン
	int val;				// kind が TK_NUM の場合、その数値
	char *str;			// トークン文字列
	int len;				// トークンの長さ
};

// 抽象構文木のノードの型
struct Node
{
	NodeKind kind; // ノードの型
	Node *lhs;		 // 左辺
	Node *rhs;		 // 右辺
	int val;			 // kind が ND_NUM の場合のみ使う
};

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

void gen(Node *node);

bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
