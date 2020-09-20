#include "shucc.h"

/* global variable */
int label_counter = 0;  // used in if statement

char* argregs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

/**
 * Push the address of the node to stack top.
 * @param node  variable to be pushed
 */
void gen_lval(Node* node) {
    printf("  mov rax, rbp\n");  // rbp must not be moved while the function is executed, so copy it (address) to rax
    printf("  sub rax, %d\n", node->lvar->offset);  // shift rax (address) by the offset of the node
    // fprintf(stderr, "the offset of lvar: %s is %d\n", node->lvar->name, node->lvar->offset);
    printf("  push rax\n");                   // push the shifted address to the stack top
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
        case ND_ASSIGN:                    // e.g. a = 1; node->lhs = a, node->rhs = 1
            if (node->lhs->kind == ND_LVAR) {
                gen_lval(node->lhs);       // push address of a to stack top
            } else if (node->lhs->kind == ND_DEREF) {
                gen(node->lhs->lhs); // node->lhs は *x で node->lhs->lhs が x
            } else {
                error("error: lvalue required as left operand of assignment");
            }
            gen(node->rhs);                // push 1 to stack top
            printf("  pop rdi\n");         // rdi = 1
            printf("  pop rax\n");         // rax = address of a
            printf("  mov [rax], rdi\n");  // set value of a to be 1
            printf("  push rdi\n");        // push 1 so that a = 1 returns 1 (e.g. b = a = 1 must be b = 1)
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
            for (int i = 0; i < node->stmts->len; i++) {
                gen(node->stmts->data[i]);
            }
            return;
        case ND_FUNC_CALL:
            for (int i = 0; i < node->args->len; i++) {
                gen(node->args->data[i]);
            }
            for (int i = node->args->len - 1; i >= 0; i--) {
                printf("  pop %s\n", argregs[i]);
            }
            printf("  call %s\n", node->name);  // callの結果がraxに入る
            printf("  push rax\n");             // raxの値をstack topに積む
            return;
        case ND_ADDR:
            gen_lval(node->lhs);  // address を積む
            return;
        case ND_DEREF:
            gen(node->lhs);                // addressがstack topに積まれる
            printf("  pop rax\n");         // rax = address
            printf("  mov rax, [rax]\n");  // rax = そのaddressの値
            printf("  push rax\n");        // 値をpush
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
 *
 */
void gen_func(Func* fn) {
    printf(".globl %s\n", fn->name);
    printf("\n%s:\n", fn->name);

    int offset = 0;
    for (int i = 0; i < fn->lvars->len; i++) {
        Lvar *lvar = vec_get(fn->lvars->vals, i);
        offset += lvar->type->type_size;
        lvar->offset = offset;
    }

    // prologue (reserve space for local variables)
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", offset);  // ローカル変数の領域を確保する

    // 引数の値を stack に push してローカル変数と同じように扱えるように
    for (int i = 0; i < fn->args->len; i++) {
        Node* arg = vec_get(fn->args, i);  // ND_LVAR
        printf("  lea rax, [rbp-%d]\n", arg->lvar->offset);
        printf("  mov [rax], %s\n", argregs[i]);  // 第 i 引数の値をraxが指すメモリにコピー
    }
    // fprintf(stderr, "arguments stacked\n");
    // 中身のコードを吐く
    gen(fn->body);

    // epilogue (when fuction ends without return stmt)
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}

/**
 * generate x86 specific assembly, prologue, and epilogue
 */
void gen_x86(Map *code) {
    printf(".intel_syntax noprefix\n");

    for (int i = 0; i < code->len; i++) {
        Func *fn = vec_get(code->vals, i);
        gen_func(fn);
    }
}
