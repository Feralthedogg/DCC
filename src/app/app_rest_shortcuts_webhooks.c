#include "internal/app/dcc_app_internal.h"

#include <dcc/rest/webhooks.h>

#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_multipart_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"
#include "internal/rest/dcc_rest_query_webhooks_internal.h"
#include "internal/rest/dcc_rest_request_webhooks_internal.h"

#include <stdlib.h>
#include <string.h>

static int dcc_app_webhook_token_invalid(const char *webhook_token) {
    return webhook_token == NULL || webhook_token[0] == '\0';
}

static dcc_status_t dcc_app_webhook_raw(
    dcc_client_t *client,
    dcc_rest_method_t method,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *suffix,
    dcc_snowflake_t message_id,
    const char *query,
    const char *body,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_webhook_token_path(
        &path, webhook_id, webhook_token, suffix, message_id
    );
    if (status == DCC_OK) {
        status = dcc_endpoint_submit_legacy_raw(
            client, method, path, query,
            body != NULL ? "application/json" : NULL,
            body, body != NULL ? strlen(body) : 0U, cb, user_data
        );
    }
    free(path);
    return status;
}

static dcc_status_t dcc_app_webhook_raw_multipart(
    dcc_client_t *client,
    dcc_rest_method_t method,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *suffix,
    dcc_snowflake_t message_id,
    const char *query,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_webhook_token_path(
        &path, webhook_id, webhook_token, suffix, message_id
    );
    char *multipart = NULL;
    size_t multipart_len = 0U;
    if (status == DCC_OK) {
        const dcc_rest_multipart_field_t field = {
            .name = "payload_json",
            .value = payload_json,
        };
        status = dcc_rest_build_multipart_body(
            &field, 1U, files, file_count, &multipart, &multipart_len
        );
    }
    if (status == DCC_OK) {
        status = dcc_endpoint_submit_legacy_raw(
            client, method, path, query, DCC_REST_MULTIPART_CONTENT_TYPE,
            multipart, multipart_len, cb, user_data
        );
    }
    free(multipart);
    free(path);
    return status;
}

static dcc_status_t dcc_app_webhook_management_raw(
    dcc_client_t *client,
    dcc_rest_method_t method,
    dcc_snowflake_t id,
    const char *body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[64];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/webhooks/%llu", (unsigned long long)id
    );
    return status == DCC_OK
        ? dcc_endpoint_submit_legacy_raw(
            client, method, path, NULL,
            body != NULL ? "application/json" : NULL,
            body, body != NULL ? strlen(body) : 0U, cb, user_data
        )
        : status;
}

dcc_status_t dcc_app_get_channel_webhooks(
    dcc_app_t *app, dcc_snowflake_t channel_id, dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || channel_id == 0U) return DCC_ERR_INVALID_ARG;
    DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_channel_webhooks,
        dcc_app_client(app), channel_id);
}

dcc_status_t dcc_app_get_guild_webhooks(
    dcc_app_t *app, dcc_snowflake_t guild_id, dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || guild_id == 0U) return DCC_ERR_INVALID_ARG;
    DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_guild_webhooks,
        dcc_app_client(app), guild_id);
}

dcc_status_t dcc_app_get_webhook(
    dcc_app_t *app, dcc_snowflake_t webhook_id, dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || webhook_id == 0U) return DCC_ERR_INVALID_ARG;
    DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_webhook,
        dcc_app_client(app), webhook_id, NULL);
}

dcc_status_t dcc_app_get_webhook_with_token(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || webhook_id == 0U || dcc_app_webhook_token_invalid(webhook_token))
        return DCC_ERR_INVALID_ARG;
    DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_webhook,
        dcc_app_client(app), webhook_id, webhook_token);
}

dcc_status_t dcc_app_create_webhook(
    dcc_app_t *app, dcc_snowflake_t channel_id, const char *json_body,
    dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || channel_id == 0U || json_body == NULL) return DCC_ERR_INVALID_ARG;
    char path[88];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/channels/%llu/webhooks",
        (unsigned long long)channel_id
    );
    return status == DCC_OK
        ? dcc_endpoint_submit_legacy_raw(
            dcc_app_client(app), DCC_REST_POST, path, NULL,
            "application/json", json_body, strlen(json_body), cb, user_data
        )
        : status;
}

