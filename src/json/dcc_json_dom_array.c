#include "internal/json/dcc_json_internal.h"

#include <string.h>

static int dcc_json_array_push(dcc_json_t *array, dcc_json_t *item) {
    dcc_json_array_data_t *data = &array->as.array;
    if (data->len == data->cap) {
        size_t next_cap = 0;
        if (dcc_json_next_cap(data->cap, 4U, sizeof(*data->items), &next_cap) != 0) {
            return -1;
        }
        dcc_json_t **next = (dcc_json_t **)dcc_json_arena_alloc(
            &array->doc->arena,
            next_cap * sizeof(*next)
        );
        if (next == NULL) {
            return -1;
        }
        if (data->len > 0) {
            memcpy(next, data->items, data->len * sizeof(*next));
        }
        data->items = next;
        data->cap = next_cap;
    }
    data->items[data->len++] = item;
    return 0;
}

dcc_status_t dcc_json_parse_array(dcc_json_parser_t *parser, dcc_json_t **out) {
    parser->cur++;
    dcc_json_t *array = dcc_json_new(parser, DCC_JSON_ARRAY);
    if (array == NULL) {
        return DCC_ERR_NOMEM;
    }

    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == ']') {
        parser->cur++;
        *out = array;
        return DCC_OK;
    }

    for (;;) {
        dcc_json_t *item = NULL;
        dcc_status_t status = dcc_json_parse_value(parser, &item);
        if (status != DCC_OK) {
            return status;
        }
        if (dcc_json_array_push(array, item) != 0) {
            return DCC_ERR_NOMEM;
        }
        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == ']') {
            parser->cur++;
            *out = array;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}
