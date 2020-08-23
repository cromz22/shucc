#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

/** 
 * struct of token types
 */
typedef enum {
	TK_RESERVED, // symbol
	TK_NUM,      // number
	TK_EOF,      // end
} TokenKind;

/**
 * token struct
 */
typedef struct Token Token;
struct Token {
	TokenKind kind; // token type
	Token * next;   // next token
	int val;        // number, when kind is TK_NUM
	char * str;     // token string
};

Token * token; // current token

/**
 * Report error.
 * @param fmt  error information
 */
void error(char * fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

char * user_input; // input program

/**
 * Report error with position.
 * @param loc  location of error
 * @param fmt  error information
 */
void error_at(char * loc, char * fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, " ");
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

/**
 * Skip symbol op.
 * @param  op    symbol to be skipped
 * @return bool  true if skipped
 */
bool consume(char op) {
	if (token->kind != TK_RESERVED || token->str[0] != op) {
		return false;
	}
	token = token->next;
	return true;
}

/**
 * Expect the next token to be the symbol op i.e. skip op.
 * @param op  symbol to be expected
 */
void expect(char op) {
	if (token->kind != TK_RESERVED || token->str[0] != op) {
		error("This is not '%c'", op);
	}
	token = token->next;
}

/**
 * Expect number and set the value of the token.
 * @return val  the value of the token
 */
int expect_number() {
	if (token->kind != TK_NUM) {
		error("This is not a number");
	}
	int val = token->val;
	token = token->next;
	return val;
}

/**
 * See if the current token is EOF.
 * @return bool  true if EOF
 */
bool at_eof() {
	return token->kind == TK_EOF;
}

/**
 * Create a new token tok, which is the next token of cur, and returns it.
 * @param  kind  kind of the new token
 * @param  cur   current token
 * @param  str   token string
 * @return tok   created token
 */
Token *new_token(TokenKind kind, Token * cur, char * str) {
	Token * tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return tok;
}

/**
 * Tokenize string *p.
 * @param p  string to be tokenized
 * @return   the first token of tokenized *p
 */
Token *tokenize(char * p) {
	Token head; // the #0 token (which is used only inside this function)
	head.next = NULL; // after the last token comes NULL
	Token * cur = &head; // initialize cur with head

	while (*p) {
		// skip spaces
		if (isspace(*p)) {
			p++;
			continue;
		}

		// tokenize + or -
		if (*p == '+' || *p == '-') {
			cur = new_token(TK_RESERVED, cur, p); // create a new token, the type of which is TK_RESERVED, the previous token of which is cur, and the string representation of which is character *p
			p++;
			continue;
		}

		// tokenize number
		/*
		 The strtol function is from stdlib.h.
		 It converts string *p to type long int with base 10.
		 If string *p is not totally number, then the rest of *p is left untouched.
		 The second argument, which is an address, will indicate the position of it.
		*/
		if (isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error("Could not tokenize the string");
	}

	new_token(TK_EOF, cur, p); // create the last token

	return head.next; // the first token of the tokenized string

}

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

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	printf("  mov rax, %d\n", expect_number());

	while(!at_eof()) {
		if (consume('+')) {
			printf("  add rax, %d\n", expect_number());
			continue;
		}

		expect('-');
		printf("  sub rax, %d\n", expect_number());
	}

	printf("  ret\n");
	return 0;
}
