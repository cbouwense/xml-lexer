#include <stdbool.h>
#include "xml_lexer.h"
#include "xml_parser.h"

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

    const bool lex_succeeded = lex(xml_file_path);

    if (!lex_succeeded) return 65; // 65 is apparently the "correct" one.
    return 0;
}