dcc_status_t dcc_app_create_webhook_params(
    dcc_app_t *app, dcc_snowflake_t channel_id,
    const dcc_rest_webhook_builder_t *builder,
    dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || channel_id == 0U || builder == NULL)
        return DCC_ERR_INVALID_ARG;
    DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_create_webhook,
        dcc_app_client(app), channel_id, builder);
}

dcc_status_t dcc_app_modify_webhook(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *json_body,
    dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || webhook_id == 0U || json_body == NULL) return DCC_ERR_INVALID_ARG;
    return dcc_app_webhook_management_raw(
        dcc_app_client(app), DCC_REST_PATCH, webhook_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_webhook_params(
    dcc_app_t *app, dcc_snowflake_t webhook_id,
    const dcc_rest_webhook_builder_t *builder,
    dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || webhook_id == 0U || builder == NULL)
        return DCC_ERR_INVALID_ARG;
    DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_modify_webhook,
        dcc_app_client(app), webhook_id, NULL, builder);
}

dcc_status_t dcc_app_modify_webhook_with_token(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    const char *json_body, dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || webhook_id == 0U || dcc_app_webhook_token_invalid(webhook_token) ||
        json_body == NULL) return DCC_ERR_INVALID_ARG;
    return dcc_app_webhook_raw(dcc_app_client(app), DCC_REST_PATCH, webhook_id,
        webhook_token, NULL, 0U, NULL, json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_webhook_with_token_params(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    const dcc_rest_webhook_builder_t *builder,
    dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || webhook_id == 0U ||
        dcc_app_webhook_token_invalid(webhook_token) || builder == NULL)
        return DCC_ERR_INVALID_ARG;
    DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_modify_webhook,
        dcc_app_client(app), webhook_id, webhook_token, builder);
}

dcc_status_t dcc_app_delete_webhook(
    dcc_app_t *app, dcc_snowflake_t webhook_id, dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || webhook_id == 0U) return DCC_ERR_INVALID_ARG;
    DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_delete_webhook,
        dcc_app_client(app), webhook_id, NULL);
}

dcc_status_t dcc_app_delete_webhook_with_token(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || webhook_id == 0U || dcc_app_webhook_token_invalid(webhook_token))
        return DCC_ERR_INVALID_ARG;
    DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_delete_webhook,
        dcc_app_client(app), webhook_id, webhook_token);
}

dcc_status_t dcc_app_execute_webhook(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    const char *query, const char *json_body, dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || webhook_id == 0U || dcc_app_webhook_token_invalid(webhook_token) ||
        json_body == NULL) return DCC_ERR_INVALID_ARG;
    return dcc_app_webhook_raw(dcc_app_client(app), DCC_REST_POST, webhook_id,
        webhook_token, NULL, 0U, query, json_body, cb, user_data);
}

dcc_status_t dcc_app_execute_webhook_options(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    uint8_t wait, dcc_snowflake_t thread_id, uint8_t with_components,
    const char *json_body, dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || webhook_id == 0U || dcc_app_webhook_token_invalid(webhook_token) ||
        json_body == NULL) return DCC_ERR_INVALID_ARG;
    dcc_rest_buffer_t query = {0};
    dcc_status_t status = dcc_rest_webhook_options_query(
        &query, wait, thread_id, with_components);
    if (status == DCC_OK) status = dcc_app_execute_webhook(
        app, webhook_id, webhook_token, query.data, json_body, cb, user_data);
    dcc_rest_buffer_deinit(&query);
    return status;
}

dcc_status_t dcc_app_execute_webhook_message(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    uint8_t wait, dcc_snowflake_t thread_id, uint8_t with_components,
    const dcc_message_builder_t *message, dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || message == NULL || wait > 1U || with_components > 1U)
        return DCC_ERR_INVALID_ARG;
    dcc_rest_webhook_execute_t execute = DCC_REST_WEBHOOK_EXECUTE_INIT;
    dcc_rest_webhook_execute_init(&execute, message);
    execute.present = DCC_REST_WEBHOOK_EXECUTE_PRESENT_WAIT |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_WITH_COMPONENTS;
    execute.wait = wait;
    execute.with_components = with_components;
    if (thread_id != 0U) {
        execute.present |= DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_ID;
        execute.thread_id = thread_id;
    }
    DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_execute_webhook,
        dcc_app_client(app), webhook_id, webhook_token, &execute);
}

