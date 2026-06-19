#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_presence_client_status_field(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    dcc_json_key_id_t key_id,
    char *desktop_status,
    size_t desktop_status_cap,
    char *mobile_status,
    size_t mobile_status_cap,
    char *web_status,
    size_t web_status_cap,
    int *handled
) {
    if (parser == NULL || presence == NULL || desktop_status == NULL ||
        mobile_status == NULL || web_status == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    *handled = 1;
    char *buffer = NULL;
    size_t buffer_cap = 0;
    const char **target = NULL;

    if (key_id == DCC_JSON_KEY_DESKTOP) {
        buffer = desktop_status;
        buffer_cap = desktop_status_cap;
        target = &presence->desktop_status;
    } else if (key_id == DCC_JSON_KEY_MOBILE) {
        buffer = mobile_status;
        buffer_cap = mobile_status_cap;
        target = &presence->mobile_status;
    } else if (key_id == DCC_JSON_KEY_WEB) {
        buffer = web_status;
        buffer_cap = web_status_cap;
        target = &presence->web_status;
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

    *target = has_value ? buffer : NULL;
    return DCC_OK;
}
