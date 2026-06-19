#ifndef DCC_JSON_GATEWAY_RESOLVED_ATTACHMENT_MEDIA_FIELDS_H
#define DCC_JSON_GATEWAY_RESOLVED_ATTACHMENT_MEDIA_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_attachment_media_field(
    dcc_json_parser_t *parser,
    dcc_attachment_t *attachment,
    dcc_json_key_id_t key_id,
    int *handled
);

#endif
