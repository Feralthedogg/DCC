#include <dcc/autocomplete.h>
#include <dcc/client.h>
#include <dcc/message.h>
#include <dcc/modal.h>
#include <dcc/rest.h>
#include <dcc/rest/interactions.h>
#include <dcc/rest/messages.h>
#include <dcc/rest/request.h>
#include <dcc/rest/webhooks.h>

#include "rest_v2_endpoint_smoke_support.h"

#if !defined(_WIN32)

#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENDPOINT_EXPECT_CALL( \
    label_, capture_, call_, method_, path_, content_type_, body_fragment_ \
) \
    do { \
        dcc_rest_request_t *request = NULL; \
        endpoint_capture_reset((capture_), 0U); \
        atomic_store_explicit( \
            &(capture_)->capture_release, \
            1U, \
            memory_order_release \
        ); \
        dcc_status_t submit_status = (call_); \
        const char *body_fragment = (body_fragment_); \
        if (endpoint_completed_contract( \
                (label_), \
                (capture_), \
                submit_status, \
                request, \
                (method_), \
                (path_), \
                (content_type_), \
                body_fragment, \
                body_fragment != NULL ? strlen(body_fragment) : 0U \
            ) != 0) { \
            return 1; \
        } \
    } while (0)

int endpoint_create_message_ownership_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback
) {
    static const unsigned char expected_file_a[] = {'A', 0U, 'B', 0xffU};
    static const unsigned char expected_file_b[] = {0U, 'x', 'y', 0U, 'z'};
    char *content = (char *)malloc(64U);
    unsigned char *file_a = (unsigned char *)malloc(sizeof(expected_file_a));
    unsigned char *file_b = (unsigned char *)malloc(sizeof(expected_file_b));
    char *file_metadata = (char *)malloc(160U);
    dcc_rest_multipart_file_t *files =
        (dcc_rest_multipart_file_t *)malloc(2U * sizeof(*files));
    if (content == NULL || file_a == NULL || file_b == NULL ||
        file_metadata == NULL || files == NULL) {
        free(content);
        free(file_a);
        free(file_b);
        free(file_metadata);
        free(files);
        return 1;
    }
    (void)snprintf(content, 64U, "owned-before-return");
    memcpy(file_a, expected_file_a, sizeof(expected_file_a));
    memcpy(file_b, expected_file_b, sizeof(expected_file_b));
    (void)snprintf(file_metadata, 32U, "files[0]");
    (void)snprintf(file_metadata + 32U, 32U, "first.bin");
    (void)snprintf(file_metadata + 64U, 32U, "application/octet-stream");
    (void)snprintf(file_metadata + 96U, 32U, "files[1]");
    (void)snprintf(file_metadata + 128U, 32U, "second.bin");

    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&message, content) != DCC_OK) {
        free(content);
        free(file_a);
        free(file_b);
        free(file_metadata);
        free(files);
        return 1;
    }
    files[0] = (dcc_rest_multipart_file_t){
        file_metadata,
        file_metadata + 32U,
        file_metadata + 64U,
        file_a,
        sizeof(expected_file_a),
    };
    files[1] = (dcc_rest_multipart_file_t){
        file_metadata + 96U,
        file_metadata + 128U,
        file_metadata + 64U,
        file_b,
        sizeof(expected_file_b),
    };
    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&payload, &message);
    payload.files = files;
    payload.file_count = 2U;

    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    endpoint_callback_reset(callback);
    options.callback = endpoint_result_callback;
    options.user_data = callback;

    /* Occupy the only REST worker before submitting the borrowed target. */
    endpoint_capture_reset(capture, 250U);
    dcc_rest_request_t *blocker_request = NULL;
    dcc_status_t blocker_submit_status = dcc_rest_get_message(
        client,
        UINT64_C(123456789),
        UINT64_C(987654321),
        NULL,
        &blocker_request
    );
    int blocker_entered = blocker_submit_status == DCC_OK &&
        blocker_request != NULL &&
        endpoint_wait_for_atomic(&capture->entered, 1U, 1000U);
    if (!blocker_entered) {
        atomic_store_explicit(&capture->capture_release, 1U, memory_order_release);
        if (blocker_request != NULL) {
            const dcc_rest_result_t *ignored = NULL;
            (void)dcc_rest_request_wait(blocker_request, 3000U, &ignored);
        }
        dcc_rest_request_destroy(blocker_request);
        free(content);
        free(file_a);
        free(file_b);
        free(file_metadata);
        free(files);
        return 1;
    }

    dcc_rest_request_t *request = NULL;
    callback->request_slot = &request;

    uint64_t started = endpoint_now_ms();
    dcc_status_t status = dcc_rest_create_message(
        client,
        UINT64_C(123456789),
        &payload,
        &options,
        &request
    );
    uint64_t elapsed = endpoint_now_ms() - started;

    /* The endpoint owns every borrowed byte as soon as submission returns. */
    payload.size = 0U;
    payload.version = UINT32_MAX;
    payload.message = NULL;
    payload.files = NULL;
    payload.file_count = SIZE_MAX;
    message.size = 0U;
    message.version = UINT32_MAX;
    message.content = "mutated-after-return";
    message.present = 0U;
    message.flags = UINT64_MAX;
    memset(content, 'q', 63U);
    memset(file_a, 'r', sizeof(expected_file_a));
    memset(file_b, 's', sizeof(expected_file_b));
    memset(file_metadata, 't', 160U);
    files[0].field_name = "changed[0]";
    files[0].filename = "changed-a.bin";
    files[0].content_type = "text/plain";
    files[0].data = NULL;
    files[0].data_len = 0U;
    files[1].field_name = "changed[1]";
    files[1].filename = "changed-b.bin";
    files[1].content_type = "text/plain";
    files[1].data = NULL;
    files[1].data_len = 0U;
    free(content);
    free(file_a);
    free(file_b);
    free(file_metadata);
    free(files);

    atomic_store_explicit(&capture->capture_release, 1U, memory_order_release);

    const dcc_rest_result_t *blocker_result = NULL;
    dcc_status_t blocker_wait_status = dcc_rest_request_wait(
        blocker_request,
        3000U,
        &blocker_result
    );
    const dcc_rest_result_t *result = NULL;
    dcc_status_t wait_status = request != NULL
        ? dcc_rest_request_wait(request, 3000U, &result)
        : DCC_ERR_STATE;
    int ok = blocker_submit_status == DCC_OK && blocker_entered &&
        blocker_wait_status == DCC_OK && blocker_result != NULL &&
        dcc_rest_result_ok(blocker_result) && status == DCC_OK && elapsed < 100U &&
        request != NULL &&
        wait_status == DCC_OK && result != NULL && dcc_rest_result_ok(result) &&
        atomic_load_explicit(&callback->calls, memory_order_acquire) == 1U &&
        atomic_load_explicit(&callback->published_handle, memory_order_acquire) == 1U &&
        callback->result != NULL &&
        callback->result->transport_status == result->transport_status &&
        callback->result->http_status == result->http_status &&
        callback->result->body_len == result->body_len &&
        memcmp(callback->result->body, result->body, result->body_len) == 0 &&
        atomic_load_explicit(&capture->calls, memory_order_acquire) == 2U &&
        strcmp(capture->method, "POST") == 0 &&
        strcmp(capture->path, "/channels/123456789/messages") == 0 &&
        strstr(capture->content_type, "multipart/form-data; boundary=") != NULL &&
        endpoint_contains(capture->body, capture->body_len, "owned-before-return", 19U) &&
        !endpoint_contains(capture->body, capture->body_len, "mutated-after-return", 20U) &&
        endpoint_contains(capture->body, capture->body_len, "first.bin", 9U) &&
        endpoint_contains(capture->body, capture->body_len, "second.bin", 10U) &&
        !endpoint_contains(capture->body, capture->body_len, "changed-a.bin", 13U) &&
        endpoint_contains(capture->body, capture->body_len, expected_file_a, sizeof(expected_file_a)) &&
        endpoint_contains(capture->body, capture->body_len, expected_file_b, sizeof(expected_file_b));
    if (!ok) {
        fprintf(
            stderr,
            "create-message detail blocker=%s/%s entered=%d status=%s "
            "elapsed=%llu wait=%s request=%p calls=%u published=%u method=%s "
            "path=%s type=%s body=%zu\n",
            dcc_status_string(blocker_submit_status),
            dcc_status_string(blocker_wait_status),
            blocker_entered,
            dcc_status_string(status),
            (unsigned long long)elapsed,
            dcc_status_string(wait_status),
            (void *)request,
            atomic_load_explicit(&capture->calls, memory_order_acquire),
            atomic_load_explicit(&callback->published_handle, memory_order_acquire),
            capture->method,
            capture->path,
            capture->content_type,
            capture->body_len
        );
    }
    dcc_rest_request_destroy(blocker_request);
    dcc_rest_request_destroy(request);
    return ok ? 0 : 1;
}

