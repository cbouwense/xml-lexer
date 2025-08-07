#define GUPPY_IMPLEMENTATION
#include "guppy.h"

typedef enum {
    TOKEN_KIND_OPEN_BRACKET  = 0,
    TOKEN_KIND_CLOSE_BRACKET = 1,
    TOKEN_KIND_IDENTIFIER    = 2,
    TOKEN_KIND_FORWARD_SLASH = 3,
    TOKEN_KIND_EOF           = 4,
} TokenKind;

int main(int argc, char **argv) {
    char *xml_file_as_string = NULL;

    const bool read_xml_file_successfully = gup_file_read_as_cstr(NULL, "root.xml", &xml_file_as_string);
    
    gup_assert_verbose(read_xml_file_successfully, "Failed to read input xml file");

    int chars_in_xml_file = gup_cstr_length_excluding_null(xml_file_as_string);

    printf("%s\n", xml_file_as_string);
    printf("%d\n", chars_in_xml_file);

    return 1;
}

// <root/>