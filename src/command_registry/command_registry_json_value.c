#include "internal/command_registry/dcc_command_registry_internal.h"
#include "internal/json/dcc_json_value_core.h"

static dcc_status_t dcc_command_registry_append_json_object(
    dcc_application_command_json_buffer_t *buffer,
    const dcc_json_t *json
) {
    dcc_status_t status = dcc_command_json_append_cstr(buffer, "{");
    for (size_t i = 0; status == DCC_OK && i < json->as.object.len; ++i) {
        if (i != 0) {
            status = dcc_command_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_command_json_append_escaped_string(buffer, json->as.object.entries[i].key);
        }
        if (status == DCC_OK) {
            status = dcc_command_json_append_cstr(buffer, ":");
        }
        if (status == DCC_OK) {
            status = dcc_command_registry_append_json_value(buffer, json->as.object.entries[i].value);
        }
    }
    return status == DCC_OK ? dcc_command_json_append_cstr(buffer, "}") : status;
}

static dcc_status_t dcc_command_registry_append_json_array(
    dcc_application_command_json_buffer_t *buffer,
    const dcc_json_t *json
) {
    dcc_status_t status = dcc_command_json_append_cstr(buffer, "[");
    for (size_t i = 0; status == DCC_OK && i < json->as.array.len; ++i) {
        if (i != 0) {
            status = dcc_command_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_command_registry_append_json_value(buffer, json->as.array.items[i]);
        }
    }
    return status == DCC_OK ? dcc_command_json_append_cstr(buffer, "]") : status;
}

dcc_status_t dcc_command_registry_append_json_value(
    dcc_application_command_json_buffer_t *buffer,
    const dcc_json_t *json
) {
    if (buffer == NULL || json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    switch (dcc_json_typeof(json)) {
        case DCC_JSON_NULL:
            return dcc_command_json_append_cstr(buffer, "null");
        case DCC_JSON_BOOL:
            return dcc_command_json_append_cstr(buffer, json->as.boolean ? "true" : "false");
        case DCC_JSON_NUMBER:
            return dcc_command_json_append_cstr(buffer, json->as.number);
        case DCC_JSON_STRING:
            return dcc_command_json_append_escaped_string(buffer, json->as.string);
        case DCC_JSON_ARRAY:
            return dcc_command_registry_append_json_array(buffer, json);
        case DCC_JSON_OBJECT:
            return dcc_command_registry_append_json_object(buffer, json);
        default:
            return DCC_ERR_JSON;
    }
}
