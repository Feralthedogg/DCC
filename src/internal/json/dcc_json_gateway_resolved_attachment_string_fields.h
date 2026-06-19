#ifndef DCC_JSON_GATEWAY_RESOLVED_ATTACHMENT_STRING_FIELDS_H
#define DCC_JSON_GATEWAY_RESOLVED_ATTACHMENT_STRING_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_resolved_buffers.h"
#include "internal/json/dcc_json_keys.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_attachment_string_field(
    dcc_json_parser_t *parser,
    dcc_attachment_t *attachment,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_attachment_buffers_t *buffers,
    int *handled
);
dcc_status_t dcc_json_gateway_parse_nullable_string_target(
    dcc_json_parser_t *parser,
    const char **target,
    char *buffer,
    size_t buffer_cap
);

#endif
