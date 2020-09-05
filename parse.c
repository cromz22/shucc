#include "shucc.h"

/*
 * Create a non-leaf node.
 * @param kind  node kind
 * @param lhs   lhs of the node
 * @param rhs   rhs of the node
 */
Node * new_node(NodeKind kind, Node * lhs, Node * rhs) {
	Node * node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

/*
 * Create a leaf node.
 * @param val  an integer
 */
 Node * new_node_num(int val) {
 	Node * node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
 }

/*
 * expr = mul ("+" mul | "-" mul)*
 */
Node * expr() {
	Node * node = mul();

	for (;;) {
		if (consume('+')) {
			node = new_node(ND_ADD, node, mul());
		} else if (consume('-')) {
			node = new_node(ND_SUB, node, mul());
		} else {
			return node;
		}
	}
}

/*
 * mul = primary ("*" primary | "/" primary)*
 */
Node * mul() {
 	Node *node = primary();

	for (;;) {
		if (consume('*')) {
			node = new_node(ND_MUL, node, primary());
		} else if (consume('/')) {
			node = new_node(ND_DIV, node, primary());
		} else {
			return node;
		}
	}
 }

/*
 * primary = "(" expr ")" | num
 */
Node * primary() {
	if (consume('(')) {
		Node * node = expr();
		expect(')');
		return node;
	}

	return new_node_num(expect_number());
}
