#include "shucc.h"

/* global variables */
char * user_input; // input program
Token * token; // current token

/**
 * main function
 * @param argc  the number of runtime arguments
 * @param argv  a string array of arguments
 */
int main(int argc, char ** argv) {
	if (argc != 2) {
		fprintf(stderr, "invalid input\n");
		return 1;
	}

	token = tokenize(argv[1]); // tokenize the given string and initialize token by the first one
	Node * node = expr(); // generate AST from input

	// prologue
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	gen(node); // generate assembly codes from AST

	printf("  pop rax\n"); // pop the result from stack top
	printf("  ret\n"); // and return the value
	return 0;
}
