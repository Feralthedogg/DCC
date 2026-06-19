#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_resolved_attachment_map(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
    return dcc_json_gateway_parse_resolved_object_map(
        parser,
        out,
        dcc_json_gateway_parse_resolved_attachment_entry,
        dcc_json_gateway_finish_resolved_attachment_map
    );
}
