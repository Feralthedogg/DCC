#ifndef DCC_JSON_GATEWAY_RESOLVED_ATTACHMENT_OBJECT_ENTRY_H
#define DCC_JSON_GATEWAY_RESOLVED_ATTACHMENT_OBJECT_ENTRY_H

#include "internal/json/dcc_json_core.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_attachment_object(
    dcc_json_parser_t *parser,
    dcc_attachment_t *attachment,
    char *filename,
    size_t filename_cap,
    char *description,
    size_t description_cap,
    char *url,
    size_t url_cap,
    char *proxy_url,
    size_t proxy_url_cap,
    char *content_type,
    size_t content_type_cap,
    char *waveform,
    size_t waveform_cap
);

#endif
