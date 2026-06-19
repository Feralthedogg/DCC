#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_activity_text_field(
    dcc_json_parser_t *parser,
    dcc_gateway_activity_t *activity,
    dcc_json_key_id_t key_id,
    char *name,
    size_t name_cap,
    char *state,
    size_t state_cap,
    char *details,
    size_t details_cap,
    char *url,
    size_t url_cap,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *buffer = NULL;
    size_t buffer_cap = 0;
    const char **target = NULL;

    switch (key_id) {
        case DCC_JSON_KEY_NAME:
            buffer = name;
            buffer_cap = name_cap;
            target = &activity->name;
            break;
        case DCC_JSON_KEY_STATE:
            buffer = state;
            buffer_cap = state_cap;
            target = &activity->state;
            break;
        case DCC_JSON_KEY_DETAILS:
            buffer = details;
            buffer_cap = details_cap;
            target = &activity->details;
            break;
        case DCC_JSON_KEY_URL:
            buffer = url;
            buffer_cap = url_cap;
            target = &activity->url;
            break;
        default:
            *handled = 0;
            return DCC_OK;
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(parser, buffer, buffer_cap, &has_value);
    if (status != DCC_OK) {
        return status;
    }

    *target = has_value ? buffer : NULL;
    *handled = 1;
    return DCC_OK;
}
