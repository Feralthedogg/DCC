#include "internal/json/dcc_json_internal.h"

dcc_json_t *dcc_json_new(dcc_json_parser_t *parser, dcc_json_type_t type) {
    dcc_json_t *json = (dcc_json_t *)dcc_json_arena_calloc(&parser->doc->arena, sizeof(*json));
    if (json != NULL) {
        json->doc = parser->doc;
        json->type = type;
    }
    return json;
}
