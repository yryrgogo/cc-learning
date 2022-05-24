#include "9cc.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

// 入力プログラム
char *user_input;
// 現在着目しているトークン
Token *token;

// エラーを報告するための関数
// printf と同じ引数をとる
void error(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

void error_at(char *loc, char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, " "); // pos 個の空白を出力
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op)
{
	if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
		return false;
	token = token->next;
	return true;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めル。
// それ以外の場合にはエラーを報告する。
void expect(char *op)
{
	if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
		error_at(token->str, "expected \"%s\"", op);
	token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number()
{
	if (token->kind != TK_NUM)
		error_at(token->str, "expected a number");
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof()
{
	return token->kind == TK_EOF;
}

// 新しいトークンを作成して cur に繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next = tok;
	return tok;
}

bool startswith(char *p, char *q)
{
	return memcmp(p, q, strlen(q) == 0);
}

// 入力文字列 p をトークナイズしてそれを返す
Token *tokenize(char *p)
{
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p)
	{
		// 空白文字をスキップ
		if (isspace(*p))
		{
			p++;
			continue;
		}

		// Multi-letter punctuator
		if (startswith(p, "==") || startswith(p, "!=") || startswith(p, ">=") || startswith(p, "<="))
		{
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}

		// Single-letter punctuator
		if (strchr("+-*/()<>", *p))
		{
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		if ('a' <= *p && *p <= 'z')
		{
			cur = new_token(TK_IDENT, cur, p++, 1);
			continue;
		}

		if (isdigit(*p))
		{
			cur = new_token(TK_NUM, cur, p, 0);
			char *q = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p - q;
			continue;
		}

		error_at(p, "invalid token");
	}

	new_token(TK_EOF, cur, p, 0);
	return head.next;
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		error("引数の個数が正しくありません");
		return 1;
	}

	user_input = argv[1];
	token = tokenize(user_input);
	Node *node = expr();

	// アセンブリの前半部分を出力
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// 抽象構文木を下りながらコード生成
	gen(node);

	// スタックトップに式全体の値が残っているはずなので
	// それを RAX にロードして関数からの返り値とする
	printf("  pop rax\n");
	printf("  ret\n");
	return 0;
}
