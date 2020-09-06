#include "shucc.h"

/**
 * Push the address of the node to stack top.
 * @param node  variable to be pushed
 */
void gen_lval(Node* node) {
    if (node->kind != ND_LVAR) {
        error("invalid left value");
    }
    printf("  mov rax, rbp\n");  // rbp must not be moved while the function is executed, so copy it (address) to rax
    printf("  sub rax, %d\n", node->offset);  // shift rax (address) by the offset of the node
    printf("  push rax\n");                   // push the shifted address to the stack top
}

/**
 * generate assembly codes from AST
 * @param node root node of AST
 */
void gen(Node* node) {
    switch (node->kind) {
        case ND_NUM:
            printf("  push %d\n", node->val);
            return;
        case ND_LVAR:                      // e.g. x
            gen_lval(node);                // push address of x to stack top
            printf("  pop rax\n");         // rax = address of x
            printf("  mov rax, [rax]\n");  // rax = value of x
            printf("  push rax\n");        // push value of x to stack
            return;
        case ND_ASSIGN:                    // e.g. a = 1; node->lhs = a, node->rhs = 1
            gen_lval(node->lhs);           // push address of a to stack top
            gen(node->rhs);                // push 1 to stack top
            printf("  pop rdi\n");         // rdi = 1
            printf("  pop rax\n");         // rax = address of a
            printf("  mov [rax], rdi\n");  // set value of a to be 1
            printf("  push rdi\n");        // push 1 so that a = 1 returns 1 (e.g. b = a = 1 must be b = 1)
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

/**
 * generate x86 specific assembly, prologue, and epilogue
 */
void gen_x86() {
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // prologue (reserve space for local variables)
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", lvars->len * 8);

    Node* node;
    for (int i = 0; i < 100; i++) {
        node = code[i];
        if (node == NULL) {
            break;
        }
        gen(node);
    }
    printf("  pop rax\n");  // pop the result from stack top

    // epilogue
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}