int endpoint_interaction_tags_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture
) {
    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    dcc_modal_builder_t modal = DCC_MODAL_BUILDER_INIT;
    dcc_autocomplete_choice_t choice = DCC_AUTOCOMPLETE_CHOICE_INIT;
    dcc_autocomplete_builder_t autocomplete = DCC_AUTOCOMPLETE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&message, "interaction-message") != DCC_OK ||
        dcc_modal_builder_set_custom_id(&modal, "modal-id") != DCC_OK ||
        dcc_modal_builder_set_title(&modal, "Modal title") != DCC_OK ||
        dcc_modal_builder_set_components_json(
            &modal,
            "[{\"type\":1,\"components\":[]}]"
        ) != DCC_OK ||
        dcc_autocomplete_choice_set_name(&choice, "choice") != DCC_OK ||
        dcc_autocomplete_choice_set_string_value(&choice, "value") != DCC_OK ||
        dcc_autocomplete_builder_set_choices(&autocomplete, &choice, 1U) != DCC_OK) {
        return 1;
    }

    dcc_rest_interaction_response_t response = DCC_REST_INTERACTION_RESPONSE_INIT;
    dcc_rest_interaction_response_init(NULL);

    dcc_rest_interaction_response_init(&response);
    if (dcc_rest_interaction_response_set_pong(&response) != DCC_OK) return 1;
    ENDPOINT_EXPECT_CALL(
        "interaction pong",
        capture,
        dcc_rest_interaction_response_create(
            client, 700U, "callback-token", &response, NULL, &request
        ),
        "POST",
        "/interactions/700/callback-token/callback",
        "application/json",
        "\"type\":1"
    );
    response.present |= DCC_REST_INTERACTION_RESPONSE_PRESENT_WITH_RESPONSE;
    response.with_response = 0U;
    ENDPOINT_EXPECT_CALL(
        "interaction explicit false with_response",
        capture,
        dcc_rest_interaction_response_create(
            client, 700U, "callback-token", &response, NULL, &request
        ),
        "POST",
        "/interactions/700/callback-token/callback?with_response=false",
        "application/json",
        "\"type\":1"
    );

    dcc_rest_interaction_response_init(&response);
    if (dcc_rest_interaction_response_set_message(&response, &message) != DCC_OK) return 1;
    response.present |= DCC_REST_INTERACTION_RESPONSE_PRESENT_WITH_RESPONSE;
    response.with_response = 1U;
    ENDPOINT_EXPECT_CALL(
        "interaction message JSON",
        capture,
        dcc_rest_interaction_response_create(
            client, 700U, "callback-token", &response, NULL, &request
        ),
        "POST",
        "/interactions/700/callback-token/callback?with_response=true",
        "application/json",
        "interaction-message"
    );
    if (!endpoint_contains(capture->body, capture->body_len, "\"type\":4", 8U) ||
        !endpoint_contains(
            capture->body,
            capture->body_len,
            "interaction-message",
            19U
        )) {
        fprintf(stderr, "interaction message JSON lost active callback data\n");
        return 1;
    }

    static const unsigned char message_file_bytes[] = {'m', 0U, 's', 'g'};
    dcc_rest_multipart_file_t message_file = {
        "files[0]", "message.bin", "application/octet-stream",
        message_file_bytes, sizeof(message_file_bytes),
    };
    response.files = &message_file;
    response.file_count = 1U;
    ENDPOINT_EXPECT_CALL(
        "interaction message multipart",
        capture,
        dcc_rest_interaction_response_create(
            client, 700U, "callback-token", &response, NULL, &request
        ),
        "POST",
        "/interactions/700/callback-token/callback?with_response=true",
        "multipart/form-data; boundary=",
        "message.bin"
    );
    if (!endpoint_contains(
            capture->body,
            capture->body_len,
            message_file_bytes,
            sizeof(message_file_bytes)
        )) {
        fprintf(stderr, "interaction message multipart lost embedded-NUL bytes\n");
        return 1;
    }
    if (!endpoint_contains(capture->body, capture->body_len, "\"type\":4", 8U) ||
        !endpoint_contains(
            capture->body,
            capture->body_len,
            "interaction-message",
            19U
        )) {
        fprintf(stderr, "interaction message multipart lost callback envelope\n");
        return 1;
    }

    dcc_rest_interaction_response_init(&response);
    if (dcc_rest_interaction_response_set_deferred_message(&response, NULL) != DCC_OK) {
        return 1;
    }
    ENDPOINT_EXPECT_CALL(
        "interaction deferred message",
        capture,
        dcc_rest_interaction_response_create(
            client, 700U, "callback-token", &response, NULL, &request
        ),
        "POST",
        "/interactions/700/callback-token/callback",
        "application/json",
        "\"type\":5"
    );
    dcc_message_builder_t deferred_data = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_flags(
            &deferred_data,
            DCC_MESSAGE_FLAG_EPHEMERAL
        ) != DCC_OK) {
        return 1;
    }
    dcc_rest_interaction_response_init(&response);
    if (dcc_rest_interaction_response_set_deferred_message(
            &response,
            &deferred_data
        ) != DCC_OK) {
        return 1;
    }
    ENDPOINT_EXPECT_CALL(
        "interaction deferred ephemeral message",
        capture,
        dcc_rest_interaction_response_create(
            client, 700U, "callback-token", &response, NULL, &request
        ),
        "POST",
        "/interactions/700/callback-token/callback",
        "application/json",
        "\"flags\":64"
    );
    if (!endpoint_contains(capture->body, capture->body_len, "\"type\":5", 8U)) {
        fprintf(stderr, "deferred interaction omitted type 5 envelope\n");
        return 1;
    }

    dcc_rest_interaction_response_init(&response);
    if (dcc_rest_interaction_response_set_deferred_update(&response) != DCC_OK) return 1;
    ENDPOINT_EXPECT_CALL(
        "interaction deferred update",
        capture,
        dcc_rest_interaction_response_create(
            client, 700U, "callback-token", &response, NULL, &request
        ),
        "POST",
        "/interactions/700/callback-token/callback",
        "application/json",
        "\"type\":6"
    );

    static const unsigned char update_file_bytes[] = {0xffU, 0U, 'u'};
    dcc_rest_multipart_file_t update_file = {
        "files[0]", "update.bin", "application/octet-stream",
        update_file_bytes, sizeof(update_file_bytes),
    };
    dcc_rest_interaction_response_init(&response);
    if (dcc_rest_interaction_response_set_update_message(&response, &message) != DCC_OK) return 1;
    response.files = &update_file;
    response.file_count = 1U;
    ENDPOINT_EXPECT_CALL(
        "interaction update multipart",
        capture,
        dcc_rest_interaction_response_create(
            client, 700U, "callback-token", &response, NULL, &request
        ),
        "POST",
        "/interactions/700/callback-token/callback",
        "multipart/form-data; boundary=",
        "update.bin"
    );
    if (!endpoint_contains(
            capture->body,
            capture->body_len,
            update_file_bytes,
            sizeof(update_file_bytes)
        )) {
        fprintf(stderr, "interaction update multipart lost embedded-NUL bytes\n");
        return 1;
    }
    if (!endpoint_contains(capture->body, capture->body_len, "\"type\":7", 8U) ||
        !endpoint_contains(
            capture->body,
            capture->body_len,
            "interaction-message",
            19U
        )) {
        fprintf(stderr, "interaction update multipart lost callback envelope\n");
        return 1;
    }

    dcc_rest_interaction_response_init(&response);
    if (dcc_rest_interaction_response_set_autocomplete(&response, &autocomplete) != DCC_OK) return 1;
    ENDPOINT_EXPECT_CALL(
        "interaction autocomplete",
        capture,
        dcc_rest_interaction_response_create(
            client, 700U, "callback-token", &response, NULL, &request
        ),
        "POST",
        "/interactions/700/callback-token/callback",
        "application/json",
        "\"type\":8"
    );
    if (!endpoint_contains(capture->body, capture->body_len, "\"name\":\"choice\"", 15U) ||
        !endpoint_contains(capture->body, capture->body_len, "\"value\":\"value\"", 15U)) {
        fprintf(stderr, "autocomplete interaction omitted active choice data\n");
        return 1;
    }

    dcc_rest_interaction_response_init(&response);
    if (dcc_rest_interaction_response_set_modal(&response, &modal) != DCC_OK) return 1;
    ENDPOINT_EXPECT_CALL(
        "interaction modal",
        capture,
        dcc_rest_interaction_response_create(
            client, 700U, "callback-token", &response, NULL, &request
        ),
        "POST",
        "/interactions/700/callback-token/callback",
        "application/json",
        "\"type\":9"
    );
    if (!endpoint_contains(capture->body, capture->body_len, "modal-id", 8U) ||
        !endpoint_contains(capture->body, capture->body_len, "Modal title", 11U) ||
        !endpoint_contains(capture->body, capture->body_len, "components", 10U)) {
        fprintf(stderr, "modal interaction omitted active modal data\n");
        return 1;
    }

    dcc_rest_interaction_response_init(&response);
    if (dcc_rest_interaction_response_set_premium_required(&response) != DCC_OK) return 1;
    ENDPOINT_EXPECT_CALL(
        "interaction premium required",
        capture,
        dcc_rest_interaction_response_create(
            client, 700U, "callback-token", &response, NULL, &request
        ),
        "POST",
        "/interactions/700/callback-token/callback",
        "application/json",
        "\"type\":10"
    );

    dcc_rest_interaction_response_init(&response);
    if (dcc_rest_interaction_response_set_launch_activity(&response) != DCC_OK) return 1;
    ENDPOINT_EXPECT_CALL(
        "interaction launch activity",
        capture,
        dcc_rest_interaction_response_create(
            client, 700U, "callback-token", &response, NULL, &request
        ),
        "POST",
        "/interactions/700/callback-token/callback",
        "application/json",
        "\"type\":12"
    );
    return 0;
}

