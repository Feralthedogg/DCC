#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_client_status_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || out == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    *handled = 1;
    char *buffer = NULL;
    size_t buffer_cap = 0;
    const char **gateway_status = NULL;
    const char **presence_status = NULL;

    if (key_id == DCC_JSON_KEY_DESKTOP) {
        buffer = out->gateway_desktop_status;
        buffer_cap = sizeof(out->gateway_desktop_status);
        gateway_status = &out->gateway.desktop_status;
        presence_status = &out->presence.desktop_status;
    } else if (key_id == DCC_JSON_KEY_MOBILE) {
        buffer = out->gateway_mobile_status;
        buffer_cap = sizeof(out->gateway_mobile_status);
        gateway_status = &out->gateway.mobile_status;
        presence_status = &out->presence.mobile_status;
    } else if (key_id == DCC_JSON_KEY_WEB) {
        buffer = out->gateway_web_status;
        buffer_cap = sizeof(out->gateway_web_status);
        gateway_status = &out->gateway.web_status;
        presence_status = &out->presence.web_status;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        buffer,
        buffer_cap,
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }

    *gateway_status = has_value ? buffer : NULL;
    *presence_status = has_value ? buffer : NULL;
    out->has_presence = 1;
    return DCC_OK;
}
