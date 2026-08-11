#include "rest_v2_endpoint_smoke_support.h"

#if !defined(_WIN32)

#include <dcc/message.h>
#include <dcc/rest/interactions.h>
#include <dcc/rest/messages.h>
#include <dcc/rest/webhooks.h>

#include "internal/objects/dcc_message_builder_serialize_internal.h"
#include "internal/objects/dcc_message_json_buffer_internal.h"
#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_multipart_build_internal.h"
#include "internal/rest/dcc_rest_multipart_internal.h"

#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENDPOINT_TEST_ALIGN_UP(value_, alignment_) \
    (((value_) + (alignment_) - 1U) / (alignment_) * (alignment_))
#define ENDPOINT_TEST_EMBED_HISTORICAL_STRIDE \
    ENDPOINT_TEST_ALIGN_UP( \
        offsetof(dcc_embed_builder_t, url), \
        _Alignof(dcc_embed_builder_t) \
    )

typedef union endpoint_historical_embed_storage {
    max_align_t alignment;
    unsigned char bytes[ENDPOINT_TEST_EMBED_HISTORICAL_STRIDE * 2U];
} endpoint_historical_embed_storage_t;

#define ENDPOINT_CHECK_PREFLIGHT_REJECTION( \
    label_, capture_, callback_, observer_, probe_limit_, failure_, call_ \
) \
    do { \
        unsigned capture_calls_before = atomic_load_explicit( \
            &(capture_)->calls, \
            memory_order_acquire \
        ); \
        unsigned callback_calls_before = atomic_load_explicit( \
            &(callback_)->calls, \
            memory_order_acquire \
        ); \
        unsigned observer_calls_before = atomic_load_explicit( \
            &(observer_)->calls, \
            memory_order_acquire \
        ); \
        dcc_rest_request_t *request = (dcc_rest_request_t *)(uintptr_t)1U; \
        dcc_endpoint_test_allocation_probe_begin((probe_limit_)); \
        dcc_message_json_buffer_test_allocation_probe_begin(); \
        dcc_rest_buffer_test_allocation_probe_begin(); \
        dcc_status_t rejected_status = (call_); \
        size_t multipart_allocation_calls = \
            dcc_rest_buffer_test_allocation_probe_end(); \
        size_t json_allocation_calls = \
            dcc_message_json_buffer_test_allocation_probe_end(); \
        size_t endpoint_allocation_calls = \
            dcc_endpoint_test_allocation_probe_end(); \
        if (rejected_status != DCC_ERR_INVALID_ARG || request != NULL || \
            endpoint_allocation_calls != 0U || json_allocation_calls != 0U || \
            multipart_allocation_calls != 0U || \
            atomic_load_explicit(&(capture_)->calls, memory_order_acquire) != \
                capture_calls_before || \
            atomic_load_explicit(&(callback_)->calls, memory_order_acquire) != \
                callback_calls_before || \
            atomic_load_explicit(&(observer_)->calls, memory_order_acquire) != \
                observer_calls_before) { \
            fprintf( \
                stderr, \
                "%s rejection detail status=%s request=%p endpoint=%zu json=%zu " \
                "multipart=%zu capture=%u/%u callback=%u/%u observer=%u/%u\n", \
                (label_), \
                dcc_status_string(rejected_status), \
                (void *)request, \
                endpoint_allocation_calls, \
                json_allocation_calls, \
                multipart_allocation_calls, \
                capture_calls_before, \
                atomic_load_explicit(&(capture_)->calls, memory_order_acquire), \
                callback_calls_before, \
                atomic_load_explicit(&(callback_)->calls, memory_order_acquire), \
                observer_calls_before, \
                atomic_load_explicit(&(observer_)->calls, memory_order_acquire) \
            ); \
            failure_; \
        } \
    } while (0)

