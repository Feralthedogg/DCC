#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_json_append_snowflake_string_array_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const dcc_snowflake_t *values,
    size_t count
) {
    dcc_status_t status = dcc_rest_json_append_member_key(body, first, key);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_snowflake_json_string_array(body, values, count);
    }
    return status;
}

dcc_status_t dcc_rest_json_append_string_array_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const char * const *values,
    size_t count
) {
    dcc_status_t status = dcc_rest_json_append_member_key(body, first, key);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_json_string_array(body, values, count);
    }
    return status;
}

dcc_status_t dcc_rest_json_append_automod_preset_array_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const dcc_auto_moderation_preset_type_t *values,
    size_t count
) {
    dcc_status_t status = dcc_rest_json_append_member_key(body, first, key);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_automod_preset_array(body, values, count);
    }
    return status;
}
