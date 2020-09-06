#include "shucc.h"

/**
 * Report error.
 * @param fmt  error information
 */
void error(char * fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

/**
 * Report error with position.
 * @param loc  location of error
 * @param fmt  error information
 */
void error_at(char * loc, char * fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, " ");
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

int INITIAL_VECTOR_SIZE = 32;

/**
 * Creates an empty vector.
 *
 * @return A vector.
 */
Vector *vec_create() {
    Vector *vec = malloc(sizeof(Vector));
    vec->data = malloc(sizeof(void *) * INITIAL_VECTOR_SIZE);
    vec->capacity = INITIAL_VECTOR_SIZE;
    vec->len = 0;
    return vec;
}

/**
 * Pushes an item to a vector.
 *
 * @param vec A vector.
 * @param item An item to be registered.
 */
void vec_push(Vector *vec, void *item) {
    if (vec->len == vec->capacity) {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
    }
    vec->data[vec->len++] = item;
}

/**
 * Gets an item from a vector.
 *
 * @param vec A vector.
 * @param index An index.
 *
 * @return An item.
 */
void *vec_get(Vector *vec, int index) {
    if (vec->len <= index) {
        error("IndexError: vector index out of range");
    }
    return vec->data[index];
}

/**
 * Creates an empty map.
 *
 * @return A map.
 */
Map *map_create() {
    Map *map = malloc(sizeof(Map));
    map->keys = vec_create();
    map->vals = vec_create();
    map->len = 0;
    return map;
}

/**
 * Inserts an item to a map.
 *
 * @param map A map.
 * @param key A key.
 * @param val A value.
 */
void map_insert(Map *map, char *key, void *val) {
    for (int i = 0; i < map->len; i++) {
        if (strcmp(vec_get(map->keys, i), key) == 0) {
            void *old_val = vec_get(map->vals, i);
            old_val = val;
            return;
        }
    }
    vec_push(map->keys, key);
    vec_push(map->vals, val);
    map->len++;
}

/**
 * Gets an item from a map.
 * NULL will be returned if the key is not in the map.
 *
 * @param map A map.
 * @param key A key.
 *
 * @return An item.
 */
void *map_at(Map *map, char *key) {
    for (int i = 0; i < map->len; i++) {
        if (strcmp(vec_get(map->keys, i), key) == 0) {
            return vec_get(map->vals, i);
        }
    }
    return NULL;
}

/**
 * Draws the abstract syntax tree of a node.
 *
 * @param node A node.
 * @param depth The depth of the node.
 * @param role The role of the node.
 */
void draw_node_tree(Node *node, int depth, char *role) {
    if (node != NULL) {
        for (int i = 0; i < depth; i++) {
            fprintf(stderr, "  ");
        }
        if (strlen(role)) {
            fprintf(stderr, "%s: ", role);
        }
        switch (node->kind) {
            case ND_ADD:
                fprintf(stderr, "ADD\n");
                draw_node_tree(node->lhs, depth + 1, "lhs");
                draw_node_tree(node->rhs, depth + 1, "rhs");
                break;
            case ND_SUB:
                fprintf(stderr, "SUB\n");
                draw_node_tree(node->lhs, depth + 1, "lhs");
                draw_node_tree(node->rhs, depth + 1, "rhs");
                break;
            case ND_MUL:
                fprintf(stderr, "MUL\n");
                draw_node_tree(node->lhs, depth + 1, "lhs");
                draw_node_tree(node->rhs, depth + 1, "rhs");
                break;
            case ND_DIV:
                fprintf(stderr, "DIV\n");
                draw_node_tree(node->lhs, depth + 1, "lhs");
                draw_node_tree(node->rhs, depth + 1, "rhs");
                break;
            case ND_EQ:
                fprintf(stderr, "EQ\n");
                draw_node_tree(node->lhs, depth + 1, "lhs");
                draw_node_tree(node->rhs, depth + 1, "rhs");
                break;
            case ND_NE:
                fprintf(stderr, "NE\n");
                draw_node_tree(node->lhs, depth + 1, "lhs");
                draw_node_tree(node->rhs, depth + 1, "rhs");
                break;
            case ND_LE:
                fprintf(stderr, "LE\n");
                draw_node_tree(node->lhs, depth + 1, "lhs");
                draw_node_tree(node->rhs, depth + 1, "rhs");
                break;
            case ND_LT:
                fprintf(stderr, "LT\n");
                draw_node_tree(node->lhs, depth + 1, "lhs");
                draw_node_tree(node->rhs, depth + 1, "rhs");
                break;
            // case ND_ASSIGN:
            //     fprintf(stderr, "ASSIGN\n");
            //     draw_node_tree(node->lhs, depth + 1, "lhs");
            //     draw_node_tree(node->rhs, depth + 1, "rhs");
            //     break;
            // case ND_RETURN:
            //     fprintf(stderr, "RETURN\n");
            //     draw_node_tree(node->lhs, depth + 1, "");
            //     break;
            // case ND_IF:
            //     fprintf(stderr, "IF\n");
            //     draw_node_tree(node->cond, depth + 1, "cond");
            //     draw_node_tree(node->then, depth + 1, "then");
            //     draw_node_tree(node->els, depth + 1, "else");
            //     break;
            // case ND_WHILE:
            //     fprintf(stderr, "WHILE\n");
            //     draw_node_tree(node->cond, depth + 1, "cond");
            //     draw_node_tree(node->then, depth + 1, "then");
            //     break;
            // case ND_FOR:
            //     fprintf(stderr, "FOR\n");
            //     draw_node_tree(node->init, depth + 1, "init");
            //     draw_node_tree(node->cond, depth + 1, "cond");
            //     draw_node_tree(node->upd, depth + 1, "update");
            //     draw_node_tree(node->then, depth + 1, "then");
            //     break;
            // case ND_BLOCK:
            //     fprintf(stderr, "BLOCK\n");
            //     for (int i = 0; i < node->stmts->len; i++) {
            //         draw_node_tree(node->stmts->data[i], depth + 1, "");
            //     }
            //     break;
            // case ND_FUNC_CALL:
            //     fprintf(stderr, "FUNC_CALL(name: %s)\n", node->name);
            //     for (int i = 0; i < node->args->len; i++) {
            //         char prefix[16] = {'\0'};
            //         sprintf(prefix, "arg%d", i);
            //         draw_node_tree(node->args->data[i], depth + 1, prefix);
            //     }
            //     break;
            // case ND_GVAR:
            //     fprintf(stderr, "GVAR(name: %s)\n", node->name);
            //     break;
            // case ND_LVAR:
            //     fprintf(stderr, "LVAR(name: %s)\n", node->name);
            //     break;
            case ND_NUM:
                fprintf(stderr, "NUM(%d)\n", node->val);
                break;
            // case ND_ADDR:
            //     fprintf(stderr, "ADDR\n");
            //     draw_node_tree(node->lhs, depth + 1, "");
            //     break;
            // case ND_DEREF:
            //     fprintf(stderr, "DEREF\n");
            //     draw_node_tree(node->lhs, depth + 1, "");
            //     break;
            default:
                break;
        }
    }
}

/**
 * Draws the abstract syntax tree of a program.
 *
 * @param code A program.
 */
void draw_ast(Node ** code) {
	Node * node;
	for (int i=0; i<100; i++) {
		node = code[i];
		if (node == NULL) {
			break;
		}
		draw_node_tree(node, 1, "");
	}
}