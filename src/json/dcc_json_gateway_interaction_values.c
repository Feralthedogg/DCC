#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_values(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '[') {
        return dcc_json_skip_value(parser);
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    out->interaction.values = out->interaction_values;
    out->interaction.values_count = 0;
    out->interaction.values_truncated = 0;

    if (parser->cur < parser->end && *parser->cur == ']') {
        parser->cur++;
        out->has_interaction = 1;
        return DCC_OK;
    }

    size_t index = 0;
    for (;;) {
        if (index < DCC_JSON_GATEWAY_INTERACTION_VALUE_CAP) {
            int has_value = 0;
            dcc_status_t status = dcc_json_parse_nullable_string_buffer(
                parser,
                out->interaction_value_storage[index],
                sizeof(out->interaction_value_storage[index]),
                &has_value
            );
            if (status != DCC_OK) {
                return status;
            }
            out->interaction_values[index] = has_value ? out->interaction_value_storage[index] : NULL;
            out->interaction.values_count = index + 1U;
        } else {
            dcc_status_t status = dcc_json_skip_value(parser);
            if (status != DCC_OK) {
                return status;
            }
            out->interaction.values_truncated = 1;
        }
        index++;

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == ']') {
            parser->cur++;
            out->has_interaction = 1;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}
