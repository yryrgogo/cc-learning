#include "9cc.h"
#include <stdlib.h>

// 現在着目しているトークン
Token *token;

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op)
{
	if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
		return false;
	token = token->next;
	return true;
}

bool consume_token(TokenKind tk){
	if (token->kind != tk)
		return false;
	token = token->next;
	return true;
}

Token *consume_ident()
{
	if (token->kind != TK_IDENT)
		return NULL;

	Token *t = token;
	token = token->next;
	return t;
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

bool is_alnum(char c)
{
	return ('a' <= c && c <= 'z') ||
				 ('A' <= c && c <= 'Z') ||
				 ('0' <= c && c <= '9') ||
				 (c == '_');
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
		if (strchr("+-*/()<>=;", *p))
		{
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6]))
		{
			cur = new_token(TK_RETURN, cur, p, 6);
			p += 6;
			continue;
		}

		if ('a' <= *p && *p <= 'z')
		{
			int i = 0;
			while ('a' <= *p && *p <= 'z')
			{
				p++;
				i++;
			}
			p -= i;
			cur = new_token(TK_IDENT, cur, p, i);
			p += i;
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
	token = head.next;

	return head.next;
}
