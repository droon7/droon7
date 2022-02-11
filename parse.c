#include"mcc.h"

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

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

    // デバッグ
    //printf("op = %s\n",op);
    //printf("token->str = %s\n",token->str);
    //printf("token->len = %d\n",token->len);
    //printf("strlen(op)->len = %d\n",strlen(op));
    

    if (strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

Token *consume_ident() {
    if (token->kind != TK_IDENT )
        return 0;

    Token *tok = token;
    token = token->next;
    return tok;
}

void expect(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        error_at(token->str, "%sではありません",op);
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

LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next)
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    return NULL;
}

int is_alnum(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
         (c == '_');
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

        //printf("%s\n",p);  //デバッグprintf
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
            cur = new_token(TK_RETURN, cur, p);
            p += 6;
            cur->len =6;
            continue;
        }

        if ('a' <= *p && *p <= 'z') {
            //printf("%s\n",p);
            cur = new_token(TK_IDENT, cur, p);
            
            int local_len=0;
            while(is_alnum(*p)) {
                local_len++;
                p++;
            }
            cur->len = local_len;
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

        if (strchr("+-*/()<>=;",*p)){
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