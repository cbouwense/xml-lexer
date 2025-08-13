#define GUPPY_IMPLEMENTATION
#include "guppy.h"

#define A (GupAllocator*)&a

typedef enum {
    TOKEN_KIND_EOF = 0,
    TOKEN_KIND_EQUAL,
    TOKEN_KIND_FORWARD_SLASH,
    TOKEN_KIND_FORWARD_SLASH_GREATER_THAN,
    TOKEN_KIND_GREATER_THAN,
    TOKEN_KIND_IDENTIFIER,
    TOKEN_KIND_LESS_THAN,
    TOKEN_KIND_LESS_THAN_FORWARD_SLASH,
    TOKEN_KIND_LESS_THAN_QUESTION,
    TOKEN_KIND_QUESTION_GREATER_THAN,
    TOKEN_KIND_QUOTE_DOUBLE,
    TOKEN_KIND_QUOTE_SINGLE,
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
        case TOKEN_KIND_EOF:                        printf("  kind: TOKEN_KIND_EOF\n");                        break;
        case TOKEN_KIND_EQUAL:                      printf("  kind: TOKEN_KIND_EQUAL\n");                      break;    
        case TOKEN_KIND_FORWARD_SLASH:              printf("  kind: TOKEN_KIND_FORWARD_SLASH\n");              break;            
        case TOKEN_KIND_FORWARD_SLASH_GREATER_THAN: printf("  kind: TOKEN_KIND_FORWARD_SLASH_GREATER_THAN\n"); break;                        
        case TOKEN_KIND_GREATER_THAN:               printf("  kind: TOKEN_KIND_GREATER_THAN\n");               break;        
        case TOKEN_KIND_IDENTIFIER:                 printf("  kind: TOKEN_KIND_IDENTIFIER\n");                 break;        
        case TOKEN_KIND_LESS_THAN:                  printf("  kind: TOKEN_KIND_LESS_THAN\n");                  break;        
        case TOKEN_KIND_LESS_THAN_FORWARD_SLASH:    printf("  kind: TOKEN_KIND_LESS_THAN_FORWARD_SLASH\n");    break;                    
        case TOKEN_KIND_LESS_THAN_QUESTION:         printf("  kind: TOKEN_KIND_LESS_THAN_QUESTION\n");         break;                
        case TOKEN_KIND_QUESTION_GREATER_THAN:      printf("  kind: TOKEN_KIND_QUESTION_GREATER_THAN\n");      break;                    
        case TOKEN_KIND_QUOTE_DOUBLE:               printf("  kind: TOKEN_KIND_QUOTE_DOUBLE\n");               break;        
        case TOKEN_KIND_QUOTE_SINGLE:               printf("  kind: TOKEN_KIND_QUOTE_SINGLE\n");               break;        
        case TOKEN_KIND_STRING_LITERAL:             printf("  kind: TOKEN_KIND_STRING_LITERAL\n");             break;        
    }

    printf("  lexeme_text: %s\n", token.lexeme_text);

    // TODO: how do I print the value, I probably have to store a value_type as well I guess?

    printf("  line: %d\n", token.line);
    printf("}\n");
}

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

void add_token(GupAllocator* a, GupArrayPtr* tokens, TokenKind token_kind, char* lexeme_text, int line) {
    Token* token = gup_alloc(a, sizeof(Token));

    token->kind        = token_kind;
    token->lexeme_text = lexeme_text;
    token->line        = line;

    gup_array_ptr_append(a, tokens, token);
}

