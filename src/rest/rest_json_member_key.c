#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_json_append_member_key(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key
) {
    dcc_status_t status = dcc_rest_json_member_sep(body, first);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "\"");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, key);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "\":");
    }
    return status;
}