#define ENDPOINT_EXPECT_PREFLIGHT_REJECTION( \
    label_, capture_, callback_, observer_, call_ \
) \
    ENDPOINT_CHECK_PREFLIGHT_REJECTION( \
        (label_), \
        (capture_), \
        (callback_), \
        (observer_), \
        0U, \
        return 1, \
        (call_) \
    )

static int endpoint_message_serializer_rejects(
    const char *label,
    const dcc_message_builder_t *message
) {
    char *const canary = (char *)(uintptr_t)1U;
    char *json = canary;
    dcc_message_json_buffer_test_allocation_probe_begin();
    dcc_status_t status = dcc_message_builder_build_json(message, &json);
    size_t allocation_calls =
        dcc_message_json_buffer_test_allocation_probe_end();
    if (status != DCC_ERR_INVALID_ARG || json != NULL || allocation_calls != 0U) {
        fprintf(
            stderr,
            "%s serializer rejection detail status=%s json=%p allocations=%zu\n",
            label,
            dcc_status_string(status),
            (void *)json,
            allocation_calls
        );
        if (json != NULL && json != canary) {
            dcc_message_builder_json_free(json);
        }
        return 1;
    }
    return 0;
}

int endpoint_nested_message_preflight_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback,
    endpoint_observer_t *observer
) {
    endpoint_callback_reset(callback);
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    options.callback = endpoint_result_callback;
    options.user_data = callback;

    dcc_component_builder_t button;
    dcc_component_builder_init(&button, DCC_COMPONENT_BUTTON);
    if (dcc_component_builder_set_style(&button, DCC_BUTTON_PRIMARY) != DCC_OK ||
        dcc_component_builder_set_label(&button, "invalid root") != DCC_OK ||
        dcc_component_builder_set_custom_id(&button, "invalid-root") != DCC_OK) {
        return 1;
    }
    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_components(&message, &button, 1U) != DCC_OK) {
        return 1;
    }
    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&payload, &message);
    ENDPOINT_EXPECT_PREFLIGHT_REJECTION(
        "message rejects legacy component outside an action row",
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );
    if (endpoint_message_serializer_rejects("legacy component", &message) != 0) {
        return 1;
    }

    dcc_component_builder_t malformed_row;
    dcc_component_builder_init(&malformed_row, DCC_COMPONENT_ACTION_ROW);
    malformed_row.children =
        (const dcc_component_builder_t *)(uintptr_t)UINTPTR_MAX;
    malformed_row.children_count = 1U;
    message = (dcc_message_builder_t)DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_components(&message, &malformed_row, 1U) != DCC_OK) {
        return 1;
    }
    dcc_rest_message_payload_init(&payload, &message);
    ENDPOINT_EXPECT_PREFLIGHT_REJECTION(
        "message rejects overflowing legacy child span before allocation",
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );
    if (endpoint_message_serializer_rejects("legacy child span", &message) != 0) {
        return 1;
    }

    dcc_component_builder_t malformed_select;
    dcc_component_builder_init(&malformed_select, DCC_COMPONENT_STRING_SELECT);
    malformed_select.custom_id = "select";
    malformed_select.options =
        (const dcc_select_option_t *)(uintptr_t)UINTPTR_MAX;
    malformed_select.options_count = SIZE_MAX;
    dcc_component_builder_t select_row;
    dcc_component_builder_init(&select_row, DCC_COMPONENT_ACTION_ROW);
    select_row.children = &malformed_select;
    select_row.children_count = 1U;
    message = (dcc_message_builder_t)DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_components(&message, &select_row, 1U) != DCC_OK) {
        return 1;
    }
    dcc_rest_message_payload_init(&payload, &message);
    ENDPOINT_EXPECT_PREFLIGHT_REJECTION(
        "message rejects overflowing legacy option span before allocation",
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );
    if (endpoint_message_serializer_rejects("legacy option span", &message) != 0) {
        return 1;
    }

    dcc_embed_builder_t invalid_embed = DCC_EMBED_BUILDER_INIT;
    invalid_embed.present = DCC_EMBED_BUILDER_PRESENT_TITLE;
    message = (dcc_message_builder_t)DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_embeds(&message, &invalid_embed, 1U) != DCC_OK) {
        return 1;
    }
    dcc_rest_message_payload_init(&payload, &message);
    ENDPOINT_EXPECT_PREFLIGHT_REJECTION(
        "message rejects malformed structured embed before allocation",
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );
    if (endpoint_message_serializer_rejects("structured embed", &message) != 0) {
        return 1;
    }

    dcc_embed_builder_t valid_embed = DCC_EMBED_BUILDER_INIT;
    message = (dcc_message_builder_t)DCC_MESSAGE_BUILDER_INIT;
    message.present = DCC_MESSAGE_BUILDER_PRESENT_EMBEDS;
    message.embeds = &valid_embed;
    message.embeds_count = SIZE_MAX;
    dcc_rest_message_payload_init(&payload, &message);
    ENDPOINT_EXPECT_PREFLIGHT_REJECTION(
        "message rejects overflowing structured embed span before allocation",
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );
    if (endpoint_message_serializer_rejects("structured embed span", &message) != 0) {
        return 1;
    }

    dcc_component_v2_builder_t text_input;
    dcc_component_v2_builder_init(&text_input, DCC_COMPONENT_V2_TEXT_INPUT);
    dcc_component_v2_builder_t label;
    dcc_component_v2_builder_init(&label, DCC_COMPONENT_V2_LABEL);
    if (dcc_component_v2_builder_set_text_input_style(
            &text_input,
            DCC_TEXT_INPUT_SHORT
        ) != DCC_OK ||
        dcc_component_v2_builder_set_custom_id(&text_input, "modal-input") != DCC_OK ||
        dcc_component_v2_builder_set_label(&label, "Modal label") != DCC_OK ||
        dcc_component_v2_builder_set_children(&label, &text_input, 1U) != DCC_OK) {
        return 1;
    }
    message = (dcc_message_builder_t)DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_components_v2(&message, &label, 1U) != DCC_OK) {
        return 1;
    }
    dcc_rest_message_payload_init(&payload, &message);
    ENDPOINT_EXPECT_PREFLIGHT_REJECTION(
        "message rejects modal-only Components V2 layout before allocation",
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );
    return endpoint_message_serializer_rejects(
        "Components V2 message context",
        &message
    );
}

