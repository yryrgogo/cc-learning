#include "holycc.h"

static int count(void) {
  static int i = 1;
  return i++;
}

void gen_graph(Node *node) {
  switch(node->kind) {
  case ND_FUNC: {
    gen_graph_func(node);
  }
  }
}

void get_no(char *no) {
  int c = count();
  sprintf(no, "%d", c);
  no[3] = '\0';
}

void *node_name(char *name, char *no) { strcat(name, no); }

void gen_graph_func(Node *node) {
  // char no = get_no();
  char no[4];
  get_no(no);

  char name[10] = "func";
  node_name(name, no);

  printf("%s[function %s]\n", name, no);

  // gen_graph_stmt(node->body, name, no);
}

// void gen_graph_stmt(Node *node, char *name) {
//   char *no = get_no();
//   switch(node->kind) {
//   case ND_RETURN:
//     printf("return%s[RETURN %s]\n", no, no);
//     printf("%s --> return%s\n", name, no);
//     printf("\n");
//     gen_expr(node->lhs);
//     return;
//   case ND_BLOCK: {
//     printf("body%s[BODY %s]\n", no, no);
//     printf("%s --> body%s\n", name, no);
//     char name[10] = "body";
//         printf("\n");
//     Node *cur = node->body;
//     while(cur) {
//       gen_stmt(cur);
//       cur = cur->next;
//     }
//     return;
//   }
//   case ND_FOR: {
//     printf("\n");
//     int c = count();

//     gen_expr(node->init);
//     gen_expr(node->cond);
//     gen_stmt(node->then);
//     gen_expr(node->inc);
//     return;
//   }
//   case ND_WHILE: {
//     printf("\n");
//     int c = count();

//     gen_expr(node->cond);
//     gen_stmt(node->then);
//     return;
//   }
//   case ND_IF: {
//     printf("\n");
//     int c = count();
//     gen_expr(node->cond);

//     gen_stmt(node->then);

//     if(node->els) {
//       gen_stmt(node->els);
//     }
//     return;
//   }
//   case ND_NOP: {
//     printf("\n");
//     return;
//   }
//   default:
//     printf("\n");
//     gen_expr(node);
//   }
// }
