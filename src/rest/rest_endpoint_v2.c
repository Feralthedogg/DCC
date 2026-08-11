#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_multipart_build_internal.h"
#include "internal/rest/dcc_rest_multipart_internal.h"

#include <stdlib.h>
#include <string.h>

#define DCC_ENDPOINT_MAX_FILES 10U
#define DCC_ENDPOINT_MAX_APPLIED_TAGS 5U

typedef struct dcc_endpoint_legacy_bridge {
    dcc_rest_cb callback;
    void *user_data;
} dcc_endpoint_legacy_bridge_t;

static void dcc_endpoint_legacy_result(
    dcc_client_t *client,
    const dcc_rest_result_t *result,
    void *user_data
) {
    dcc_endpoint_legacy_bridge_t *bridge =
        (dcc_endpoint_legacy_bridge_t *)user_data;
    dcc_rest_response_t response = {
        .size = sizeof(response),
        .status = result != NULL ? result->http_status : 0U,
        .error = result != NULL ? dcc_rest_result_status(result) : DCC_ERR_RUNTIME,
        .body = result != NULL ? result->body : NULL,
        .body_len = result != NULL ? result->body_len : 0U,
    };
    if (bridge != NULL && bridge->callback != NULL) {
        bridge->callback(client, &response, bridge->user_data);
    }
    free(bridge);
}

static int dcc_endpoint_options_valid(const dcc_rest_call_options_t *options) {
    return options != NULL && options->size >= sizeof(*options) &&
        options->version == DCC_REST_CALL_OPTIONS_VERSION &&
        options->priority >= DCC_REST_PRIORITY_LOW &&
        options->priority <= DCC_REST_PRIORITY_HIGH &&
        (options->callback != NULL || options->user_data == NULL);
}

static int dcc_endpoint_current_record(
    size_t size,
    uint32_t version,
    size_t current_size,
    uint32_t current_version
) {
    return size >= current_size && version == current_version;
}