static int endpoint_generic_multipart_rejects(
    const char *label,
    const dcc_rest_multipart_field_t *fields,
    size_t field_count,
    const dcc_rest_multipart_file_t *files,
    size_t file_count
) {
    char *const body_canary = (char *)(uintptr_t)1U;
    char *body = body_canary;
    size_t body_len = 17U;
    dcc_rest_buffer_test_allocation_probe_begin();
    dcc_status_t status = dcc_rest_build_multipart_body(
        fields,
        field_count,
        files,
        file_count,
        &body,
        &body_len
    );
    size_t allocation_calls = dcc_rest_buffer_test_allocation_probe_end();
    if (status != DCC_ERR_INVALID_ARG || body != NULL || body_len != 0U ||
        allocation_calls != 0U) {
        fprintf(
            stderr,
            "%s detail status=%s body=%p length=%zu allocations=%zu\n",
            label,
            dcc_status_string(status),
            (void *)body,
            body_len,
            allocation_calls
        );
        if (body != NULL && body != body_canary) {
            free(body);
        }
        return 1;
    }
    return 0;
}

static dcc_embed_builder_t *endpoint_historical_embed_at(
    endpoint_historical_embed_storage_t *storage,
    size_t index
) {
    return (dcc_embed_builder_t *)(
        storage->bytes + ENDPOINT_TEST_EMBED_HISTORICAL_STRIDE * index
    );
}

