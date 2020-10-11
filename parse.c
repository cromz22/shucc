#include "shucc.h"

/* global variable used inside parse.c */
Map* funcs;     // functions
Map* gvars;     // global variables
Vector* strls;  // string literals
Func* fn;       // function now being parsed

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

/**
 * Read type.
 * @return read type
 */
Type* read_type() {
    Type* type;
    if (consume("int")) {
        type = new_ty_int();
    } else if (consume("char")) {
        type = new_ty_char();
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
Type* new_ty_int() { return new_ty(TY_INT, 4); }

/**
 * Create a new char type instance
 * @return  created char instance
 */
Type* new_ty_char() { return new_ty(TY_CHAR, 1); }

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

Node* new_node_gvar(char* name, Type* type) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_GVAR;
    node->func_name = name;
    node->type = type;
}

Type* read_array(Type* type) {
    Vector* nums = vec_create();
    while (1) {
        if (consume("[")) {
            // type = new_ty_array(type, expect_number()); // type を作らずに vector に push
            vec_push(nums, (void*)(intptr_t)expect_number());
            expect("]");
        } else {
            break;
        }
    }

    // このへんで vector を後ろから見て type を作る
    for (int i = nums->size - 1; i >= 0; i--) {
        type = new_ty_array(type, (int)(intptr_t)vec_get(nums, i));
    }
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
    node->type = type;

    lvar->type = read_array(lvar->type);

    map_insert(fn->lvars, tok->str, lvar);

    return node;
}

Program* program() {
    // fprintf(stderr, "hello from program\n");
    Program* prog = calloc(1, sizeof(Program));
    funcs = map_create();
    gvars = map_create();
    strls = vec_create();
    while (token->kind != TK_EOF) {
        func_or_gvar();  // inside func tokens are updated
    }
    prog->funcs = funcs;
    prog->gvars = gvars;
    prog->strls = strls;
    return prog;
}

void func_or_gvar() {
    // fprintf(stderr, "hello from func\n");
    Type* type = read_type();
    Token* tok = consume_ident();
    if (!tok) {
        error("Unexpected token");
    }

    if (consume("(")) {  // funcs
        fn = calloc(1, sizeof(Func));
        fn->name = tok->str;
        fn->return_type = type;
        fn->lvars = map_create();
        // read arguments
        fn->args = vec_create();
        while (!consume(")")) {
            if (0 < fn->args->size) {
                expect(",");
            }
            vec_push(fn->args, declaration());
        }

        map_insert(funcs, fn->name, fn);

        expect("{");
        // read body
        Node* node = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;
        node->stmts = vec_create();
        while (!consume("}")) {
            vec_push(node->stmts, (void*)stmt());
        }
        fn->body = node;
    } else {  // gvars
        if (map_at(gvars, tok->str)) {
            error("global variable '%s' is already defined", tok->str);
        }

        type = read_array(type);

        Gvar* gv = calloc(1, sizeof(Gvar));
        gv->name = tok->str;
        gv->len = tok->len;
        gv->type = type;
        Node* node = calloc(1, sizeof(Node));
        node->kind = ND_GVAR;
        node->gvar = gv;

        map_insert(gvars, tok->str, gv);
        expect(";");
    }
}

Node* stmt() {
    Node* node;

    if (peek("int") || peek("char")) {
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
            Func* fn = map_at(funcs, tok->str);
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
            Lvar* lvar = map_at(fn->lvars, tok->str);
            if (lvar) {
                node->kind = ND_LVAR;
                node->lvar = lvar;
            } else {
                Gvar* gvar = map_at(gvars, tok->str);
                if (!gvar) {
                    error("variable '%s' is not defined", tok->str);
                }
                node->kind = ND_GVAR;
                node->gvar = gvar;
            }
        }
        return node;
    }

    // case primary = string literal
    if (consume("\"")) {
        Token* tok = consume_string();
        node = calloc(1, sizeof(Node));
        node->kind = ND_STRL;
        node->type = new_ty_ptr(new_ty_char());  // literal の type は parse で決める
        node->strl_id = strls->size;
        vec_push(strls, tok->str);
        expect("\"");
        return node;
    }

    // case primary = num
    return new_node_num(expect_number());
}
