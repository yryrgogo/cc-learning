#include "9cc.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

// 入力プログラム
char* user_input;

extern Token* token;
extern Node* code[100];

// エラーを報告するための関数
// printf と同じ引数をとる
void error(char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

void error_at(char* loc, char* fmt, ...)
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

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		error("引数の個数が正しくありません");
		return 1;
	}

	user_input = argv[1];
	tokenize(user_input);

	// for (;;)
	// {
	// 	if (token->kind == TK_EOF) {
	// 		break;
	// 	}
	// 	// char s[10];
	// 	// printf("%s\n", strncpy(s, token->str, token->len));
	// 	printf("%s\n", token->str);
	// 	token = token->next;
	// }
	// return 0;

	program();

	// アセンブリの前半部分を出力
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// プロローグ
	// 変数26個分の領域を確保する
	// NOTE: これらを追加しわすれて、139が返るエラーにハマってた
	printf("  push rbp\n");
	printf("  mov rbp, rsp\n");
	printf("  sub rsp, 208\n");

	// 抽象構文木を下りながらコード生成
	for (int i = 0; code[i]; i++)
	{
		gen(code[i]);
		// 式の評価結果としてスタックに一つの値が残っているはずなので、
		// スタックが溢れないようにポップしておく
		printf("  pop rax\n");
	}

	// エピローグ
	// スタックトップに式全体の値が残っているはずなので
	// それを RAX にロードして関数からの返り値とする
	printf("  mov rsp, rbp\n");
	printf("  pop rbp\n");
	printf("  ret\n");
	return 0;
}
