#include "holycc.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 入力プログラム
char *user_input;

extern Token *token;
extern Node *code[100];
extern LVar *locals;

void error_at(char *loc, char *fmt, ...) {
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

int main(int argc, char **argv) {
  if(argc != 2) {
    error_at(NULL, "引数の個数が正しくありません");
    return 1;
  }

  user_input = argv[1];
  tokenize(user_input);

  // for(;;) {
  //   if(token->kind == TK_EOF) {
  //     break;
  //   }
  //   printf("%s\n", token->str);
  //   token = token->next;
  // }
  // return 0;

  program();

  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");

  // 抽象構文木を下りながらコード生成
  for(int i = 0; code[i]; i++) {
    gen(code[i]);
  }

  return 0;
}
