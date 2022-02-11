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
    TK_IDENT,
    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
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
    ND_LVAR,
    ND_ASSIGN,
    ND_RETURN,
    ND_IF,
    ND_ELSE,
    ND_WHILE,
    ND_FOR,
} NodeKind;

typedef struct Token Token;

typedef struct Node Node;

typedef struct LVar LVar;

// トークン型
struct Token {
    TokenKind kind; //トークンの型
    Token *next; //次の入力トークン
    int val; //kindがTK_NUMの場合、その数値
    char *str; //トークン文字列
    int len; //トークンの長さ
};

struct Node {
    NodeKind kind;//ノードの型
    Node *lhs;  //左辺
    Node *rhs;  //右辺
    int val;    //kindがND_NUMの場合のみ使用
    int offset; //kindがND_LVARのみ使用
};

struct LVar {
    LVar *next;
    char *name;
    int len;
    int offset;
};

extern Token *token;
extern char *user_input;
extern Node *code[100];
extern LVar *locals;
extern int labelnum;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

bool consume(char *op);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str);
Token *tokenize(char *p);
LVar *find_lvar(Token *tok);
int is_alnum(char c);

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
void gen(Node *node);