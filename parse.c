#include "holycc.h"

extern Token *token;
LVar *locals;
LVar *locals_head;
Node *code[100];

Node *new_node(NodeKind kind)
{
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	return node;
}

Node *new_num(int val)
{
	Node *node = new_node(ND_NUM);
	node->val = val;
	return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs)
{
	Node *node = new_node(kind);
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *program()
{
	int i = 0;
	while (!at_eof())
		code[i++] = stmt();
	code[i] = NULL;
}

// stmt = expr ";"
//        | "{" stmt* "}"
//        | "if" "(" expr ")" stmt("else" stmt)?
//        | "while" "(" expr ")" stmt
//        | "for" "(" expr ? ";" expr ? ";" expr ? ")" stmt
Node *stmt()
{
	Node *node;
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
	else if (consume_token(TK_FOR))
	{
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
	}
	else if (consume_token(TK_WHILE))
	{
		expect("(");

		node = calloc(1, sizeof(Node));
		node->kind = ND_WHILE;
		node->cond = expr();

		expect(")");

		node->then = stmt();
	}
	else if (consume("{"))
	{
		node = calloc(1, sizeof(Node));
		node->kind = ND_BLOCK;
		Node head;
		Node *cur = &head;

		while (!consume("}"))
		{
			cur->next = stmt();
			cur = cur->next;
		}
		node->body = head.next;
	}
	else
	{
		node = expr();
		expect(";");
	}

	return node;
}

Node *expr()
{
	return assign();
}

Node *assign()
{
	Node *node = equality();

	if (consume("="))
		node = new_binary(ND_ASSIGN, node, equality());
	return node;
}

Node *equality()
{
	Node *node = relational();

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

Node *relational()
{
	Node *node = add();

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

Node *add()
{
	Node *node = mul();

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

Node *mul()
{
	Node *node = unary();

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

Node *unary()
{
	if (consume("+"))
		return primary();
	if (consume("-"))
		return new_binary(ND_SUB, new_num(0), unary());
	return primary();
}

// primary = num
//         | ident ("(" ")")?
//         | "(" expr ")"
Node *primary()
{
	// 次のトークンが "(" なら、"(" expr ")" のはず
	if (consume("("))
	{
		Node *node = expr();
		expect(")");
		return node;
	}

	Token *tok = consume_ident();
	if (tok && consume("("))
	{
		// 関数呼び出し
		expect(")");
		Node *node = calloc(1, sizeof(Node));
		node->kind = ND_FUNC;
		node->name = tok->str;
		node->len = tok->len;

		return node;
	}
	else if (tok)
	{
		Node *node = calloc(1, sizeof(Node));
		node->kind = ND_LVAR;

		// LVar は今のところアクティベーションレコードの領域の計算と ND_LVAR Node のオフセットの計算にしか使っていない
		LVar *lvar = find_lvar(tok);
		if (!lvar)
		{
			lvar = calloc(1, sizeof(LVar));
			lvar->name = tok->str;
			lvar->len = tok->len;
			if (!locals)
			{
				lvar->offset = 8;
				locals = lvar;
				locals_head = locals;
			}
			else
			{
				lvar->offset = locals_head->offset + 8;
				locals_head->next = lvar;
				locals_head = lvar;
			}
		}

		node->offset = lvar->offset;

		return node;
	}

	// そうでなければ数値のはず
	return new_num(expect_number());
}

LVar *find_lvar(Token *tok)
{
	for (LVar *var = locals; var; var = var->next)
		if (var->len == tok->len && !memcmp(tok->loc, var->name, var->len))
			return var;
	return NULL;
}
