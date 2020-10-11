#include "shucc.h"

/* global variables */
char* user_input;  // input program
Token* token;      // current token

/**
 * Print out each token str
 * @param argv  a string array of arguments
 */
void check_tokenization(Token* tok) {
    while (tok->kind != TK_EOF) {
        if (tok->kind == TK_NUM) {
            fprintf(stderr, "%d ", tok->val);
        } else {
            fprintf(stderr, "%s ", tok->str);
        }
        tok = tok->next;
    }
    fprintf(stderr, "\n");
}

/**
 * Read file
 * @param path  file path
 */
char* read_file(char* path) {
    // open file
    FILE* fp = fopen(path, "r");
    if (!fp) error("cannot open %s: %s", path, strerror(errno));

    // seek toward end, get file size, and seek again toward start
    if (fseek(fp, 0, SEEK_END) == -1) {
        error("%s: fseek: %s", path, strerror(errno));
    }
    size_t size = ftell(fp);
    if (fseek(fp, 0, SEEK_SET) == -1) {
        error("%s: fseek: %s", path, strerror(errno));
    }

    // move file content to buf
    char* buf = calloc(1, size + 2);  // 2: \n\0
    fread(buf, size, 1, fp);

    // end file with \n and append \0
    if (size == 0 || buf[size - 1] != '\n') {
        buf[size++] = '\n';
    }
    buf[size] = '\0';
    fclose(fp);
    return buf;
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
        token = tokenize(read_file(argv[1]));
        check_tokenization(token);
        fprintf(stderr, "tokenize OK\n");

        Program* prog = program();
        fprintf(stderr, "AST OK\n");

        sema(prog);
        fprintf(stderr, "sema OK\n");

        draw_ast(prog);
        fprintf(stderr, "draw AST OK\n");

        gen_x86_64(prog);
    } else {
        token = tokenize(read_file(argv[1]));  // tokenize the given string and initialize token by the first one
        Program* prog = program();             // generate AST from input
        sema(prog);
        gen_x86_64(prog);
    }

    return 0;
}
