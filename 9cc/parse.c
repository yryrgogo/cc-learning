#include "9cc.h"
#include <stdio.h>
#include <stdlib.h>

Node* code[100];
LVar* locals;

Node* new_node(NodeKind kind)
{
	Node* node = calloc(1, sizeof(Node));
	node->kind = kind;
	return node;
}

Node* new_num(int val)
{
	Node* node = new_node(ND_NUM);
	node->val = val;
	return node;
}

Node* new_binary(NodeKind kind, Node* lhs, Node* rhs)
{
	Node* node = new_node(kind);
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node* program()
{
	int i = 0;
	while (!at_eof())
		code[i++] = stmt();
	code[i] = NULL;
}

Node* stmt()
{
	Node* node;
	if (consume_token(TK_RETURN))
	{
		node = calloc(1, sizeof(Node));
		node->kind = ND_RETURN;
		node->lhs = expr();
		expect(";");
	}
	else if (consume_token(TK_IF))
	{
		expect("(");

		node = calloc(1, sizeof(Node));
		node->kind = ND_IF;
		node->cond = expr();

		expect(")");

		node->then = stmt();

		if (consume_token(TK_ELSE))
		{
			node->els = stmt();
		}
	}
	else
	{
		node = expr();
		expect(";");
	}

	return node;
}

Node* expr()
{
	return assign();
}

Node* assign()
{
	Node* node = equality();

	if (consume("="))
		node = new_binary(ND_ASSIGN, node, equality());
	return node;
}

Node* equality()
{
	Node* node = relational();

	for (;;)
	{
		if (consume("=="))
			node = new_binary(ND_EQ, node, relational());
		else if (consume("!="))
			node = new_binary(ND_NE, node, relational());
		else
			return node;
	}
}

Node* relational()
{
	Node* node = add();

	for (;;)
	{
		if (consume("<="))
			node = new_binary(ND_LE, node, add());
		else if (consume(">="))
			node = new_binary(ND_LE, add(), node);
		else if (consume("<"))
			node = new_binary(ND_LT, node, add());
		else if (consume(">"))
			node = new_binary(ND_LT, add(), node);
		else
			return node;
	}
}

Node* add()
{
	Node* node = mul();

	for (;;)
	{
		if (consume("+"))
			node = new_binary(ND_ADD, node, mul());
		else if (consume("-"))
			node = new_binary(ND_SUB, node, mul());
		else
			return node;
	}
}

Node* mul()
{
	Node* node = unary();

	for (;;)
	{
		if (consume("*"))
			node = new_binary(ND_MUL, node, unary());
		else if (consume("/"))
			node = new_binary(ND_DIV, node, unary());
		else
			return node;
	}
}

Node* unary()
{
	if (consume("+"))
		return primary();
	if (consume("-"))
		return new_binary(ND_SUB, new_num(0), unary());
	return primary();
}

Node* primary()
{
	// 次のトークンが "(" なら、"(" expr ")" のはず
	if (consume("("))
	{
		Node* node = expr();
		expect(")");
		return node;
	}

	Token* tok = consume_ident();
	if (tok)
	{
		Node* node = calloc(1, sizeof(Node));
		node->kind = ND_LVAR;
		node->offset = (tok->str[0] - 'a' + 1) * 8;
		return node;
	}

	// そうでなければ数値のはず
	return new_num(expect_number());
}

LVar* find_lvar(Token* tok)
{
	for (LVar* var = locals; var; var = var->next)
		if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
			return var;
	return NULL;
}
