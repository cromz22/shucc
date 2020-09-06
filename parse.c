#include "shucc.h"

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
Lvar* find_lvar(Token* tok) { return map_at(lvars, tok->str); }

void program() {
    lvars = map_create();

    int i = 0;
    while (!at_eof()) {
        code[i] = stmt();
        i++;
    }
    code[i] = NULL;
}

Node* stmt() {
    Node* node = expr();
    expect(";");
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
        node->kind = ND_LVAR;
        Lvar* lvar = find_lvar(tok);
        if (!lvar) {  // if this is the first time for the lvar to appear
            lvar = calloc(1, sizeof(Lvar));
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = (lvars->len + 1) * 8;
            map_insert(lvars, tok->str, lvar);
        }
        node->offset = lvar->offset;
        return node;
    }

    // case primary = num
    return new_node_num(expect_number());
}
