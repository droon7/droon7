#include "mcc.h"

Token *token;
char *user_input;
Node *code[100];
LVar *locals ;
int labelnum;
char *argreg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
int count_for_allign16byte = 0;

int main ( int argc, char **argv){
    if ( argc != 2){
        fprintf(stderr, "引数の個数が正しくない\n");
        return 1;
    }

    locals = calloc(1,sizeof(LVar));
    labelnum = 0;

    //トークナイズしてパースする
    user_input = argv[1];

    // printf("main\n");
    token = tokenize(user_input);
    program();
    
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");


    //プロローグ
    //変数26個分の領域を確保する
    printf("  push rbp\n");count_for_allign16byte -= 1;
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");


    for(int i = 0; code[i]; i++) {
        gen(code[i]);

        // 式の評価結果としてスタックに一つの値が残っている
        // はずなので、スタックが溢れないようにポップしておく
        printf("  pop rax\n"); count_for_allign16byte += 1;
    }

    
    //エピローグ
    //最後の式の結果がraxに残っているのでそれが返り値になる
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n"); count_for_allign16byte += 1;
    printf("  ret\n");

    return 0;
} 