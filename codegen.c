#include "mcc.h"

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