static int endpoint_exact_json_measure_contract(void) {
    endpoint_historical_embed_storage_t embed_storage;
    memset(&embed_storage, 0, sizeof(embed_storage));
    for (size_t i = 0U; i < 2U; ++i) {
        dcc_embed_builder_t *embed = endpoint_historical_embed_at(
            &embed_storage,
            i
        );
        embed->size = ENDPOINT_TEST_EMBED_HISTORICAL_STRIDE;
        embed->version = DCC_EMBED_BUILDER_VERSION;
        embed->present = DCC_EMBED_BUILDER_PRESENT_TITLE |
            DCC_EMBED_BUILDER_PRESENT_DESCRIPTION;
        embed->title = i == 0U ? "historical-left" : "historical-right";
        embed->description = i == 0U ? "line\none" : "line \"two\"";
    }

    dcc_component_builder_t button;
    dcc_component_builder_init(&button, DCC_COMPONENT_BUTTON);
    dcc_component_builder_t row;
    dcc_component_builder_init(&row, DCC_COMPONENT_ACTION_ROW);
    if (dcc_component_builder_set_style(&button, DCC_BUTTON_PRIMARY) != DCC_OK ||
        dcc_component_builder_set_label(&button, "Measure") != DCC_OK ||
        dcc_component_builder_set_custom_id(&button, "measure.button") != DCC_OK ||
        dcc_component_builder_set_children(&row, &button, 1U) != DCC_OK) {
        return 1;
    }

    const dcc_poll_answer_t answer = {
        .media = { .text = "yes" },
    };
    dcc_poll_builder_t poll = DCC_POLL_BUILDER_INIT;
    poll.present = DCC_POLL_BUILDER_PRESENT_QUESTION |
        DCC_POLL_BUILDER_PRESENT_ANSWERS;
    poll.question.text = "ready?";
    poll.answers = &answer;
    poll.answer_count = 1U;

    const dcc_snowflake_t users[] = {42U};
    const dcc_snowflake_t roles[] = {84U};
    dcc_allowed_mentions_builder_t mentions = DCC_ALLOWED_MENTIONS_BUILDER_INIT;
    mentions.present = DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_USERS |
        DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_ROLES |
        DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_REPLIED_USER;
    mentions.users = users;
    mentions.user_count = sizeof(users) / sizeof(users[0]);
    mentions.roles = roles;
    mentions.role_count = sizeof(roles) / sizeof(roles[0]);
    mentions.replied_user = 1U;

    const dcc_snowflake_t stickers[] = {123U};
    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&message, "exact \"message\"\n") != DCC_OK ||
        dcc_message_builder_set_sticker_ids(
            &message,
            stickers,
            sizeof(stickers) / sizeof(stickers[0])
        ) != DCC_OK ||
        dcc_message_builder_set_message_reference_json(
            &message,
            " {\"message_id\":\"raw\"} "
        ) != DCC_OK ||
        dcc_message_builder_set_allowed_mentions(&message, &mentions) != DCC_OK ||
        dcc_message_builder_set_embeds(
            &message,
            endpoint_historical_embed_at(&embed_storage, 0U),
            2U
        ) != DCC_OK ||
        dcc_message_builder_set_components(&message, &row, 1U) != DCC_OK ||
        dcc_message_builder_set_attachments_json(
            &message,
            " [ {\"id\":\"raw-attachment\"} ] "
        ) != DCC_OK ||
        dcc_message_builder_set_poll(&message, &poll) != DCC_OK) {
        return 1;
    }

    dcc_component_v2_builder_t text_display;
    dcc_component_v2_builder_init(
        &text_display,
        DCC_COMPONENT_V2_TEXT_DISPLAY
    );
    dcc_message_builder_t v2_message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_component_v2_builder_set_content(
            &text_display,
            "v2 exact \"text\""
        ) != DCC_OK ||
        dcc_message_builder_set_components_v2(
            &v2_message,
            &text_display,
            1U
        ) != DCC_OK ||
        dcc_message_builder_set_attachments_json(
            &v2_message,
            "[{\"id\":\"v2-raw\"}]"
        ) != DCC_OK) {
        return 1;
    }

    dcc_message_builder_t raw_message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_message_reference_json(
            &raw_message,
            " {\"message_id\":\"raw-only\"} "
        ) != DCC_OK ||
        dcc_message_builder_set_allowed_mentions_json(
            &raw_message,
            " {\"parse\":[\"users\"]} "
        ) != DCC_OK ||
        dcc_message_builder_set_embeds_json(
            &raw_message,
            " [ {\"description\":\"raw embed\"} ] "
        ) != DCC_OK ||
        dcc_message_builder_set_components_json(
            &raw_message,
            " [ {\"type\":1,\"components\":[]} ] "
        ) != DCC_OK ||
        dcc_message_builder_set_attachments_json(
            &raw_message,
            " [ {\"id\":\"raw-only\"} ] "
        ) != DCC_OK ||
        dcc_message_builder_set_poll_json(
            &raw_message,
            " {\"question\":{\"text\":\"raw poll\"}} "
        ) != DCC_OK) {
        return 1;
    }

    dcc_rest_interaction_response_t response =
        DCC_REST_INTERACTION_RESPONSE_INIT;
    if (dcc_rest_interaction_response_set_message(&response, &message) != DCC_OK) {
        return 1;
    }

    const dcc_snowflake_t applied_tags[] = {55U, 66U};
    dcc_rest_webhook_execute_t execute = DCC_REST_WEBHOOK_EXECUTE_INIT;
    dcc_rest_webhook_execute_init(&execute, &v2_message);
    execute.present = DCC_REST_WEBHOOK_EXECUTE_PRESENT_USERNAME |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_AVATAR_URL |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_NAME |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_APPLIED_TAG_IDS |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_WAIT |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_WITH_COMPONENTS;
    execute.username = "webhook \"name\"";
    execute.avatar_url = "https://example.com/avatar.png";
    execute.thread_name = "thread\nname";
    execute.applied_tag_ids = applied_tags;
    execute.applied_tag_count =
        sizeof(applied_tags) / sizeof(applied_tags[0]);
    execute.wait = 1U;
    execute.with_components = 1U;

    dcc_endpoint_webhook_execute_view_t execute_view;
    if (dcc_endpoint_webhook_execute_preflight(&execute, &execute_view) != DCC_OK) {
        return 1;
    }

    size_t message_len = 0U;
    size_t raw_message_len = 0U;
    size_t interaction_len = 0U;
    size_t webhook_len = 0U;
    dcc_message_json_buffer_test_allocation_probe_begin();
    dcc_rest_buffer_test_allocation_probe_begin();
    dcc_status_t message_status = dcc_message_builder_measure_json(
        &message,
        &message_len
    );
    dcc_status_t raw_message_status = dcc_message_builder_measure_json(
        &raw_message,
        &raw_message_len
    );
    dcc_status_t interaction_status =
        dcc_endpoint_measure_interaction_message_json(
            response.type,
            response.data.message,
            &interaction_len
        );
    dcc_status_t webhook_status = dcc_endpoint_measure_webhook_execute_json(
        &execute_view,
        &webhook_len
    );
    size_t rest_allocations = dcc_rest_buffer_test_allocation_probe_end();
    size_t message_allocations =
        dcc_message_json_buffer_test_allocation_probe_end();
    if (message_status != DCC_OK || raw_message_status != DCC_OK ||
        interaction_status != DCC_OK ||
        webhook_status != DCC_OK || message_allocations != 0U ||
        rest_allocations != 0U) {
        fprintf(
            stderr,
            "exact JSON measure detail message=%s raw=%s interaction=%s webhook=%s "
            "allocations=%zu/%zu\n",
            dcc_status_string(message_status),
            dcc_status_string(raw_message_status),
            dcc_status_string(interaction_status),
            dcc_status_string(webhook_status),
            message_allocations,
            rest_allocations
        );
        return 1;
    }

    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&payload, &message);
    dcc_endpoint_body_t message_body = {0};
    dcc_endpoint_body_t interaction_body = {0};
    dcc_endpoint_body_t webhook_body = {0};
    char *raw_message_json = NULL;
    message_status = dcc_endpoint_build_message_body(&payload, &message_body);
    raw_message_status = dcc_message_builder_build_json(
        &raw_message,
        &raw_message_json
    );
    interaction_status = dcc_endpoint_build_interaction_body(
        &response,
        &interaction_body
    );
    webhook_status = dcc_endpoint_build_webhook_execute_body(
        &execute,
        &webhook_body
    );
    int matches = message_status == DCC_OK && raw_message_status == DCC_OK &&
        interaction_status == DCC_OK && webhook_status == DCC_OK &&
        message_len == message_body.len && raw_message_json != NULL &&
        raw_message_len == strlen(raw_message_json) &&
        interaction_len == interaction_body.len && webhook_len == webhook_body.len &&
        strstr(message_body.data, "historical-left") != NULL &&
        strstr(message_body.data, "raw-attachment") != NULL &&
        strstr(raw_message_json, "raw embed") != NULL &&
        strstr(raw_message_json, "raw poll") != NULL &&
        strstr(webhook_body.data, "v2-raw") != NULL &&
        strstr(webhook_body.data, "applied_tags") != NULL;
    if (!matches) {
        fprintf(
            stderr,
            "exact JSON build detail status=%s/%s/%s/%s "
            "length=%zu/%zu %zu/%zu %zu/%zu %zu/%zu\n",
            dcc_status_string(message_status),
            dcc_status_string(raw_message_status),
            dcc_status_string(interaction_status),
            dcc_status_string(webhook_status),
            message_len,
            message_body.len,
            raw_message_len,
            raw_message_json != NULL ? strlen(raw_message_json) : 0U,
            interaction_len,
            interaction_body.len,
            webhook_len,
            webhook_body.len
        );
    }
    dcc_endpoint_body_deinit(&message_body);
    dcc_endpoint_body_deinit(&interaction_body);
    dcc_endpoint_body_deinit(&webhook_body);
    dcc_message_builder_json_free(raw_message_json);
    return matches ? 0 : 1;
}

