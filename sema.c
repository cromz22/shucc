#include "shucc.h"

Node *walk(Node *node) {
    switch (node->kind) {
        case ND_NUM:
            return node;
        case ND_LVAR:
            return node;
        // case ND_GVAR:
        //     return maybe_decay(node, decay);
        case ND_IF:
            node->cond = walk(node->cond);
            node->then = walk(node->then);
            if (node->els) {
                node->els = walk(node->els);
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
        case ND_ADD:
            node->lhs = walk(node->lhs);
            node->rhs = walk(node->rhs);
            // if (node->rhs->ty->ty == TY_PTR) {
            //     Node *tmp = node->lhs;
            //     node->lhs = node->rhs;
            //     node->rhs = tmp;
            // }
            // if (node->lhs->ty->ty == TY_PTR) {
            //     node->rhs = scale_ptr(ND_MUL, node->rhs, node->lhs->ty);
            //     node->ty = node->lhs->ty;
            // } else {
            //     node->ty = int_ty();
            // }
            return node;
        case ND_SUB:
            node->lhs = walk(node->lhs);
            node->rhs = walk(node->rhs);

            // Type *lty = node->lhs->ty;
            // Type *rty = node->rhs->ty;

            // if (lty->ty == TY_PTR && rty->ty == TY_PTR) {
            //     node = scale_ptr(ND_DIV, node, lty);
            //     node->ty = lty;
            // } else {
            //     node->ty = int_ty();
            // }
            return node;
        case ND_ASSIGN:
            node->lhs = walk(node->lhs);
            // check_referable(node->lhs);
            node->rhs = walk(node->rhs);
            // node->ty = node->lhs->ty;
            return node;
        case ND_MUL:
        case ND_DIV:
        case ND_EQ:
        case ND_NE:
        case ND_LE:
        case ND_LT:
            node->lhs = walk(node->lhs);
            node->rhs = walk(node->rhs);
            // check_int(node->lhs);
            // check_int(node->rhs);
            // node->ty = int_ty();
            return node;
        case ND_ADDR:
            node->lhs = walk(node->lhs);
            // check_referable(node->lhs);
            // node->ty = ptr_to(node->lhs->ty);
            return node;
        case ND_DEREF:
            node->lhs = walk(node->lhs);
            // assert(node->lhs->ty->ty == TY_PTR);
            // node->ty = node->lhs->ty->base;
            return node;
        case ND_RETURN:
            node->lhs = walk(node->lhs);
            return node;
        case ND_FUNC_CALL:
            for (int i = 0; i < node->args->len; i++) {
                Node *arg = vec_get(node->args, i);
                vec_set(node->args, i, walk(arg));
            }
            return node;
        case ND_BLOCK:
            for (int i = 0; i < node->stmts->len; i++) {
                Node *stmt = vec_get(node->args, i);
                vec_set(node->stmts, i, walk(stmt));
            }
            return node;
        default:
            fprintf(stderr, "\n\n%d\n\n", node->kind);
            // assert(0 && "Unknown node type");
    }
}

void sema(Func **code) {
    for (int i = 0; i < 100; i++) {
        Func *fn = code[i];
        if (!fn) {
            break;
        }
        fn->body = walk(fn->body);
    }
}
