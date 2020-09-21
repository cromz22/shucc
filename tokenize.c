#include "shucc.h"

/**
 * Skip symbol op.
 * @param  op    symbol to be skipped
 * @return bool  true if skipped
 */
bool consume(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) {
        return false;
    }
    token = token->next;
    return true;
}

/**
 * Skip identifier.
 * @return identifier
 */
Token *consume_ident() {
    if (token->kind != TK_IDENT) {
        return false;
    }
    Token *ident = token;
    token = token->next;
    return ident;
}

/**
 * Expect the next token to be the symbol op (i.e. skip op) and raise error if not skipped
 * @param op  symbol to be expected
 */
void expect(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) {
        error("This is not '%s'", op);
    }
    token = token->next;
}

/**
 * Expect number and set the value of the token.
 * @return val  the value of the token
 */
int expect_number() {
    if (token->kind != TK_NUM) {
        error("This is not a number");
    }
    int val = token->val;
    token = token->next;
    return val;
}

/**
 * Check if the current token is a specific symbol or operation.
 * @param  op    A symbol or operation
 * @return bool
 */
bool peek(char *op) {
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) {
        return false;
    }
    return true;
}

/**
 * See if the current token is EOF.
 * @return bool  true if EOF
 */
bool at_eof() { return token->kind == TK_EOF; }

/**
 * Create a new token tok, which is the next token of cur, and returns it.
 * @param  kind  kind of the new token
 * @param  cur   current token
 * @param  str   token string
 * @param  len   length of reserved token
 * @return tok   created token
 */
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));

    // map_insert (used inside primary@parse.c) uses strcmp, so str has to end with \0
    char *str_sliced = calloc(len + 1, sizeof(char));
    strncpy(str_sliced, str, len);
    str_sliced[len] = '\0';

    tok->kind = kind;
    tok->str = str_sliced;
    tok->len = len;
    cur->next = tok;
    return tok;
}

/**
 * Check if *p starts with *q.
 * @param p  string to be checked
 * @param q  string to check
 */
bool startswith(char *p, char *q) { return memcmp(p, q, strlen(q)) == 0; }

/**
 * Check if c is alphabet, number, or _.
 * @param c  charcter to be checked
 * @return   true if c is alphabet, number, or _
 */
bool is_alnum(char c) { return isalnum(c) || c == '_'; }

/**
 * Recognize TK_RESERVED and returns it.
 * @param p token to be recognized
 * @return  the keyword recognized
 */
char *read_reserved(char *p) {
    char *keywords[] = {"return", "if", "else", "while", "for", "int"};
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        int len = strlen(keywords[i]);
        if (startswith(p, keywords[i]) && !is_alnum(p[len])) {  // variable like return1 is skipped
            return keywords[i];
        }
    }
    return NULL;
}

/**
 * Tokenize string *p.
 * @param p  string to be tokenized
 * @return   the first token of tokenized *p
 */
Token *tokenize(char *p) {
    Token head;          // the #0 token (which is used only inside this function)
    head.next = NULL;    // after the last token comes NULL
    Token *cur = &head;  // initialize cur with head

    while (*p) {
        // skip spaces
        if (isspace(*p)) {
            p++;
            continue;
        }

        // tokenize keywords
        char *keyword = read_reserved(p);
        if (keyword) {
            int len = strlen(keyword);
            cur = new_token(TK_RESERVED, cur, p, len);
            p += len;
            continue;
        }

        // tokenize local variables
        if (isalpha(*p) || *p == '_') {  // the first letter must be alphabet or _
            int len = 1;
            while (is_alnum(p[len])) {  // letters thereafter may include numbers
                len++;
            }
            cur = new_token(TK_IDENT, cur, p, len);
            p += len;
            continue;
        }

        // tokenize reserved token
        // two-letter reserved token
        if (startswith(p, "<=") || startswith(p, ">=") || startswith(p, "!=") || startswith(p, "==")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }
        // single-letter reserved token
        if (ispunct(*p)) {  // !"#$%&'()*+,-./:;<=>?@[\]^_`{|}
            cur = new_token(TK_RESERVED, cur, p, 1);
            // create a new token, the type of which is TK_RESERVED, the previous token of which is cur,
            // and the string representation of which is character *p
            p++;
            continue;
        }

        // tokenize number
        /*
         The strtol function is from stdlib.h.
         It converts string *p to type long int with base 10.
         If string *p is not totally number, then the rest of *p is left untouched.
         The second argument, which is an address, will indicate the position of it.
        */
        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error("Could not tokenize the string");
    }

    new_token(TK_EOF, cur, p, 0);  // create the last token

    return head.next;  // the first token of the tokenized string
}
