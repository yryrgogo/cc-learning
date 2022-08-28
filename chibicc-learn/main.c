#include "chibicc.h"

int main(int argc, char **argv) {
  if(argc != 2) {
    fprintf(stderr, "%s: invalid number of arguments\n", argv[0]);
    return 1;
  }

  // Tokenize and parse.
  Token *tok = tokenize(argv[1]);
  Obj *prog = parse(tok);

  // Traverse the AST to emit assembly.
  codegen(prog);

  return 0;
}
