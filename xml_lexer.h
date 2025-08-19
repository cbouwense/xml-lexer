#include <stdbool.h>
#include <stdlib.h>

typedef enum {
    STATE_DEFAULT = 0,
    STATE_STRING_LITERAL,
    STATE_TEXT_CONTENT,
} State;

typedef enum {
    TOKEN_KIND_AMPERSAND = 0,
    TOKEN_KIND_BANG,
    TOKEN_KIND_BOX_BRACKET_LEFT,
    TOKEN_KIND_BOX_BRACKET_RIGHT,
    TOKEN_KIND_COMMA,
    TOKEN_KIND_EOF,
    TOKEN_KIND_EQUAL,
    TOKEN_KIND_FORWARD_SLASH,
    TOKEN_KIND_GREATER_THAN,
    TOKEN_KIND_HASH,
    TOKEN_KIND_IDENTIFIER,
    TOKEN_KIND_LESS_THAN,
    TOKEN_KIND_MINUS,
    TOKEN_KIND_PARENTHESIS_OPEN,
    TOKEN_KIND_PARENTHESIS_CLOSE,
    TOKEN_KIND_PERCENT,
    TOKEN_KIND_PIPE,
    TOKEN_KIND_PLUS,
    TOKEN_KIND_QUESTION,
    TOKEN_KIND_QUOTE_DOUBLE,
    TOKEN_KIND_QUOTE_SINGLE,
    TOKEN_KIND_SEMICOLON,
    TOKEN_KIND_STAR,
    TOKEN_KIND_STRING_LITERAL,
} TokenKind;

typedef struct {
    TokenKind kind;
    char*     lexeme_text;
    int       line; 
} Token;

void token_print(const Token token);
bool lex(const char* xml_file_path);
