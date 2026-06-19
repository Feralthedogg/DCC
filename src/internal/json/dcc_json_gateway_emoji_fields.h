#ifndef DCC_JSON_GATEWAY_EMOJI_FIELDS_H
#define DCC_JSON_GATEWAY_EMOJI_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"
#include "internal/json/dcc_json_keys.h"

#include <stdint.h>

dcc_status_t dcc_json_gateway_parse_emoji(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);
void dcc_json_gateway_apply_emoji_id(dcc_json_gateway_payload_t *out, uint64_t value);
void dcc_json_gateway_apply_emoji_name(dcc_json_gateway_payload_t *out, const char *value);
void dcc_json_gateway_apply_emoji_animated(dcc_json_gateway_payload_t *out, int value);
dcc_status_t dcc_json_gateway_parse_emoji_object_fields(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);
dcc_status_t dcc_json_gateway_parse_emoji_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
);

#endif
