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

/*
 * kind of nodes in the syntax tree
 */
typedef enum {
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_NUM, // integer
} NodeKind;

/*
 * definition of node
 */
typedef struct Node Node;
struct Node {
	NodeKind kind; // kind of the node
	Node * lhs;    // left child
	Node * rhs;    // right child
	int val;       // leaf node (integer)
};

/* utils.c */
void error(char * fmt, ...);
void error_at(char * loc, char * fmt, ...);

/* tokenize.c */
bool consume(char op);
void expect(char op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token * cur, char * str);
Token *tokenize(char * p);

/* parse.c */
Node * new_node(NodeKind kind, Node * lhs, Node * rhs);
Node * new_node_num(int val);
Node * expr();    // expr = mul ("+" mul | "-" mul)*
Node * mul();     // mul = unary ("*" unary | "/" unary)*
Node * unary();   // unary = ("+" | "-")? primary
Node * primary(); // primary = "(" expr ")" | num

/* codegen.c */
void gen(Node * node);

/* global variables */
extern char * user_input; // input program
extern Token * token; // current token
