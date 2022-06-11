#include "holycc.h"

extern Token *token;
static LVar *locals;
static LVar *locals_head;
static LVar *args;
Node *code[100];

int func_offset = 0;

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

void program()
{
	int i = 0;
	while (!at_eof())
		code[i++] = toplevel();
	code[i] = NULL;
}

Node *toplevel()
{

	Token *tok = consume_ident();
	Node *node = calloc(1, sizeof(Node));

	node->name = tok->str;
	node->len = tok->len;

	if (tok && consume("("))
	{
		int arg_count = 0;
		args = NULL;
		node->args = func_args_definition(&arg_count);
		args = node->args;

		if (equal(token, "{"))
		{
			node->kind = ND_FUNC;
			locals = NULL;
			func_offset = arg_count * 8;
			node->body = stmt();
			func_offset = 0;
			node->locals = locals;
		}
		else
		{
			error_at(token->str, "toplevel に定義できる構文になっていません。");
		}
	}

	return node;
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
	// 関数呼び出し
	if (tok && consume("("))
	{
		Node *node = calloc(1, sizeof(Node));
		node->name = tok->str;
		node->len = tok->len;
		node->args = func_call_args();

		node->kind = ND_FUNC_CALL;

		return node;
	}
	else if (tok)
	{
		Node *node = local_variable(tok);
		return node;
	}

	// そうでなければ数値のはず
	return new_num(expect_number());
}

Node *local_variable(Token *tok)
{
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_LVAR;

	// LVar は今のところアクティベーションレコードの領域の計算と ND_LVAR Node のオフセットの計算にしか使っていない
	// Is it predefined local variable or not?
	LVar *lvar = find_lvar(tok);

	if (!lvar)
	{
		lvar = calloc(1, sizeof(LVar));
		lvar->name = tok->str;
		lvar->len = tok->len;

		if (!locals)
		{
			lvar->offset = 8 + func_offset;
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

Node *func_args_definition(int *arg_count)
{
	Node head = {};
	Node *cur = &head;

	for (;;)
	{
		// arguments
		if (equal_token(TK_IDENT))
		{
			Node *param = primary();
			cur = cur->next = param;
			(*arg_count)++;

			consume(",");
		}

		if (consume(")"))
		{
			break;
		}
	}

	return head.next;
}

Node *func_call_args()
{
	Node head = {};
	Node *cur = &head;

	for (;;)
	{
		// arguments
		if (equal_token(TK_NUM))
		{
			Node *param = expr();
			cur = cur->next = param;

			consume(",");
		}

		if (consume(")"))
		{
			break;
		}
	}

	return head.next;
}

LVar *find_lvar(Token *tok)
{
	for (LVar *var = locals; var; var = var->next)
		if (var->len == tok->len && !memcmp(tok->loc, var->name, var->len))
			return var;
	return NULL;
}

Node *find_args(Token *tok)
{
	for (Node *var = args; var; var = var->next)
		if (var->len == tok->len && !memcmp(tok->loc, var->name, var->len))
			return var;
	return NULL;
}