int endpoint_multipart_overflow_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback,
    endpoint_observer_t *observer
) {
    if (endpoint_exact_json_measure_contract() != 0) {
        return 1;
    }
    static const unsigned char one_byte[] = {0xa5U};
    dcc_rest_multipart_file_t huge_file = {
        "files[0]",
        "huge.bin",
        "application/octet-stream",
        one_byte,
        SIZE_MAX,
    };
    if (endpoint_generic_multipart_rejects(
            "generic multipart SIZE_MAX overflow",
            NULL,
            0U,
            &huge_file,
            1U
        ) != 0) {
        return 1;
    }

    dcc_rest_multipart_file_t near_file = huge_file;
    near_file.data_len = SIZE_MAX - 200U;
    size_t file_only_len = 0U;
    if (dcc_rest_multipart_measure(
            NULL,
            0U,
            &near_file,
            1U,
            &file_only_len
        ) != DCC_OK || file_only_len == 0U) {
        fprintf(stderr, "multipart boundary fixture is not file-only valid\n");
        return 1;
    }
    const dcc_rest_multipart_field_t payload_json = {
        "payload_json",
        "{}",
    };
    if (endpoint_generic_multipart_rejects(
            "generic multipart payload envelope overflow",
            &payload_json,
            1U,
            &near_file,
            1U
        ) != 0) {
        return 1;
    }

    endpoint_callback_reset(callback);
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    options.callback = endpoint_result_callback;
    options.user_data = callback;
    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&message, "multipart overflow") != DCC_OK) {
        return 1;
    }

    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&payload, &message);
    payload.files = &near_file;
    payload.file_count = 1U;
    ENDPOINT_EXPECT_PREFLIGHT_REJECTION(
        "message rejects mandatory multipart envelope before JSON serialization",
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );

    dcc_rest_interaction_response_t response =
        DCC_REST_INTERACTION_RESPONSE_INIT;
    if (dcc_rest_interaction_response_set_message(&response, &message) != DCC_OK) {
        return 1;
    }
    response.files = &near_file;
    response.file_count = 1U;
    ENDPOINT_EXPECT_PREFLIGHT_REJECTION(
        "interaction rejects mandatory multipart envelope before JSON serialization",
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client,
            124U,
            "token",
            &response,
            &options,
            &request
        )
    );

    dcc_rest_webhook_execute_t execute = DCC_REST_WEBHOOK_EXECUTE_INIT;
    dcc_rest_webhook_execute_init(&execute, &message);
    execute.files = &near_file;
    execute.file_count = 1U;
    ENDPOINT_EXPECT_PREFLIGHT_REJECTION(
        "webhook rejects mandatory multipart envelope before JSON serialization",
        capture,
        callback,
        observer,
        dcc_rest_execute_webhook(
            client,
            125U,
            "token",
            &execute,
            &options,
            &request
        )
    );

    dcc_rest_multipart_file_t exact_file = huge_file;
    exact_file.data_len = SIZE_MAX - 272U;
    size_t minimum_envelope_len = 0U;
    if (dcc_rest_multipart_measure(
            &payload_json,
            1U,
            &exact_file,
            1U,
            &minimum_envelope_len
        ) != DCC_OK || minimum_envelope_len != SIZE_MAX - 1U) {
        fprintf(
            stderr,
            "actual JSON boundary fixture detail length=%zu expected=%zu\n",
            minimum_envelope_len,
            SIZE_MAX - 1U
        );
        return 1;
    }
    size_t known_length_envelope_len = 0U;
    if (dcc_rest_multipart_measure_field_value_length(
            payload_json.name,
            strlen(payload_json.value),
            &exact_file,
            1U,
            &known_length_envelope_len
        ) != DCC_OK ||
        known_length_envelope_len != minimum_envelope_len) {
        fprintf(
            stderr,
            "known-length multipart parity detail length=%zu expected=%zu\n",
            known_length_envelope_len,
            minimum_envelope_len
        );
        return 1;
    }

    int exact_boundary_failed = 0;
    payload.files = &exact_file;
    ENDPOINT_CHECK_PREFLIGHT_REJECTION(
        "message rejects actual payload length overflow before allocation",
        capture,
        callback,
        observer,
        SIZE_MAX - 1U,
        exact_boundary_failed = 1,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );

    response.files = &exact_file;
    ENDPOINT_CHECK_PREFLIGHT_REJECTION(
        "interaction rejects actual payload length overflow before allocation",
        capture,
        callback,
        observer,
        SIZE_MAX - 1U,
        exact_boundary_failed = 1,
        dcc_rest_interaction_response_create(
            client,
            124U,
            "token",
            &response,
            &options,
            &request
        )
    );

    execute.files = &exact_file;
    ENDPOINT_CHECK_PREFLIGHT_REJECTION(
        "webhook rejects actual payload length overflow before allocation",
        capture,
        callback,
        observer,
        SIZE_MAX - 1U,
        exact_boundary_failed = 1,
        dcc_rest_execute_webhook(
            client,
            125U,
            "token",
            &execute,
            &options,
            &request
        )
    );
    if (exact_boundary_failed) {
        return 1;
    }

    dcc_rest_multipart_file_t files[] = {
        {
            "files[0]",
            "small.bin",
            "application/octet-stream",
            one_byte,
            sizeof(one_byte),
        },
        {
            "files[1]",
            "later.bin",
            "application/octet-stream",
            one_byte,
            SIZE_MAX - 200U,
        },
    };
    execute.files = files;
    execute.file_count = sizeof(files) / sizeof(files[0]);
    ENDPOINT_EXPECT_PREFLIGHT_REJECTION(
        "webhook rejects overflow introduced by a later multipart file",
        capture,
        callback,
        observer,
        dcc_rest_execute_webhook(
            client,
            126U,
            "token",
            &execute,
            &options,
            &request
        )
    );
    return 0;
}

#endif
