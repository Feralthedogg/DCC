#include "internal/json/dcc_json_internal.h"
#include "internal/json/dcc_json_dom_api.h"

#include <stddef.h>
#include <stdlib.h>

dcc_status_t dcc_json_parse(const char *input, size_t len, dcc_json_t **out) {
    if (input == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    dcc_json_doc_t *doc = (dcc_json_doc_t *)calloc(1, sizeof(*doc));
    if (doc == NULL) {
        return DCC_ERR_NOMEM;
    }

    dcc_status_t status = dcc_json_stage1_scan(&doc->stage1, input, len);
    if (status != DCC_OK) {
        dcc_json_stage1_deinit(&doc->stage1);
        free(doc);
        return status;
    }

    dcc_json_parser_t parser;
    parser.cur = input;
    parser.end = input + len;
    parser.doc = doc;

    dcc_json_t *json = NULL;
    status = dcc_json_parse_value(&parser, &json);
    if (status != DCC_OK) {
        dcc_json_free(json);
        dcc_json_arena_deinit(&doc->arena);
        dcc_json_stage1_deinit(&doc->stage1);
        free(doc);
        return status;
    }

    dcc_json_skip_ws(&parser);
    if (parser.cur != parser.end) {
        dcc_json_arena_deinit(&doc->arena);
        dcc_json_stage1_deinit(&doc->stage1);
        free(doc);
        return DCC_ERR_JSON;
    }

    doc->root = json;
    *out = json;
    return DCC_OK;
}

void dcc_json_free(dcc_json_t *json) {
    if (json == NULL) {
        return;
    }
    dcc_json_doc_t *doc = json->doc;
    if (doc == NULL) {
        return;
    }
    dcc_json_arena_deinit(&doc->arena);
    dcc_json_stage1_deinit(&doc->stage1);
    free(doc);
}
