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
