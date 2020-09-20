#include "shucc.h"

Map *code;
Func* fn;  // 今読んでる関数


Type *new_ty(TypeKind kind, int size) {
    Type *ret = calloc(1, sizeof(Type)); // Typeのcallocはここでしかしない
    ret->kind = kind;
    ret->type_size = size;
    return ret;
}

Type *int_ty() { return new_ty(TY_INT, 8); } // TODO 4

// Type *char_ty() { return new_ty(TY_CHAR, 1); }

Type *ptr_ty(Type *dest) {
    Type *type = new_ty(TY_PTR, 8);
    type->ptr_to = dest;
    return type;
}

// /**
//  * lvars を参照してこれまで確保されたスタック領域の総和を計算
//  */
// int get_offset(Map *lvars) {
//     int offset = 0;
//     for (int i = 0; i < lvars->len; i++) {
//         LVar *lvar = vec_get(lvars->vals, i);
//         if(!lvar->type)fprintf(stderr, "type is null!!!!!!!!!\n");
//         offset += lvar->type->type_size;
//     }
//     return offset;
// }

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
    node->type = int_ty();
    return node;
}

/*
 * Check if there is a local variable whose tok is tok.
 * @param tok  token to be checked
 */
Lvar* find_lvar(Token* tok) { return map_at(fn->lvars, tok->str); }

/**
 * Read a type.
 */
Type* read_type() {
    Type* type;
    if (consume("int")) {
        type = int_ty();
    } else {
        error("No such type");
    }
    while (consume("*")) {
        type = ptr_ty(type);
    }
    return type;
}

Node* declaration() {
    Type* type = read_type();
    Lvar* lvar = calloc(1, sizeof(Lvar));
    Token* tok = consume_ident();
    lvar->type = type;
    lvar->name = tok->str;
    lvar->len = tok->len;
    Node* node = calloc(1, sizeof(Node));
    node->type = type;
    node->lvar = lvar;
    node->kind = ND_LVAR;
    map_insert(fn->lvars, tok->str, lvar);
    return node;  // ND_LVAR
}

// program = func*
Map *program() {
    code = map_create();
    int i = 0;
    while (!at_eof()) {
        func();
    }
    return code;
}

void func() {
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
    fn->args = vec_create();
    while (!consume(")")) {
        if (0 < fn->args->len) {
            expect(",");
        }
        vec_push(fn->args, declaration());
    }
    
    map_insert(code, fn->name, fn);
    
    expect("{");
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
    node->stmts = vec_create();  // initialize stmts
    while (!consume("}")) {
        vec_push(node->stmts, (void*)stmt());  // push stmt to stmts
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
        return primary();
    }
    if (consume("-")) {
        return new_node(ND_SUB, new_node_num(0), primary());
    }
    if (consume("&")) {
        return new_node(ND_ADDR, unary(), NULL);
    }
    if (consume("*")) {
        return new_node(ND_DEREF, unary(), NULL);
    }
    return primary();
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
        if (consume("(")) {  // function
            Func *fn = map_at(code, tok->str);
            if (!fn) {
                error("function '%s' not defined", tok->str);
            }
            node = calloc(1, sizeof(Node));
            node->kind = ND_FUNC_CALL;
            node->func = fn;
            node->name = tok->str;
            node->args = vec_create();
            while (!consume(")")) {
                if (0 < node->args->len) {
                    expect(",");
                }
                Node* arg = expr();
                vec_push(node->args, arg);
            }
            return node;
        } else {  // variable
            node = calloc(1, sizeof(Node));
            node->kind = ND_LVAR;
            Lvar* lvar = find_lvar(tok);
            if (!lvar) {  // if this is the first time for the lvar to appear
                error("lvar not defined");
            }
            node->lvar = lvar;
            return node;
        }
    }

    // case primary = num
    return new_node_num(expect_number());
}
