#!/bin/bash

graph() {
  ./holycc "$1" "1" > ast_graph.md
	cat ast_graph.md
	exit 0
}


graph "
int main() {
	int a;
	a = 5;
	return a;
}"
