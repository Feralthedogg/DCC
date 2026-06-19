#include "internal/objects/dcc_message_json_members_internal.h"

dcc_status_t dcc_message_json_member_prefix(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const char *name
) {
    dcc_status_t status = dcc_message_json_append_cstr(buffer, *first ? "\"" : ",\"");
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_message_json_append_cstr(buffer, name);
    if (status != DCC_OK) {
        return status;
    }
    *first = 0;
    return dcc_message_json_append_cstr(buffer, "\":");
}

dcc_status_t dcc_message_json_append_string_member(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const char *name,
    const char *value
) {
    dcc_status_t status = dcc_message_json_member_prefix(buffer, first, name);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_message_json_append_escaped_string(buffer, value);
}

dcc_status_t dcc_message_json_append_u64_member(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const char *name,
    uint64_t value
) {
    dcc_status_t status = dcc_message_json_member_prefix(buffer, first, name);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_message_json_append_u64(buffer, value);
}

dcc_status_t dcc_message_json_append_bool_member(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const char *name,
    uint8_t value
) {
    dcc_status_t status = dcc_message_json_member_prefix(buffer, first, name);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_message_json_append_cstr(buffer, value ? "true" : "false");
}

dcc_status_t dcc_message_json_append_raw_member(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const char *name,
    const char *value
) {
    dcc_status_t status = dcc_message_json_member_prefix(buffer, first, name);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_message_json_append_cstr(buffer, value);
}
