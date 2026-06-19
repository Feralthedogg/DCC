#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_json_member_sep(dcc_rest_buffer_t *body, int *first) {
    if (body == NULL || first == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (*first) {
        *first = 0;
        return DCC_OK;
    }
    return dcc_rest_buffer_append_cstr(body, ",");
}

dcc_status_t dcc_rest_json_append_string_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const char *value
) {
    dcc_status_t status = dcc_rest_json_append_member_key(body, first, key);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_json_string(body, value);
    }
    return status;
}

dcc_status_t dcc_rest_json_append_nullable_string_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const char *value
) {
    if (value != NULL && value[0] != '\0') {
        return dcc_rest_json_append_string_member(body, first, key, value);
    }
    return dcc_rest_json_append_raw_member(body, first, key, "null");
}

dcc_status_t dcc_rest_json_append_u64_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    uint64_t value
) {
    dcc_status_t status = dcc_rest_json_append_member_key(body, first, key);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_u64_text(body, value);
    }
    return status;
}

dcc_status_t dcc_rest_json_append_u64_string_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    uint64_t value
) {
    dcc_status_t status = dcc_rest_json_append_member_key(body, first, key);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "\"");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_u64_text(body, value);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "\"");
    }
    return status;
}

dcc_status_t dcc_rest_json_append_bool_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    uint8_t value
) {
    dcc_status_t status = dcc_rest_json_append_member_key(body, first, key);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, value != 0 ? "true" : "false");
    }
    return status;
}

dcc_status_t dcc_rest_json_append_raw_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const char *value
) {
    dcc_status_t status = dcc_rest_json_append_member_key(body, first, key);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, value);
    }
    return status;
}
