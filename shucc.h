#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Token Token;
typedef struct Node Node;
typedef struct Lvar Lvar;
typedef struct Vector Vector;
typedef struct Map Map;

/**
 * TokenKind and Token
 */
typedef enum {
    TK_IDENT,     // identifier
    TK_RESERVED,  // symbol
    TK_NUM,       // number
    TK_EOF,       // end
} TokenKind;

struct Token {
    TokenKind kind;  // token type
    Token *next;     // next token
    int val;         // number, when kind is TK_NUM
    char *str;       // token string
    int len;         // length of token (e.g. 2 when ==)
};

/**
 * NodeKind and Node
 */
typedef enum {
    ND_BLOCK,   // {}
    ND_FOR,     // for
    ND_WHILE,   // while
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

struct Node {
    NodeKind kind;  // kind of the node
    Node *lhs;      // left child
    Node *rhs;      // right child
    int val;        // leaf node (integer)
    int offset;     // offset of variables (from RBP)
    Node *cond;     // [IF, WHILE, FOR] condition
    Node *then;     // [IF, WHILE, FOR] statement
    Node *els;      // [IF] else statement
    Node *init;     // [FOR] first expression to initialize loop
    Node *loop;     // [FOR] third expression to update loop
    Vector *stmts;  // [BLOCK] statements inside a block
};

/**
 * Lvar (struct for local variables)
 */
struct Lvar {
    char *name;  // variable name
    int len;     // length of the name
    int offset;  // offset of the variable from RBP
};

/**
 * Vector (~ array in Python)
 */
struct Vector {
    void **data;   // contents of the vector
    int capacity;  // capacity of the vector ( >= # contents )
    int len;       // # contents of the vector
};

/**
 * Map (~ dictionary in Python)
 */
struct Map {
    Vector *keys;  // keys
    Vector *vals;  // values
    int len;       // # contents of the Map
};

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
                     //      | "{" stmt* "}""
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
