#include "internal/json/dcc_json_internal.h"

#include <string.h>

static int dcc_json_object_push(dcc_json_t *object, char *key, dcc_json_t *value) {
    dcc_json_object_data_t *data = &object->as.object;
    if (data->len == data->cap) {
        size_t next_cap = 0;
        if (dcc_json_next_cap(data->cap, 4U, sizeof(*data->entries), &next_cap) != 0) {
            return -1;
        }
        dcc_json_object_entry_t *next = (dcc_json_object_entry_t *)dcc_json_arena_alloc(
            &object->doc->arena,
            next_cap * sizeof(*next)
        );
        if (next == NULL) {
            return -1;
        }
        if (data->len > 0) {
            memcpy(next, data->entries, data->len * sizeof(*next));
        }
        data->entries = next;
        data->cap = next_cap;
    }
    data->entries[data->len].key = key;
    data->entries[data->len].value = value;
    data->len++;
    return 0;
}

dcc_status_t dcc_json_parse_object(dcc_json_parser_t *parser, dcc_json_t **out) {
    parser->cur++;
    dcc_json_t *object = dcc_json_new(parser, DCC_JSON_OBJECT);
    if (object == NULL) {
        return DCC_ERR_NOMEM;
    }

    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == '}') {
        parser->cur++;
        *out = object;
        return DCC_OK;
    }

    for (;;) {
        char *key = NULL;
        if (dcc_json_parse_string_arena(parser, &key) != 0) {
            return DCC_ERR_JSON;
        }
        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end || *parser->cur != ':') {
            return DCC_ERR_JSON;
        }
        parser->cur++;

        dcc_json_t *value = NULL;
        dcc_status_t status = dcc_json_parse_value(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        if (dcc_json_object_push(object, key, value) != 0) {
            return DCC_ERR_NOMEM;
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == '}') {
            parser->cur++;
            *out = object;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}