dcc_status_t dcc_app_execute_webhook_text(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    uint8_t wait, dcc_snowflake_t thread_id, const char *content,
    dcc_rest_cb cb, void *user_data
) {
    if (content == NULL) return DCC_ERR_INVALID_ARG;
    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    dcc_status_t status = dcc_message_builder_set_content(&message, content);
    return status == DCC_OK
        ? dcc_app_execute_webhook_message(app, webhook_id, webhook_token, wait,
            thread_id, 0U, &message, cb, user_data)
        : status;
}

dcc_status_t dcc_app_execute_webhook_multipart(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    const char *query, const char *payload_json,
    const dcc_rest_multipart_file_t *files, size_t file_count,
    dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || webhook_id == 0U || dcc_app_webhook_token_invalid(webhook_token) ||
        payload_json == NULL || (file_count != 0U && files == NULL))
        return DCC_ERR_INVALID_ARG;
    return dcc_app_webhook_raw_multipart(dcc_app_client(app), DCC_REST_POST,
        webhook_id, webhook_token, NULL, 0U, query, payload_json, files,
        file_count, cb, user_data);
}

dcc_status_t dcc_app_execute_webhook_multipart_options(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    uint8_t wait, dcc_snowflake_t thread_id, uint8_t with_components,
    const char *payload_json, const dcc_rest_multipart_file_t *files,
    size_t file_count, dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || payload_json == NULL) return DCC_ERR_INVALID_ARG;
    dcc_rest_buffer_t query = {0};
    dcc_status_t status = dcc_rest_webhook_options_query(
        &query, wait, thread_id, with_components);
    if (status == DCC_OK) status = dcc_app_execute_webhook_multipart(
        app, webhook_id, webhook_token, query.data, payload_json, files,
        file_count, cb, user_data);
    dcc_rest_buffer_deinit(&query);
    return status;
}

dcc_status_t dcc_app_get_webhook_message(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    dcc_snowflake_t message_id, const char *query,
    dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || webhook_id == 0U || message_id == 0U ||
        dcc_app_webhook_token_invalid(webhook_token)) return DCC_ERR_INVALID_ARG;
    return dcc_app_webhook_raw(dcc_app_client(app), DCC_REST_GET, webhook_id,
        webhook_token, "messages", message_id, query, NULL, cb, user_data);
}

dcc_status_t dcc_app_get_webhook_message_thread(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    dcc_snowflake_t message_id, dcc_snowflake_t thread_id,
    dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || thread_id == 0U) return DCC_ERR_INVALID_ARG;
    dcc_rest_webhook_message_query_t query = DCC_REST_WEBHOOK_MESSAGE_QUERY_INIT;
    query.present = DCC_REST_WEBHOOK_MESSAGE_QUERY_PRESENT_THREAD_ID;
    query.thread_id = thread_id;
    DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_webhook_message,
        dcc_app_client(app), webhook_id, webhook_token, message_id, &query);
}

dcc_status_t dcc_app_modify_webhook_message(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    dcc_snowflake_t message_id, const char *query, const char *json_body,
    dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || webhook_id == 0U || message_id == 0U || json_body == NULL ||
        dcc_app_webhook_token_invalid(webhook_token)) return DCC_ERR_INVALID_ARG;
    return dcc_app_webhook_raw(dcc_app_client(app), DCC_REST_PATCH, webhook_id,
        webhook_token, "messages", message_id, query, json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_webhook_message_thread(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    dcc_snowflake_t message_id, dcc_snowflake_t thread_id,
    const char *json_body, dcc_rest_cb cb, void *user_data
) {
    if (thread_id == 0U) return DCC_ERR_INVALID_ARG;
    dcc_rest_buffer_t query = {0};
    dcc_status_t status = dcc_rest_query_append_u64_value(&query, "thread_id", thread_id);
    if (status == DCC_OK) status = dcc_app_modify_webhook_message(
        app, webhook_id, webhook_token, message_id, query.data, json_body,
        cb, user_data);
    dcc_rest_buffer_deinit(&query);
    return status;
}

dcc_status_t dcc_app_modify_webhook_message_builder(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    dcc_snowflake_t message_id, const char *query,
    const dcc_message_builder_t *message, dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || message == NULL) return DCC_ERR_INVALID_ARG;
    char *json = NULL;
    dcc_status_t status = dcc_message_builder_build_json(message, &json);
    if (status == DCC_OK) status = dcc_app_modify_webhook_message(
        app, webhook_id, webhook_token, message_id, query, json, cb, user_data);
    dcc_message_builder_json_free(json);
    return status;
}

