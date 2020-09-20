#include "shucc.h"

// Node *scale_ptr(int op, Node *base, Type *type) {
//     Node *node = calloc(1, sizeof(Node));
//     node->kind = op;
//     node->lhs = base;
//     node->rhs = new_node_num(type->ptr_to->size);
//     return node;
// }

void check_int(Node *node) {
    int ty = node->type->kind;
    assert(ty == TY_INT && "Not an integer");
}

// node がアドレスを持っていることを保証
void check_referable(Node *node) {
    NodeKind kind = node->kind;
    assert((kind == ND_LVAR || kind == ND_DEREF) && "Not referable");
}

bool same_type(Type *x, Type *y) {
    if (x->kind != y->kind) {
        return false;
    }

    switch (x->kind) {
    case TY_PTR:
        return same_type(x->ptr_to, y->ptr_to);
    // case ARRAY:
    //     return x->array_size == y->array_size && same_type(x->ptr_to, y->ptr_to);
    default:
        return true;
    }
}

/**
 * Walk from leaf to root and set type size
 */
Node *walk(Node *node) {
    if (!node) {
        error("cannot walk on NULL node\n");
    }
    switch (node->kind) {
        case ND_NUM:
            return node;
        case ND_LVAR:
            node->type = node->lvar->type;
            return node;
        // case ND_GVAR:
        //     return maybe_decay(node, decay);
        case ND_IF:
            node->cond = walk(node->cond);
            // fprintf(stderr, "cond walked\n");
            node->then = walk(node->then);
            // fprintf(stderr, "then walked\n");
            if (node->els) {
                // fprintf(stderr, "else start to walk\n");
                node->els = walk(node->els);
                // fprintf(stderr, "else walked\n");
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
            // fprintf(stderr, "add walking\n");

            if (node->rhs->type->kind == TY_PTR) { //  PTRが必ず左辺に来るようにして場合分けを減らす
                Node *tmp = node->lhs;
                node->lhs = node->rhs;
                node->rhs = tmp;
            }
            assert(node->rhs->type->kind == TY_INT && "invalid operation TY_PTR + TY_PTR"); // PTR + PTR は死ぬ
            if (node->lhs->type->kind == TY_PTR) { // and rhs->type->kind == TY_INT
                node->rhs = new_node(ND_MUL, node->rhs, new_node_num(node->rhs->type->type_size));
                node->rhs->type = int_ty();
                node->type = node->lhs->type;
            } else {
                node->type = int_ty();
            }
            return node;
        case ND_SUB:
            node->lhs = walk(node->lhs);
            node->rhs = walk(node->rhs);

            Type *lty = node->lhs->type;
            Type *rty = node->rhs->type;

            if (lty->kind == TY_PTR) {
                if (rty->kind == TY_PTR) {
                    if (!same_type(lty, rty)) {
                        error("Incompatible pointer");
                    }
                    node = new_node(ND_DIV, node, new_node_num(lty->type_size));
                } else {
                    node->rhs = new_node(ND_MUL, node->rhs, new_node_num(lty->type_size));
                }
                node->type = lty;
            } else {
                if (rty->kind == TY_PTR) {
                    error("Invalid operands: %d and %d", lty->kind, rty->kind);
                }
                node->type = int_ty();
        }
            return node;
        case ND_ASSIGN:
            node->lhs = walk(node->lhs);
            check_referable(node->lhs);
            node->rhs = walk(node->rhs);
            node->type = node->lhs->type;
            return node;
        case ND_MUL:
        case ND_DIV:
        case ND_EQ:
        case ND_NE:
        case ND_LE:
        case ND_LT:
            node->lhs = walk(node->lhs);
            node->rhs = walk(node->rhs);
            check_int(node->lhs);
            check_int(node->rhs);
            node->type = int_ty();
            return node;
        case ND_ADDR:
            node->lhs = walk(node->lhs);
            check_referable(node->lhs);
            node->type = ptr_ty(node->lhs->type);
            return node;
        case ND_DEREF:
            node->lhs = walk(node->lhs);
            assert(node->lhs->type->kind == TY_PTR && "expected TY_PTR");
            node->type = node->lhs->type->ptr_to;
            return node;
        case ND_RETURN:
            node->lhs = walk(node->lhs);
            return node;
        case ND_FUNC_CALL:
            for (int i = 0; i < node->args->len; i++) {
                Node *arg = vec_get(node->args, i);
                vec_set(node->args, i, walk(arg));
            }
            node->type = node->func->return_type;
            return node;
        case ND_BLOCK:
            // fprintf(stderr, "num stmts: %d\n", node->stmts->len);
            for (int i = 0; i < node->stmts->len; i++) {
                Node *stmt = vec_get(node->stmts, i);
                Node *walked_stmt = walk(stmt);
                // fprintf(stderr, "block walking\n");
                vec_set(node->stmts, i, walked_stmt);
            }
            return node;
        default:
            error("Unknown node kind: %d", node->kind);
    }
}

void sema(Map *code) {
    for (int i = 0; i < code->len; i++) {
        Func *fn = vec_get(code->vals, i);
        fn->body = walk(fn->body);
    }
}