int endpoint_auto_release_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback
) {
    endpoint_callback_reset(callback);
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    options.callback = endpoint_result_callback;
    options.user_data = callback;
    endpoint_capture_reset(capture, 0U);
    atomic_store_explicit(&capture->capture_release, 1U, memory_order_release);
    dcc_status_t submit_status = dcc_rest_get_message(
        client,
        777U,
        778U,
        &options,
        NULL
    );
    dcc_status_t drain_status = submit_status == DCC_OK
        ? dcc_rest_async_wait(client, 3000U)
        : DCC_ERR_STATE;
    int ok = submit_status == DCC_OK && drain_status == DCC_OK &&
        atomic_load_explicit(&callback->calls, memory_order_acquire) == 1U &&
        callback->result != NULL && dcc_rest_result_status(callback->result) == DCC_OK &&
        atomic_load_explicit(&capture->calls, memory_order_acquire) == 1U &&
        strcmp(capture->method, "GET") == 0 &&
        strcmp(capture->path, "/channels/777/messages/778") == 0;
    if (!ok) {
        fprintf(
            stderr,
            "auto-release detail submit=%s drain=%s callback=%u result=%s "
            "calls=%u method=%s path=%s\n",
            dcc_status_string(submit_status),
            dcc_status_string(drain_status),
            atomic_load_explicit(&callback->calls, memory_order_acquire),
            callback->result != NULL
                ? dcc_status_string(dcc_rest_result_status(callback->result))
                : "null",
            atomic_load_explicit(&capture->calls, memory_order_acquire),
            capture->method,
            capture->path
        );
    }
    return ok ? 0 : 1;
}

