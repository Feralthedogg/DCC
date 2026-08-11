#include "rest_v2_endpoint_smoke_support.h"

#if !defined(_WIN32)

#include <dcc/message.h>
#include <dcc/rest/interactions.h>
#include <dcc/rest/messages.h>
#include <dcc/rest/webhooks.h>

#include "internal/objects/dcc_message_json_buffer_internal.h"
#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_multipart_build_internal.h"
#include "internal/rest/dcc_rest_multipart_internal.h"

#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ENDPOINT_EXPECT_PREFLIGHT_REJECTION( \
    label_, capture_, callback_, observer_, call_ \
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
        dcc_endpoint_test_allocation_probe_begin(0U); \
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
            return 1; \
        } \
    } while (0)

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

int endpoint_multipart_overflow_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback,
    endpoint_observer_t *observer
) {
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
