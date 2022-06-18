#include "holycc.h"

static int count(void) {
  static int i = 1;
  return i++;
}

void get_no(char *no) {
  int c = count();
  sprintf(no, "%d", c);
  no[3] = '\0';
}

void *node_name(char *name) {
  char no[4];
  get_no(no);
  strcat(name, no);
}

void gen_graph(Node *node) {
  switch(node->kind) {
  case ND_FUNC: {
    gen_func_node(node);
  }
  default: {
    printf("%%%% gen_graph\n");
    printf("\n");
  }
  }
}

void gen_func_node(Node *node) {
  char name[12] = "func";
  node_name(name);

  printf("%s[%s]\n", name, name);

  gen_graph_stmt(node->body, name);
}

void gen_edge(char *parent_name, char *name) {
  printf("%s[%s]\n", name, name);
  printf("%s --> %s\n", parent_name, name);
  printf("\n");
}

void gen_block_edge(char *new_name, char *parent_name) {
  char name[12] = "block";
  node_name(name);
  strcpy(new_name, name);

  gen_edge(parent_name, name);
}

void gen_return_edge(char *new_name, char *parent_name) {
  char name[12] = "return";
  node_name(name);
  strcpy(new_name, name);

  gen_edge(parent_name, name);
}

void gen_num_edge(char *new_name, char *parent_name) {
  char name[12] = "num";
  node_name(name);
  strcpy(new_name, name);

  gen_edge(parent_name, name);
}

void gen_lvar_edge(char *new_name, char *parent_name) {
  char name[12] = "lvar";
  node_name(name);
  strcpy(new_name, name);

  gen_edge(parent_name, name);
}

void gen_assign_edge(char *new_name, char *parent_name) {
  char name[12] = "assign";
  node_name(name);
  strcpy(new_name, name);

  gen_edge(parent_name, name);
}

void gen_graph_stmt(Node *node, char *parent_name) {
  char name[12] = "";
  switch(node->kind) {
  case ND_RETURN:
    gen_return_edge(name, parent_name);
    gen_graph_expr(node->lhs, name);
    return;
  case ND_BLOCK: {
    gen_block_edge(name, parent_name);
    Node *cur = node->body;
    while(cur) {
      gen_graph_stmt(cur, name);
      cur = cur->next;
    }
    return;
  }
  default: {
    gen_graph_expr(node, parent_name);
    printf("\n");
  }
    // case ND_FOR: {
    //   printf("\n");
    //   int c = count();

    //   gen_expr(node->init);
    //   gen_expr(node->cond);
    //   gen_stmt(node->then);
    //   gen_expr(node->inc);
    //   return;
    // }
    // case ND_WHILE: {
    //   printf("\n");
    //   int c = count();

    //   gen_expr(node->cond);
    //   gen_stmt(node->then);
    //   return;
    // }
    // case ND_IF: {
    //   printf("\n");
    //   int c = count();
    //   gen_expr(node->cond);

    //   gen_stmt(node->then);

    //   if(node->els) {
    //     gen_stmt(node->els);
    //   }
    //   return;
    // }
    // case ND_NOP: {
    //   printf("\n");
    //   return;
    // }
    // default:
    //   printf("\n");
    //   gen_expr(node);
  }
}

void gen_graph_expr(Node *node, char *parent_name) {
  char name[12] = "";

  switch(node->kind) {
  case ND_NUM:
    gen_num_edge(name, parent_name);
    return;
  case ND_LVAR:
    gen_lvar_edge(name, parent_name);
    return;
  case ND_ASSIGN:
    gen_assign_edge(name, parent_name);
    gen_graph_expr(node->lhs, name);
    gen_graph_expr(node->rhs, name);
  }
}
