#ifndef DCC_JSON_PARSER_STATE_H
#define DCC_JSON_PARSER_STATE_H

#include "internal/json/dcc_json_dom_core.h"

typedef struct dcc_json_parser {
    const char *cur;
    const char *end;
    dcc_json_doc_t *doc;
} dcc_json_parser_t;

#endif