int endpoint_interaction_crud_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture
) {
    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&message, "interaction-crud") != DCC_OK) {
        return 1;
    }
    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&payload, &message);
    static const unsigned char interaction_file_bytes[] = {'i', 0U, 'c'};
    dcc_rest_multipart_file_t interaction_file = {
        "files[0]",
        "interaction-crud.bin",
        "application/octet-stream",
        interaction_file_bytes,
        sizeof(interaction_file_bytes),
    };

    ENDPOINT_EXPECT_CALL(
        "original response get",
        capture,
        dcc_rest_interaction_original_response_get(
            client, 800U, "interaction-token", NULL, &request
        ),
        "GET",
        "/webhooks/800/interaction-token/messages/@original",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "original response edit",
        capture,
        dcc_rest_interaction_original_response_edit(
            client, 800U, "interaction-token", &payload, NULL, &request
        ),
        "PATCH",
        "/webhooks/800/interaction-token/messages/@original",
        "application/json",
        "interaction-crud"
    );
    payload.files = &interaction_file;
    payload.file_count = 1U;
    ENDPOINT_EXPECT_CALL(
        "original response edit multipart",
        capture,
        dcc_rest_interaction_original_response_edit(
            client, 800U, "interaction-token", &payload, NULL, &request
        ),
        "PATCH",
        "/webhooks/800/interaction-token/messages/@original",
        "multipart/form-data; boundary=",
        "interaction-crud.bin"
    );
    if (!endpoint_contains(
            capture->body,
            capture->body_len,
            interaction_file_bytes,
            sizeof(interaction_file_bytes)
        ) ||
        !endpoint_contains(capture->body, capture->body_len, "interaction-crud", 16U)) {
        fprintf(stderr, "original-edit multipart lost payload_json or file bytes\n");
        return 1;
    }
    payload.files = NULL;
    payload.file_count = 0U;
    ENDPOINT_EXPECT_CALL(
        "original response delete",
        capture,
        dcc_rest_interaction_original_response_delete(
            client, 800U, "interaction-token", NULL, &request
        ),
        "DELETE",
        "/webhooks/800/interaction-token/messages/@original",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "followup create",
        capture,
        dcc_rest_interaction_followup_create(
            client, 800U, "interaction-token", &payload, NULL, &request
        ),
        "POST",
        "/webhooks/800/interaction-token",
        "application/json",
        "interaction-crud"
    );
    payload.files = &interaction_file;
    payload.file_count = 1U;
    ENDPOINT_EXPECT_CALL(
        "followup create multipart",
        capture,
        dcc_rest_interaction_followup_create(
            client, 800U, "interaction-token", &payload, NULL, &request
        ),
        "POST",
        "/webhooks/800/interaction-token",
        "multipart/form-data; boundary=",
        "interaction-crud.bin"
    );
    if (!endpoint_contains(
            capture->body,
            capture->body_len,
            interaction_file_bytes,
            sizeof(interaction_file_bytes)
        ) ||
        !endpoint_contains(capture->body, capture->body_len, "interaction-crud", 16U)) {
        fprintf(stderr, "followup-create multipart lost payload_json or file bytes\n");
        return 1;
    }
    payload.files = NULL;
    payload.file_count = 0U;
    ENDPOINT_EXPECT_CALL(
        "followup get",
        capture,
        dcc_rest_interaction_followup_get(
            client, 800U, "interaction-token", 801U, NULL, &request
        ),
        "GET",
        "/webhooks/800/interaction-token/messages/801",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "followup edit",
        capture,
        dcc_rest_interaction_followup_edit(
            client, 800U, "interaction-token", 801U, &payload, NULL, &request
        ),
        "PATCH",
        "/webhooks/800/interaction-token/messages/801",
        "application/json",
        "interaction-crud"
    );
    payload.files = &interaction_file;
    payload.file_count = 1U;
    ENDPOINT_EXPECT_CALL(
        "followup edit multipart",
        capture,
        dcc_rest_interaction_followup_edit(
            client, 800U, "interaction-token", 801U, &payload, NULL, &request
        ),
        "PATCH",
        "/webhooks/800/interaction-token/messages/801",
        "multipart/form-data; boundary=",
        "interaction-crud.bin"
    );
    if (!endpoint_contains(
            capture->body,
            capture->body_len,
            interaction_file_bytes,
            sizeof(interaction_file_bytes)
        ) ||
        !endpoint_contains(capture->body, capture->body_len, "interaction-crud", 16U)) {
        fprintf(stderr, "followup-edit multipart lost payload_json or file bytes\n");
        return 1;
    }
    payload.files = NULL;
    payload.file_count = 0U;
    ENDPOINT_EXPECT_CALL(
        "followup delete",
        capture,
        dcc_rest_interaction_followup_delete(
            client, 800U, "interaction-token", 801U, NULL, &request
        ),
        "DELETE",
        "/webhooks/800/interaction-token/messages/801",
        NULL,
        NULL
    );
    return 0;
}

