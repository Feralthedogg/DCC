#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_attachment_string_field(
    dcc_json_parser_t *parser,
    dcc_attachment_t *attachment,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_attachment_buffers_t *buffers,
    int *handled
) {
    if (handled == NULL || attachment == NULL || buffers == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *buffer = NULL;
    size_t buffer_cap = 0;
    const char **target = NULL;

    if (key_id == DCC_JSON_KEY_FILENAME) {
        buffer = buffers->filename;
        buffer_cap = buffers->filename_cap;
        target = &attachment->filename;
    } else if (key_id == DCC_JSON_KEY_DESCRIPTION) {
        buffer = buffers->description;
        buffer_cap = buffers->description_cap;
        target = &attachment->description;
    } else if (key_id == DCC_JSON_KEY_URL) {
        buffer = buffers->url;
        buffer_cap = buffers->url_cap;
        target = &attachment->url;
    } else if (key_id == DCC_JSON_KEY_PROXY_URL) {
        buffer = buffers->proxy_url;
        buffer_cap = buffers->proxy_url_cap;
        target = &attachment->proxy_url;
    } else if (key_id == DCC_JSON_KEY_CONTENT_TYPE) {
        buffer = buffers->content_type;
        buffer_cap = buffers->content_type_cap;
        target = &attachment->content_type;
    } else if (key_id == DCC_JSON_KEY_WAVEFORM) {
        buffer = buffers->waveform;
        buffer_cap = buffers->waveform_cap;
        target = &attachment->waveform;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_gateway_parse_nullable_string_target(
        parser,
        target,
        buffer,
        buffer_cap
    );
    if (status != DCC_OK) {
        return status;
    }

    *handled = 1;
    return DCC_OK;
}
