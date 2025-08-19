typedef struct GupXmlExprAttribute GupXmlExprAttribute;
typedef struct GupXmlExprContent GupXmlExprContent;
typedef struct GupXmlExprEndTag GupXmlExprEndTag;
typedef struct GupXmlExprStartTag GupXmlExprStartTag;
typedef struct GupXmlExprElement GupXmlExprElement;
typedef struct GupXmlExprDoctypeDeclaration GupXmlExprDoctypeDeclaration;
typedef struct GupXmlExprXmlDeclaration GupXmlExprXmlDeclaration;
typedef struct GupXmlExprProlog GupXmlExprProlog;
typedef struct GupXmlExprDocument GupXmlExprDocument;

struct GupXmlExprAttribute {
    char* name;
    char* value;
};

struct GupXmlExprContent {
    GupXmlExprElement* element;
    char* char_data;
};

struct GupXmlExprEndTag {
    char* name;
};

struct GupXmlExprStartTag {
    char* name;
    GupXmlExprAttribute* attribute;
};

typedef GupXmlExprStartTag* GupXmlExprEmptyElementTag;

struct GupXmlExprElement {
    GupXmlExprStartTag* start_tag;
    GupXmlExprContent* content;
    GupXmlExprEndTag* end_tag;
    GupXmlExprEmptyElementTag* empty_element_tag;
};

struct GupXmlExprDoctypeDeclaration {
    char* name;
    char* external_id;
    char* internal_subset;
};

struct GupXmlExprXmlDeclaration {
    GupXmlExprAttribute* attribute;
};

struct GupXmlExprProlog {
    GupXmlExprXmlDeclaration* xml_declaration;
};

struct GupXmlExprDocument {
    GupXmlExprProlog* prolog;
    GupXmlExprElement* element;
};
