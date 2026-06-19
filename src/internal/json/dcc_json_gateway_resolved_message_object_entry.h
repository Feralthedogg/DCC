#ifndef DCC_JSON_GATEWAY_RESOLVED_MESSAGE_OBJECT_ENTRY_H
#define DCC_JSON_GATEWAY_RESOLVED_MESSAGE_OBJECT_ENTRY_H

#include "internal/json/dcc_json_core.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_resolved_message_object(
    dcc_json_parser_t *parser,
    dcc_message_t *message,
    char *content,
    size_t content_cap,
    char *timestamp,
    size_t timestamp_cap,
    char *edited_timestamp,
    size_t edited_timestamp_cap,
    char *author_username,
    size_t author_username_cap,
    char *author_global_name,
    size_t author_global_name_cap,
    char *author_avatar,
    size_t author_avatar_cap
);

#endif
