#include"mcc.h"




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

Node *program() {

    //printf("program\n");
    int i = 0;
    while (!at_eof())
        code[i++] = stmt();
    code[i] = NULL;
}

Node *stmt() {
    Node *node = expr();
    expect(";");
    return node;
}

Node *expr() {
    Node *node = assign();

    return node;
}

Node *assign() {
    Node *node = equality();
    if(consume ("="))
        node = new_node(ND_ASSIGN, node, assign());
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
        else if(consume(">="))
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

    Token *tok = consume_ident();

    if (tok) {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        LVar *lvar = find_lvar(tok);
        if (lvar) {
            node->offset = lvar->offset;
        } else {
            lvar = calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = locals->offset + 8;
            node->offset = lvar->offset;
            locals = lvar;
        }
        return node;
    }

    return new_node_num(expect_number());
}



//変数のアドレスをプッシュ
void gen_lval(Node *node) {
    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

//コードジェネレーター、構文木からコードを出力する。
void gen(Node *node) {

    //printf("gen\n");

    switch (node->kind) {
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("  pop rax\n"); //アドレスをポップ
        printf("  mov rax, [rax]\n");//アドレスにストアされてる値をロード
        printf("  push rax\n");//値をプッシュ
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("  pop rdi\n"); //gen(node->rhs)からの数値をポップ
        printf("  pop rax\n");// gen_lval(node->lhs)からのアドレスをポップ
        printf("  mov [rax], rdi\n"); //rdi（値）をrax（アドレス)にストア
        printf("  push rdi\n");// 値をプッシュ
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind){  //右辺値を評価するswitch文?
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
        case ND_EQ:
            printf("  cmp rax, rdi\n");
            printf("  sete al\n");
            printf("  movzb  rax, al\n");
            break;
        case ND_NEQ:
            printf("  cmp rax, rdi\n");
            printf("  setne al\n");
            printf("  movzb  rax, al\n");
            break;
        case ND_MOR:
            printf("  cmp rdi, rax\n");
            printf("  setl al\n");
            printf("  movzb  rax, al\n");
            break;
        case ND_MOT:
            printf("  cmp rdi, rax\n");
            printf("  setle al\n");
            printf("  movzb  rax, al\n");
            break;
        case ND_LES:
            printf("  cmp rax, rdi\n");
            printf("  setl al\n");
            printf("  movzb  rax, al\n");
            break;
        case ND_LET:
            printf("  cmp rax, rdi\n");
            printf("  setle al\n");
            printf("  movzb  rax, al\n");
            break;
    }

    printf("  push rax\n");
}