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
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
    ND_EQ,
    ND_NEQ,
    ND_MOR,
    ND_MOT,
    ND_LES,
    ND_LET,
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

extern Token *token;
extern char *user_input;


void error_at(char *loc, char *fmt, ...);

bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str);
Token *tokenize(char *p);

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *equality();
Node *expr();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
void gen(Node *node);