dcc_status_t dcc_app_modify_webhook_message_builder_thread(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    dcc_snowflake_t message_id, dcc_snowflake_t thread_id,
    const dcc_message_builder_t *message, dcc_rest_cb cb, void *user_data
) {
    if (message == NULL) return DCC_ERR_INVALID_ARG;
    char *json = NULL;
    dcc_status_t status = dcc_message_builder_build_json(message, &json);
    if (status == DCC_OK) status = dcc_app_modify_webhook_message_thread(
        app, webhook_id, webhook_token, message_id, thread_id, json,
        cb, user_data);
    dcc_message_builder_json_free(json);
    return status;
}

dcc_status_t dcc_app_modify_webhook_message_multipart(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    dcc_snowflake_t message_id, const char *query, const char *payload_json,
    const dcc_rest_multipart_file_t *files, size_t file_count,
    dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || webhook_id == 0U || message_id == 0U || payload_json == NULL ||
        dcc_app_webhook_token_invalid(webhook_token) ||
        (file_count != 0U && files == NULL)) return DCC_ERR_INVALID_ARG;
    return dcc_app_webhook_raw_multipart(dcc_app_client(app), DCC_REST_PATCH,
        webhook_id, webhook_token, "messages", message_id, query, payload_json,
        files, file_count, cb, user_data);
}

dcc_status_t dcc_app_modify_webhook_message_multipart_thread(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    dcc_snowflake_t message_id, dcc_snowflake_t thread_id,
    const char *payload_json, const dcc_rest_multipart_file_t *files,
    size_t file_count, dcc_rest_cb cb, void *user_data
) {
    if (thread_id == 0U) return DCC_ERR_INVALID_ARG;
    dcc_rest_buffer_t query = {0};
    dcc_status_t status = dcc_rest_query_append_u64_value(&query, "thread_id", thread_id);
    if (status == DCC_OK) status = dcc_app_modify_webhook_message_multipart(
        app, webhook_id, webhook_token, message_id, query.data, payload_json,
        files, file_count, cb, user_data);
    dcc_rest_buffer_deinit(&query);
    return status;
}

dcc_status_t dcc_app_delete_webhook_message(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    dcc_snowflake_t message_id, const char *query,
    dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || webhook_id == 0U || message_id == 0U ||
        dcc_app_webhook_token_invalid(webhook_token)) return DCC_ERR_INVALID_ARG;
    return dcc_app_webhook_raw(dcc_app_client(app), DCC_REST_DELETE, webhook_id,
        webhook_token, "messages", message_id, query, NULL, cb, user_data);
}

dcc_status_t dcc_app_delete_webhook_message_thread(
    dcc_app_t *app, dcc_snowflake_t webhook_id, const char *webhook_token,
    dcc_snowflake_t message_id, dcc_snowflake_t thread_id,
    dcc_rest_cb cb, void *user_data
) {
    if (app == NULL || thread_id == 0U) return DCC_ERR_INVALID_ARG;
    dcc_rest_webhook_message_query_t query = DCC_REST_WEBHOOK_MESSAGE_QUERY_INIT;
    query.present = DCC_REST_WEBHOOK_MESSAGE_QUERY_PRESENT_THREAD_ID;
    query.thread_id = thread_id;
    DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_delete_webhook_message,
        dcc_app_client(app), webhook_id, webhook_token, message_id, &query);
}
