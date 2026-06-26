#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_core_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <dcc/rest/official_surface.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DCC_BULK_BAN_MAX_USERS 200U
#define DCC_APPLICATION_TAG_MAX 5U
#define DCC_APPLICATION_TAG_LEN_MAX 20U
#define DCC_APPLICATION_INTEGRATION_CONFIG_MAX 2U
#define DCC_BULK_BAN_MAX_DELETE_SECONDS 604800U
#define DCC_GUILD_SOUNDBOARD_SOUND_NAME_MIN 2U
#define DCC_GUILD_SOUNDBOARD_SOUND_NAME_MAX 32U
#define DCC_LOBBY_SECRET_MAX 250U
#define DCC_LOBBY_IDLE_TIMEOUT_MIN 5U
#define DCC_LOBBY_IDLE_TIMEOUT_MAX 604800U
#define DCC_LOBBY_BULK_MEMBER_MAX 25U
#define DCC_LOBBY_MODERATION_METADATA_MAX_KEYS 5U
#define DCC_LOBBY_MODERATION_METADATA_KEY_MAX 1024U
#define DCC_LOBBY_MODERATION_METADATA_VALUE_MAX 2000U

static dcc_status_t dcc_rest_escape_required(const char *value, char **out) {
    return value != NULL && value[0] != '\0' ? dcc_rest_escape_path_segment(value, out) : DCC_ERR_INVALID_ARG;
}

static dcc_status_t dcc_rest_path_snowflake(
    char *path,
    size_t path_size,
    const char *format,
    dcc_snowflake_t id
) {
    return id != 0 ? dcc_rest_format_path(path, path_size, format, (unsigned long long)id) : DCC_ERR_INVALID_ARG;
}

static int dcc_rest_string_len_between(const char *value, size_t min_len, size_t max_len) {
    if (value == NULL) {
        return 0;
    }
    size_t len = strlen(value);
    return len >= min_len && len <= max_len;
}

static dcc_status_t dcc_rest_json_append_double_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    double value
) {
    char text[32];
    int written = snprintf(text, sizeof(text), "%.17g", value);
    if (written < 0 || (size_t)written >= sizeof(text)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_json_append_raw_member(body, first, key, text);
}

static dcc_status_t dcc_rest_json_append_nullable_raw_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const char *value
) {
    return dcc_rest_json_append_raw_member(body, first, key, value != NULL ? value : "null");
}

static dcc_status_t dcc_rest_json_append_escaped_key_string_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const char *value
) {
    dcc_status_t status = dcc_rest_json_member_sep(body, first);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_json_string(body, key);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, ":");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_json_string(body, value);
    }
    return status;
}

static int dcc_rest_string_array_valid(
    const char *const *values,
    size_t count,
    size_t max_count,
    size_t max_len
) {
    if (count > max_count || (count != 0 && values == NULL)) {
        return 0;
    }
    for (size_t i = 0; i < count; ++i) {
        if (values[i] == NULL) {
            return 0;
        }
        if (max_len != 0 && strlen(values[i]) > max_len) {
            return 0;
        }
    }
    return 1;
}

static int dcc_application_install_params_valid(const dcc_application_install_params_t *params) {
    return params != NULL &&
        params->size >= sizeof(*params) &&
        params->scope_count != 0 &&
        dcc_rest_string_array_valid(params->scopes, params->scope_count, SIZE_MAX, 0) &&
        params->has_permissions != 0 &&
        params->permissions != NULL &&
        params->permissions[0] != '\0';
}

static dcc_status_t dcc_rest_append_application_install_params(
    dcc_rest_buffer_t *body,
    const dcc_application_install_params_t *params
) {
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "{");
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_array_member(body, &first, "scopes", params->scopes, params->scope_count);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(body, &first, "permissions", params->permissions);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "}");
    }
    return status;
}

static dcc_status_t dcc_rest_json_append_application_install_params_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const dcc_application_install_params_t *params
) {
    dcc_status_t status = dcc_rest_json_append_member_key(body, first, key);
    if (status == DCC_OK) {
        status = dcc_rest_append_application_install_params(body, params);
    }
    return status;
}

static int dcc_application_event_webhook_status_valid(uint32_t status) {
    return status == DCC_APPLICATION_EVENT_WEBHOOK_DISABLED ||
        status == DCC_APPLICATION_EVENT_WEBHOOK_ENABLED;
}

static int dcc_application_integration_type_valid(uint32_t integration_type) {
    return integration_type == DCC_APPLICATION_INTEGRATION_TYPE_GUILD_INSTALL ||
        integration_type == DCC_APPLICATION_INTEGRATION_TYPE_USER_INSTALL;
}

static int dcc_application_integration_configs_valid(
    const dcc_application_integration_type_config_t *configs,
    size_t count
) {
    uint8_t seen[2] = {0, 0};
    if (count > DCC_APPLICATION_INTEGRATION_CONFIG_MAX || (count != 0 && configs == NULL)) {
        return 0;
    }
    for (size_t i = 0; i < count; ++i) {
        const dcc_application_integration_type_config_t *config = &configs[i];
        if (config->size < sizeof(*config) ||
            !dcc_application_integration_type_valid(config->integration_type) ||
            seen[config->integration_type] != 0) {
            return 0;
        }
        seen[config->integration_type] = 1;
        if (config->has_oauth2_install_params != 0 &&
            !dcc_application_install_params_valid(config->oauth2_install_params)) {
            return 0;
        }
    }
    return 1;
}

