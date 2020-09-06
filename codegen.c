#include "shucc.h"

/*
 * Push to stack top.
 */
void gen_lval(Node * node) {
	if (node->kind != ND_LVAR) {
		error("invalid left value");
	}
	printf("  mov rax, rbp\n");
	printf("  sub rax, %d\n", node->offset);
	printf("  push rax\n");
}

/*
 * generate assembly codes from AST
 */
void gen(Node * node) {
	if (node->kind == ND_NUM) {
		printf("  push %d\n", node->val);
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("  pop rdi\n");
	printf("  pop rax\n");

	switch (node->kind) {
		case ND_EQ:
			printf("  cmp rax, rdi\n");
			printf("  sete al\n");
			printf("  movzb rax, al\n");
			break;
		case ND_NE:
			printf("  cmp rax, rdi\n");
			printf("  setne al\n");
			printf("  movzb rax, al\n");
			break;
		case ND_LE:
			printf("  cmp rax, rdi\n");
			printf("  setle al\n");
			printf("  movzb rax, al\n");
			break;
		case ND_LT:
			printf("  cmp rax, rdi\n");
			printf("  setl al\n");
			printf("  movzb rax, al\n");
			break;
		case ND_ADD:
			printf("  add rax, rdi\n");
			break;
		case ND_SUB:
			printf("  sub rax, rdi\n");
			break;
		case ND_MUL:
			printf("  imul rax, rdi\n");
			break;
		case ND_DIV:
			printf("  cqo\n");
			printf("  idiv rdi\n");
			break;
	}

	printf("  push rax\n");
}

/*
 *
 */
void gen_x86(Node ** code) {
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// reserve space for a-z
	printf("  push rbp\n");
	printf("  mov rbp, rsp\n");
	printf("  sub rsp, 208\n"); // 26 * 8

	Node * node;
	for(int i=0; i<100; i++) {
		node = code[i];
		if (node == NULL) {
			break;
		}
		gen(node);
	}
	printf("  pop rax\n"); // pop the result from stack top
	
	// epilogue
	printf("  mov rsp, rbp\n");
	printf("  pop rbp\n");
	printf("  ret\n");
}
