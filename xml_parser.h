typedef struct {
    char* name;
    char* value;
} GupXmlExprAttribute;

typedef struct {
    GupXmlExprElement element;
    char* char_data;
} GupXmlExprContent;

typedef struct {
    char* name;
} GupXmlExprEndTag;

typedef struct {
    char* name;
    GupXmlExprAttribute attribute;
} GupXmlExprStartTag;

typedef GupXmlExprStartTag GupXmlExprEmptyElementTag;

typedef struct {
    GupXmlExprStartTag start_tag;
    GupXmlExprContent content;
    GupXmlExprEndTag end_tag;
    GupXmlExprEmptyElementTag empty_element_tag;
} GupXmlExprElement;

typedef struct {
    char* name;
    char* external_id;
    char* internal_subset;
} GupXmlExprDoctypeDeclaration;

typedef struct {
    GupXmlExprAttribute attribute;
} GupXmlExprXmlDeclaration;

typedef struct {
    GupXmlExprXmlDeclaration xml_declaration;
} GupXmlExprProlog;

typedef struct {
    GupXmlExprProlog prolog;
    GupXmlExprElement element;
} GupXmlExprDocument;
