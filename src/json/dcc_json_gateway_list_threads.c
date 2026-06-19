#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_thread_array(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || *parser->cur != '[' || out == NULL) {
        return DCC_ERR_JSON;
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    size_t stored = 0;
    uint8_t truncated = 0;
    if (parser->cur < parser->end && *parser->cur == ']') {
        parser->cur++;
        goto done;
    }

    for (;;) {
        if (parser->cur < parser->end && *parser->cur == '{' && stored < DCC_JSON_GATEWAY_OBJECT_LIST_CAP) {
            dcc_status_t status = dcc_json_gateway_parse_thread_list_object(
                parser,
                &out->gateway_threads[stored],
                out->gateway_thread_names[stored],
                sizeof(out->gateway_thread_names[stored]),
                out->gateway_thread_topics[stored],
                sizeof(out->gateway_thread_topics[stored])
            );
            if (status != DCC_OK) {
                return status;
            }
            if (out->gateway_threads[stored].id != 0 && stored < DCC_JSON_GATEWAY_ID_LIST_CAP) {
                out->gateway_ids[stored] = out->gateway_threads[stored].id;
            }
            stored++;
        } else {
            dcc_status_t status = dcc_json_skip_value(parser);
            if (status != DCC_OK) {
                return status;
            }
            truncated = 1;
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == ']') {
            parser->cur++;
            goto done;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }

done:
    dcc_json_gateway_finish_thread_array(out, stored, truncated);
    return DCC_OK;
}
