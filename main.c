#include "shucc.h"

/* global variables */
char* user_input;  // input program
Token* token;      // current token
// Func* code[100];   // top-level array of statements

/**
 * main function
 * @param argc  the number of runtime arguments
 * @param argv  a string array of arguments
 */
int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "invalid input\n");
        return 1;
    }

    token = tokenize(argv[1]);  // tokenize the given string and initialize token by the first one
    // fprintf(stderr, "tokenize OK\n");
    Map *code = program();  // generate AST from input
    // fprintf(stderr, "AST OK\n");
    // draw_ast(code);
    sema(code);
    // fprintf(stderr, "sema OK\n");

    gen_x86(code);

    return 0;
}
