#define GUPPY_IMPLEMENTATION
#include "guppy.h"

#define A (GupAllocator*)&a

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

// ---------------------------------------------------------------------------------------------------------------------

void token_print(const Token token) {
    printf("{\n");

    switch (token.kind) {
        case TOKEN_KIND_AMPERSAND:         printf("  kind: TOKEN_KIND_AMPERSAND\n");         break;
        case TOKEN_KIND_BANG:              printf("  kind: TOKEN_KIND_BANG\n");              break;
        case TOKEN_KIND_BOX_BRACKET_LEFT:  printf("  kind: TOKEN_KIND_BOX_BRACKET_LEFT\n");  break;
        case TOKEN_KIND_BOX_BRACKET_RIGHT: printf("  kind: TOKEN_KIND_BOX_BRACKET_RIGHT\n"); break;
        case TOKEN_KIND_COMMA:             printf("  kind: TOKEN_KIND_COMMA\n");             break;
        case TOKEN_KIND_EOF:               printf("  kind: TOKEN_KIND_EOF\n");               break;
        case TOKEN_KIND_EQUAL:             printf("  kind: TOKEN_KIND_EQUAL\n");             break;    
        case TOKEN_KIND_FORWARD_SLASH:     printf("  kind: TOKEN_KIND_FORWARD_SLASH\n");     break;                      
        case TOKEN_KIND_GREATER_THAN:      printf("  kind: TOKEN_KIND_GREATER_THAN\n");      break;        
        case TOKEN_KIND_HASH:              printf("  kind: TOKEN_KIND_HASH\n");              break;        
        case TOKEN_KIND_IDENTIFIER:        printf("  kind: TOKEN_KIND_IDENTIFIER\n");        break;        
        case TOKEN_KIND_LESS_THAN:         printf("  kind: TOKEN_KIND_LESS_THAN\n");         break;        
        case TOKEN_KIND_MINUS:             printf("  kind: TOKEN_KIND_MINUS\n");             break;        
        case TOKEN_KIND_PARENTHESIS_OPEN:  printf("  kind: TOKEN_KIND_PARENTHESIS_OPEN\n");  break;        
        case TOKEN_KIND_PARENTHESIS_CLOSE: printf("  kind: TOKEN_KIND_PARENTHESIS_CLOSE\n"); break;        
        case TOKEN_KIND_PERCENT:           printf("  kind: TOKEN_KIND_PERCENT\n");           break;        
        case TOKEN_KIND_PIPE:              printf("  kind: TOKEN_KIND_PIPE\n");              break;        
        case TOKEN_KIND_PLUS:              printf("  kind: TOKEN_KIND_PLUS\n");              break;        
        case TOKEN_KIND_QUESTION:          printf("  kind: TOKEN_KIND_QUESTION\n");          break;                    
        case TOKEN_KIND_QUOTE_DOUBLE:      printf("  kind: TOKEN_KIND_QUOTE_DOUBLE\n");      break;        
        case TOKEN_KIND_QUOTE_SINGLE:      printf("  kind: TOKEN_KIND_QUOTE_SINGLE\n");      break;        
        case TOKEN_KIND_SEMICOLON:         printf("  kind: TOKEN_KIND_SEMICOLON\n");         break;        
        case TOKEN_KIND_STAR:              printf("  kind: TOKEN_KIND_STAR\n");              break;        
        case TOKEN_KIND_STRING_LITERAL:    printf("  kind: TOKEN_KIND_STRING_LITERAL\n");    break;        
    }

    printf("  lexeme_text: %s\n", token.lexeme_text);

    printf("  line: %d\n", token.line);
    printf("}\n");
}

// TODO: actual error reporting now

// void report(const int line, const char* where, const char* message, bool* had_error) {
//     fprintf(stderr, "[line %d] Error %s: %s", where, message);
//     had_error = true;
// }

// void error(int line, const char* message, int* had_error) {
//     report(line, "", message, NULL, had_error);
// }

