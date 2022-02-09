#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// トークンの種類
typedef enum {
    TK_RESERVED, //記号
    TK_NUM,     //整数トークン
    TK_EOF,     //入力の終わり
} TokenKind;

typedef enum {
    ND_EQ,
    ND_NEQ,
    ND_MOR,
    ND_MOT,
    ND_LES,
    ND_LET,
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
} NodeKind;

typedef struct Token Token;

typedef struct Node Node;

// トークン型
struct Token {
    TokenKind kind; //トークンの型
    Token *next; //次の入力トークン
    int val; //kindがTK_NUMの場合、その数値
    char *str; //トークン文字列
    int len;
};

struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Token *token;
char *user_input;

void error_at(char *loc, char *fmt, ...){
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

bool consume(char *op){
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

void expect(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        error_at(token->str, "'%C'ではありません",op);
    token = token->next;
}

// 次のトークンが数値の場合、トークンを一つ進めてその数値を返す
// それ以外の場合はエラーを返す
int expect_number(){
    if(token->kind != TK_NUM)
        error_at(token->str,"数ではありません");
    int val = token->val;
    token = token->next;
    return val; //
}

bool at_eof() {
    return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str  = str;
    cur->next = tok;
    return tok;
}

Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (!strncmp(p, ">=", 2) ||
            !strncmp(p, "<=", 2) ||
            !strncmp(p, "==", 2) ||
            !strncmp(p, "!=", 2)
        ){
            cur = new_token(TK_RESERVED, cur, p);
            p = p+2;
            cur->len = 2;
            continue;
        }

        if (strchr("+-*/()<>",*p)){
            cur = new_token(TK_RESERVED, cur, p++);
            cur->len = 1;
            continue;
        }

        if (isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(user_input, "トークナイズ出来ません");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1,sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *equality();
Node *expr();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();


Node *expr() {
    Node *node = equality();

    return node;
}

Node *equality() {
    Node *node = relational();

    for(;;) {
        if(consume("=="))
            node = new_node(ND_EQ, node, relational());
        else if(consume("!="))
            node = new_node(ND_NEQ, node, relational());
        else
            return node;
    }
}

Node *relational() {
    Node *node = add();

    for(;;) {
        if(consume("<"))
            node = new_node(ND_LES, node, add());
        else if(consume("<="))
            node = new_node(ND_LET, node, add());
        else if(consume(">"))
            node = new_node(ND_MOR, node, add());
        else if(consume("<="))
            node = new_node(ND_MOT, node, add());
        else 
            return node;
    }
}

Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else    
            return node;
 
   }
}

Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else    
            return node;
    }
}

Node *unary(){
        if (consume("+"))
            return primary();
        else if(consume("-"))
            return new_node(ND_SUB,new_node_num(0), primary());
        return primary();
}

Node *primary() {

    if (consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }

    return new_node_num(expect_number());
}

void gen(Node *node) {
    if (node->kind == ND_NUM) {
        printf("  push  %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind){
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