static dcc_status_t dcc_rest_append_application_integration_config(
    dcc_rest_buffer_t *body,
    const dcc_application_integration_type_config_t *config
) {
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "{");
    if (status == DCC_OK && config->has_oauth2_install_params != 0) {
        status = dcc_rest_json_append_application_install_params_member(
            body,
            &first,
            "oauth2_install_params",
            config->oauth2_install_params
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "}");
    }
    return status;
}

static dcc_status_t dcc_rest_json_append_application_integration_configs_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const dcc_application_integration_type_config_t *configs,
    size_t count
) {
    dcc_status_t status = dcc_rest_json_append_member_key(body, first, key);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "{");
    }
    for (size_t i = 0; i < count && status == DCC_OK; ++i) {
        if (i != 0) {
            status = dcc_rest_buffer_append_cstr(body, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(
                body,
                configs[i].integration_type == DCC_APPLICATION_INTEGRATION_TYPE_GUILD_INSTALL ? "\"0\":" : "\"1\":"
            );
        }
        if (status == DCC_OK) {
            status = dcc_rest_append_application_integration_config(body, &configs[i]);
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "}");
    }
    return status;
}

dcc_status_t dcc_rest_get_gateway(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_request_method(client, DCC_REST_GET, "/gateway", NULL, cb, user_data);
}

dcc_status_t dcc_rest_modify_current_application(
    dcc_client_t *client,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_request_method(client, DCC_REST_PATCH, "/applications/@me", json_body, cb, user_data);
}

