#include "shucc.h"

/* global variables used inside codegen.c */
int label_counter = 0;                                       // used for if, else, and for statement
char* argregs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};  // registers for function arguments

/**
 * Push the address of the node to stack top.
 * @param node  variable to be pushed
 */
void gen_lval(Node* node) {
    printf("  mov rax, rbp\n");  // rbp must not be moved while the function is executed, so copy it (address) to rax
    printf("  sub rax, %d\n", node->lvar->offset);  // shift rax (address) by the offset of the node
    printf("  push rax\n");                         // push the shifted address to the stack top
}

/**
 * generate assembly codes from AST
 * @param node  root node of AST
 */
void gen(Node* node) {
    int local_label_counter;  // needed to work properly when if, while, and for are used at the same time

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
        case ND_ASSIGN:
            if (node->lhs->kind == ND_LVAR) {
                gen_lval(node->lhs);
            } else if (node->lhs->kind == ND_DEREF) {
                gen(node->lhs->lhs);  // e.g. x: some address, node->lhs: *x, node->lhs->lhs: x
            } else {
                error("invalid left value");
            }
            gen(node->rhs);
            printf("  pop rdi\n");
            printf("  pop rax\n");
            printf("  mov [rax], rdi\n");
            printf("  push rdi\n");
            return;
        case ND_RETURN:
            gen(node->lhs);
            printf("  pop rax\n");
            // epilogue (redundant)
            printf("  mov rsp, rbp\n");
            printf("  pop rbp\n");
            printf("  ret\n");
            return;
        case ND_IF:                                            // e.g. if (A) B else C
            local_label_counter = label_counter++;             /* need to keep label_counter at the time */
            gen(node->cond);                                   // push (the value of) A to stack top
            printf("  pop rax\n");                             // rax = A
            printf("  cmp rax, 0\n");                          // compare A with 0
            printf("  je .Lels%03d\n", local_label_counter);   // if A == 0 jump to .LelsXXX (do not execute B)
            gen(node->then);                                   // if A != 0 do not jump (execute B)
            printf("  jmp .Lend%03d\n", local_label_counter);  // when B is finished jump to .LendXXX (do not execute C)
            printf(".Lels%03d:\n", local_label_counter);       //
            if (node->els) {                                   //
                gen(node->els);                                // codes for C
            }                                                  //
            printf(".Lend%03d:\n", local_label_counter);       // continue generating other assembly codes after this
            local_label_counter++;                             // prevent labels from overlapping
            return;
        case ND_WHILE:
            local_label_counter = label_counter++;
            printf(".Lbegin%03d:\n", local_label_counter);       // e.g. while (A) B
            gen(node->cond);                                     // push A to stack top
            printf("  pop rax\n");                               // rax = A
            printf("  cmp rax, 0\n");                            // compare A with 0
            printf("  je .Lend%03d\n", local_label_counter);     // if A == 0 jump to .LendXXX (do not execute B)
            gen(node->then);                                     // execute B
            printf("  jmp .Lbegin%03d\n", local_label_counter);  // loop again
            printf(".Lend%03d:\n", local_label_counter);
            local_label_counter++;
            return;
        case ND_FOR:                                             // e.g. for (A; B; C) D
            local_label_counter = label_counter++;               //
            gen(node->init);                                     // push A
            printf(".Lbegin%03d:\n", local_label_counter);       //
            gen(node->cond);                                     // push B
            printf("  pop rax\n");                               // rax = B
            printf("  cmp rax, 0\n");                            // compare B with 0
            printf("  je .Lend%03d\n", local_label_counter);     // if B == 0 jump to .LendXXX (do not execute D and C)
            gen(node->then);                                     // execute D
            gen(node->loop);                                     // execute C
            printf("  jmp .Lbegin%03d\n", local_label_counter);  // loop
            printf(".Lend%03d:\n", local_label_counter);         //
            local_label_counter++;
            return;
        case ND_BLOCK:
            for (int i = 0; i < node->stmts->size; i++) {
                gen(node->stmts->data[i]);
            }
            return;
        case ND_FUNC_CALL:
            for (int i = 0; i < node->args->size; i++) {
                gen(node->args->data[i]);
            }
            for (int i = node->args->size - 1; i >= 0; i--) {
                printf("  pop %s\n", argregs[i]);
            }
            printf("  call %s\n", node->func_name);  // rax = result of calling the function
            printf("  push rax\n");                  // push rax to stack top
            return;
        case ND_ADDR:             // e.g. &a
            gen_lval(node->lhs);  // push a to stack top
            return;
        case ND_DEREF:                     // e.g. *a
            gen(node->lhs);                // push address of a to stack top
            printf("  pop rax\n");         // rax = address of a
            printf("  mov rax, [rax]\n");  // rax = valud of a
            printf("  push rax\n");        // push the value to stack top
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
 * generate codes for function prologue, body, and epilogue
 */
void gen_func(Func* fn) {
    printf(".global %s\n", fn->name);
    printf("%s:\n", fn->name);

    int offset = 0;
    for (int i = 0; i < fn->lvars->size; i++) {
        Lvar* lvar = vec_get(fn->lvars->vals, i);
        offset += lvar->type->type_size;
        lvar->offset = offset;
    }

    // prologue (reserve space for local variables)
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", offset);

    // arguments
    for (int i = 0; i < fn->args->size; i++) {
        Node* arg = vec_get(fn->args, i);
        printf("  lea rax, [rbp-%d]\n", arg->lvar->offset);
        printf("  mov [rax], %s\n", argregs[i]);
    }

    // body
    gen(fn->body);

    // epilogue (needed when function ends without return statement)
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}

/**
 * generate x86_64 specific assembly
 */
void gen_x86_64() {
    printf(".intel_syntax noprefix\n");

    Func* fn;
    for (int i = 0; i < code->size; i++) {
        fn = vec_get(code->vals, i);
        gen_func(fn);
    }
}