int main(int argc, char **argv) {
    bool had_error = false;
    GupAllocatorArena a = gup_allocator_arena_create(1024 * 1024); // 1 MB
    // GupAllocatorBucket a = gup_allocator_bucket_create();

    // Read the contents of the XML file into memory as a simple string
    // TODO: parameterize input file
    char *xml_file_as_string = NULL;
    bool success = gup_file_read_as_cstr(A, "simple.xml", &xml_file_as_string);
    gup_assert_verbose(success, "Failed to read input xml file");

    // Variable to keep track of the location of the cursor
    int start = 0;
    int curr = 0;
    int line = 1;

    // State
    bool is_tokenizing_string_literal = false;
    char string_literal_delim_kind = TOKEN_KIND_EOF;

    GupArrayPtr tokens = gup_array_ptr_create(A);
    const int char_count_in_xml_file = gup_cstr_length_excluding_null(xml_file_as_string);

    // Each iteration of this loop poops out exactly one token.
    while (start < char_count_in_xml_file) {
        // This is the first character of the next token we're creating.
        const char c = xml_file_as_string[start];
        // printf("%c (%d)\n", c, c);

        switch (c) {
            case '<': {
                const char next_c = xml_file_as_string[ start + 1 ];

                switch (next_c) {
                    case '?': {
                        add_token(A, &tokens, TOKEN_KIND_LESS_THAN_QUESTION, NULL, line);
                        curr++;
                    } break;

                    case '/': {
                        add_token(A, &tokens, TOKEN_KIND_LESS_THAN_FORWARD_SLASH, NULL, line);
                        curr++;
                    } break;

                    default : {
                        add_token(A, &tokens, TOKEN_KIND_LESS_THAN, NULL, line);
                    } break;
                }
            } break;

            case '/': {
                const char next_c = xml_file_as_string[ start + 1 ];

                if (next_c == '>') {
                    add_token(A, &tokens, TOKEN_KIND_FORWARD_SLASH_GREATER_THAN, NULL, line);
                    curr++;
                } else {
                    // TODO: I don't even think this is valid? Maybe that's for the parser to decide.
                    add_token(A, &tokens, TOKEN_KIND_FORWARD_SLASH, NULL, line);
                }
            } break;

            case '>': {
                add_token(A, &tokens, TOKEN_KIND_GREATER_THAN, NULL, line);
                is_tokenizing_string_literal = true;
                string_literal_delim_kind = TOKEN_KIND_QUOTE_SINGLE;
            } break;

            case '?': {
                const char next_c = xml_file_as_string[ start + 1 ];

                if (next_c == '>') {
                    add_token(A, &tokens, TOKEN_KIND_QUESTION_GREATER_THAN, NULL, line);
                    curr++;
                }
            } break;

            case '\n': {
                line++;
            } break;

            case '=': {
                add_token(A, &tokens, TOKEN_KIND_EQUAL, NULL, line);
            } break;

            case '\'': {
                add_token(A, &tokens, TOKEN_KIND_QUOTE_SINGLE, NULL, line);
                is_tokenizing_string_literal = !is_tokenizing_string_literal;
                string_literal_delim_kind = TOKEN_KIND_QUOTE_SINGLE;
            } break;

            case '\"': {
                add_token(A, &tokens, TOKEN_KIND_QUOTE_DOUBLE, NULL, line);
                is_tokenizing_string_literal = !is_tokenizing_string_literal;
                string_literal_delim_kind = TOKEN_KIND_QUOTE_DOUBLE;
            } break;

            // Whitespace isn't all noise in XML, but it never starts a new token. Since this is the outer switch where
            // we are starting new tokens, we skip any whitespace here specifically.
            case ' ': break;
            
            default: {
                if (is_tokenizing_string_literal) {
                    char next_c = xml_file_as_string[ curr + 1 ];
                    
                    char delim = '\0';
                    switch (string_literal_delim_kind) {
                        case TOKEN_KIND_QUOTE_DOUBLE: delim = '"';  break;
                        case TOKEN_KIND_QUOTE_SINGLE: delim = '\''; break;
                        case TOKEN_KIND_GREATER_THAN: delim = '<';  break;
                        default: {
                            fprintf(stderr, "Encountered invalid string literal delimeter kind. This indicates a bug with the xml lexer.\n");
                            exit(1);
                        } break;
                    }

                    while (next_c != delim) {
                        curr++;
                        next_c = xml_file_as_string[ curr + 1 ];
                    }

                    const size_t string_literal_length = (curr - start + 1); // +1 for null terminator.
                    char* string_literal = gup_alloc(A, sizeof(char) * string_literal_length);
                    gup_cstr_copy_n(string_literal, xml_file_as_string + start, string_literal_length);

                    add_token(A, &tokens, TOKEN_KIND_STRING_LITERAL, string_literal, line);
                    curr += string_literal_length - 2;
                } else if (is_valid_identifier_starting_char(c)) {
                    // Scan over the identifier while it's valid
                    char next_c = xml_file_as_string[curr + 1];
                    while (is_valid_identifier_char(next_c)) {
                        curr++;
                        next_c = xml_file_as_string[curr + 1];
                    }

                    // Allocate the lexeme string
                    const size_t lexeme_length = curr - start + 1; // plus 1 null terminator
                    char* lexeme = gup_alloc(A, sizeof(char) * lexeme_length);

                    // Copy the string into lexeme.
                    gup_cstr_copy_n(lexeme, xml_file_as_string + start, lexeme_length);

                    add_token(A, &tokens, TOKEN_KIND_IDENTIFIER, lexeme, line);
                } else {
                    had_error = true;
                    fprintf(stderr, "Unexpected character `%c` at line %d\n", c, line);
                }
            } break;
        }

        curr++;
        start = curr;
    }

    for (int i = 0; i < tokens.count; i++) {
        token_print(*( (Token*) (tokens.data[i]) ));
    }

    // Cleanup
    gup_allocator_arena_destroy(&a);

    if (had_error) return 65; // 65 is apparently the "correct" one.
    return 0;
}

// <root/>