#include "shucc.h"

/* global variable used inside parse.c */
Func* fn;  // function now being parsed

/*
 * Create a non-leaf node.
 * @param kind  node kind
 * @param lhs   lhs of the node
 * @param rhs   rhs of the node
 */
Node* new_node(NodeKind kind, Node* lhs, Node* rhs) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

/*
 * Create a leaf node.
 * @param val  an integer
 */
Node* new_node_num(int val) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    node->type = new_ty_int();
    return node;
}

/*
 * Check if there is a local variable whose name is tok.
 * @param tok  token to be checked
 */
Lvar* find_lvar(Token* tok) { return map_at(fn->lvars, tok->str); }

/**
 * Read type.
 * @return read type
 */
Type* read_type() {
    Type* type;
    if (consume("int")) {
        type = new_ty_int();
    } else {
        error("No such type");
    }
    while (consume("*")) {
        type = new_ty_ptr(type);
    }
    return type;
}

/**
 * Create a new Type instance
 * @param kind  type kind of the new instance
 * @param size  type size of the new instance (e.g. 4 if int)
 * @return      created instance
 */
Type* new_ty(TypeKind kind, int size) {
    Type* type = calloc(1, sizeof(Type));  // calloc of Type is done only in this function
    type->kind = kind;
    type->type_size = size;
    return type;
}

/**
 * Create a new int type instance
 * @return  created int instance
 */
Type* new_ty_int() {
    return new_ty(TY_INT, 8);  // TODO: 4
}

/**
 * Create a new pointer type instance
 * @param dest  destination of the pointer (e.g. int if int*)
 * @return      created pointer instance
 */
Type* new_ty_ptr(Type* dest) {
    Type* type = new_ty(TY_PTR, 8);
    type->ptr_to = dest;
    return type;
}

/**
 * Create a new array type instance
 * @param dest  destination of the pointer (e.g. int if int*)
 * @return      created pointer instance
 */
Type* new_ty_array(Type* dest, int num) {
    Type* type = new_ty(TY_ARRAY, num * dest->type_size);
    // fprintf(stderr, "type->type_size: %d\n", type->type_size);
    type->ptr_to = dest;
    type->array_size = num;
    return type;
}

/* grammatical functions */

Node* declaration() {
    Type* type = read_type();

    Lvar* lvar = calloc(1, sizeof(Lvar));
    Token* tok = consume_ident();
    lvar->name = tok->str;
    lvar->len = tok->len;
    lvar->type = type;

    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->lvar = lvar;

    if (consume("[")) {
        int num = expect_number();
        lvar->type = new_ty_array(type, num);
        expect("]");
    }

    map_insert(fn->lvars, tok->str, lvar);

    return node;
}

Map* program() {
    // fprintf(stderr, "hello from program\n");
    code = map_create();
    while (token->kind != TK_EOF) {
        func();  // inside func tokens are updated
    }
    return code;
}

void func() {
    // fprintf(stderr, "hello from func\n");
    Type* type = read_type();
    Token* tok = consume_ident();
    if (!tok) {
        error("Unexpected token");
    }

    fn = calloc(1, sizeof(Func));
    fn->name = tok->str;
    fn->return_type = type;
    fn->lvars = map_create();
    expect("(");
    // read arguments
    fn->args = vec_create();
    while (!consume(")")) {
        if (0 < fn->args->size) {
            expect(",");
        }
        vec_push(fn->args, declaration());
    }

    map_insert(code, fn->name, fn);

    expect("{");
    // read body
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
    node->stmts = vec_create();
    while (!consume("}")) {
        vec_push(node->stmts, (void*)stmt());
    }
    fn->body = node;
}