dcc_status_t dcc_endpoint_prepare(
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request,
    dcc_rest_call_options_t *out_options
) {
    if (out_request != NULL) {
        *out_request = NULL;
    }
    if (out_options == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (options == NULL) {
        *out_options = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
        return DCC_OK;
    }
    if (!dcc_endpoint_options_valid(options)) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_options = *options;
    return DCC_OK;
}

dcc_status_t dcc_endpoint_submit(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const dcc_endpoint_body_t *body,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_request_desc_t description = DCC_REST_REQUEST_DESC_INIT;
    description.method = method;
    description.path = path;
    if (body != NULL) {
        description.content_type = body->content_type;
        description.body = body->data;
        description.body_len = body->len;
    }
    description.options = *options;
    return dcc_rest_submit(client, &description, out_request);
}

dcc_status_t dcc_endpoint_path_with_query(
    const char *path,
    const dcc_rest_buffer_t *query,
    char **out_path
) {
    if (out_path == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_path = NULL;
    if (path == NULL || path[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    size_t path_len = strlen(path);
    size_t query_len = query != NULL ? query->len : 0U;
    if (path_len > SIZE_MAX - query_len - 2U) {
        return DCC_ERR_NOMEM;
    }
    char *combined = (char *)malloc(path_len + (query_len != 0U ? query_len + 1U : 0U) + 1U);
    if (combined == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(combined, path, path_len);
    size_t used = path_len;
    if (query_len != 0U) {
        combined[used++] = '?';
        memcpy(combined + used, query->data, query_len);
        used += query_len;
    }
    combined[used] = '\0';
    *out_path = combined;
    return DCC_OK;
}

static dcc_status_t dcc_endpoint_body_from_json_files(
    char *json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_endpoint_body_t *out_body
) {
    if (out_body == NULL || json == NULL || file_count > DCC_ENDPOINT_MAX_FILES ||
        (file_count != 0U && files == NULL)) {
        free(json);
        return DCC_ERR_INVALID_ARG;
    }
    *out_body = (dcc_endpoint_body_t){0};
    if (file_count == 0U) {
        out_body->data = json;
        out_body->len = strlen(json);
        out_body->content_type = "application/json";
        return DCC_OK;
    }
    dcc_rest_multipart_field_t field = {"payload_json", json};
    char *multipart = NULL;
    size_t multipart_len = 0U;
    dcc_status_t status = dcc_rest_build_multipart_body(
        &field, 1U, files, file_count, &multipart, &multipart_len
    );
    free(json);
    if (status != DCC_OK) {
        return status;
    }
    out_body->data = multipart;
    out_body->len = multipart_len;
    out_body->content_type = DCC_REST_MULTIPART_CONTENT_TYPE;
    return DCC_OK;
}

dcc_status_t dcc_endpoint_build_message_body(
    const dcc_rest_message_payload_t *payload,
    dcc_endpoint_body_t *out_body
) {
    if (out_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_body = (dcc_endpoint_body_t){0};
    const size_t historical_size = offsetof(dcc_rest_message_payload_t, files);
    if (payload == NULL || payload->version != DCC_REST_MESSAGE_PAYLOAD_VERSION ||
        (payload->size != historical_size && payload->size < sizeof(*payload))) {
        return DCC_ERR_INVALID_ARG;
    }
    const int has_file_fields = payload->size >= sizeof(*payload);
    const dcc_rest_multipart_file_t *files = has_file_fields ? payload->files : NULL;
    size_t file_count = has_file_fields ? payload->file_count : 0U;
    if (file_count > DCC_ENDPOINT_MAX_FILES || (file_count != 0U && files == NULL) ||
        (payload->message == NULL && file_count == 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (file_count != 0U &&
        dcc_rest_multipart_validate(NULL, 0U, files, file_count) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    char *json = NULL;
    dcc_status_t status = payload->message != NULL
        ? dcc_message_builder_build_json(payload->message, &json)
        : DCC_OK;
    if (status == DCC_OK && json == NULL) {
        json = (char *)malloc(3U);
        if (json == NULL) {
            status = DCC_ERR_NOMEM;
        } else {
            memcpy(json, "{}", 3U);
        }
    }
    return status == DCC_OK
        ? dcc_endpoint_body_from_json_files(json, files, file_count, out_body)
        : status;
}

static dcc_status_t dcc_endpoint_interaction_message_json(
    dcc_interaction_response_type_t type,
    const dcc_message_builder_t *message,
    char **out_json
) {
    char *message_json = NULL;
    dcc_status_t status = dcc_message_builder_build_json(message, &message_json);
    if (status != DCC_OK) {
        return status;
    }
    dcc_rest_buffer_t body = {0};
    status = dcc_rest_buffer_append_cstr(&body, "{\"type\":");
    if (status == DCC_OK) status = dcc_rest_buffer_append_u64_text(&body, (uint64_t)type);
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&body, ",\"data\":");
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&body, message_json);
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&body, "}");
    free(message_json);
    if (status != DCC_OK) {
        dcc_rest_buffer_deinit(&body);
        return status;
    }
    *out_json = body.data;
    return DCC_OK;
}

static dcc_status_t dcc_endpoint_interaction_type_json(
    dcc_interaction_response_type_t type,
    char **out_json
) {
    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{\"type\":");
    if (status == DCC_OK) status = dcc_rest_buffer_append_u64_text(&body, (uint64_t)type);
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&body, "}");
    if (status != DCC_OK) {
        dcc_rest_buffer_deinit(&body);
        return status;
    }
    *out_json = body.data;
    return DCC_OK;
}

dcc_status_t dcc_endpoint_build_interaction_body(
    const dcc_rest_interaction_response_t *response,
    dcc_endpoint_body_t *out_body
) {
    const uint64_t data_mask = DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE |
        DCC_REST_INTERACTION_RESPONSE_PRESENT_MODAL |
        DCC_REST_INTERACTION_RESPONSE_PRESENT_AUTOCOMPLETE;
    const uint64_t known_mask = data_mask |
        DCC_REST_INTERACTION_RESPONSE_PRESENT_WITH_RESPONSE;
    if (out_body == NULL || response == NULL ||
        !dcc_endpoint_current_record(response->size, response->version,
            sizeof(*response), DCC_REST_INTERACTION_RESPONSE_VERSION) ||
        (response->present & ~known_mask) != 0U ||
        response->file_count > DCC_ENDPOINT_MAX_FILES ||
        (response->file_count != 0U && response->files == NULL) ||
        ((response->present & DCC_REST_INTERACTION_RESPONSE_PRESENT_WITH_RESPONSE) != 0U &&
            response->with_response > 1U)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (response->file_count != 0U &&
        dcc_rest_multipart_validate(
            NULL, 0U, response->files, response->file_count
        ) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_body = (dcc_endpoint_body_t){0};
    uint64_t expected_data = 0U;
    int files_allowed = 0;
    char *json = NULL;
    dcc_status_t status = DCC_OK;
    switch (response->type) {
        case DCC_INTERACTION_RESPONSE_PONG:
        case DCC_INTERACTION_RESPONSE_DEFERRED_UPDATE_MESSAGE:
        case DCC_INTERACTION_RESPONSE_PREMIUM_REQUIRED:
        case DCC_INTERACTION_RESPONSE_LAUNCH_ACTIVITY:
            status = dcc_endpoint_interaction_type_json(response->type, &json);
            break;
        case DCC_INTERACTION_RESPONSE_CHANNEL_MESSAGE_WITH_SOURCE:
        case DCC_INTERACTION_RESPONSE_UPDATE_MESSAGE:
            expected_data = DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE;
            files_allowed = 1;
            if (response->data.message == NULL) return DCC_ERR_INVALID_ARG;
            status = dcc_endpoint_interaction_message_json(
                response->type, response->data.message, &json
            );
            break;
        case DCC_INTERACTION_RESPONSE_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE:
            expected_data = response->present & DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE;
            if (expected_data != 0U) {
                const dcc_message_builder_t *message = response->data.message;
                if (message == NULL || message->size < sizeof(*message) ||
                    message->version != DCC_MESSAGE_BUILDER_VERSION ||
                    message->present != DCC_MESSAGE_BUILDER_PRESENT_FLAGS ||
                    (message->flags & ~DCC_MESSAGE_FLAG_EPHEMERAL) != 0U) {
                    return DCC_ERR_INVALID_ARG;
                }
                status = dcc_endpoint_interaction_message_json(
                    response->type, message, &json
                );
            } else {
                status = dcc_endpoint_interaction_type_json(response->type, &json);
            }
            break;
        case DCC_INTERACTION_RESPONSE_AUTOCOMPLETE:
            expected_data = DCC_REST_INTERACTION_RESPONSE_PRESENT_AUTOCOMPLETE;
            if (response->data.autocomplete == NULL) return DCC_ERR_INVALID_ARG;
            status = dcc_autocomplete_builder_build_json(
                response->data.autocomplete, &json
            );
            break;
        case DCC_INTERACTION_RESPONSE_MODAL:
            expected_data = DCC_REST_INTERACTION_RESPONSE_PRESENT_MODAL;
            if (response->data.modal == NULL) return DCC_ERR_INVALID_ARG;
            status = dcc_modal_builder_build_json(response->data.modal, &json);
            break;
        default:
            return DCC_ERR_INVALID_ARG;
    }
    if ((response->present & data_mask) != expected_data ||
        (!files_allowed && response->file_count != 0U)) {
        free(json);
        return DCC_ERR_INVALID_ARG;
    }
    return status == DCC_OK
        ? dcc_endpoint_body_from_json_files(
            json, response->files, response->file_count, out_body
        )
        : status;
}

dcc_status_t dcc_endpoint_build_webhook_builder_body(
    const dcc_rest_webhook_builder_t *builder,
    int create,
    int token_authenticated,
    dcc_endpoint_body_t *out_body
) {
    const uint64_t known = DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME |
        DCC_REST_WEBHOOK_BUILDER_PRESENT_AVATAR |
        DCC_REST_WEBHOOK_BUILDER_PRESENT_CHANNEL_ID;
    if (out_body == NULL || builder == NULL ||
        !dcc_endpoint_current_record(builder->size, builder->version,
            sizeof(*builder), DCC_REST_WEBHOOK_BUILDER_VERSION) ||
        (builder->present & ~known) != 0U || builder->present == 0U ||
        (create && (builder->present & DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME) == 0U) ||
        ((create || token_authenticated) &&
            (builder->present & DCC_REST_WEBHOOK_BUILDER_PRESENT_CHANNEL_ID) != 0U) ||
        ((builder->present & DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME) != 0U &&
            (builder->name == NULL || builder->name[0] == '\0')) ||
        ((builder->present & DCC_REST_WEBHOOK_BUILDER_PRESENT_CHANNEL_ID) != 0U &&
            builder->channel_id == 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_rest_buffer_t json = {0};
    int first = 1;
    dcc_status_t status = dcc_rest_buffer_append_cstr(&json, "{");
    if (status == DCC_OK && (builder->present & DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME) != 0U) {
        status = dcc_rest_json_append_string_member(&json, &first, "name", builder->name);
    }
    if (status == DCC_OK && (builder->present & DCC_REST_WEBHOOK_BUILDER_PRESENT_AVATAR) != 0U) {
        status = dcc_rest_json_append_nullable_string_member(&json, &first, "avatar", builder->avatar);
    }
    if (status == DCC_OK && (builder->present & DCC_REST_WEBHOOK_BUILDER_PRESENT_CHANNEL_ID) != 0U) {
        status = dcc_rest_json_append_u64_string_member(
            &json, &first, "channel_id", builder->channel_id
        );
    }
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&json, "}");
    if (status != DCC_OK) {
        dcc_rest_buffer_deinit(&json);
        return status;
    }
    *out_body = (dcc_endpoint_body_t){json.data, json.len, "application/json"};
    return DCC_OK;
}

dcc_status_t dcc_endpoint_build_webhook_execute_body(
    const dcc_rest_webhook_execute_t *execute,
    dcc_endpoint_body_t *out_body
) {
    const uint64_t known = DCC_REST_WEBHOOK_EXECUTE_PRESENT_USERNAME |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_AVATAR_URL |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_NAME |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_APPLIED_TAG_IDS |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_WAIT |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_ID |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_WITH_COMPONENTS;
    if (out_body == NULL || execute == NULL ||
        !dcc_endpoint_current_record(execute->size, execute->version,
            sizeof(*execute), DCC_REST_WEBHOOK_EXECUTE_VERSION) ||
        (execute->present & ~known) != 0U || execute->file_count > DCC_ENDPOINT_MAX_FILES ||
        (execute->file_count != 0U && execute->files == NULL) ||
        ((execute->present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_WAIT) != 0U && execute->wait > 1U) ||
        ((execute->present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_WITH_COMPONENTS) != 0U && execute->with_components > 1U) ||
        ((execute->present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_ID) != 0U && execute->thread_id == 0U) ||
        ((execute->present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_NAME) != 0U &&
            (execute->thread_name == NULL || execute->thread_name[0] == '\0')) ||
        ((execute->present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_NAME) != 0U &&
            (execute->present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_ID) != 0U) ||
        ((execute->present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_USERNAME) != 0U && execute->username == NULL) ||
        ((execute->present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_AVATAR_URL) != 0U && execute->avatar_url == NULL) ||
        ((execute->present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_APPLIED_TAG_IDS) != 0U &&
            (execute->applied_tag_count > DCC_ENDPOINT_MAX_APPLIED_TAGS ||
             (execute->applied_tag_count != 0U && execute->applied_tag_ids == NULL)))) {
        return DCC_ERR_INVALID_ARG;
    }
    if ((execute->present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_APPLIED_TAG_IDS) != 0U) {
        for (size_t i = 0U; i < execute->applied_tag_count; ++i) {
            if (execute->applied_tag_ids[i] == 0U) return DCC_ERR_INVALID_ARG;
        }
    }
    if (execute->file_count != 0U &&
        dcc_rest_multipart_validate(
            NULL, 0U, execute->files, execute->file_count
        ) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    char *message_json = NULL;
    dcc_status_t status = execute->message != NULL
        ? dcc_message_builder_build_json(execute->message, &message_json)
        : DCC_OK;
    if (status != DCC_OK) return status;
    dcc_rest_buffer_t json = {0};
    int first = 1;
    status = dcc_rest_buffer_append_cstr(&json, "{");
    if (status == DCC_OK && message_json != NULL) {
        size_t length = strlen(message_json);
        if (length < 2U || message_json[0] != '{' || message_json[length - 1U] != '}') {
            status = DCC_ERR_INVALID_ARG;
        } else if (length > 2U) {
            status = dcc_rest_buffer_append(&json, message_json + 1U, length - 2U);
            first = 0;
        }
    }
    free(message_json);
    if (status == DCC_OK && (execute->present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_USERNAME) != 0U) {
        status = dcc_rest_json_append_string_member(&json, &first, "username", execute->username);
    }
    if (status == DCC_OK && (execute->present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_AVATAR_URL) != 0U) {
        status = dcc_rest_json_append_string_member(&json, &first, "avatar_url", execute->avatar_url);
    }
    if (status == DCC_OK && (execute->present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_NAME) != 0U) {
        status = dcc_rest_json_append_string_member(&json, &first, "thread_name", execute->thread_name);
    }
    if (status == DCC_OK && (execute->present & DCC_REST_WEBHOOK_EXECUTE_PRESENT_APPLIED_TAG_IDS) != 0U) {
        status = dcc_rest_json_append_snowflake_string_array_member(
            &json, &first, "applied_tags", execute->applied_tag_ids,
            execute->applied_tag_count
        );
    }
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&json, "}");
    if (status != DCC_OK) {
        dcc_rest_buffer_deinit(&json);
        return status;
    }
    return dcc_endpoint_body_from_json_files(
        json.data, execute->files, execute->file_count, out_body
    );
}

void dcc_endpoint_body_deinit(dcc_endpoint_body_t *body) {
    if (body != NULL) {
        free(body->data);
        *body = (dcc_endpoint_body_t){0};
    }
}

dcc_status_t dcc_endpoint_legacy_options(
    dcc_rest_cb callback,
    void *user_data,
    dcc_rest_call_options_t *out_options,
    void **out_bridge
) {
    if (out_options == NULL || out_bridge == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_options = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
    *out_bridge = NULL;
    if (callback == NULL) {
        return DCC_OK;
    }
    dcc_endpoint_legacy_bridge_t *bridge =
        (dcc_endpoint_legacy_bridge_t *)malloc(sizeof(*bridge));
    if (bridge == NULL) {
        return DCC_ERR_NOMEM;
    }
    bridge->callback = callback;
    bridge->user_data = user_data;
    out_options->callback = dcc_endpoint_legacy_result;
    out_options->user_data = bridge;
    *out_bridge = bridge;
    return DCC_OK;
}

void dcc_endpoint_legacy_bridge_release(void *bridge) {
    free(bridge);
}
