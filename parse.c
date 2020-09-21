#include "shucc.h"

/* global variable */
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
    return node;
}

/*
 * Check if there is a local variable whose name is tok.
 * @param tok  token to be checked
 */
Lvar* find_lvar(Token* tok) { return map_at(fn->lvars, tok->str); }

void program() {
    int i = 0;
    while (!at_eof()) {
        code[i] = func();
        i++;
    }
    code[i] = NULL;
}

Func* func() {
    Token* tok = consume_ident();
    if (!tok) {
        error("Unexpected token");
    }

    fn = calloc(1, sizeof(Func));
    fn->name = tok->str;
    fn->lvars = map_create();
    expect("(");
    // read arguments
    fn->args = vec_create();
    while (!consume(")")) {
        if (0 < fn->args->size) {
            expect(",");
        }
        Node* node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        Lvar* lvar = calloc(1, sizeof(Lvar));
        tok = consume_ident();
        lvar->name = tok->str;
        lvar->len = tok->len;
        lvar->offset = (fn->lvars->size + 1) * 8;

        map_insert(fn->lvars, tok->str, lvar);

        node->offset = lvar->offset;
        vec_push(fn->args, node);
    }
    expect("{");
    // read body
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
    node->stmts = vec_create();
    while (!consume("}")) {
        vec_push(node->stmts, (void*)stmt());
    }
    fn->body = node;
    return fn;
}

Node* stmt() {
    Node* node;

    if (consume("{")) {
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
        node = calloc(1, sizeof(Node));
        if (consume("(")) {  // function
            node->kind = ND_FUNC_CALL;
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
                lvar = calloc(1, sizeof(Lvar));
                lvar->name = tok->str;
                lvar->len = tok->len;
                lvar->offset = (fn->lvars->size + 1) * 8;
                map_insert(fn->lvars, tok->str, lvar);
            }
            node->offset = lvar->offset;
        }
        return node;
    }

    // case primary = num
    return new_node_num(expect_number());
}
