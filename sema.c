#include "shucc.h"

/**
 * Check if x and y has same type
 * @return  true if same
 */
bool same_type(Type* x, Type* y) {
    if (x->kind != y->kind) {
        return false;
    }

    switch (x->kind) {
        case TY_PTR:
            return same_type(x->ptr_to, y->ptr_to);
        default:
            return true;
    }
}

/**
 * Ensure node kind can be a left value
 */
void check_referable(Node* node) {
    NodeKind kind = node->kind;
    assert((kind == ND_LVAR || kind == ND_DEREF) && "Not referable");
}

/**
 * Ensure node type is int
 */
void check_int(Node* node) {
    int ty = node->type->kind;
    assert(ty == TY_INT && "Not an integer");
}

/**
 * Add a type to a node (except for a leaf node)
 */
Node* walk(Node* node) {
    // fprintf(stderr, "hello from walk!\n");
    assert(node && "Cannot walk on NULL node");
    switch (node->kind) {
        case ND_SIZEOF:
            node->lhs = walk(node->lhs);
            return new_node_num(node->lhs->type->type_size);
        case ND_ADDR:
            node->lhs = walk(node->lhs);
            check_referable(node->lhs);
            node->type = new_ty_ptr(node->lhs->type);
            return node;
        case ND_DEREF:
            node->lhs = walk(node->lhs);
            assert(node->lhs->type->kind == TY_PTR && "expected TY_PTR");
            node->type = node->lhs->type->ptr_to;
            return node;
        case ND_FUNC_CALL:
            for (int i = 0; i < node->args->size; i++) {
                Node* arg = vec_get(node->args, i);
                vec_set(node->args, i, arg);
            }
            node->type = node->func->return_type;
            return node;
        case ND_BLOCK:
            // fprintf(stderr, "walking ND_BLOCK\n");
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
            // fprintf(stderr, "walking ND_RETURN\n");
            node->lhs = walk(node->lhs);
            return node;
        case ND_LVAR:
            // fprintf(stderr, "walking ND_LVAR\n");
            node->type = node->lvar->type;
            return node;
        case ND_ASSIGN:
            // fprintf(stderr, "walking ND_ASSIGN\n");
            node->lhs = walk(node->lhs);
            check_referable(node->lhs);
            node->rhs = walk(node->rhs);
            node->type = node->lhs->type;
            return node;
        case ND_EQ:
        case ND_NE:
        case ND_LE:
        case ND_LT:
        case ND_MUL:
        case ND_DIV:
            node->lhs = walk(node->lhs);
            node->rhs = walk(node->rhs);
            check_int(node->lhs);
            check_int(node->rhs);
            node->type = new_ty_int();
            return node;
        case ND_ADD:
            node->lhs = walk(node->lhs);
            node->rhs = walk(node->rhs);

            if (node->rhs->type->kind == TY_PTR) {  //  PTR comes to lhs
                Node* tmp = node->lhs;
                node->lhs = node->rhs;
                node->rhs = tmp;
            }
            assert(node->rhs->type->kind == TY_INT && "invalid operation TY_PTR + TY_PTR");
            if (node->lhs->type->kind == TY_PTR) {  // and rhs->type->kind == TY_INT
                node->rhs = new_node(ND_MUL, node->rhs, new_node_num(node->rhs->type->type_size));
                node->rhs->type = new_ty_int();
                node->type = node->lhs->type;
            } else {
                node->type = new_ty_int();
            }
            return node;
        case ND_SUB:
            node->lhs = walk(node->lhs);
            node->rhs = walk(node->rhs);

            Type* lty = node->lhs->type;
            Type* rty = node->rhs->type;
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
                node->type = new_ty_int();
            }

            return node;
        case ND_NUM:
            // fprintf(stderr, "walking ND_NUM\n");
            return node;
        default:
            error("Unknown node kind: %d", node->kind);
    }
}

void sema() {
    for (int i = 0; i < code->size; i++) {
        Func* fn = vec_get(code->vals, i);
        fn->body = walk(fn->body);
    }
}