int endpoint_webhook_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture
) {
    dcc_rest_webhook_builder_t builder = DCC_REST_WEBHOOK_BUILDER_INIT;
    dcc_rest_webhook_builder_init(&builder);
    dcc_rest_webhook_builder_init(NULL);
    builder.present = DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME |
        DCC_REST_WEBHOOK_BUILDER_PRESENT_AVATAR;
    builder.name = "typed webhook";
    builder.avatar = "data:image/png;base64,AA==";
    builder.channel_id = UINT64_C(902);

    ENDPOINT_EXPECT_CALL(
        "create webhook",
        capture,
        dcc_rest_create_webhook(client, 901U, &builder, NULL, &request),
        "POST",
        "/channels/901/webhooks",
        "application/json",
        "typed webhook"
    );
    if (!endpoint_contains(
            capture->body,
            capture->body_len,
            "data:image/png;base64,AA==",
            sizeof("data:image/png;base64,AA==") - 1U
        ) ||
        endpoint_contains(capture->body, capture->body_len, "\"channel_id\"", 12U)) {
        fprintf(stderr, "create webhook omitted avatar or serialized stale channel_id\n");
        return 1;
    }
    ENDPOINT_EXPECT_CALL(
        "get channel webhooks",
        capture,
        dcc_rest_get_channel_webhooks(client, 901U, NULL, &request),
        "GET",
        "/channels/901/webhooks",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "get guild webhooks",
        capture,
        dcc_rest_get_guild_webhooks(client, 903U, NULL, &request),
        "GET",
        "/guilds/903/webhooks",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "get webhook without token",
        capture,
        dcc_rest_get_webhook(client, 904U, NULL, NULL, &request),
        "GET",
        "/webhooks/904",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "get webhook with token",
        capture,
        dcc_rest_get_webhook(client, 904U, "hook-token", NULL, &request),
        "GET",
        "/webhooks/904/hook-token",
        NULL,
        NULL
    );
    builder.present |= DCC_REST_WEBHOOK_BUILDER_PRESENT_CHANNEL_ID;
    ENDPOINT_EXPECT_CALL(
        "modify webhook without token",
        capture,
        dcc_rest_modify_webhook(client, 904U, NULL, &builder, NULL, &request),
        "PATCH",
        "/webhooks/904",
        "application/json",
        "typed webhook"
    );
    if (!endpoint_contains(capture->body, capture->body_len, "\"channel_id\"", 12U) ||
        !endpoint_contains(capture->body, capture->body_len, "902", 3U)) {
        fprintf(stderr, "bot-authenticated webhook modify omitted channel_id\n");
        return 1;
    }
    builder.present &= ~DCC_REST_WEBHOOK_BUILDER_PRESENT_CHANNEL_ID;
    ENDPOINT_EXPECT_CALL(
        "modify webhook with token",
        capture,
        dcc_rest_modify_webhook(
            client, 904U, "hook-token", &builder, NULL, &request
        ),
        "PATCH",
        "/webhooks/904/hook-token",
        "application/json",
        "typed webhook"
    );
    if (endpoint_contains(capture->body, capture->body_len, "\"channel_id\"", 12U)) {
        fprintf(stderr, "token webhook modify serialized stale channel_id\n");
        return 1;
    }
    ENDPOINT_EXPECT_CALL(
        "delete webhook without token",
        capture,
        dcc_rest_delete_webhook(client, 904U, NULL, NULL, &request),
        "DELETE",
        "/webhooks/904",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "delete webhook with token",
        capture,
        dcc_rest_delete_webhook(client, 904U, "hook-token", NULL, &request),
        "DELETE",
        "/webhooks/904/hook-token",
        NULL,
        NULL
    );

    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&message, "webhook execute body") != DCC_OK) {
        return 1;
    }
    dcc_rest_webhook_execute_t execute = DCC_REST_WEBHOOK_EXECUTE_INIT;
    dcc_rest_webhook_execute_init(&execute, &message);
    dcc_rest_webhook_execute_init(NULL, &message);
    ENDPOINT_EXPECT_CALL(
        "execute webhook JSON zero files",
        capture,
        dcc_rest_execute_webhook(
            client, 904U, "hook-token", &execute, NULL, &request
        ),
        "POST",
        "/webhooks/904/hook-token",
        "application/json",
        "webhook execute body"
    );

    const dcc_snowflake_t applied_tags[] = {UINT64_C(11), UINT64_C(12)};
    execute.present = DCC_REST_WEBHOOK_EXECUTE_PRESENT_USERNAME |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_AVATAR_URL |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_NAME |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_APPLIED_TAG_IDS |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_WAIT |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_WITH_COMPONENTS;
    execute.username = "override user";
    execute.avatar_url = "https://example.invalid/avatar.png";
    execute.thread_name = "typed thread";
    execute.applied_tag_ids = applied_tags;
    execute.applied_tag_count = sizeof(applied_tags) / sizeof(applied_tags[0]);
    execute.wait = 1U;
    execute.with_components = 1U;
    ENDPOINT_EXPECT_CALL(
        "execute webhook full JSON options",
        capture,
        dcc_rest_execute_webhook(
            client, 904U, "hook-token", &execute, NULL, &request
        ),
        "POST",
        "/webhooks/904/hook-token?wait=true&with_components=true",
        "application/json",
        "typed thread"
    );
    if (!endpoint_contains(capture->body, capture->body_len, "override user", 13U) ||
        !endpoint_contains(capture->body, capture->body_len, "avatar.png", 10U) ||
        !endpoint_contains(capture->body, capture->body_len, "11", 2U) ||
        !endpoint_contains(capture->body, capture->body_len, "12", 2U)) {
        fprintf(stderr, "webhook execute JSON omitted typed webhook-only fields\n");
        return 1;
    }

    execute.present = DCC_REST_WEBHOOK_EXECUTE_PRESENT_WAIT |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_ID |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_WITH_COMPONENTS;
    execute.wait = 1U;
    execute.thread_id = UINT64_C(905);
    execute.with_components = 1U;
    ENDPOINT_EXPECT_CALL(
        "execute webhook existing thread options",
        capture,
        dcc_rest_execute_webhook(
            client, 904U, "hook-token", &execute, NULL, &request
        ),
        "POST",
        "/webhooks/904/hook-token?wait=true&thread_id=905&with_components=true",
        "application/json",
        "webhook execute body"
    );
    if (endpoint_contains(capture->body, capture->body_len, "\"username\"", 10U) ||
        endpoint_contains(capture->body, capture->body_len, "\"avatar_url\"", 12U) ||
        endpoint_contains(capture->body, capture->body_len, "\"thread_name\"", 13U) ||
        endpoint_contains(capture->body, capture->body_len, "\"applied_tags\"", 14U)) {
        fprintf(stderr, "webhook execute serialized stale absent JSON fields\n");
        return 1;
    }

    execute.present = DCC_REST_WEBHOOK_EXECUTE_PRESENT_WAIT;
    execute.wait = 0U;
    ENDPOINT_EXPECT_CALL(
        "execute webhook explicit false wait",
        capture,
        dcc_rest_execute_webhook(
            client, 904U, "hook-token", &execute, NULL, &request
        ),
        "POST",
        "/webhooks/904/hook-token?wait=false",
        "application/json",
        "webhook execute body"
    );

    static const unsigned char webhook_file_a[] = {'w', 0U, 'a', 0xffU};
    static const unsigned char webhook_file_b[] = {0U, 'b', 0U};
    dcc_rest_multipart_file_t webhook_files[] = {
        {
            "files[0]", "webhook-a.bin", "application/octet-stream",
            webhook_file_a, sizeof(webhook_file_a),
        },
        {
            "files[1]", "webhook-b.bin", NULL,
            webhook_file_b, sizeof(webhook_file_b),
        },
    };
    execute.present = DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_ID |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_WITH_COMPONENTS;
    execute.thread_id = UINT64_C(906);
    execute.with_components = 0U;
    execute.files = webhook_files;
    execute.file_count = sizeof(webhook_files) / sizeof(webhook_files[0]);
    ENDPOINT_EXPECT_CALL(
        "execute webhook multipart",
        capture,
        dcc_rest_execute_webhook(
            client, 904U, "hook-token", &execute, NULL, &request
        ),
        "POST",
        "/webhooks/904/hook-token?thread_id=906&with_components=false",
        "multipart/form-data; boundary=",
        "webhook-a.bin"
    );
    if (!endpoint_contains(capture->body, capture->body_len, "webhook-b.bin", 13U) ||
        !endpoint_contains(
            capture->body,
            capture->body_len,
            "name=\"payload_json\"",
            sizeof("name=\"payload_json\"") - 1U
        ) ||
        !endpoint_contains(
            capture->body,
            capture->body_len,
            "webhook execute body",
            sizeof("webhook execute body") - 1U
        ) ||
        !endpoint_contains(
            capture->body,
            capture->body_len,
            webhook_file_a,
            sizeof(webhook_file_a)
        ) ||
        !endpoint_contains(
            capture->body,
            capture->body_len,
            webhook_file_b,
            sizeof(webhook_file_b)
        ) ||
        endpoint_contains(capture->body, capture->body_len, "\"username\"", 10U) ||
        endpoint_contains(capture->body, capture->body_len, "\"avatar_url\"", 12U) ||
        endpoint_contains(capture->body, capture->body_len, "\"thread_name\"", 13U) ||
        endpoint_contains(capture->body, capture->body_len, "\"applied_tags\"", 14U)) {
        fprintf(stderr, "webhook multipart lost payload_json, fields, or files\n");
        return 1;
    }

    dcc_rest_webhook_message_query_t message_query =
        DCC_REST_WEBHOOK_MESSAGE_QUERY_INIT;
    dcc_rest_webhook_message_query_init(&message_query);
    dcc_rest_webhook_message_query_init(NULL);
    message_query.present = DCC_REST_WEBHOOK_MESSAGE_QUERY_PRESENT_THREAD_ID;
    message_query.thread_id = UINT64_C(907);
    ENDPOINT_EXPECT_CALL(
        "get webhook message thread",
        capture,
        dcc_rest_get_webhook_message(
            client, 904U, "hook-token", 908U, &message_query, NULL, &request
        ),
        "GET",
        "/webhooks/904/hook-token/messages/908?thread_id=907",
        NULL,
        NULL
    );

    dcc_rest_message_payload_t message_payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&message_payload, &message);
    dcc_rest_webhook_message_edit_t edit = DCC_REST_WEBHOOK_MESSAGE_EDIT_INIT;
    dcc_rest_webhook_message_edit_init(&edit, &message_payload);
    dcc_rest_webhook_message_edit_init(NULL, &message_payload);
    edit.present = DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_THREAD_ID |
        DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_WITH_COMPONENTS;
    edit.thread_id = UINT64_C(907);
    edit.with_components = 1U;
    ENDPOINT_EXPECT_CALL(
        "edit webhook message thread and components",
        capture,
        dcc_rest_modify_webhook_message(
            client, 904U, "hook-token", 908U, &edit, NULL, &request
        ),
        "PATCH",
        "/webhooks/904/hook-token/messages/908?thread_id=907&with_components=true",
        "application/json",
        "webhook execute body"
    );
    edit.present = DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_WITH_COMPONENTS;
    edit.with_components = 0U;
    ENDPOINT_EXPECT_CALL(
        "edit webhook message explicit false components",
        capture,
        dcc_rest_modify_webhook_message(
            client, 904U, "hook-token", 908U, &edit, NULL, &request
        ),
        "PATCH",
        "/webhooks/904/hook-token/messages/908?with_components=false",
        "application/json",
        "webhook execute body"
    );
    static const unsigned char webhook_edit_bytes[] = {'m', 0U, 'e'};
    dcc_rest_multipart_file_t webhook_edit_file = {
        "files[0]",
        "webhook-edit.bin",
        "application/octet-stream",
        webhook_edit_bytes,
        sizeof(webhook_edit_bytes),
    };
    message_payload.files = &webhook_edit_file;
    message_payload.file_count = 1U;
    ENDPOINT_EXPECT_CALL(
        "edit webhook message multipart",
        capture,
        dcc_rest_modify_webhook_message(
            client, 904U, "hook-token", 908U, &edit, NULL, &request
        ),
        "PATCH",
        "/webhooks/904/hook-token/messages/908?with_components=false",
        "multipart/form-data; boundary=",
        "webhook-edit.bin"
    );
    if (!endpoint_contains(
            capture->body,
            capture->body_len,
            webhook_edit_bytes,
            sizeof(webhook_edit_bytes)
        ) ||
        !endpoint_contains(
            capture->body,
            capture->body_len,
            "webhook execute body",
            20U
        )) {
        fprintf(stderr, "webhook-message edit multipart lost payload_json or bytes\n");
        return 1;
    }
    message_payload.files = NULL;
    message_payload.file_count = 0U;
    ENDPOINT_EXPECT_CALL(
        "delete webhook message thread",
        capture,
        dcc_rest_delete_webhook_message(
            client, 904U, "hook-token", 908U, &message_query, NULL, &request
        ),
        "DELETE",
        "/webhooks/904/hook-token/messages/908?thread_id=907",
        NULL,
        NULL
    );

    static const char compat_body[] = "{\"compat\":true}";
    dcc_rest_webhook_compat_payload_t compat =
        DCC_REST_WEBHOOK_COMPAT_PAYLOAD_INIT;
    dcc_rest_webhook_compat_payload_init(
        &compat,
        compat_body,
        sizeof(compat_body) - 1U
    );
    dcc_rest_webhook_compat_payload_init(NULL, compat_body, sizeof(compat_body) - 1U);
    ENDPOINT_EXPECT_CALL(
        "execute GitHub webhook",
        capture,
        dcc_rest_execute_webhook_github(
            client, 904U, "hook-token", &compat, NULL, &request
        ),
        "POST",
        "/webhooks/904/hook-token/github",
        "application/json",
        compat_body
    );
    ENDPOINT_EXPECT_CALL(
        "execute Slack webhook",
        capture,
        dcc_rest_execute_webhook_slack(
            client, 904U, "hook-token", &compat, NULL, &request
        ),
        "POST",
        "/webhooks/904/hook-token/slack",
        "application/json",
        compat_body
    );
    return 0;
}


#endif
