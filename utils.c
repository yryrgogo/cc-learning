#include "holycc.h"

int max(int a, int b) { return a > b ? a : b; }

// walk nodes and print them
void walk_nodes(Node *node) {
  if(!node) {
    return;
  }
  printf("%d: %s\n", node->kind, node_kind_name(node->kind));
  walk_nodes(node->lhs);
  walk_nodes(node->rhs);
  walk_nodes(node->init);
  walk_nodes(node->cond);
  walk_nodes(node->inc);
  walk_nodes(node->then);
  walk_nodes(node->els);
  walk_nodes(node->body);
  walk_nodes(node->args);
  walk_nodes(node->next);
  // walk_nodes(node->locals);
}

// update local var offset
void update_lvar_offset(Node *node, HashMap *lvar_map, int max_offset) {
  // TODO: 変数番号とその byte サイズのマップを作るのが良さげ
  if(!node) {
    return;
  } else {
    if(node->kind && node->kind != ND_CALL && node->name) {
      int val = hashmap_get(lvar_map, node->name);
      if(val >= 0) {
        // printf("before name: %s %d\n", node->name, node->offset);
        int new_offset;
        if(node->ty->kind == TY_ARRAY) {
          new_offset =
              max_offset - node->offset + size_of_type(node->ty->ptr_to);
        } else {
          new_offset = max_offset - node->offset + val;
        }
        node->offset = new_offset;
        // printf("after name: %s %d\n", node->name, node->offset);
      }
    }
  }
  // printf("name: %s %d\n", node->name, node->offset);

  update_lvar_offset(node->lhs, lvar_map, max_offset);
  update_lvar_offset(node->rhs, lvar_map, max_offset);
  update_lvar_offset(node->init, lvar_map, max_offset);
  update_lvar_offset(node->cond, lvar_map, max_offset);
  update_lvar_offset(node->inc, lvar_map, max_offset);
  update_lvar_offset(node->then, lvar_map, max_offset);
  update_lvar_offset(node->els, lvar_map, max_offset);
  update_lvar_offset(node->body, lvar_map, max_offset);
  update_lvar_offset(node->args, lvar_map, max_offset);
  update_lvar_offset(node->next, lvar_map, max_offset);
}

char *node_kind_name(NodeKind kind) {
  switch(kind) {
  case ND_ADD:
    return "ND_ADD";
  case ND_SUB:
    return "ND_SUB";
  case ND_MUL:
    return "ND_MUL";
  case ND_DIV:
    return "ND_DIV";
  case ND_EQ:
    return "ND_EQ";
  case ND_NE:
    return "ND_NE";
  case ND_LT:
    return "ND_LT";
  case ND_LE:
    return "ND_LE";
  case ND_ASSIGN:
    return "ND_ASSIGN";
  case ND_DEREF:
    return "ND_DEREF";
  case ND_ADDR:
    return "ND_ADDR";
  case ND_CALL:
    return "ND_CALL";
  case ND_NUM:
    return "ND_NUM";
  case ND_LVAR:
    return "ND_LVAR";
  case ND_RETURN:
    return "ND_RETURN";
  case ND_IF:
    return "ND_IF";
  case ND_WHILE:
    return "ND_WHILE";
  case ND_FOR:
    return "ND_FOR";
  case ND_BLOCK:
    return "ND_BLOCK";
  case ND_FUNC:
    return "ND_FUNC";
  case ND_NOP:
    return "ND_NOP";
  default:
    return "UNKNOWN";
  }
}
