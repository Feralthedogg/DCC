#include "internal/dcc_command_sync.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

static dcc_status_t dcc_command_sync_raw_push(
    dcc_command_sync_raw_store_t *store,
    char *json
) {
    if (store->count == store->cap) {
        if (store->cap > SIZE_MAX / 2U) {
            return DCC_ERR_NOMEM;
        }
        size_t next_cap = store->cap != 0U ? store->cap * 2U : 8U;
        if (next_cap > SIZE_MAX / sizeof(*store->items)) {
            return DCC_ERR_NOMEM;
        }
        char **items = (char **)realloc(store->items, next_cap * sizeof(*items));
        if (items == NULL) {
            return DCC_ERR_NOMEM;
        }
        store->items = items;
        store->cap = next_cap;
    }
    store->items[store->count++] = json;
    return DCC_OK;
}

static dcc_status_t dcc_command_sync_raw_json(
    dcc_command_sync_raw_store_t *store,
    const dcc_json_t *json,
    const char **out
) {
    *out = NULL;
    if (json == NULL || dcc_json_typeof(json) == DCC_JSON_NULL) {
        return DCC_OK;
    }
    dcc_application_command_json_buffer_t buffer = {0};
    dcc_status_t status = dcc_command_registry_append_json_value(&buffer, json);
    if (status != DCC_OK) {
        dcc_command_json_buffer_deinit(&buffer);
        return status;
    }
    status = dcc_command_sync_raw_push(store, buffer.data);
    if (status != DCC_OK) {
        dcc_command_json_buffer_deinit(&buffer);
        return status;
    }
    *out = buffer.data;
    return DCC_OK;
}

static int dcc_command_sync_parse_u64_string(const char *text, uint64_t *out) {
    if (text == NULL || text[0] == '\0' || out == NULL) {
        return -1;
    }
    errno = 0;
    char *end = NULL;
    unsigned long long value = strtoull(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0') {
        return -1;
    }
    *out = (uint64_t)value;
    return 0;
}

static dcc_status_t dcc_command_sync_set_raw_field(
    dcc_command_sync_raw_store_t *store,
    dcc_application_command_builder_t *builder,
    const dcc_json_t *command,
    const char *name
) {
    const char *raw = NULL;
    dcc_status_t status = dcc_command_sync_raw_json(
        store,
        dcc_json_object_get(command, name),
        &raw
    );
    if (status != DCC_OK || raw == NULL) {
        return status;
    }
    if (strcmp(name, "name_localizations") == 0) {
        return dcc_application_command_builder_set_name_localizations_json(builder, raw);
    }
    if (strcmp(name, "description_localizations") == 0) {
        return dcc_application_command_builder_set_description_localizations_json(builder, raw);
    }
    if (strcmp(name, "options") == 0) {
        return dcc_application_command_builder_set_options_json(builder, raw);
    }
    if (strcmp(name, "integration_types") == 0) {
        return dcc_application_command_builder_set_integration_types_json(builder, raw);
    }
    if (strcmp(name, "contexts") == 0) {
        return dcc_application_command_builder_set_contexts_json(builder, raw);
    }
    return DCC_ERR_INVALID_ARG;
}

static dcc_status_t dcc_command_sync_set_permission(
    dcc_application_command_builder_t *builder,
    const dcc_json_t *json
) {
    if (json == NULL) {
        return DCC_OK;
    }
    if (dcc_json_typeof(json) == DCC_JSON_NULL) {
        return dcc_application_command_builder_set_default_member_permissions_null(builder);
    }
    uint64_t value = 0;
    if (dcc_json_u64(json, &value) == 0 ||
        (dcc_json_typeof(json) == DCC_JSON_STRING &&
         dcc_command_sync_parse_u64_string(dcc_json_string(json), &value) == 0)) {
        return dcc_application_command_builder_set_default_member_permissions(builder, value);
    }
    return DCC_ERR_JSON;
}