bool is_alpha(const char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool is_numeric(const char c) {
    return c >= '0' && c <='9';
}

bool is_valid_identifier_starting_char(const char c) {
    return is_alpha(c) || c == '_' || c == ':';
}

bool is_valid_identifier_char(const char c) {
    return is_alpha(c) || is_numeric(c) || c == '-' || c == '_' || c == ':' || c == '.';
}

void add_token(GupAllocator* a, GupArrayPtr* tokens, TokenKind token_kind, char* lexeme_text, int line, int* curr) {
    Token* token = gup_alloc(a, sizeof(Token));

    token->kind        = token_kind;
    token->lexeme_text = lexeme_text;
    token->line        = line;

    gup_array_ptr_append(a, tokens, token);

    *curr = *curr + 1;
}

void add_string_token(GupAllocator* a, GupArrayPtr* tokens, const char* xml_file_as_string, int* curr, int start, int* line, TokenKind kind, char delim, const int char_count_in_xml_file) {
    char next_c = xml_file_as_string[ *curr + 1 ];

    while (next_c != delim) {
        *curr = *curr + 1;

        if (*curr == char_count_in_xml_file) {
            break;
        }
        else if (*curr > char_count_in_xml_file) {
            fprintf(stderr, "Cursor somehow overshot the characters in the file\n");
            exit(1);
        }

        if (xml_file_as_string[ *curr ] == '\n') *line = *line + 1;

        next_c = xml_file_as_string[ *curr + 1 ];
    }

    // Copy the string literal into the token
    const size_t token_string_length = *curr - start + 2;
    char* token_string = gup_alloc(a, sizeof(char) * token_string_length);
    gup_cstr_copy_n(token_string, xml_file_as_string + start, token_string_length);

    add_token(a, tokens, kind, token_string, *line, curr);
}

TokenKind delim_to_token_kind(const char delim) {
    switch (delim) {
        case '\'': return TOKEN_KIND_QUOTE_SINGLE;
        case '"':  return TOKEN_KIND_QUOTE_DOUBLE;
        case '<':  return TOKEN_KIND_LESS_THAN;
        default: {
            fprintf(stderr, "Unexpected delim '%c' what are you doin bro", delim);
            exit(1);
        } break;
    }
}

int main(int argc, char** argv) {
    // Process arguments
    char* default_xml_file_path = "hard.xml";
    char* xml_file_path = NULL;

    // Executable name only, no arguments
    if (argc == 1) {        
        xml_file_path = default_xml_file_path;
    }
    // If there is at least one argument name, use the first one as the xml_file_path and just
    // ignore the rest of them.
    // TODO: validate the inputs 
    else {  
        xml_file_path = argv[1];
    }

    bool had_error = false;
    GupAllocatorBucket a = gup_allocator_bucket_create();

    // Read the contents of the XML file into memory as a simple string
    char *xml_file_as_string = NULL;
    bool success = gup_file_read_as_cstr(A, xml_file_path, &xml_file_as_string);
    gup_assert_verbose(success, "Failed to read input xml file");

    State state = STATE_DEFAULT;
    int start   = 0;
    int curr    = 0;
    int line    = 1;
    // This is the thing that will signify the tokenizer that the string literal has ended. For example, if we find a
    // double quote as our first character of a token, then we will know that we are parsing a string literal, and that
    // once we find another double quote we have reached the end of that string literal.
    char delim  = '\0';

    GupArrayPtr tokens = gup_array_ptr_create(A);
    const int char_count_in_xml_file = gup_cstr_length_excluding_null(xml_file_as_string);

    // Each iteration of this loop poops out exactly one token.
    while (start < char_count_in_xml_file) {
        const char c = xml_file_as_string[start];

        if (c == '\n') line++;

        if (state == STATE_STRING_LITERAL || state == STATE_TEXT_CONTENT) {
            if (c == delim) {
                add_token(A, &tokens, delim_to_token_kind(delim), NULL, line, &curr);
                
                state = STATE_DEFAULT;
                delim = '\0';
            } else {
                add_string_token(A, &tokens, xml_file_as_string, &curr, start, &line, TOKEN_KIND_STRING_LITERAL, delim, char_count_in_xml_file);
            }
        } else {
            switch (c) {
                case '<': {
                    add_token(A, &tokens, TOKEN_KIND_LESS_THAN, NULL, line, &curr);
                } break;

                case '>': {
                    add_token(A, &tokens, TOKEN_KIND_GREATER_THAN, NULL, line, &curr);
                    state = STATE_TEXT_CONTENT;
                    delim = '<';
                } break;

                case '?': {
                    add_token(A, &tokens, TOKEN_KIND_QUESTION, NULL, line, &curr);
                } break;

                case '!': {
                    add_token(A, &tokens, TOKEN_KIND_BANG, NULL, line, &curr);
                } break;

                case '/': {
                    add_token(A, &tokens, TOKEN_KIND_FORWARD_SLASH, NULL, line, &curr);
                } break;

                case '=': {
                    add_token(A, &tokens, TOKEN_KIND_EQUAL, NULL, line, &curr);
                } break;

                case '\'': {
                    add_token(A, &tokens, TOKEN_KIND_QUOTE_SINGLE, NULL, line, &curr);
                    state = STATE_STRING_LITERAL;
                    delim = '\'';
                } break;

                case '"': {
                    add_token(A, &tokens, TOKEN_KIND_QUOTE_DOUBLE, NULL, line, &curr);
                    state = STATE_STRING_LITERAL;
                    delim = '"';
                } break;

                case '[': {
                    add_token(A, &tokens, TOKEN_KIND_BOX_BRACKET_LEFT, NULL, line, &curr);
                } break;

                case ']': {
                    add_token(A, &tokens, TOKEN_KIND_BOX_BRACKET_RIGHT, NULL, line, &curr);
                } break;

                case '(': {
                    add_token(A, &tokens, TOKEN_KIND_PARENTHESIS_OPEN, NULL, line, &curr);
                } break;

                case ')': {
                    add_token(A, &tokens, TOKEN_KIND_PARENTHESIS_CLOSE, NULL, line, &curr);
                } break;

                case ',': {
                    add_token(A, &tokens, TOKEN_KIND_COMMA, NULL, line, &curr);
                } break;

                case '+': {
                    add_token(A, &tokens, TOKEN_KIND_PLUS, NULL, line, &curr);
                } break;

                case '-': {
                    add_token(A, &tokens, TOKEN_KIND_MINUS, NULL, line, &curr);
                } break;

                case '#': {
                    add_token(A, &tokens, TOKEN_KIND_HASH, NULL, line, &curr);
                } break;

                case '|': {
                    add_token(A, &tokens, TOKEN_KIND_PIPE, NULL, line, &curr);
                } break;

                case '*': {
                    add_token(A, &tokens, TOKEN_KIND_STAR, NULL, line, &curr);
                } break;

                case '&': {
                    add_token(A, &tokens, TOKEN_KIND_AMPERSAND, NULL, line, &curr);
                } break;

                case '%': {
                    add_token(A, &tokens, TOKEN_KIND_PERCENT, NULL, line, &curr);
                } break;

                case ';': {
                    add_token(A, &tokens, TOKEN_KIND_SEMICOLON, NULL, line, &curr);
                } break;

                // Whitespace isn't all noise in XML, but it never starts a new token. Since this is the outer switch where
                // we are starting new tokens, we skip any whitespace here specifically.
                case ' ': {
                    curr++;
                } break;

                case '\n': {
                    line++;
                    curr++;
                } break;
                
                default: {
                    // Identifier
                    if (is_valid_identifier_starting_char(c)) {
                        char next_c = xml_file_as_string[ curr + 1 ];

                        while (is_valid_identifier_char(next_c)) {
                            curr++;
                            next_c = xml_file_as_string[ curr + 1 ];
                            if (next_c == '\n') line++;
                        }

                        // Copy the identifier string into the token
                        const size_t identifier_string_length = curr - start + 2;
                        char* identifier_string = gup_alloc(A, sizeof(char) * identifier_string_length);
                        gup_cstr_copy_n(identifier_string, xml_file_as_string + start, identifier_string_length);

                        add_token(A, &tokens, TOKEN_KIND_IDENTIFIER, identifier_string, line, &curr);
                    } else {
                        had_error = true;
                        fprintf(stderr, "Unexpected character `%c` at line %d\n", c, line);
                    }
                } break;
            }
        }

        start = curr;
    }

    for (int i = 0; i < tokens.count; i++) {
        token_print(*( (Token*) (tokens.data[i]) ));
    }

    // Cleanup
    gup_allocator_bucket_destroy(&a);

    if (had_error) return 65; // 65 is apparently the "correct" one.
    return 0;
}
