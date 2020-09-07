#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * struct of token types
 */
typedef enum {
    TK_IDENT,     // identifier
    TK_RESERVED,  // symbol
    TK_NUM,       // number
    TK_EOF,       // end
} TokenKind;

/**
 * token struct
 */
typedef struct Token Token;
struct Token {
    TokenKind kind;  // token type
    Token *next;     // next token
    int val;         // number, when kind is TK_NUM
    char *str;       // token string
    int len;         // length of token (e.g. 2 when ==)
};

/**
 * kind of nodes in the syntax tree
 */
typedef enum {
    ND_IF,      // if
    ND_RETURN,  // return
    ND_LVAR,    // local variable
    ND_ASSIGN,  // =
    ND_EQ,      // ==
    ND_NE,      // !=
    ND_LE,      // <=
    ND_LT,      // <
    ND_ADD,     // +
    ND_SUB,     // -
    ND_MUL,     // *
    ND_DIV,     // /
    ND_NUM,     // integer
} NodeKind;

/**
 * definition of node
 */
typedef struct Node Node;
struct Node {
    NodeKind kind;  // kind of the node
    Node *lhs;      // left child
    Node *rhs;      // right child
    int val;        // leaf node (integer)
    int offset;     // offset of variables (from RBP)
    Node *cond;     // [IF] condition
    Node *then;     // [IF] statement
    Node *els;      // [IF] else statement
};

/**
 * definition of Vector (~ array in Python)
 */
typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

/**
 * definition of Map (~ dictionary in Python)
 */
typedef struct {
    Vector *keys;
    Vector *vals;
    int len;
} Map;

/**
 * definition of Lvar (struct for local variables)
 */
typedef struct Lvar {
    char *name;
    int len;
    int offset;
} Lvar;

/* utils.c */
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
Vector *vec_create();
void vec_push(Vector *vec, void *elem);
void *vec_get(Vector *vec, int key);
Map *map_create();
void map_insert(Map *map, char *key, void *val);
void *map_at(Map *map, char *key);
void draw_node_tree(Node *node, int depth, char *role);
void draw_ast(Node **code);

/* tokenize.c */
bool consume(char *op);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
Token *tokenize(char *p);

/* parse.c */
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
void program();      // program = stmt*
Node *stmt();        // stmt = "return"? expr ";"
                     //      | "if" "(" expr ")" stmt ("else" stmt)?
                     //      | "while" "(" expr ")" stmt
                     //      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
Node *expr();        // expr = assign
Node *assign();      // assign = equality ("=" assign)?
Node *equality();    // equality = relational ("==" relational | "!=" relational)*
Node *relational();  // relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *add();         // add = mul ("+" mul | "-" mul)*
Node *mul();         // mul = unary ("*" unary | "/" unary)*
Node *unary();       // unary = ("+" | "-")? primary
Node *primary();     // primary = "(" expr ")" | num | ident

/* codegen.c */
void gen(Node *node);
void gen_x86();

/* global variables */
extern char *user_input;  // input program
extern Token *token;      // current token
extern Node *code[100];   // top-level array of statements
extern Map *lvars;        // lvar manager
