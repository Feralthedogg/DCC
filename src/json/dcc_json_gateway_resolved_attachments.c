#include "internal/json/dcc_json_internal.h"

#include <string.h>

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
) {
    const dcc_json_gateway_attachment_buffers_t buffers = {
        filename,
        filename_cap,
        description,
        description_cap,
        url,
        url_cap,
        proxy_url,
        proxy_url_cap,
        content_type,
        content_type_cap,
        waveform,
        waveform_cap
    };

    if (parser == NULL || attachment == NULL || filename == NULL ||
        description == NULL || url == NULL || proxy_url == NULL || content_type == NULL ||
        waveform == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    memset(attachment, 0, sizeof(*attachment));
    parser->cur++;
    return dcc_json_gateway_parse_attachment_object_fields(parser, attachment, &buffers);
}
