#include "holycc.h"

static int count(void)
{
	static int i = 1;
	return i++;
}

void gen_lval(Node *node)
{
	if (node->kind != ND_LVAR)
		error_at(__FILE__, "代入の左辺値が変数ではありません。");

	printf("  mov rax, rbp\n");
	printf("  sub rax, %d\n", node->offset);
	printf("  push rax\n");
}

void gen(Node *node)
{
	switch (node->kind)
	{
	case ND_FUNC:
	{
		gen_func(node);
		break;
	}
	default:
	{
		gen_stmt(node);
		printf("  pop rax\n");
	}
	}

	return;
}

void gen_stmt(Node *node)
{
	switch (node->kind)
	{
	case ND_RETURN:
		gen_expr(node->lhs);
		printf("  pop rax\n");
		printf("  mov rsp, rbp\n");
		printf("  pop rbp\n");
		printf("  ret\n");
		break;
	case ND_BLOCK:
	{
		Node *cur = node->body;
		while (cur)
		{
			gen_stmt(cur);
			cur = cur->next;
			// statement の評価結果としてスタックに一つの値が残っているはずなので、
			// スタックが溢れないようにポップしておく
			printf("  pop rax\n");
		}
		break;
	}
	case ND_FOR:
	{
		int c = count();

		gen_expr(node->init);
		printf("  jmp .L.for.begin.%d\n", c);

		printf(".L.for.begin.%d:\n", c);
		gen_expr(node->cond);
		printf("  pop rax\n");
		printf("  cmp rax, 0\n");
		printf("  je .L.for.end.%d\n", c);
		gen_stmt(node->then);
		gen_expr(node->inc);
		printf("  jmp .L.for.begin.%d\n", c);

		printf(".L.for.end.%d:\n", c);
		break;
	}
	case ND_WHILE:
	{
		int c = count();

		printf("  jmp .L.while.begin.%d\n", c);

		printf(".L.while.begin.%d:\n", c);
		gen_expr(node->cond);
		printf("  pop rax\n");
		printf("  cmp rax, 0\n");
		printf("  je .L.while.end.%d\n", c);
		gen_stmt(node->then);
		printf("  jmp .L.while.begin.%d\n", c);

		printf(".L.while.end.%d:\n", c);
		break;
	}
	case ND_IF:
	{
		int c = count();
		gen_expr(node->cond);
		printf("  pop rax\n");
		printf("  cmp rax, 0\n");
		printf("  je .L.else.%d\n", c);

		gen_stmt(node->then);

		printf("  jmp .L.end.%d\n", c);
		printf(".L.else.%d:\n", c);
		if (node->els)
		{
			gen_stmt(node->els);
		}

		printf(".L.end.%d:\n", c);
		break;
	}
	default:
		gen_expr(node);
	}

	return;
}

void gen_calculator(Node *node)
{
	if (node->lhs)
		gen_expr(node->lhs);
	if ((node->rhs))
		gen_expr(node->rhs);

	if (node->lhs && node->rhs)
	{
		printf("  pop rdi\n");
		printf("  pop rax\n");
	}

	switch (node->kind)
	{
	case ND_ADD:
		printf("  add rax, rdi\n");
		break;
	case ND_SUB:
		printf("  sub rax, rdi\n");
		break;
	case ND_MUL:
		printf("  imul rax, rdi\n");
		break;
	case ND_DIV:
		printf("  cqo\n");
		printf("  idiv rdi\n");
		break;
	case ND_EQ:
		printf("  cmp rax, rdi\n");
		printf("  sete al\n");
		printf("  movzb rax, al\n");
		break;
	case ND_NE:
		printf("  cmp rax, rdi\n");
		printf("  setne al\n");
		printf("  movzb rax, al\n");
		break;
	case ND_LT:
		printf("  cmp rax, rdi\n");
		printf("  setl al\n");
		printf("  movzb rax, al\n");
		break;
	case ND_LE:
		printf("  cmp rax, rdi\n");
		printf("  setle al\n");
		printf("  movzb rax, al\n");
		break;
	}

	printf("  push rax\n");
}

void gen_expr(Node *node)
{
	switch (node->kind)
	{
	case ND_NUM:
		printf("  push %d\n", node->val);
		break;
	case ND_LVAR:
		gen_lval(node);
		printf("  pop rax\n");
		printf("  mov rax, [rax]\n");
		printf("  push rax\n");
		break;
	case ND_ASSIGN:
		gen_lval(node->lhs);
		gen_expr(node->rhs);
		printf("  pop rdi\n");
		printf("  pop rax\n");
		printf("  mov [rax], rdi\n");
		printf("  push rdi\n");
		printf("  push rax\n");
		break;
	case ND_FUNC_CALL:
		gen_func_call(node);
		break;
	default:
		gen_calculator(node);
		break;
	}
}

void gen_func(Node *node)
{
	char s[node->len + 1];
	memcpy(s, node->name, node->len);
	s[node->len] = '\0';
	printf("%s:\n", s);

	// Prologue
	printf("  push rbp\n");
	printf("  mov rbp, rsp\n");

	void gen_arg(Node * argv, char *register_name)
	{
		printf("  mov rax, rbp\n");
		printf("  sub rax, %d\n", argv->offset);
		printf("  mov [rax], %s\n", register_name);
	}

	// 引数を Stack に書き出す
	int args_count = 0;
	for (Node *argv = node->args; argv; argv = argv->next)
	{
		switch (args_count)
		{
		case 0:
			gen_arg(argv, "rdi");
			break;
		case 1:
			gen_arg(argv, "rsi");
			break;
		case 2:
			gen_arg(argv, "rdx");
			break;
		case 3:
			gen_arg(argv, "rcx");
			break;
		case 4:
			gen_arg(argv, "r8");
			break;
		case 5:
			gen_arg(argv, "r9");
			break;
		}
		args_count++;
	}

	int locals_count = 0;
	for (LVar *var = node->locals; var; var = var->next)
	{
		locals_count++;
	}
	if (locals_count > 0)
		printf("  sub rsp, %d\n", locals_count * 8);

	gen_stmt(node->body);

	// Epilogue
	printf("  mov rsp, rbp\n");
	printf("  pop rbp\n");
	printf("  ret\n");

	return;
}

void gen_func_call(Node *node)
{
	if (node->args)
	{
		int count = 0;
		for (Node *argv = node->args; argv; argv = argv->next)
		{
			switch (count)
			{
			case 0:
				printf("  mov rdi, %d\n", argv->val);
				break;
			case 1:
				printf("  mov rsi, %d\n", argv->val);
				break;
			case 2:
				printf("  mov rdx, %d\n", argv->val);
				break;
			case 3:
				printf("  mov rcx, %d\n", argv->val);
				break;
			case 4:
				printf("  mov r8, %d\n", argv->val);
				break;
			case 5:
				printf("  mov r9, %d\n", argv->val);
				break;
			}
			count++;
		}
	}
	char s[node->len + 1];
	memcpy(s, node->name, node->len);
	s[node->len] = '\0';
	printf("  call %s\n", s);
	// TODO: 戻り値があれば rax に入っているはずなのでとりあえず rax を push しているが適切かは？
	printf("  push rax\n");
	return;
}