dcc_status_t dcc_rest_build_application_modify_body(
    const dcc_application_modify_params_t *params,
    char **out_json
) {
    if (out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    if (params == NULL || params->size < sizeof(*params)) {
        return DCC_ERR_INVALID_ARG;
    }
    if ((params->has_custom_install_url != 0 && params->custom_install_url == NULL) ||
        (params->has_description != 0 && params->description == NULL) ||
        (params->has_role_connections_verification_url != 0 &&
            params->role_connections_verification_url == NULL) ||
        (params->has_install_params != 0 && !dcc_application_install_params_valid(params->install_params)) ||
        (params->has_integration_types_config != 0 &&
            !dcc_application_integration_configs_valid(
                params->integration_types_config,
                params->integration_type_config_count
            )) ||
        (params->has_interactions_endpoint_url != 0 && params->interactions_endpoint_url == NULL) ||
        (params->has_tags != 0 &&
            !dcc_rest_string_array_valid(
                params->tags,
                params->tag_count,
                DCC_APPLICATION_TAG_MAX,
                DCC_APPLICATION_TAG_LEN_MAX
            )) ||
        (params->has_event_webhooks_url != 0 && params->event_webhooks_url == NULL) ||
        (params->has_event_webhooks_status != 0 &&
            !dcc_application_event_webhook_status_valid(params->event_webhooks_status)) ||
        (params->has_event_webhooks_types != 0 &&
            !dcc_rest_string_array_valid(params->event_webhooks_types, params->event_webhooks_type_count, SIZE_MAX, 0))) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    if (status == DCC_OK && params->has_custom_install_url != 0) {
        status = dcc_rest_json_append_string_member(&body, &first, "custom_install_url", params->custom_install_url);
    }
    if (status == DCC_OK && params->has_description != 0) {
        status = dcc_rest_json_append_string_member(&body, &first, "description", params->description);
    }
    if (status == DCC_OK && params->has_role_connections_verification_url != 0) {
        status = dcc_rest_json_append_string_member(
            &body,
            &first,
            "role_connections_verification_url",
            params->role_connections_verification_url
        );
    }
    if (status == DCC_OK && params->has_install_params != 0) {
        status = dcc_rest_json_append_application_install_params_member(
            &body,
            &first,
            "install_params",
            params->install_params
        );
    }
    if (status == DCC_OK && params->has_integration_types_config != 0) {
        status = dcc_rest_json_append_application_integration_configs_member(
            &body,
            &first,
            "integration_types_config",
            params->integration_types_config,
            params->integration_type_config_count
        );
    }
    if (status == DCC_OK && params->has_flags != 0) {
        status = dcc_rest_json_append_u64_member(&body, &first, "flags", params->flags);
    }
    if (status == DCC_OK && params->has_icon != 0) {
        status = dcc_rest_json_append_nullable_string_member(&body, &first, "icon", params->icon);
    }
    if (status == DCC_OK && params->has_cover_image != 0) {
        status = dcc_rest_json_append_nullable_string_member(&body, &first, "cover_image", params->cover_image);
    }
    if (status == DCC_OK && params->has_interactions_endpoint_url != 0) {
        status = dcc_rest_json_append_string_member(
            &body,
            &first,
            "interactions_endpoint_url",
            params->interactions_endpoint_url
        );
    }
    if (status == DCC_OK && params->has_tags != 0) {
        status = dcc_rest_json_append_string_array_member(&body, &first, "tags", params->tags, params->tag_count);
    }
    if (status == DCC_OK && params->has_event_webhooks_url != 0) {
        status = dcc_rest_json_append_string_member(&body, &first, "event_webhooks_url", params->event_webhooks_url);
    }
    if (status == DCC_OK && params->has_event_webhooks_status != 0) {
        status = dcc_rest_json_append_u64_member(
            &body,
            &first,
            "event_webhooks_status",
            params->event_webhooks_status
        );
    }
    if (status == DCC_OK && params->has_event_webhooks_types != 0) {
        status = dcc_rest_json_append_string_array_member(
            &body,
            &first,
            "event_webhooks_types",
            params->event_webhooks_types,
            params->event_webhooks_type_count
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out_json = body.data;
        body.data = NULL;
        body.len = 0;
        body.cap = 0;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_rest_modify_current_application_params(
    dcc_client_t *client,
    const dcc_application_modify_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_rest_build_application_modify_body(params, &json);
    if (status == DCC_OK) {
        status = dcc_rest_modify_current_application(client, json, cb, user_data);
    }
    dcc_rest_official_body_json_free(json);
    return status;
}

dcc_status_t dcc_rest_get_activity_instance(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *instance_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char *instance = NULL;
    dcc_status_t status = application_id != 0 ? dcc_rest_escape_required(instance_id, &instance) : DCC_ERR_INVALID_ARG;
    char path[176];
    if (status == DCC_OK) {
        status = dcc_rest_format_path(
            path,
            sizeof(path),
            "/applications/%llu/activity-instances/%s",
            (unsigned long long)application_id,
            instance
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data);
    }
    free(instance);
    return status;
}

dcc_status_t dcc_rest_send_soundboard_sound(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_path_snowflake(
        path,
        sizeof(path),
        "/channels/%llu/send-soundboard-sound",
        channel_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_build_soundboard_send_body(
    const dcc_soundboard_send_params_t *params,
    char **out_json
) {
    if (out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    if (params == NULL || params->size < sizeof(*params) || params->sound_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "sound_id", params->sound_id);
    }
    if (status == DCC_OK && params->source_guild_id != 0) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "source_guild_id", params->source_guild_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out_json = body.data;
        body.data = NULL;
        body.len = 0;
        body.cap = 0;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_rest_send_soundboard_sound_params(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_soundboard_send_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_rest_build_soundboard_send_body(params, &json);
    if (status == DCC_OK) {
        status = dcc_rest_send_soundboard_sound(client, channel_id, json, cb, user_data);
    }
    dcc_rest_official_body_json_free(json);
    return status;
}

dcc_status_t dcc_rest_get_default_soundboard_sounds(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_request_method(client, DCC_REST_GET, "/soundboard-default-sounds", NULL, cb, user_data);
}

dcc_status_t dcc_rest_get_guild_soundboard_sounds(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/guilds/%llu/soundboard-sounds", guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_guild_soundboard_sound(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sound_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = guild_id != 0 && sound_id != 0 ?
        dcc_rest_format_path(
            path,
            sizeof(path),
            "/guilds/%llu/soundboard-sounds/%llu",
            (unsigned long long)guild_id,
            (unsigned long long)sound_id
        ) :
        DCC_ERR_INVALID_ARG;
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_create_guild_soundboard_sound(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/guilds/%llu/soundboard-sounds", guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}

static dcc_status_t dcc_rest_build_guild_soundboard_sound_body(
    const dcc_guild_soundboard_sound_params_t *params,
    char **out_json,
    int create_body
) {
    if (out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        (create_body && !dcc_rest_string_len_between(
            params->name,
            DCC_GUILD_SOUNDBOARD_SOUND_NAME_MIN,
            DCC_GUILD_SOUNDBOARD_SOUND_NAME_MAX
        )) ||
        (create_body && (params->sound == NULL || params->sound[0] == '\0')) ||
        (!create_body &&
         params->name == NULL &&
         !params->has_volume &&
         !params->has_emoji_id &&
         !params->has_emoji_name) ||
        (!create_body &&
         params->name != NULL &&
         !dcc_rest_string_len_between(
             params->name,
             DCC_GUILD_SOUNDBOARD_SOUND_NAME_MIN,
             DCC_GUILD_SOUNDBOARD_SOUND_NAME_MAX
         )) ||
        (params->has_volume &&
         (!(params->volume == params->volume) || params->volume < 0.0 || params->volume > 1.0))) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    if (status == DCC_OK && params->name != NULL) {
        status = dcc_rest_json_append_string_member(&body, &first, "name", params->name);
    }
    if (status == DCC_OK && create_body) {
        status = dcc_rest_json_append_string_member(&body, &first, "sound", params->sound);
    }
    if (status == DCC_OK && params->has_volume) {
        status = dcc_rest_json_append_double_member(&body, &first, "volume", params->volume);
    }
    if (status == DCC_OK && params->has_emoji_id) {
        status = params->emoji_id != 0 ?
            dcc_rest_json_append_u64_string_member(&body, &first, "emoji_id", params->emoji_id) :
            dcc_rest_json_append_raw_member(&body, &first, "emoji_id", "null");
    }
    if (status == DCC_OK && params->has_emoji_name) {
        status = params->emoji_name != NULL ?
            dcc_rest_json_append_string_member(&body, &first, "emoji_name", params->emoji_name) :
            dcc_rest_json_append_raw_member(&body, &first, "emoji_name", "null");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out_json = body.data;
        body.data = NULL;
        body.len = 0;
        body.cap = 0;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_rest_build_guild_soundboard_sound_create_body(
    const dcc_guild_soundboard_sound_params_t *params,
    char **out_json
) {
    return dcc_rest_build_guild_soundboard_sound_body(params, out_json, 1);
}

dcc_status_t dcc_rest_create_guild_soundboard_sound_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_guild_soundboard_sound_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_rest_build_guild_soundboard_sound_create_body(params, &json);
    if (status == DCC_OK) {
        status = dcc_rest_create_guild_soundboard_sound(client, guild_id, json, cb, user_data);
    }
    dcc_rest_official_body_json_free(json);
    return status;
}

dcc_status_t dcc_rest_modify_guild_soundboard_sound(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sound_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = guild_id != 0 && sound_id != 0 ?
        dcc_rest_format_path(
            path,
            sizeof(path),
            "/guilds/%llu/soundboard-sounds/%llu",
            (unsigned long long)guild_id,
            (unsigned long long)sound_id
        ) :
        DCC_ERR_INVALID_ARG;
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_build_guild_soundboard_sound_modify_body(
    const dcc_guild_soundboard_sound_params_t *params,
    char **out_json
) {
    return dcc_rest_build_guild_soundboard_sound_body(params, out_json, 0);
}

dcc_status_t dcc_rest_modify_guild_soundboard_sound_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sound_id,
    const dcc_guild_soundboard_sound_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_rest_build_guild_soundboard_sound_modify_body(params, &json);
    if (status == DCC_OK) {
        status = dcc_rest_modify_guild_soundboard_sound(client, guild_id, sound_id, json, cb, user_data);
    }
    dcc_rest_official_body_json_free(json);
    return status;
}

dcc_status_t dcc_rest_delete_guild_soundboard_sound(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sound_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = guild_id != 0 && sound_id != 0 ?
        dcc_rest_format_path(
            path,
            sizeof(path),
            "/guilds/%llu/soundboard-sounds/%llu",
            (unsigned long long)guild_id,
            (unsigned long long)sound_id
        ) :
        DCC_ERR_INVALID_ARG;
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_sku_subscriptions(
    dcc_client_t *client,
    dcc_snowflake_t sku_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/skus/%llu/subscriptions", sku_id);
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_sku_subscription(
    dcc_client_t *client,
    dcc_snowflake_t sku_id,
    dcc_snowflake_t subscription_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = sku_id != 0 && subscription_id != 0 ?
        dcc_rest_format_path(
            path,
            sizeof(path),
            "/skus/%llu/subscriptions/%llu",
            (unsigned long long)sku_id,
            (unsigned long long)subscription_id
        ) :
        DCC_ERR_INVALID_ARG;
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_bulk_ban_guild_members(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[88];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/guilds/%llu/bulk-ban", guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_build_bulk_ban_body(
    const dcc_bulk_ban_params_t *params,
    char **out_json
) {
    if (out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->user_ids == NULL ||
        params->user_id_count == 0 ||
        params->user_id_count > DCC_BULK_BAN_MAX_USERS ||
        (params->has_delete_message_seconds &&
         params->delete_message_seconds > DCC_BULK_BAN_MAX_DELETE_SECONDS)) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < params->user_id_count; i++) {
        if (params->user_ids[i] == 0) {
            return DCC_ERR_INVALID_ARG;
        }
    }

    dcc_rest_buffer_t body = {0};
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    if (status == DCC_OK) {
        status = dcc_rest_json_append_snowflake_string_array_member(
            &body,
            &first,
            "user_ids",
            params->user_ids,
            params->user_id_count
        );
    }
    if (status == DCC_OK && params->has_delete_message_seconds) {
        status = dcc_rest_json_append_u64_member(
            &body,
            &first,
            "delete_message_seconds",
            params->delete_message_seconds
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out_json = body.data;
        body.data = NULL;
        body.len = 0;
        body.cap = 0;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_rest_bulk_ban_guild_members_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_bulk_ban_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_rest_build_bulk_ban_body(params, &json);
    if (status == DCC_OK) {
        status = dcc_rest_bulk_ban_guild_members(client, guild_id, json, cb, user_data);
    }
    dcc_rest_official_body_json_free(json);
    return status;
}

dcc_status_t dcc_rest_get_guild_role(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = guild_id != 0 && role_id != 0 ?
        dcc_rest_format_path(
            path,
            sizeof(path),
            "/guilds/%llu/roles/%llu",
            (unsigned long long)guild_id,
            (unsigned long long)role_id
        ) :
        DCC_ERR_INVALID_ARG;
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_guild_role_member_counts(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[104];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/guilds/%llu/roles/member-counts", guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_guild_widget_json(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[88];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/guilds/%llu/widget.json", guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_guild_widget_png(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *style,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[88];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/guilds/%llu/widget.png", guild_id);
    char *encoded_style = NULL;
    dcc_rest_buffer_t query = {0};
    if (status == DCC_OK && style != NULL && style[0] != '\0') {
        status = dcc_rest_escape_path_segment(style, &encoded_style);
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&query, "style=");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&query, encoded_style);
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_request_with_query(
            client,
            DCC_REST_GET,
            path,
            query.data,
            NULL,
            cb,
            user_data
        );
    }
    free(encoded_style);
    dcc_rest_buffer_deinit(&query);
    return status;
}

dcc_status_t dcc_rest_modify_guild_incident_actions(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[104];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/guilds/%llu/incident-actions", guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PUT, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_build_guild_incident_actions_body(
    const dcc_guild_incident_actions_params_t *params,
    char **out_json
) {
    if (out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        (!params->has_invites_disabled_until && !params->has_dms_disabled_until)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    if (status == DCC_OK && params->has_invites_disabled_until) {
        status = params->invites_disabled_until != NULL ?
            dcc_rest_json_append_string_member(
                &body,
                &first,
                "invites_disabled_until",
                params->invites_disabled_until
            ) :
            dcc_rest_json_append_raw_member(&body, &first, "invites_disabled_until", "null");
    }
    if (status == DCC_OK && params->has_dms_disabled_until) {
        status = params->dms_disabled_until != NULL ?
            dcc_rest_json_append_string_member(
                &body,
                &first,
                "dms_disabled_until",
                params->dms_disabled_until
            ) :
            dcc_rest_json_append_raw_member(&body, &first, "dms_disabled_until", "null");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out_json = body.data;
        body.data = NULL;
        body.len = 0;
        body.cap = 0;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_rest_modify_guild_incident_actions_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_guild_incident_actions_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_rest_build_guild_incident_actions_body(params, &json);
    if (status == DCC_OK) {
        status = dcc_rest_modify_guild_incident_actions(client, guild_id, json, cb, user_data);
    }
    dcc_rest_official_body_json_free(json);
    return status;
}

void dcc_rest_official_body_json_free(char *json) {
    free(json);
}

static dcc_status_t dcc_rest_invite_path(
    char *path,
    size_t path_size,
    const char *format,
    const char *invite_code
) {
    char *code = NULL;
    dcc_status_t status = dcc_rest_escape_required(invite_code, &code);
    if (status == DCC_OK) {
        status = dcc_rest_format_path(path, path_size, format, code);
    }
    free(code);
    return status;
}

dcc_status_t dcc_rest_get_invite_target_users(
    dcc_client_t *client,
    const char *invite_code,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[160];
    dcc_status_t status = dcc_rest_invite_path(path, sizeof(path), "/invites/%s/target-users", invite_code);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_put_invite_target_users(
    dcc_client_t *client,
    const char *invite_code,
    const char *filename,
    const void *csv_data,
    size_t csv_len,
    dcc_rest_cb cb,
    void *user_data
) {
    if (filename == NULL || filename[0] == '\0' || csv_data == NULL || csv_len == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    char path[160];
    dcc_status_t status = dcc_rest_invite_path(path, sizeof(path), "/invites/%s/target-users", invite_code);
    dcc_rest_multipart_file_t file = {
        .field_name = "target_users_file",
        .filename = filename,
        .content_type = "text/csv",
        .data = csv_data,
        .data_len = csv_len,
    };
    return status == DCC_OK ?
        dcc_rest_request_method_multipart(client, DCC_REST_PUT, path, NULL, 0, &file, 1, cb, user_data) :
        status;
}

dcc_status_t dcc_rest_get_invite_target_users_job_status(
    dcc_client_t *client,
    const char *invite_code,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[176];
    dcc_status_t status = dcc_rest_invite_path(path, sizeof(path), "/invites/%s/target-users/job-status", invite_code);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_entitlement(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = application_id != 0 && entitlement_id != 0 ?
        dcc_rest_format_path(
            path,
            sizeof(path),
            "/applications/%llu/entitlements/%llu",
            (unsigned long long)application_id,
            (unsigned long long)entitlement_id
        ) :
        DCC_ERR_INVALID_ARG;
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_sticker_pack(
    dcc_client_t *client,
    dcc_snowflake_t pack_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/sticker-packs/%llu", pack_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_current_user_guild_member(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/users/@me/guilds/%llu/member", guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_delete_current_user_application_role_connection(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_path_snowflake(
        path,
        sizeof(path),
        "/users/@me/applications/%llu/role-connection",
        application_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}

static dcc_status_t dcc_rest_webhook_compat_path(
    char *path,
    size_t path_size,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *suffix
) {
    char *token = NULL;
    dcc_status_t status = webhook_id != 0 ? dcc_rest_escape_required(webhook_token, &token) : DCC_ERR_INVALID_ARG;
    if (status == DCC_OK) {
        status = dcc_rest_format_path(
            path,
            path_size,
            "/webhooks/%llu/%s/%s",
            (unsigned long long)webhook_id,
            token,
            suffix
        );
    }
    free(token);
    return status;
}

dcc_status_t dcc_rest_execute_webhook_slack(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[192];
    dcc_status_t status = dcc_rest_webhook_compat_path(path, sizeof(path), webhook_id, webhook_token, "slack");
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_execute_webhook_github(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[192];
    dcc_status_t status = dcc_rest_webhook_compat_path(path, sizeof(path), webhook_id, webhook_token, "github");
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_create_lobby(dcc_client_t *client, const char *json_body, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_request_method(client, DCC_REST_POST, "/lobbies", json_body, cb, user_data);
}

dcc_status_t dcc_rest_create_or_join_lobby(dcc_client_t *client, const char *json_body, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_request_method(client, DCC_REST_PUT, "/lobbies", json_body, cb, user_data);
}

static int dcc_rest_lobby_member_specs_valid(const dcc_lobby_member_spec_t *members, size_t count) {
    if (count > DCC_LOBBY_BULK_MEMBER_MAX) {
        return 0;
    }
    if (count == 0) {
        return 1;
    }
    if (members == NULL) {
        return 0;
    }
    for (size_t i = 0; i < count; i++) {
        if (members[i].size < sizeof(members[i]) || members[i].user_id == 0) {
            return 0;
        }
    }
    return 1;
}

static dcc_status_t dcc_rest_append_lobby_member_spec(
    dcc_rest_buffer_t *body,
    const dcc_lobby_member_spec_t *member
) {
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "{");
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_string_member(body, &first, "id", member->user_id);
    }
    if (status == DCC_OK && member->has_metadata) {
        status = dcc_rest_json_append_nullable_raw_member(
            body,
            &first,
            "metadata",
            member->metadata_json
        );
    }
    if (status == DCC_OK && member->has_flags) {
        status = dcc_rest_json_append_u64_member(body, &first, "flags", member->flags);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "}");
    }
    return status;
}

static dcc_status_t dcc_rest_json_append_lobby_members_member(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_lobby_member_spec_t *members,
    size_t member_count
) {
    dcc_status_t status = dcc_rest_json_append_member_key(body, first, "members");
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "[");
    }
    for (size_t i = 0; status == DCC_OK && i < member_count; i++) {
        if (i != 0) {
            status = dcc_rest_buffer_append_cstr(body, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_append_lobby_member_spec(body, &members[i]);
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "]");
    }
    return status;
}

static dcc_status_t dcc_rest_build_lobby_common_body(
    const dcc_lobby_params_t *params,
    char **out_json,
    int require_field
) {
    if (out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        (params->has_members &&
         !dcc_rest_lobby_member_specs_valid(params->members, params->member_count)) ||
        (params->has_idle_timeout_seconds &&
         (params->idle_timeout_seconds < DCC_LOBBY_IDLE_TIMEOUT_MIN ||
          params->idle_timeout_seconds > DCC_LOBBY_IDLE_TIMEOUT_MAX)) ||
        (require_field &&
         !params->has_metadata &&
         !params->has_members &&
         !params->has_idle_timeout_seconds)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    if (status == DCC_OK && params->has_metadata) {
        status = dcc_rest_json_append_nullable_raw_member(
            &body,
            &first,
            "metadata",
            params->metadata_json
        );
    }
    if (status == DCC_OK && params->has_members) {
        status = dcc_rest_json_append_lobby_members_member(
            &body,
            &first,
            params->members,
            params->member_count
        );
    }
    if (status == DCC_OK && params->has_idle_timeout_seconds) {
        status = dcc_rest_json_append_u64_member(
            &body,
            &first,
            "idle_timeout_seconds",
            params->idle_timeout_seconds
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out_json = body.data;
        body.data = NULL;
        body.len = 0;
        body.cap = 0;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_rest_build_lobby_body(
    const dcc_lobby_params_t *params,
    char **out_json
) {
    return dcc_rest_build_lobby_common_body(params, out_json, 0);
}

dcc_status_t dcc_rest_create_lobby_params(
    dcc_client_t *client,
    const dcc_lobby_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_rest_build_lobby_body(params, &json);
    if (status == DCC_OK) {
        status = dcc_rest_create_lobby(client, json, cb, user_data);
    }
    dcc_rest_official_body_json_free(json);
    return status;
}

dcc_status_t dcc_rest_build_lobby_create_or_join_body(
    const dcc_lobby_create_or_join_params_t *params,
    char **out_json
) {
    if (out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->secret == NULL ||
        params->secret[0] == '\0' ||
        strlen(params->secret) > DCC_LOBBY_SECRET_MAX ||
        (params->has_idle_timeout_seconds &&
         (params->idle_timeout_seconds < DCC_LOBBY_IDLE_TIMEOUT_MIN ||
          params->idle_timeout_seconds > DCC_LOBBY_IDLE_TIMEOUT_MAX))) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(&body, &first, "secret", params->secret);
    }
    if (status == DCC_OK && params->has_idle_timeout_seconds) {
        status = dcc_rest_json_append_u64_member(
            &body,
            &first,
            "idle_timeout_seconds",
            params->idle_timeout_seconds
        );
    }
    if (status == DCC_OK && params->has_lobby_metadata) {
        status = dcc_rest_json_append_nullable_raw_member(
            &body,
            &first,
            "lobby_metadata",
            params->lobby_metadata_json
        );
    }
    if (status == DCC_OK && params->has_member_metadata) {
        status = dcc_rest_json_append_nullable_raw_member(
            &body,
            &first,
            "member_metadata",
            params->member_metadata_json
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out_json = body.data;
        body.data = NULL;
        body.len = 0;
        body.cap = 0;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_rest_create_or_join_lobby_params(
    dcc_client_t *client,
    const dcc_lobby_create_or_join_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_rest_build_lobby_create_or_join_body(params, &json);
    if (status == DCC_OK) {
        status = dcc_rest_create_or_join_lobby(client, json, cb, user_data);
    }
    dcc_rest_official_body_json_free(json);
    return status;
}

dcc_status_t dcc_rest_get_lobby(dcc_client_t *client, dcc_snowflake_t lobby_id, dcc_rest_cb cb, void *user_data) {
    char path[80];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/lobbies/%llu", lobby_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_modify_lobby(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/lobbies/%llu", lobby_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_modify_lobby_params(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const dcc_lobby_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_rest_build_lobby_common_body(params, &json, 1);
    if (status == DCC_OK) {
        status = dcc_rest_modify_lobby(client, lobby_id, json, cb, user_data);
    }
    dcc_rest_official_body_json_free(json);
    return status;
}

dcc_status_t dcc_rest_delete_lobby(dcc_client_t *client, dcc_snowflake_t lobby_id, dcc_rest_cb cb, void *user_data) {
    char path[80];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/lobbies/%llu", lobby_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_add_lobby_member(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = lobby_id != 0 && user_id != 0 ?
        dcc_rest_format_path(
            path,
            sizeof(path),
            "/lobbies/%llu/members/%llu",
            (unsigned long long)lobby_id,
            (unsigned long long)user_id
        ) :
        DCC_ERR_INVALID_ARG;
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PUT, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_build_lobby_member_body(
    const dcc_lobby_member_params_t *params,
    char **out_json
) {
    if (out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    if (params == NULL || params->size < sizeof(*params)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    if (status == DCC_OK && params->has_metadata) {
        status = dcc_rest_json_append_nullable_raw_member(
            &body,
            &first,
            "metadata",
            params->metadata_json
        );
    }
    if (status == DCC_OK && params->has_flags) {
        status = dcc_rest_json_append_u64_member(&body, &first, "flags", params->flags);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out_json = body.data;
        body.data = NULL;
        body.len = 0;
        body.cap = 0;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_rest_add_lobby_member_params(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_snowflake_t user_id,
    const dcc_lobby_member_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_rest_build_lobby_member_body(params, &json);
    if (status == DCC_OK) {
        status = dcc_rest_add_lobby_member(client, lobby_id, user_id, json, cb, user_data);
    }
    dcc_rest_official_body_json_free(json);
    return status;
}

dcc_status_t dcc_rest_bulk_update_lobby_members(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[104];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/lobbies/%llu/members/bulk", lobby_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}

static dcc_status_t dcc_rest_append_lobby_bulk_member(
    dcc_rest_buffer_t *body,
    const dcc_lobby_member_bulk_item_t *item
) {
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "{");
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_string_member(body, &first, "id", item->user_id);
    }
    if (status == DCC_OK && item->has_metadata) {
        status = dcc_rest_json_append_nullable_raw_member(
            body,
            &first,
            "metadata",
            item->metadata_json
        );
    }
    if (status == DCC_OK && item->has_flags) {
        status = dcc_rest_json_append_u64_member(body, &first, "flags", item->flags);
    }
    if (status == DCC_OK && item->has_remove_member) {
        status = dcc_rest_json_append_bool_member(body, &first, "remove_member", item->remove_member);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "}");
    }
    return status;
}

dcc_status_t dcc_rest_build_lobby_member_bulk_body(
    const dcc_lobby_member_bulk_params_t *params,
    char **out_json
) {
    if (out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->items == NULL ||
        params->item_count == 0 ||
        params->item_count > DCC_LOBBY_BULK_MEMBER_MAX) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < params->item_count; i++) {
        if (params->items[i].size < sizeof(params->items[i]) || params->items[i].user_id == 0) {
            return DCC_ERR_INVALID_ARG;
        }
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "[");
    for (size_t i = 0; status == DCC_OK && i < params->item_count; i++) {
        if (i != 0) {
            status = dcc_rest_buffer_append_cstr(&body, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_append_lobby_bulk_member(&body, &params->items[i]);
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "]");
    }
    if (status == DCC_OK) {
        *out_json = body.data;
        body.data = NULL;
        body.len = 0;
        body.cap = 0;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_rest_bulk_update_lobby_members_params(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const dcc_lobby_member_bulk_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_rest_build_lobby_member_bulk_body(params, &json);
    if (status == DCC_OK) {
        status = dcc_rest_bulk_update_lobby_members(client, lobby_id, json, cb, user_data);
    }
    dcc_rest_official_body_json_free(json);
    return status;
}

dcc_status_t dcc_rest_remove_lobby_member(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = lobby_id != 0 && user_id != 0 ?
        dcc_rest_format_path(
            path,
            sizeof(path),
            "/lobbies/%llu/members/%llu",
            (unsigned long long)lobby_id,
            (unsigned long long)user_id
        ) :
        DCC_ERR_INVALID_ARG;
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_leave_lobby(dcc_client_t *client, dcc_snowflake_t lobby_id, dcc_rest_cb cb, void *user_data) {
    char path[104];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/lobbies/%llu/members/@me", lobby_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_link_lobby_channel(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/lobbies/%llu/channel-linking", lobby_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_build_lobby_channel_link_body(
    const dcc_lobby_channel_link_params_t *params,
    char **out_json
) {
    if (out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    if (params == NULL || params->size < sizeof(*params) || params->channel_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "channel_id", params->channel_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out_json = body.data;
        body.data = NULL;
        body.len = 0;
        body.cap = 0;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_rest_link_lobby_channel_params(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const dcc_lobby_channel_link_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_rest_build_lobby_channel_link_body(params, &json);
    if (status == DCC_OK) {
        status = dcc_rest_link_lobby_channel(client, lobby_id, json, cb, user_data);
    }
    dcc_rest_official_body_json_free(json);
    return status;
}

dcc_status_t dcc_rest_unlink_lobby_channel(dcc_client_t *client, dcc_snowflake_t lobby_id, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_link_lobby_channel(client, lobby_id, NULL, cb, user_data);
}

dcc_status_t dcc_rest_send_lobby_message(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/lobbies/%llu/messages", lobby_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_build_lobby_message_body(
    const dcc_lobby_message_params_t *params,
    char **out_json
) {
    if (out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->content == NULL ||
        params->content[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(&body, &first, "content", params->content);
    }
    if (status == DCC_OK && params->has_metadata) {
        status = dcc_rest_json_append_nullable_raw_member(&body, &first, "metadata", params->metadata_json);
    }
    if (status == DCC_OK && params->has_flags) {
        status = dcc_rest_json_append_u64_member(&body, &first, "flags", params->flags);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out_json = body.data;
        body.data = NULL;
        body.len = 0;
        body.cap = 0;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_rest_send_lobby_message_params(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const dcc_lobby_message_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_rest_build_lobby_message_body(params, &json);
    if (status == DCC_OK) {
        status = dcc_rest_send_lobby_message(client, lobby_id, json, cb, user_data);
    }
    dcc_rest_official_body_json_free(json);
    return status;
}

dcc_status_t dcc_rest_get_lobby_messages(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/lobbies/%llu/messages", lobby_id);
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_update_lobby_message_moderation_metadata(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_snowflake_t message_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[152];
    dcc_status_t status = lobby_id != 0 && message_id != 0 ?
        dcc_rest_format_path(
            path,
            sizeof(path),
            "/lobbies/%llu/messages/%llu/moderation-metadata",
            (unsigned long long)lobby_id,
            (unsigned long long)message_id
        ) :
        DCC_ERR_INVALID_ARG;
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PUT, path, json_body, cb, user_data) : status;
}

static int dcc_rest_lobby_moderation_metadata_valid(
    const dcc_lobby_message_moderation_metadata_params_t *params
) {
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->items == NULL ||
        params->item_count == 0 ||
        params->item_count > DCC_LOBBY_MODERATION_METADATA_MAX_KEYS) {
        return 0;
    }
    for (size_t i = 0; i < params->item_count; i++) {
        const char *key = params->items[i].key;
        const char *value = params->items[i].value;
        if (key == NULL ||
            key[0] == '\0' ||
            value == NULL ||
            strlen(key) > DCC_LOBBY_MODERATION_METADATA_KEY_MAX ||
            strlen(value) > DCC_LOBBY_MODERATION_METADATA_VALUE_MAX) {
            return 0;
        }
    }
    return 1;
}

dcc_status_t dcc_rest_build_lobby_message_moderation_metadata_body(
    const dcc_lobby_message_moderation_metadata_params_t *params,
    char **out_json
) {
    if (out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    if (!dcc_rest_lobby_moderation_metadata_valid(params)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    for (size_t i = 0; status == DCC_OK && i < params->item_count; i++) {
        status = dcc_rest_json_append_escaped_key_string_member(
            &body,
            &first,
            params->items[i].key,
            params->items[i].value
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out_json = body.data;
        body.data = NULL;
        body.len = 0;
        body.cap = 0;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_rest_update_lobby_message_moderation_metadata_params(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_snowflake_t message_id,
    const dcc_lobby_message_moderation_metadata_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_rest_build_lobby_message_moderation_metadata_body(params, &json);
    if (status == DCC_OK) {
        status = dcc_rest_update_lobby_message_moderation_metadata(
            client,
            lobby_id,
            message_id,
            json,
            cb,
            user_data
        );
    }
    dcc_rest_official_body_json_free(json);
    return status;
}

dcc_status_t dcc_rest_create_lobby_channel_invite_for_self(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[120];
    dcc_status_t status = dcc_rest_path_snowflake(path, sizeof(path), "/lobbies/%llu/members/@me/invites", lobby_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_create_lobby_channel_invite_for_user(
    dcc_client_t *client,
    dcc_snowflake_t lobby_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[144];
    dcc_status_t status = lobby_id != 0 && user_id != 0 ?
        dcc_rest_format_path(
            path,
            sizeof(path),
            "/lobbies/%llu/members/%llu/invites",
            (unsigned long long)lobby_id,
            (unsigned long long)user_id
        ) :
        DCC_ERR_INVALID_ARG;
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, NULL, cb, user_data) : status;
}
