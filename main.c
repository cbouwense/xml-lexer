#define GUPPY_IMPLEMENTATION
#include "guppy.h"

typedef enum {
    TOKEN_KIND_OPEN_BRACKET  = 0,
    TOKEN_KIND_CLOSE_BRACKET = 1,
    TOKEN_KIND_IDENTIFIER    = 2,
    TOKEN_KIND_FORWARD_SLASH = 3,
    TOKEN_KIND_EOF           = 4,
} TokenKind;

// ---------------------------------------------------------------------------------------------------------------------
// Global state
// ---------------------------------------------------------------------------------------------------------------------

bool had_error = false;

// ---------------------------------------------------------------------------------------------------------------------

void report(const int line, const char* where, const char* message) {
    fprintf(STDERR, "[line %d] Error %s: %s", where, message);
    had_error = true;
}

void error(int line, const char* message) {
    report(line, "", message);
}

int main(int argc, char **argv) {
    char *xml_file_as_string = NULL;
    const bool read_xml_file_successfully = gup_file_read_as_cstr(NULL, "root.xml", &xml_file_as_string);
    gup_assert_verbose(read_xml_file_successfully, "Failed to read input xml file");

    GupArrayPtr tokens = gup_array_ptr_create(NULL);
    const int char_count_in_xml_file = gup_cstr_length_excluding_null(xml_file_as_string);
    for (int i = 0; i < char_count_in_xml_file; i++) {

    }

    // Cleanup
    gup_array_ptr_create(NULL);

    if (had_error) return 65; // 65 is apparently the "correct" one.
    return 0;
}

// <root/>