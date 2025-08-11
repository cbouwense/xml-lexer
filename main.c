#define GUPPY_IMPLEMENTATION
#include "guppy.h"

#define A (GupAllocator*)&a

typedef enum {
    TOKEN_KIND_LESS_THAN     = 0,
    TOKEN_KIND_GREATER_THAN  = 1,
    TOKEN_KIND_IDENTIFIER    = 2,
    TOKEN_KIND_FORWARD_SLASH = 3,
    TOKEN_KIND_EOF           = 4,
} TokenKind;

typedef struct {
    TokenKind kind;
    char*     lexeme_text;
    void*     value;
    int       line; 
} Token;

// ---------------------------------------------------------------------------------------------------------------------

void report(const int line, const char* where, const char* message) {
    fprintf(stderr, "[line %d] Error %s: %s", where, message);
    had_error = true;
}

void error(int line, const char* message) {
    report(line, "", message);
}

void add_token(GupAllocator* a, GupArrayPtr* tokens, TokenKind token_kind, char* lexeme_text, void* value, int line) {
    Token* token = NULL;
    token = gup_alloc(a, sizeof(token));

    token->kind        = token_kind;
    token->lexeme_text = lexeme_text;
    token->value       = value;
    token->line        = line;

    gup_array_ptr_append(A, tokens, token);
}

int main(int argc, char **argv) {
    bool had_error = false;
    GupAllocatorArena a = gup_allocator_arena_create(1024 * 1024); // 1 MB

    // Read the contents of the XML file into memory as a simple string
    // TODO: parametrize input file
    char *xml_file_as_string = NULL;
    bool success = gup_file_read_as_cstr(A, "root.xml", &xml_file_as_string);
    gup_assert_verbose(success, "Failed to read input xml file");

    // Variable to keep track of the location of the cursor
    int start = 0;
    int current = 0;
    int line = 1;
    GupArrayPtr tokens = gup_array_ptr_create(A);
    const int char_count_in_xml_file = gup_cstr_length_excluding_null(xml_file_as_string);

    for (int i = 0, j = 0; i < char_count_in_xml_file; i = j) {
        const char c = xml_file_as_string[i];
        printf("%c(%d)\n", c, c);

        switch (c) {
            case '<': {
                add_token(A, &tokens, TOKEN_KIND_LESS_THAN, NULL, NULL, line);
            } break;

            case '/': {
                add_token(A, &tokens, TOKEN_KIND_FORWARD_SLASH, NULL, NULL, line);
            } break;

            case '>': {
                add_token(A, &tokens, TOKEN_KIND_GREATER_THAN, NULL, NULL, line);
            } break;

            default: {
                had_error = true;
                fprintf(stderr, "Unexpected character `%c` at line %d", c, line);
            } break;
        }

        j++;
    }

    // Cleanup
    gup_allocator_arena_destroy(&a);

    if (had_error) return 65; // 65 is apparently the "correct" one.
    return 0;
}

// <root/>