#include "shucc.h"

/* global variables */
char* user_input;  // input program
Token* token;      // current token
Map* code;         // top-level array of statements

/**
 * Print out each token str
 * @param argv  a string array of arguments
 */
void check_tokenization(char** argv) {
    token = tokenize(argv[1]);
    while (token->kind != TK_EOF) {
        fprintf(stderr, "%s ", token->str);
        token = token->next;
    }
    fprintf(stderr, "\n");
    // now token is the last one, need to update token
    token = tokenize(argv[1]);
}

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

    int debug = 0;
    if (debug) {
        token = tokenize(argv[1]);
        check_tokenization(argv);
        fprintf(stderr, "tokenize OK\n");

        code = program();
        fprintf(stderr, "AST OK\n");
        draw_ast();
        fprintf(stderr, "draw AST OK\n");

        sema();
        fprintf(stderr, "sema OK");

        gen_x86_64();
    } else {
        token = tokenize(argv[1]);  // tokenize the given string and initialize token by the first one
        code = program();           // generate AST from input
        sema();
        gen_x86_64();
    }

    return 0;
}
