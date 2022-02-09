#include "mcc.h"
#include "parse.c"
#include "codegen.c"



int main ( int argc, char **argv){
    if ( argc != 2){
        fprintf(stderr, "引数の個数が正しくない\n");
        return 1;
    }

    //トークナイズしてパースする
    user_input = argv[1];
    token = tokenize(user_input);
    Node *node = expr();
    
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    /*
    printf("  mov rax, %d\n", expect_number());


    while(!at_eof()){
        if(consume('+')) {
            printf("  add rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("  sub rax, %d\n", expect_number());
    }
    */
    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
} 