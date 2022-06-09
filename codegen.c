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
	case ND_NUM:
		printf("  push %d\n", node->val);
		return;
	case ND_LVAR:
		gen_lval(node);
		printf("  pop rax\n");
		printf("  mov rax, [rax]\n");
		printf("  push rax\n");
		return;
	case ND_ASSIGN:
		gen_lval(node->lhs);
		gen(node->rhs);

		printf("  pop rdi\n");
		printf("  pop rax\n");
		printf("  mov [rax], rdi\n");
		printf("  push rdi\n");
		return;
	case ND_RETURN:
		gen(node->lhs);
		printf("  pop rax\n");
		printf("  mov rsp, rbp\n");
		printf("  pop rbp\n");
		printf("  ret\n");
		return;
	case ND_BLOCK:
	{
		Node *cur = node->body;
		while (cur)
		{
			gen(cur);
			cur = cur->next;
			if (cur)
			{
				// statement の評価結果としてスタックに一つの値が残っているはずなので、
				// スタックが溢れないようにポップしておく
				// 最後の statement については別途ポップされるためスキップ
				printf("  pop rax\n");
			}
		}
		return;
	}
	case ND_FOR:
	{
		int c = count();

		gen(node->init);
		printf("  jmp .L.for.begin.%d\n", c);

		printf(".L.for.begin.%d:\n", c);
		gen(node->cond);
		printf("  pop rax\n");
		printf("  cmp rax, 0\n");
		printf("  je .L.for.end.%d\n", c);
		gen(node->then);
		gen(node->inc);
		printf("  jmp .L.for.begin.%d\n", c);

		printf(".L.for.end.%d:\n", c);
		return;
	}
	case ND_WHILE:
	{
		int c = count();

		printf("  jmp .L.while.begin.%d\n", c);

		printf(".L.while.begin.%d:\n", c);
		gen(node->cond);
		printf("  pop rax\n");
		printf("  cmp rax, 0\n");
		printf("  je .L.while.end.%d\n", c);
		gen(node->then);
		printf("  jmp .L.while.begin.%d\n", c);

		printf(".L.while.end.%d:\n", c);
		return;
	}
	case ND_IF:
	{
		int c = count();
		gen(node->cond);
		printf("  pop rax\n");
		printf("  cmp rax, 0\n");
		printf("  je .L.else.%d\n", c);

		gen(node->then);

		printf("  jmp .L.end.%d\n", c);
		printf(".L.else.%d:\n", c);
		if (node->els)
		{
			gen(node->els);
		}

		printf(".L.end.%d:\n", c);
		return;
	}
	case ND_FUNC:
	{
		gen_func(node);
		return;
	}
	case ND_FUNC_CALL:
	{
		gen_func_call(node);
		return;
	}
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("  pop rdi\n");
	printf("  pop rax\n");

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
	}

	printf("  push rax\n");
}

// void get_stmt(Node *node)
// {

// }

void gen_func(Node *node)
{
	char s[node->len + 1];
	memcpy(s, node->name, node->len);
	s[node->len] = '\0';
	printf("%s:\n", s);

	// プロローグ
	printf("  push rbp\n");
	printf("  mov rbp, rsp\n");

	// 引数を Stack に書き出す
	int count = 0;
	for (Node *argv = node->args; argv; argv = argv->next)
	{
		switch (count)
		{
		case 0:
			printf("  push rdi\n");
		case 1:
			printf("  push rsi\n");
		case 2:
			printf("  push rdx\n");
		case 3:
			printf("  push rcx\n");
		case 4:
			printf("  push r8\n");
		case 5:
			printf("  push r9\n");
		}
		count++;
	}

	int locals_count = 0;
	for (LVar *var = node->locals; var; var = var->next)
		locals_count++;
	printf("  sub rsp, %d\n", locals_count * 8);

	gen(node->body);

	// main 関数以外のプロローグ
	if (memcmp(s, "main\0", 5) != 0)
	{
		printf("  ret\n");
	}

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
				printf("  push %d\n", argv->val);
				printf("  pop rdi\n");
			case 1:
				printf("  push %d\n", argv->val);
				printf("  pop rsi\n");
			case 2:
				printf("  push %d\n", argv->val);
				printf("  pop rdx\n");
			case 3:
				printf("  push %d\n", argv->val);
				printf("  pop rcx\n");
			case 4:
				printf("  push %d\n", argv->val);
				printf("  pop r8\n");
			case 5:
				printf("  push %d\n", argv->val);
				printf("  pop r9\n");
			}
			count++;
		}
	}
	char s[node->len];
	memcpy(s, node->name, node->len);
	printf("  call %s\n", s);
	// FIXME: 暫定で stack の帳尻合わせ。gen 関数をリファクタした方が良い
	printf("  push 0\n");
	return;
}
