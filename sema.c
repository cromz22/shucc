#include "shucc.h"

Node* walk(Node* node) {
    // fprintf(stderr, "hello from walk!\n");
    assert(node && "Cannot walk on NULL node");
    switch (node->kind) {
        case ND_ADDR:
            node->lhs = walk(node->lhs);
            return node;
        case ND_DEREF:
            node->lhs = walk(node->lhs);
            return node;
        case ND_FUNC_CALL:
            for (int i = 0; i < node->args->size; i++) {
                Node* arg = walk(vec_get(node->args, i));
                vec_set(node->args, i, arg);
            }
            return node;
        case ND_BLOCK:
            for (int i = 0; i < node->stmts->size; i++) {
                Node* stmt = vec_get(node->stmts, i);
                vec_set(node->stmts, i, walk(stmt));
            }
            return node;
        case ND_FOR:
            if (node->init) {
                node->init = walk(node->init);
            }
            if (node->cond) {
                node->cond = walk(node->cond);
            }
            if (node->loop) {
                node->loop = walk(node->loop);
            }
            node->then = walk(node->then);
            return node;
        case ND_WHILE:
            node->cond = walk(node->cond);
            node->then = walk(node->then);
            return node;
        case ND_IF:
            node->cond = walk(node->cond);
            node->then = walk(node->then);
            if (node->els) {
                node->els = walk(node->els);
            }
            return node;
        case ND_RETURN:
            node->lhs = walk(node->lhs);
            return node;
        case ND_LVAR:
            return node;
        case ND_ASSIGN:
            node->lhs = walk(node->lhs);
            node->rhs = walk(node->rhs);
            return node;
        case ND_EQ:
        case ND_NE:
        case ND_LE:
        case ND_LT:
        case ND_MUL:
        case ND_DIV:
            node->lhs = walk(node->lhs);
            node->rhs = walk(node->rhs);
            return node;
        case ND_ADD:
            node->lhs = walk(node->lhs);
            node->rhs = walk(node->rhs);
            return node;
        case ND_SUB:
            node->lhs = walk(node->lhs);
            node->rhs = walk(node->rhs);
            return node;
        case ND_NUM:
            return node;
        default:
            error("Unknown node kind: %d", node->kind);
    }
}

void sema() {
    for (int i = 0; i < code->size; i++) {
        Func* fn = vec_get(code, i);
        fn->body = walk(fn->body);
    }
}