static dcc_status_t dcc_command_sync_builder_from_json(
    dcc_command_sync_local_t *local,
    const dcc_json_t *json,
    dcc_application_command_builder_t *builder
) {
    if (dcc_json_typeof(json) != DCC_JSON_OBJECT) {
        return DCC_ERR_JSON;
    }
    const char *name = dcc_json_string(dcc_json_object_get(json, "name"));
    if (name == NULL || name[0] == '\0') {
        return DCC_ERR_JSON;
    }

    uint64_t type = DCC_APPLICATION_COMMAND_CHAT_INPUT;
    const dcc_json_t *type_json = dcc_json_object_get(json, "type");
    if (type_json != NULL && dcc_json_u64(type_json, &type) != 0) {
        return DCC_ERR_JSON;
    }
    dcc_application_command_builder_init(builder);
    dcc_status_t status =
        dcc_application_command_builder_set_name(builder, name);
    if (status == DCC_OK) {
        status = dcc_application_command_builder_set_type(
            builder,
            (dcc_application_command_type_t)type
        );
    }

    const char *description = dcc_json_string(dcc_json_object_get(json, "description"));
    if (status == DCC_OK && description != NULL) {
        status = dcc_application_command_builder_set_description(builder, description);
    }
    if (status == DCC_OK && type == DCC_APPLICATION_COMMAND_CHAT_INPUT && description == NULL) {
        status = DCC_ERR_JSON;
    }

    const char *raw_fields[] = {
        "name_localizations",
        "description_localizations",
        "options",
        "integration_types",
        "contexts",
    };
    for (size_t i = 0; status == DCC_OK && i < sizeof(raw_fields) / sizeof(raw_fields[0]); ++i) {
        status = dcc_command_sync_set_raw_field(&local->raw, builder, json, raw_fields[i]);
    }

    if (status == DCC_OK) {
        status = dcc_command_sync_set_permission(
            builder,
            dcc_json_object_get(json, "default_member_permissions")
        );
    }
    int bool_value = 0;
    const dcc_json_t *dm_permission = dcc_json_object_get(json, "dm_permission");
    if (status == DCC_OK && dm_permission != NULL) {
        status = dcc_json_bool(dm_permission, &bool_value) == 0
            ? dcc_application_command_builder_set_dm_permission(builder, (uint8_t)bool_value)
            : DCC_ERR_JSON;
    }
    const dcc_json_t *nsfw = dcc_json_object_get(json, "nsfw");
    if (status == DCC_OK && nsfw != NULL) {
        status = dcc_json_bool(nsfw, &bool_value) == 0
            ? dcc_application_command_builder_set_nsfw(builder, (uint8_t)bool_value)
            : DCC_ERR_JSON;
    }
    return status;
}

dcc_status_t dcc_command_sync_parse_local_commands(
    const char *json,
    size_t json_len,
    dcc_command_sync_local_t *out
) {
    if (json == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(out, 0, sizeof(*out));
    dcc_command_registry_init(&out->registry);

    dcc_status_t status = dcc_json_parse(json, json_len, &out->root);
    if (status != DCC_OK) {
        dcc_command_sync_local_deinit(out);
        return status;
    }
    if (dcc_json_typeof(out->root) != DCC_JSON_ARRAY) {
        dcc_command_sync_local_deinit(out);
        return DCC_ERR_JSON;
    }

    size_t count = dcc_json_array_size(out->root);
    for (size_t i = 0; i < count; ++i) {
        dcc_application_command_builder_t builder;
        status = dcc_command_sync_builder_from_json(
            out,
            dcc_json_array_get(out->root, i),
            &builder
        );
        if (status == DCC_OK) {
            status = dcc_command_registry_add_builder(&out->registry, &builder);
        }
        if (status != DCC_OK) {
            dcc_command_sync_local_deinit(out);
            return status;
        }
    }
    return DCC_OK;
}

void dcc_command_sync_local_deinit(dcc_command_sync_local_t *local) {
    if (local == NULL) {
        return;
    }
    dcc_command_registry_deinit(&local->registry);
    for (size_t i = 0; i < local->raw.count; ++i) {
        free(local->raw.items[i]);
    }
    free(local->raw.items);
    dcc_json_free(local->root);
    memset(local, 0, sizeof(*local));
}
