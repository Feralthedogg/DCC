#include "internal/command_registry/dcc_command_registry_internal.h"
#include "internal/dcc_core_internal.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

static int dcc_command_registry_parse_u64_text(const char *text, uint64_t *out) {
    if (text == NULL || text[0] == '\0' || out == NULL) {
        return 0;
    }
    errno = 0;
    char *end = NULL;
    unsigned long long value = strtoull(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0') {
        return 0;
    }
    *out = (uint64_t)value;
    return 1;
}

static dcc_snowflake_t dcc_command_registry_json_snowflake(const dcc_json_t *json) {
    uint64_t value = 0;
    if (json == NULL) {
        return 0;
    }
    if (dcc_json_u64(json, &value) == 0) {
        return value;
    }
    if (dcc_json_typeof(json) == DCC_JSON_STRING &&
        dcc_command_registry_parse_u64_text(dcc_json_string(json), &value)) {
        return value;
    }
    return 0;
}

static dcc_status_t dcc_command_registry_append_remote_member(
    dcc_application_command_json_buffer_t *buffer,
    int *first,
    const char *key,
    const dcc_json_t *value
) {
    if (value == NULL || dcc_json_typeof(value) == DCC_JSON_NULL) {
        return DCC_OK;
    }
    dcc_status_t status = dcc_command_json_member_prefix(buffer, first, key);
    return status == DCC_OK ? dcc_command_registry_append_json_value(buffer, value) : status;
}

static int dcc_command_registry_json_array_nonempty(const dcc_json_t *json) {
    return json != NULL && dcc_json_typeof(json) == DCC_JSON_ARRAY && dcc_json_array_size(json) != 0;
}

static dcc_status_t dcc_command_registry_remote_canonical_json(
    const dcc_json_t *json,
    char **out
) {
    dcc_application_command_json_buffer_t buffer = {0};
    int first = 1;
    dcc_status_t status = dcc_command_json_append_cstr(&buffer, "{");
    const dcc_json_t *type_json = dcc_json_object_get(json, "type");
    uint64_t type_value = 0;
    uint32_t type = dcc_json_u64(type_json, &type_value) == 0
        ? (uint32_t)type_value
        : (uint32_t)DCC_APPLICATION_COMMAND_CHAT_INPUT;

    if (status == DCC_OK) {
        status = dcc_command_registry_append_remote_member(&buffer, &first, "name", dcc_json_object_get(json, "name"));
    }
    if (status == DCC_OK) {
        status = dcc_command_registry_append_remote_member(
            &buffer,
            &first,
            "name_localizations",
            dcc_json_object_get(json, "name_localizations")
        );
    }
    const dcc_json_t *description = dcc_json_object_get(json, "description");
    if (status == DCC_OK &&
        description != NULL &&
        (type == DCC_APPLICATION_COMMAND_CHAT_INPUT ||
         (dcc_json_string(description) != NULL && dcc_json_string(description)[0] != '\0'))) {
        status = dcc_command_registry_append_remote_member(&buffer, &first, "description", description);
    }
    if (status == DCC_OK) {
        status = dcc_command_registry_append_remote_member(
            &buffer,
            &first,
            "description_localizations",
            dcc_json_object_get(json, "description_localizations")
        );
    }
    if (status == DCC_OK && type_json != NULL) {
        status = dcc_command_registry_append_remote_member(&buffer, &first, "type", type_json);
    }
    const dcc_json_t *options = dcc_json_object_get(json, "options");
    if (status == DCC_OK && dcc_command_registry_json_array_nonempty(options)) {
        status = dcc_command_registry_append_remote_member(&buffer, &first, "options", options);
    }
    if (status == DCC_OK) {
        status = dcc_command_registry_append_remote_member(
            &buffer,
            &first,
            "default_member_permissions",
            dcc_json_object_get(json, "default_member_permissions")
        );
    }
    if (status == DCC_OK) {
        status = dcc_command_registry_append_remote_member(&buffer, &first, "dm_permission", dcc_json_object_get(json, "dm_permission"));
    }
    if (status == DCC_OK) {
        status = dcc_command_registry_append_remote_member(&buffer, &first, "nsfw", dcc_json_object_get(json, "nsfw"));
    }
    if (status == DCC_OK) {
        status = dcc_command_registry_append_remote_member(
            &buffer,
            &first,
            "integration_types",
            dcc_json_object_get(json, "integration_types")
        );
    }
    if (status == DCC_OK) {
        status = dcc_command_registry_append_remote_member(&buffer, &first, "contexts", dcc_json_object_get(json, "contexts"));
    }
    if (status == DCC_OK) {
        status = dcc_command_json_append_cstr(&buffer, "}");
    }
    if (status != DCC_OK) {
        dcc_command_json_buffer_deinit(&buffer);
        return status;
    }
    *out = buffer.data;
    return DCC_OK;
}

dcc_status_t dcc_command_registry_remote_entry_from_json(
    const dcc_json_t *json,
    dcc_command_registry_remote_entry_t *entry
) {
    if (json == NULL || entry == NULL || dcc_json_typeof(json) != DCC_JSON_OBJECT) {
        return DCC_ERR_JSON;
    }
    const char *name = dcc_json_string(dcc_json_object_get(json, "name"));
    if (name == NULL || name[0] == '\0') {
        return DCC_ERR_JSON;
    }

    uint64_t type = DCC_APPLICATION_COMMAND_CHAT_INPUT;
    (void)dcc_json_u64(dcc_json_object_get(json, "type"), &type);
    memset(entry, 0, sizeof(*entry));
    entry->size = sizeof(*entry);
    entry->id = dcc_command_registry_json_snowflake(dcc_json_object_get(json, "id"));
    entry->name = dcc_strdup(name);
    entry->type = (uint32_t)type;
    dcc_status_t status = dcc_command_registry_remote_canonical_json(json, (char **)&entry->canonical_json);
    if (entry->name == NULL || status != DCC_OK) {
        int missing_name = entry->name == NULL;
        free((char *)entry->name);
        free((char *)entry->canonical_json);
        memset(entry, 0, sizeof(*entry));
        return missing_name ? DCC_ERR_NOMEM : status;
    }
    return DCC_OK;
}
