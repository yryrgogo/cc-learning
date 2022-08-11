#include "holycc.h"

// 入力プログラム
char *user_input;

extern LVar *locals;
extern Node *code[100];
extern Token *token;
extern HashMap *str_literal_map;

#define TOMBSTONE ((void *)-1)

// 入力ファイル名
char *filename;

// エラーの起きた場所を報告するための関数
// 下のようなフォーマットでエラーメッセージを表示する
//
// foo.c:10: x = y + + 5;
//                   ^ 式ではありません
void error_at(char *loc, char *msg, ...) {
  // locが含まれている行の開始地点と終了地点を取得
  char *line = loc;
  while(user_input < line && line[-1] != '\n')
    line--;

  char *end = loc;
  while(*end != '\n')
    end++;

  // 見つかった行が全体の何行目なのかを調べる
  int line_num = 1;
  for(char *p = user_input; p < line; p++)
    if(*p == '\n')
      line_num++;

  // 見つかった行を、ファイル名と行番号と一緒に表示
  int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
  fprintf(stderr, "%.*s\n", (int)(end - line), line);

  // エラー箇所を"^"で指し示して、エラーメッセージを表示
  int pos = loc - line + indent;
  fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
  fprintf(stderr, "^ %s\n", msg);
  exit(1);
}

char *read_file(char *path) {
  FILE *fp = fopen(path, "r");
  if(!fp)
    error_at("cannot open %s: %s", path, strerror(errno));

  // ファイルの長さを調べる
  if(fseek(fp, 0, SEEK_END) == -1)
    error_at("%s: fseek: %s", path, strerror(errno));
  size_t size = ftell(fp);
  if(fseek(fp, 0, SEEK_SET) == -1)
    error_at("%s: fseek: %s", path, strerror(errno));

  // ファイルの内容を読み込む
  char *buf = calloc(1, size + 2);
  fread(buf, size, 1, fp);

  // ファイルが必ず"\n\0"で終わるようにする
  if(size == 0 || buf[size - 1] != '\n')
    buf[size++] = '\n';
  buf[size] = '\0';
  fclose(fp);

  return buf;
}

void write_token() {
  FILE *fp = fopen("token.txt", "w");

  Token *tok = token;
  int i = 0;
  while(tok->kind != TK_EOF) {
    char *name = calloc(1, sizeof(char) * (tok->len + 1));
    memcpy(name, tok->str, tok->len);
    name[tok->len] = '\0';
    fprintf(fp, "%d\t:\t%s\t:\t%d\n", i, name, tok->kind);
    tok = tok->next;
    i++;
  }

  fclose(fp);
}

void write_graph() {
  FILE *fp = fopen("graph.md", "w");

  fprintf(fp, "```mermaid\n");
  fprintf(fp, "flowchart TB\n");
  fprintf(fp, "\n");

  for(int i = 0; code[i]; i++) {
    gen_graph(fp, code[i]);
    fprintf(fp, "\n");
  }

  fprintf(fp, "```\n");

  fclose(fp);
}

int main(int argc, char **argv) {
  if(argc != 2) {
    error_at(NULL, "引数の個数が正しくありません");
    return 1;
  }

  char *path = argv[1];
  user_input = read_file(path);
  // user_input = argv[1];

  tokenize(user_input);
  write_token();

  program();

  write_graph();

  // // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");

  for(int i = 0; i < str_literal_map->capacity; i++) {
    if(str_literal_map->buckets[i].key &&
       str_literal_map->buckets[i].key != TOMBSTONE) {
      printf("L%d:\n", str_literal_map->buckets[i].val);
      printf("  .string \"%s\"\n", str_literal_map->buckets[i].key);
      printf("  .text\n");
    }
  }

  // // 抽象構文木を下りながらコード生成
  for(int i = 0; code[i]; i++) {
    gen(code[i]);
  }

  return 0;
}