Node* stmt() {
    Node* node;

    if (peek("int")) {
        node = declaration();
        expect(";");
    } else if (consume("{")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;
        node->stmts = vec_create();  // initialize stmts
        while (!consume("}")) {
            vec_push(node->stmts, (void*)stmt());  // push stmt to stmts
        }
    } else if (consume("return")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
        if (!consume(";")) {
            // fprintf(stderr, "hello from stmt return\n");
            error("';' is expected");
        }
    } else if (consume("if")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        if (consume("else")) {
            node->els = stmt();
        }
    } else if (consume("while")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
    } else if (consume("for")) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_FOR;
        expect("(");
        node->init = expr();
        expect(";");
        node->cond = expr();
        expect(";");
        node->loop = expr();
        expect(")");
        node->then = stmt();
    } else {
        node = expr();
        if (!consume(";")) {
            error("';' is expected");
        }
    }

    return node;
}

Node* expr() {
    Node* node = assign();
    return node;
}

Node* assign() {
    Node* node = equality();
    if (consume("=")) {
        node = new_node(ND_ASSIGN, node, assign());
    }
    return node;
}

Node* equality() {
    Node* node = relational();

    for (;;) {
        if (consume("==")) {
            node = new_node(ND_EQ, node, relational());
        } else if (consume("!=")) {
            node = new_node(ND_NE, node, relational());
        } else {
            return node;
        }
    }
}

Node* relational() {
    Node* node = add();

    for (;;) {
        if (consume("<=")) {
            node = new_node(ND_LE, node, add());
        } else if (consume(">=")) {
            node = new_node(ND_LE, add(), node);  // ND_GE
        } else if (consume("<")) {
            node = new_node(ND_LT, node, add());
        } else if (consume(">")) {
            node = new_node(ND_LT, add(), node);  // ND_GT
        } else {
            return node;
        }
    }
}

Node* add() {
    Node* node = mul();

    for (;;) {
        if (consume("+")) {
            node = new_node(ND_ADD, node, mul());
        } else if (consume("-")) {
            node = new_node(ND_SUB, node, mul());
        } else {
            return node;
        }
    }
}

Node* mul() {
    Node* node = unary();

    for (;;) {
        if (consume("*")) {
            node = new_node(ND_MUL, node, unary());
        } else if (consume("/")) {
            node = new_node(ND_DIV, node, unary());
        } else {
            return node;
        }
    }
}

Node* unary() {
    if (consume("+")) {
        return postfix();
    }
    if (consume("-")) {
        return new_node(ND_SUB, new_node_num(0), postfix());
    }
    if (consume("&")) {
        return new_node(ND_ADDR, unary(), NULL);
    }
    if (consume("*")) {
        return new_node(ND_DEREF, unary(), NULL);
    }
    if (consume("sizeof")) {
        return new_node(ND_SIZEOF, unary(), NULL);
    }
    return postfix();
}

Node* postfix() {
    Node* node = primary();
    while (true) {
        if (consume("[")) {
            Node* expression = expr();
            expect("]");
            Node* sum = new_node(ND_ADD, node, expression);
            node = new_node(ND_DEREF, sum, NULL);
        } else {
            return node;
        }
    }
}

Node* primary() {
    Node* node;
    // case primary = "(" expr ")"
    if (consume("(")) {
        node = expr();
        expect(")");
        return node;
    }

    // case primary = ident
    Token* tok = consume_ident();
    if (tok) {
        node = calloc(1, sizeof(Node));
        if (consume("(")) {  // function
            Func* fn = map_at(code, tok->str);
            if (!fn) {
                error("function '%s' is not defined", tok->str);
            }
            node->kind = ND_FUNC_CALL;
            node->func = fn;
            node->func_name = tok->str;
            node->args = vec_create();
            while (!consume(")")) {
                if (0 < node->args->size) {
                    expect(",");
                }
                Node* arg = expr();
                vec_push(node->args, arg);
            }
        } else {  // local variable
            node->kind = ND_LVAR;
            Lvar* lvar = find_lvar(tok);
            if (!lvar) {  // if this is the first time for the lvar to appear
                error("lvar not defined");
            }
            node->lvar = lvar;
        }
        return node;
    }

    // case primary = num
    return new_node_num(expect_number());
}
