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
    ND_ADDR,       // &
    ND_DEREF,      // *
    ND_BLOCK,      // {}
    ND_FUNC_CALL,  // function call
    ND_FOR,        // for
    ND_WHILE,      // while
    ND_IF,         // if
    ND_RETURN,     // return
    ND_LVAR,       // local variable
    ND_ASSIGN,     // =
    ND_EQ,         // ==
    ND_NE,         // !=
    ND_LE,         // <=
    ND_LT,         // <
    ND_ADD,        // +
    ND_SUB,        // -
    ND_MUL,        // *
    ND_DIV,        // /
    ND_NUM,        // integer
} NodeKind;

/**
 * definition of Vector (~ array in Python)
 */
typedef struct {
    void **data;   // contents of the vector
    int capacity;  // capacity of the vector ( >= # contents )
    int len;       // # contents of the vector
} Vector;

typedef enum {
    TY_INT,
    TY_PTR,
    TY_ARRAY,
} TypeKind;

typedef struct Type Type;
struct Type {
    TypeKind kind;
    Type *ptr_to;
    // size_t array_size;  // array size when kind == ARRAY
};

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
    Node *cond;     // [IF, WHILE, FOR] condition
    Node *then;     // [IF, WHILE, FOR] statement
    Node *els;      // [IF] else statement
    Node *init;     // [FOR] first expression to initialize loop
    Node *loop;     // [FOR] third expression to update loop
    Vector *stmts;  // [BLOCK] statements inside a block
    char *name;     // [FUNC_CALL] function name
    Vector *args;   // [FUND CALL] arguments
    Type *type;
};

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

typedef struct Func {
    char *name;
    Node *body;
    Map *lvars;
    Vector *args;
    Type *return_type;
} Func;

/* utils.c */
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
Vector *vec_create();
void vec_push(Vector *vec, void *elem);
void *vec_get(Vector *vec, int key);
void *vec_set(Vector *vec, int index, void *item);
Map *map_create();
void map_insert(Map *map, char *key, void *val);
void *map_at(Map *map, char *key);
void draw_node_tree(Node *node, int depth, char *role);
void draw_ast(Func **code);

/* tokenize.c */
bool peek(char *op);
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
void program();       // program = func*
Func *func();         // func = ident "(" ")" "{" stmt* "}"
Node *stmt();         // stmt = "return"? expr ";"
                      //      | "if" "(" expr ")" stmt ("else" stmt)?
                      //      | "while" "(" expr ")" stmt
                      //      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
                      //      | "{" stmt* "}"
                      //      | declaration ";"
Node *declaration();  // declaration = "int" ident
Node *expr();         // expr = assign
Node *assign();       // assign = equality ("=" assign)?
Node *equality();     // equality = relational ("==" relational | "!=" relational)*
Node *relational();   // relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *add();          // add = mul ("+" mul | "-" mul)*
Node *mul();          // mul = unary ("*" unary | "/" unary)*
Node *unary();        // unary = ("+" | "-")? primary | "*" unary | "&" unary
Node *primary();      // primary = "(" expr ")" | num | ident ("(" ")")?

/* codegen.c */
void gen(Node *node);
void gen_x86();

/* global variables */
extern char *user_input;  // input program
extern Token *token;      // current token
extern Func *code[100];   // top-level array of statements
