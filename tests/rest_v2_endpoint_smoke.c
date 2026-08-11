#include <dcc/autocomplete.h>
#include <dcc/client.h>
#include <dcc/message.h>
#include <dcc/modal.h>
#include <dcc/rest/interactions.h>
#include <dcc/rest/messages.h>
#include <dcc/rest/request.h>
#include <dcc/rest/webhooks.h>

#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_intercept_internal.h"

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <errno.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "support/rest_v2_endpoint_smoke_support.h"

uint64_t endpoint_now_ms(void) {
    struct timespec now;
    (void)clock_gettime(CLOCK_MONOTONIC, &now);
    return (uint64_t)now.tv_sec * UINT64_C(1000) +
        (uint64_t)now.tv_nsec / UINT64_C(1000000);
}

static void endpoint_sleep_ms(uint32_t delay_ms) {
    struct timespec delay = {
        .tv_sec = (time_t)(delay_ms / 1000U),
        .tv_nsec = (long)(delay_ms % 1000U) * 1000000L,
    };
    while (nanosleep(&delay, &delay) != 0 && errno == EINTR) {
    }
}

static void *endpoint_runner_main(void *user_data) {
    endpoint_runner_t *runner = (endpoint_runner_t *)user_data;
    runner->status = dcc_client_wait(runner->client);
    return NULL;
}

static dcc_status_t endpoint_intercept(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const void *body,
    size_t body_len,
    const char *content_type,
    dcc_rest_cb callback,
    void *callback_user_data,
    void *intercept_user_data
) {
    endpoint_capture_t *capture = (endpoint_capture_t *)intercept_user_data;
    atomic_store_explicit(&capture->entered, 1U, memory_order_release);
    while (atomic_load_explicit(&capture->capture_release, memory_order_acquire) == 0U) {
        endpoint_sleep_ms(1U);
    }
    (void)snprintf(capture->method, sizeof(capture->method), "%s", method);
    (void)snprintf(capture->path, sizeof(capture->path), "%s", path);
    (void)snprintf(
        capture->content_type,
        sizeof(capture->content_type),
        "%s",
        content_type != NULL ? content_type : ""
    );
    capture->body_len = body_len < sizeof(capture->body)
        ? body_len
        : sizeof(capture->body);
    if (capture->body_len != 0U) {
        memcpy(capture->body, body, capture->body_len);
    }
    atomic_fetch_add_explicit(&capture->calls, 1U, memory_order_acq_rel);
    endpoint_sleep_ms(atomic_load_explicit(&capture->delay_ms, memory_order_acquire));

    static const char response_body[] = "{\"id\":\"42\"}";
    dcc_rest_response_t response = {
        .size = sizeof(response),
        .status = 200U,
        .error = DCC_OK,
        .body = response_body,
        .body_len = sizeof(response_body) - 1U,
    };
    callback(client, &response, callback_user_data);
    return DCC_OK;
}

void endpoint_result_callback(
    dcc_client_t *client,
    const dcc_rest_result_t *result,
    void *user_data
) {
    endpoint_callback_t *state = (endpoint_callback_t *)user_data;
    (void)client;
    if (state->request_slot != NULL && *state->request_slot != NULL) {
        atomic_store_explicit(&state->published_handle, 1U, memory_order_release);
    }
    if (state->result == NULL) {
        (void)dcc_rest_result_clone(result, &state->result);
    }
    atomic_fetch_add_explicit(&state->calls, 1U, memory_order_acq_rel);
}

static void endpoint_error_observer(
    dcc_client_t *client,
    const dcc_error_t *error,
    void *user_data
) {
    endpoint_observer_t *observer = (endpoint_observer_t *)user_data;
    (void)client;
    (void)error;
    atomic_fetch_add_explicit(&observer->calls, 1U, memory_order_acq_rel);
}

int endpoint_contains(
    const unsigned char *haystack,
    size_t haystack_len,
    const void *needle,
    size_t needle_len
) {
    const unsigned char *bytes = (const unsigned char *)needle;
    if (needle_len == 0U) {
        return 1;
    }
    if (needle_len > haystack_len) {
        return 0;
    }
    for (size_t index = 0U; index <= haystack_len - needle_len; ++index) {
        if (memcmp(haystack + index, bytes, needle_len) == 0) {
            return 1;
        }
    }
    return 0;
}

void endpoint_capture_reset(endpoint_capture_t *capture, uint32_t delay_ms) {
    memset(capture->method, 0, sizeof(capture->method));
    memset(capture->path, 0, sizeof(capture->path));
    memset(capture->content_type, 0, sizeof(capture->content_type));
    memset(capture->body, 0, sizeof(capture->body));
    capture->body_len = 0U;
    atomic_store_explicit(&capture->calls, 0U, memory_order_release);
    atomic_store_explicit(&capture->delay_ms, delay_ms, memory_order_release);
    atomic_store_explicit(&capture->entered, 0U, memory_order_release);
    atomic_store_explicit(&capture->capture_release, 0U, memory_order_release);
}

int endpoint_wait_for_atomic(
    const atomic_uint *value,
    unsigned expected,
    uint32_t timeout_ms
) {
    uint64_t started = endpoint_now_ms();
    do {
        if (atomic_load_explicit(value, memory_order_acquire) >= expected) {
            return 1;
        }
        endpoint_sleep_ms(1U);
    } while (endpoint_now_ms() - started < timeout_ms);
    return atomic_load_explicit(value, memory_order_acquire) >= expected;
}

void endpoint_callback_reset(endpoint_callback_t *callback) {
    dcc_rest_result_free(callback->result);
    callback->result = NULL;
    callback->request_slot = NULL;
    atomic_store_explicit(&callback->calls, 0U, memory_order_release);
    atomic_store_explicit(&callback->published_handle, 0U, memory_order_release);
}

int endpoint_completed_contract(
    const char *label,
    endpoint_capture_t *capture,
    dcc_status_t submit_status,
    dcc_rest_request_t *request,
    const char *expected_method,
    const char *expected_path,
    const char *expected_content_type,
    const void *expected_body_fragment,
    size_t expected_body_fragment_len
) {
    const dcc_rest_result_t *result = NULL;
    dcc_status_t wait_status = request != NULL
        ? dcc_rest_request_wait(request, 3000U, &result)
        : DCC_ERR_STATE;
    int ok = submit_status == DCC_OK && request != NULL && wait_status == DCC_OK &&
        result != NULL && dcc_rest_result_status(result) == DCC_OK &&
        atomic_load_explicit(&capture->calls, memory_order_acquire) == 1U &&
        strcmp(capture->method, expected_method) == 0 &&
        strcmp(capture->path, expected_path) == 0 &&
        (expected_content_type == NULL ||
            strstr(capture->content_type, expected_content_type) != NULL) &&
        (expected_body_fragment == NULL || endpoint_contains(
            capture->body,
            capture->body_len,
            expected_body_fragment,
            expected_body_fragment_len
        ));
    if (!ok) {
        fprintf(
            stderr,
            "%s detail submit=%s wait=%s request=%p calls=%u method=%s "
            "path=%s type=%s body=%zu\n",
            label,
            dcc_status_string(submit_status),
            dcc_status_string(wait_status),
            (void *)request,
            atomic_load_explicit(&capture->calls, memory_order_acquire),
            capture->method,
            capture->path,
            capture->content_type,
            capture->body_len
        );
    }
    dcc_rest_request_destroy(request);
    return ok ? 0 : 1;
}

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

#define ENDPOINT_EXPECT_REJECTION( \
    label_, expected_status_, capture_, callback_, observer_, call_ \
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
        int allocation_probe_armed = \
            (expected_status_) == DCC_ERR_INVALID_ARG; \
        if (allocation_probe_armed) { \
            dcc_endpoint_test_allocation_probe_begin(0U); \
        } \
        dcc_status_t rejected_status = (call_); \
        size_t allocation_calls = allocation_probe_armed \
            ? dcc_endpoint_test_allocation_probe_end() \
            : 0U; \
        if (rejected_status != (expected_status_) || request != NULL || \
            allocation_calls != 0U || \
            atomic_load_explicit(&(capture_)->calls, memory_order_acquire) != \
                capture_calls_before || \
            atomic_load_explicit(&(callback_)->calls, memory_order_acquire) != \
                callback_calls_before || \
            atomic_load_explicit(&(observer_)->calls, memory_order_acquire) != \
                observer_calls_before) { \
            fprintf( \
                stderr, \
                "%s rejection detail status=%s request=%p allocations=%zu capture=%u/%u " \
                "callback=%u/%u observer=%u/%u\n", \
                (label_), \
                dcc_status_string(rejected_status), \
                (void *)request, \
                allocation_calls, \
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

static int endpoint_rejection_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback,
    endpoint_observer_t *observer
) {
    endpoint_callback_reset(callback);
    unsigned rejection_capture_baseline = atomic_load_explicit(
        &capture->calls,
        memory_order_acquire
    );
    unsigned rejection_callback_baseline = atomic_load_explicit(
        &callback->calls,
        memory_order_acquire
    );
    unsigned rejection_observer_baseline = atomic_load_explicit(
        &observer->calls,
        memory_order_acquire
    );
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    options.callback = endpoint_result_callback;
    options.user_data = callback;

    dcc_rest_message_payload_t empty = DCC_REST_MESSAGE_PAYLOAD_INIT;
    ENDPOINT_EXPECT_REJECTION(
        "empty message payload",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &empty, &options, &request)
    );

    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&message, "valid") != DCC_OK) {
        return 1;
    }
    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&payload, &message);
    dcc_rest_message_payload_init(NULL, &message);

    payload.version = DCC_REST_MESSAGE_PAYLOAD_VERSION + 1U;
    ENDPOINT_EXPECT_REJECTION(
        "unsupported message payload version",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );
    payload = (dcc_rest_message_payload_t)DCC_REST_MESSAGE_PAYLOAD_INIT;
    payload.message = &message;
    payload.size = offsetof(dcc_rest_message_payload_t, message);
    ENDPOINT_EXPECT_REJECTION(
        "uncovered historical message payload prefix",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );
    payload = (dcc_rest_message_payload_t)DCC_REST_MESSAGE_PAYLOAD_INIT;
    payload.message = &message;
    payload.file_count = 1U;
    ENDPOINT_EXPECT_REJECTION(
        "message payload missing file array",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );
    payload.file_count = SIZE_MAX;
    payload.files = (const dcc_rest_multipart_file_t *)(uintptr_t)UINTPTR_MAX;
    ENDPOINT_EXPECT_REJECTION(
        "overflowed message file count",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );

    static const unsigned char one_byte[] = {1U};
    dcc_rest_multipart_file_t malformed_file = {
        NULL, "bad.bin", "application/octet-stream", one_byte, sizeof(one_byte),
    };
    payload = (dcc_rest_message_payload_t)DCC_REST_MESSAGE_PAYLOAD_INIT;
    payload.message = &message;
    payload.files = &malformed_file;
    payload.file_count = 1U;
    ENDPOINT_EXPECT_REJECTION(
        "missing multipart field name",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );
    malformed_file.field_name = "files[0]";
    malformed_file.filename = NULL;
    ENDPOINT_EXPECT_REJECTION(
        "missing multipart filename",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );
    malformed_file.filename = "bad.bin";
    malformed_file.content_type = "application/octet-stream\r\nX-Bad: yes";
    ENDPOINT_EXPECT_REJECTION(
        "invalid multipart content type",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );
    malformed_file.field_name = "files[0]\r\nX-Bad: yes";
    malformed_file.filename = "bad.bin";
    malformed_file.content_type = NULL;
    ENDPOINT_EXPECT_REJECTION(
        "CRLF in multipart field name",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );
    malformed_file.field_name = "files[0]";
    malformed_file.filename = "bad.bin\r\nX-Bad: yes";
    ENDPOINT_EXPECT_REJECTION(
        "CRLF in multipart filename",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );
    malformed_file.filename = "bad.bin";
    malformed_file.content_type = NULL;
    malformed_file.data = NULL;
    malformed_file.data_len = 1U;
    ENDPOINT_EXPECT_REJECTION(
        "invalid multipart data span",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 123U, &payload, &options, &request)
    );

    ENDPOINT_EXPECT_REJECTION(
        "overflowed bulk message count",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_bulk_delete_messages(
            client,
            123U,
            (const dcc_snowflake_t *)(uintptr_t)UINTPTR_MAX,
            SIZE_MAX,
            &options,
            &request
        )
    );
    const dcc_snowflake_t one_bulk_id[] = {UINT64_C(1)};
    ENDPOINT_EXPECT_REJECTION(
        "bulk message count below two",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_bulk_delete_messages(
            client,
            123U,
            one_bulk_id,
            sizeof(one_bulk_id) / sizeof(one_bulk_id[0]),
            &options,
            &request
        )
    );
    dcc_snowflake_t too_many_bulk_ids[101];
    for (size_t index = 0U;
         index < sizeof(too_many_bulk_ids) / sizeof(too_many_bulk_ids[0]);
         ++index) {
        too_many_bulk_ids[index] = (dcc_snowflake_t)index + UINT64_C(1);
    }
    ENDPOINT_EXPECT_REJECTION(
        "bulk message count over 100",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_bulk_delete_messages(
            client,
            123U,
            too_many_bulk_ids,
            sizeof(too_many_bulk_ids) / sizeof(too_many_bulk_ids[0]),
            &options,
            &request
        )
    );

    dcc_rest_message_list_query_t list_query = DCC_REST_MESSAGE_LIST_QUERY_INIT;
    list_query.present = DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AROUND |
        DCC_REST_MESSAGE_LIST_QUERY_PRESENT_BEFORE;
    list_query.around = 1U;
    list_query.before = 2U;
    ENDPOINT_EXPECT_REJECTION(
        "mutually exclusive message cursors",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_get_channel_messages(client, 123U, &list_query, &options, &request)
    );
    list_query = (dcc_rest_message_list_query_t)DCC_REST_MESSAGE_LIST_QUERY_INIT;
    list_query.present = DCC_REST_MESSAGE_LIST_QUERY_PRESENT_LIMIT;
    list_query.limit = 0U;
    ENDPOINT_EXPECT_REJECTION(
        "message page limit below one",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_get_channel_messages(client, 123U, &list_query, &options, &request)
    );
    list_query.limit = 101U;
    ENDPOINT_EXPECT_REJECTION(
        "message page limit over 100",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_get_channel_messages(client, 123U, &list_query, &options, &request)
    );

    dcc_rest_reaction_query_t reaction_query = DCC_REST_REACTION_QUERY_INIT;
    reaction_query.present = DCC_REST_REACTION_QUERY_PRESENT_TYPE;
    reaction_query.type = (dcc_rest_reaction_type_t)2;
    ENDPOINT_EXPECT_REJECTION(
        "unsupported reaction type",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_get_message_reactions(
            client, 123U, 124U, "x", &reaction_query, &options, &request
        )
    );
    reaction_query = (dcc_rest_reaction_query_t)DCC_REST_REACTION_QUERY_INIT;
    reaction_query.present = DCC_REST_REACTION_QUERY_PRESENT_LIMIT;
    reaction_query.limit = 0U;
    ENDPOINT_EXPECT_REJECTION(
        "reaction limit below one",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_get_message_reactions(
            client, 123U, 124U, "x", &reaction_query, &options, &request
        )
    );
    reaction_query.limit = 101U;
    ENDPOINT_EXPECT_REJECTION(
        "reaction limit over 100",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_get_message_reactions(
            client, 123U, 124U, "x", &reaction_query, &options, &request
        )
    );
    reaction_query.present = UINT64_C(1) << 63U;
    ENDPOINT_EXPECT_REJECTION(
        "unknown reaction query presence bit",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_get_message_reactions(
            client, 123U, 124U, "x", &reaction_query, &options, &request
        )
    );
    reaction_query = (dcc_rest_reaction_query_t)DCC_REST_REACTION_QUERY_INIT;
    reaction_query.present = DCC_REST_REACTION_QUERY_PRESENT_AFTER;
    reaction_query.after = 1U;
    reaction_query.size = offsetof(dcc_rest_reaction_query_t, after);
    ENDPOINT_EXPECT_REJECTION(
        "uncovered historical reaction query prefix",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_get_message_reactions(
            client, 123U, 124U, "x", &reaction_query, &options, &request
        )
    );

    dcc_rest_id_page_t poll_page = DCC_REST_ID_PAGE_INIT;
    poll_page.present = DCC_REST_ID_PAGE_PRESENT_LIMIT;
    poll_page.limit = 0U;
    ENDPOINT_EXPECT_REJECTION(
        "poll voter limit below one",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_get_poll_answer_voters(
            client, 123U, 124U, 1U, &poll_page, &options, &request
        )
    );
    poll_page.limit = 101U;
    ENDPOINT_EXPECT_REJECTION(
        "poll voter limit over 100",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_get_poll_answer_voters(
            client, 123U, 124U, 1U, &poll_page, &options, &request
        )
    );
    poll_page.present = DCC_REST_ID_PAGE_PRESENT_BEFORE;
    poll_page.before = 1U;
    ENDPOINT_EXPECT_REJECTION(
        "poll voters reject before cursor",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_get_poll_answer_voters(
            client, 123U, 124U, 1U, &poll_page, &options, &request
        )
    );

    dcc_rest_pin_page_t pin_page = DCC_REST_PIN_PAGE_INIT;
    pin_page.present = DCC_REST_PIN_PAGE_PRESENT_BEFORE;
    pin_page.before = NULL;
    ENDPOINT_EXPECT_REJECTION(
        "pin page missing before string",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_get_channel_pins(client, 123U, &pin_page, &options, &request)
    );
    pin_page = (dcc_rest_pin_page_t)DCC_REST_PIN_PAGE_INIT;
    pin_page.present = DCC_REST_PIN_PAGE_PRESENT_LIMIT;
    pin_page.limit = 0U;
    ENDPOINT_EXPECT_REJECTION(
        "pin page limit below one",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_get_channel_pins(client, 123U, &pin_page, &options, &request)
    );
    pin_page.limit = 51U;
    ENDPOINT_EXPECT_REJECTION(
        "pin page limit over 50",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_get_channel_pins(client, 123U, &pin_page, &options, &request)
    );

    dcc_rest_interaction_response_t response = DCC_REST_INTERACTION_RESPONSE_INIT;
    response.type = DCC_INTERACTION_RESPONSE_MODAL;
    response.present = DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE;
    response.data.message = &message;
    ENDPOINT_EXPECT_REJECTION(
        "cross-tag interaction message as modal",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    response.type = DCC_INTERACTION_RESPONSE_CHANNEL_MESSAGE_WITH_SOURCE;
    response.present = DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE;
    response.data.message = NULL;
    ENDPOINT_EXPECT_REJECTION(
        "interaction message requires builder",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    response.type = DCC_INTERACTION_RESPONSE_UPDATE_MESSAGE;
    response.present = DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE;
    response.data.message = NULL;
    ENDPOINT_EXPECT_REJECTION(
        "interaction update requires builder",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    response.type = DCC_INTERACTION_RESPONSE_AUTOCOMPLETE;
    response.present = DCC_REST_INTERACTION_RESPONSE_PRESENT_AUTOCOMPLETE;
    response.data.autocomplete = NULL;
    ENDPOINT_EXPECT_REJECTION(
        "interaction autocomplete requires builder",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    response.type = DCC_INTERACTION_RESPONSE_MODAL;
    response.present = DCC_REST_INTERACTION_RESPONSE_PRESENT_MODAL;
    response.data.modal = NULL;
    ENDPOINT_EXPECT_REJECTION(
        "interaction modal requires builder",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    response.type = (dcc_interaction_response_type_t)99;
    ENDPOINT_EXPECT_REJECTION(
        "unsupported interaction response type",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    if (dcc_rest_interaction_response_set_pong(&response) != DCC_OK) return 1;
    response.present |= DCC_REST_INTERACTION_RESPONSE_PRESENT_WITH_RESPONSE;
    response.with_response = 2U;
    ENDPOINT_EXPECT_REJECTION(
        "invalid interaction with_response boolean",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    if (dcc_rest_interaction_response_set_pong(&response) != DCC_OK) return 1;
    ENDPOINT_EXPECT_REJECTION(
        "interaction callback rejects empty token",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "", &response, &options, &request
        )
    );
    dcc_rest_multipart_file_t valid_file = {
        "files[0]", "pong.bin", NULL, one_byte, sizeof(one_byte),
    };
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    if (dcc_rest_interaction_response_set_pong(&response) != DCC_OK) return 1;
    response.files = &valid_file;
    response.file_count = 1U;
    ENDPOINT_EXPECT_REJECTION(
        "pong rejects multipart files",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    if (dcc_rest_interaction_response_set_deferred_message(&response, NULL) != DCC_OK) {
        return 1;
    }
    response.files = &valid_file;
    response.file_count = 1U;
    ENDPOINT_EXPECT_REJECTION(
        "deferred message rejects multipart files",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    if (dcc_rest_interaction_response_set_deferred_update(&response) != DCC_OK) {
        return 1;
    }
    response.files = &valid_file;
    response.file_count = 1U;
    ENDPOINT_EXPECT_REJECTION(
        "deferred update rejects multipart files",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );

    dcc_autocomplete_choice_t file_choice = DCC_AUTOCOMPLETE_CHOICE_INIT;
    dcc_autocomplete_builder_t file_autocomplete = DCC_AUTOCOMPLETE_BUILDER_INIT;
    dcc_modal_builder_t file_modal = DCC_MODAL_BUILDER_INIT;
    if (dcc_autocomplete_choice_set_name(&file_choice, "choice") != DCC_OK ||
        dcc_autocomplete_choice_set_string_value(&file_choice, "value") != DCC_OK ||
        dcc_autocomplete_builder_set_choices(
            &file_autocomplete,
            &file_choice,
            1U
        ) != DCC_OK ||
        dcc_modal_builder_set_custom_id(&file_modal, "modal-id") != DCC_OK ||
        dcc_modal_builder_set_title(&file_modal, "Modal title") != DCC_OK ||
        dcc_modal_builder_set_components_json(
            &file_modal,
            "[{\"type\":1,\"components\":[]}]"
        ) != DCC_OK) {
        return 1;
    }
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    if (dcc_rest_interaction_response_set_autocomplete(
            &response,
            &file_autocomplete
        ) != DCC_OK) {
        return 1;
    }
    response.files = &valid_file;
    response.file_count = 1U;
    ENDPOINT_EXPECT_REJECTION(
        "autocomplete rejects multipart files",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    if (dcc_rest_interaction_response_set_modal(&response, &file_modal) != DCC_OK) {
        return 1;
    }
    response.files = &valid_file;
    response.file_count = 1U;
    ENDPOINT_EXPECT_REJECTION(
        "modal rejects multipart files",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    if (dcc_rest_interaction_response_set_premium_required(&response) != DCC_OK) {
        return 1;
    }
    response.files = &valid_file;
    response.file_count = 1U;
    ENDPOINT_EXPECT_REJECTION(
        "premium response rejects multipart files",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    if (dcc_rest_interaction_response_set_launch_activity(&response) != DCC_OK) {
        return 1;
    }
    response.files = &valid_file;
    response.file_count = 1U;
    ENDPOINT_EXPECT_REJECTION(
        "launch activity rejects multipart files",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );
    dcc_message_builder_t malformed_message = DCC_MESSAGE_BUILDER_INIT;
    malformed_message.version++;
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    response.type = DCC_INTERACTION_RESPONSE_CHANNEL_MESSAGE_WITH_SOURCE;
    response.present = DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE;
    response.data.message = &malformed_message;
    ENDPOINT_EXPECT_REJECTION(
        "malformed nested interaction message",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );
    dcc_message_builder_t deferred_content = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&deferred_content, "not allowed") != DCC_OK) {
        return 1;
    }
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    response.type = DCC_INTERACTION_RESPONSE_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE;
    response.present = DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE;
    response.data.message = &deferred_content;
    ENDPOINT_EXPECT_REJECTION(
        "deferred interaction rejects content",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );
    dcc_message_builder_t deferred_other_flags = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_flags(
            &deferred_other_flags,
            DCC_MESSAGE_FLAG_SUPPRESS_EMBEDS
        ) != DCC_OK) {
        return 1;
    }
    response = (dcc_rest_interaction_response_t)DCC_REST_INTERACTION_RESPONSE_INIT;
    response.type = DCC_INTERACTION_RESPONSE_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE;
    response.present = DCC_REST_INTERACTION_RESPONSE_PRESENT_MESSAGE;
    response.data.message = &deferred_other_flags;
    ENDPOINT_EXPECT_REJECTION(
        "deferred interaction rejects non-ephemeral flags",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_response_create(
            client, 125U, "token", &response, &options, &request
        )
    );
    ENDPOINT_EXPECT_REJECTION(
        "original response rejects empty token",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_interaction_original_response_get(
            client, 125U, "", &options, &request
        )
    );

    dcc_rest_webhook_builder_t webhook_builder = DCC_REST_WEBHOOK_BUILDER_INIT;
    webhook_builder.present = DCC_REST_WEBHOOK_BUILDER_PRESENT_AVATAR;
    webhook_builder.avatar = "avatar";
    ENDPOINT_EXPECT_REJECTION(
        "create webhook requires name",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_create_webhook(client, 126U, &webhook_builder, &options, &request)
    );
    webhook_builder.present = DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME |
        DCC_REST_WEBHOOK_BUILDER_PRESENT_CHANNEL_ID;
    webhook_builder.name = "cannot move during create";
    webhook_builder.channel_id = 127U;
    ENDPOINT_EXPECT_REJECTION(
        "create webhook rejects channel id",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_create_webhook(client, 126U, &webhook_builder, &options, &request)
    );
    webhook_builder.present = DCC_REST_WEBHOOK_BUILDER_PRESENT_CHANNEL_ID;
    webhook_builder.channel_id = 127U;
    ENDPOINT_EXPECT_REJECTION(
        "token webhook modify rejects channel id",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_modify_webhook(
            client, 126U, "hook-token", &webhook_builder, &options, &request
        )
    );
    ENDPOINT_EXPECT_REJECTION(
        "optional webhook token rejects empty string",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_get_webhook(client, 126U, "", &options, &request)
    );

    dcc_rest_webhook_execute_t execute = DCC_REST_WEBHOOK_EXECUTE_INIT;
    dcc_rest_webhook_execute_init(&execute, &message);
    ENDPOINT_EXPECT_REJECTION(
        "execute webhook rejects empty token",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_execute_webhook(
            client, 126U, "", &execute, &options, &request
        )
    );
    ENDPOINT_EXPECT_REJECTION(
        "webhook message rejects empty token",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_get_webhook_message(
            client, 126U, "", 128U, NULL, &options, &request
        )
    );
    execute.present = DCC_REST_WEBHOOK_EXECUTE_PRESENT_WAIT;
    execute.wait = 2U;
    ENDPOINT_EXPECT_REJECTION(
        "invalid webhook wait boolean",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_execute_webhook(
            client, 126U, "hook-token", &execute, &options, &request
        )
    );
    execute = (dcc_rest_webhook_execute_t)DCC_REST_WEBHOOK_EXECUTE_INIT;
    execute.message = &message;
    execute.present = DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_NAME |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_ID;
    execute.thread_name = "new thread";
    execute.thread_id = 127U;
    ENDPOINT_EXPECT_REJECTION(
        "webhook new and existing thread conflict",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_execute_webhook(
            client, 126U, "hook-token", &execute, &options, &request
        )
    );
    execute = (dcc_rest_webhook_execute_t)DCC_REST_WEBHOOK_EXECUTE_INIT;
    execute.message = &message;
    execute.present = DCC_REST_WEBHOOK_EXECUTE_PRESENT_APPLIED_TAG_IDS;
    execute.applied_tag_ids = (const dcc_snowflake_t *)(uintptr_t)UINTPTR_MAX;
    execute.applied_tag_count = SIZE_MAX;
    ENDPOINT_EXPECT_REJECTION(
        "overflowed webhook applied tag count",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_execute_webhook(
            client, 126U, "hook-token", &execute, &options, &request
        )
    );

    dcc_rest_message_payload_t valid_payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&valid_payload, &message);
    dcc_rest_webhook_message_edit_t edit = DCC_REST_WEBHOOK_MESSAGE_EDIT_INIT;
    dcc_rest_webhook_message_edit_init(&edit, &valid_payload);
    edit.present = DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_WITH_COMPONENTS;
    edit.with_components = 2U;
    ENDPOINT_EXPECT_REJECTION(
        "invalid webhook edit with_components boolean",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_modify_webhook_message(
            client, 126U, "hook-token", 128U, &edit, &options, &request
        )
    );

    dcc_rest_webhook_compat_payload_t compat =
        DCC_REST_WEBHOOK_COMPAT_PAYLOAD_INIT;
    compat.version++;
    ENDPOINT_EXPECT_REJECTION(
        "unsupported opaque webhook payload version",
        DCC_ERR_INVALID_ARG,
        capture,
        callback,
        observer,
        dcc_rest_execute_webhook_github(
            client, 126U, "hook-token", &compat, &options, &request
        )
    );
    dcc_status_t rejection_drain_status = dcc_rest_async_wait(client, 3000U);
    endpoint_sleep_ms(25U);
    if (rejection_drain_status != DCC_OK ||
        atomic_load_explicit(&capture->calls, memory_order_acquire) !=
            rejection_capture_baseline ||
        atomic_load_explicit(&callback->calls, memory_order_acquire) !=
            rejection_callback_baseline ||
        atomic_load_explicit(&observer->calls, memory_order_acquire) !=
            rejection_observer_baseline) {
        fprintf(
            stderr,
            "rejection quiescence detail drain=%s capture=%u/%u callback=%u/%u "
            "observer=%u/%u\n",
            dcc_status_string(rejection_drain_status),
            rejection_capture_baseline,
            atomic_load_explicit(&capture->calls, memory_order_acquire),
            rejection_callback_baseline,
            atomic_load_explicit(&callback->calls, memory_order_acquire),
            rejection_observer_baseline,
            atomic_load_explicit(&observer->calls, memory_order_acquire)
        );
        return 1;
    }
    return 0;
}

static int endpoint_preflight_precedence_contract(dcc_client_t *client) {
    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&message, "preflight") != DCC_OK) {
        return 1;
    }
    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&payload, &message);

    dcc_rest_request_t *request = (dcc_rest_request_t *)(uintptr_t)1U;
    dcc_endpoint_test_allocation_probe_begin(0U);
    dcc_status_t followup_status = dcc_rest_interaction_followup_edit(
        client, 0U, "token", 1U, &payload, NULL, &request
    );
    size_t followup_allocations = dcc_endpoint_test_allocation_probe_end();
    if (followup_status != DCC_ERR_INVALID_ARG || request != NULL ||
        followup_allocations != 0U) {
        fprintf(
            stderr,
            "followup preflight precedence failed: status=%s request=%p allocations=%zu\n",
            dcc_status_string(followup_status),
            (void *)request,
            followup_allocations
        );
        return 1;
    }

    dcc_rest_webhook_message_query_t query =
        DCC_REST_WEBHOOK_MESSAGE_QUERY_INIT;
    query.present = DCC_REST_WEBHOOK_MESSAGE_QUERY_PRESENT_THREAD_ID;
    query.thread_id = 1U;
    request = (dcc_rest_request_t *)(uintptr_t)1U;
    dcc_endpoint_test_allocation_probe_begin(0U);
    dcc_status_t webhook_status = dcc_rest_get_webhook_message(
        client, 0U, "token", 1U, &query, NULL, &request
    );
    size_t webhook_allocations = dcc_endpoint_test_allocation_probe_end();
    if (webhook_status != DCC_ERR_INVALID_ARG || request != NULL ||
        webhook_allocations != 0U) {
        fprintf(
            stderr,
            "webhook preflight precedence failed: status=%s request=%p allocations=%zu\n",
            dcc_status_string(webhook_status),
            (void *)request,
            webhook_allocations
        );
        return 1;
    }

    static const unsigned char file_bytes[] = {1U};
    dcc_rest_multipart_file_t file = {
        "files[0]", "pong.bin", NULL, file_bytes, sizeof(file_bytes),
    };
    dcc_rest_interaction_response_t response = DCC_REST_INTERACTION_RESPONSE_INIT;
    if (dcc_rest_interaction_response_set_pong(&response) != DCC_OK) {
        return 1;
    }
    response.files = &file;
    response.file_count = 1U;
    request = (dcc_rest_request_t *)(uintptr_t)1U;
    dcc_endpoint_test_allocation_probe_begin(0U);
    dcc_status_t interaction_status = dcc_rest_interaction_response_create(
        client, 1U, "token", &response, NULL, &request
    );
    size_t interaction_allocations = dcc_endpoint_test_allocation_probe_end();
    if (interaction_status != DCC_ERR_INVALID_ARG || request != NULL ||
        interaction_allocations != 0U) {
        fprintf(
            stderr,
            "interaction preflight precedence failed: status=%s request=%p allocations=%zu\n",
            dcc_status_string(interaction_status),
            (void *)request,
            interaction_allocations
        );
        return 1;
    }

    dcc_modal_builder_t invalid_modal = DCC_MODAL_BUILDER_INIT;
    response = (dcc_rest_interaction_response_t)
        DCC_REST_INTERACTION_RESPONSE_INIT;
    response.type = DCC_INTERACTION_RESPONSE_MODAL;
    response.present = DCC_REST_INTERACTION_RESPONSE_PRESENT_MODAL;
    response.data.modal = &invalid_modal;
    request = (dcc_rest_request_t *)(uintptr_t)1U;
    dcc_endpoint_test_allocation_probe_begin(0U);
    interaction_status = dcc_rest_interaction_response_create(
        client, 1U, "token", &response, NULL, &request
    );
    interaction_allocations = dcc_endpoint_test_allocation_probe_end();
    if (interaction_status != DCC_ERR_INVALID_ARG || request != NULL ||
        interaction_allocations != 0U) {
        fprintf(
            stderr,
            "modal preflight precedence failed: status=%s request=%p allocations=%zu\n",
            dcc_status_string(interaction_status),
            (void *)request,
            interaction_allocations
        );
        return 1;
    }

    dcc_autocomplete_choice_t invalid_choice = DCC_AUTOCOMPLETE_CHOICE_INIT;
    invalid_choice.present = DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE;
    invalid_choice.value_type = DCC_AUTOCOMPLETE_CHOICE_STRING;
    invalid_choice.value_string = "value";
    dcc_autocomplete_builder_t invalid_autocomplete =
        DCC_AUTOCOMPLETE_BUILDER_INIT;
    invalid_autocomplete.present = DCC_AUTOCOMPLETE_BUILDER_PRESENT_CHOICES;
    invalid_autocomplete.choices = &invalid_choice;
    invalid_autocomplete.choices_count = 1U;
    response = (dcc_rest_interaction_response_t)
        DCC_REST_INTERACTION_RESPONSE_INIT;
    response.type = DCC_INTERACTION_RESPONSE_AUTOCOMPLETE;
    response.present = DCC_REST_INTERACTION_RESPONSE_PRESENT_AUTOCOMPLETE;
    response.data.autocomplete = &invalid_autocomplete;
    request = (dcc_rest_request_t *)(uintptr_t)1U;
    dcc_endpoint_test_allocation_probe_begin(0U);
    interaction_status = dcc_rest_interaction_response_create(
        client, 1U, "token", &response, NULL, &request
    );
    interaction_allocations = dcc_endpoint_test_allocation_probe_end();
    if (interaction_status != DCC_ERR_INVALID_ARG || request != NULL ||
        interaction_allocations != 0U) {
        fprintf(
            stderr,
            "autocomplete preflight precedence failed: status=%s request=%p allocations=%zu\n",
            dcc_status_string(interaction_status),
            (void *)request,
            interaction_allocations
        );
        return 1;
    }
    return 0;
}

static int endpoint_message_payload_success_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture
) {
    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&message, "create-message-json") != DCC_OK) {
        return 1;
    }
    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&payload, &message);
    ENDPOINT_EXPECT_CALL(
        "create message zero-file JSON",
        capture,
        dcc_rest_create_message(client, 100U, &payload, NULL, &request),
        "POST",
        "/channels/100/messages",
        "application/json",
        "create-message-json"
    );

    if (dcc_message_builder_set_content(&message, "covered-message-prefix") != DCC_OK) {
        return 1;
    }
    payload = (dcc_rest_message_payload_t)DCC_REST_MESSAGE_PAYLOAD_INIT;
    payload.size = offsetof(dcc_rest_message_payload_t, files);
    payload.message = &message;
    ENDPOINT_EXPECT_CALL(
        "covered historical message payload prefix",
        capture,
        dcc_rest_create_message(client, 100U, &payload, NULL, &request),
        "POST",
        "/channels/100/messages",
        "application/json",
        "covered-message-prefix"
    );

    static const unsigned char file_only_bytes[] = {'f', 0U, 'i', 'l', 'e'};
    dcc_rest_multipart_file_t file = {
        "files[0]",
        "file-only.bin",
        "application/octet-stream",
        file_only_bytes,
        sizeof(file_only_bytes),
    };
    payload = (dcc_rest_message_payload_t)DCC_REST_MESSAGE_PAYLOAD_INIT;
    payload.files = &file;
    payload.file_count = 1U;
    ENDPOINT_EXPECT_CALL(
        "create message file-only multipart",
        capture,
        dcc_rest_create_message(client, 100U, &payload, NULL, &request),
        "POST",
        "/channels/100/messages",
        "multipart/form-data; boundary=",
        "file-only.bin"
    );
    if (!endpoint_contains(
            capture->body,
            capture->body_len,
            "name=\"payload_json\"",
            19U
        ) ||
        !endpoint_contains(capture->body, capture->body_len, "\r\n\r\n{}\r\n", 8U) ||
        !endpoint_contains(
            capture->body,
            capture->body_len,
            file_only_bytes,
            sizeof(file_only_bytes)
        )) {
        fprintf(stderr, "file-only message did not preserve empty payload_json or bytes\n");
        return 1;
    }
    return 0;
}

static int endpoint_message_routes_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture
) {
    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&message, "message-route-body") != DCC_OK) {
        return 1;
    }
    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&payload, &message);

    dcc_rest_message_list_query_t list_query = DCC_REST_MESSAGE_LIST_QUERY_INIT;
    dcc_rest_message_list_query_init(&list_query);
    dcc_rest_message_list_query_init(NULL);
    list_query.present = DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AROUND |
        DCC_REST_MESSAGE_LIST_QUERY_PRESENT_LIMIT;
    list_query.around = UINT64_C(202);
    list_query.limit = 25U;
    ENDPOINT_EXPECT_CALL(
        "message list defaults",
        capture,
        dcc_rest_get_channel_messages(client, 101U, NULL, NULL, &request),
        "GET",
        "/channels/101/messages",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "message list query",
        capture,
        dcc_rest_get_channel_messages(client, 101U, &list_query, NULL, &request),
        "GET",
        "/channels/101/messages?around=202&limit=25",
        NULL,
        NULL
    );
    struct extended_message_list_query {
        dcc_rest_message_list_query_t query;
        uint64_t future_tail;
    } extended_query = {
        DCC_REST_MESSAGE_LIST_QUERY_INIT,
        UINT64_C(0),
    };
    extended_query.query.size = sizeof(extended_query);
    extended_query.query.present = DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AFTER;
    extended_query.query.after = UINT64_C(203);
    ENDPOINT_EXPECT_CALL(
        "larger message list query",
        capture,
        dcc_rest_get_channel_messages(
            client, 101U, &extended_query.query, NULL, &request
        ),
        "GET",
        "/channels/101/messages?after=203",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "get message",
        capture,
        dcc_rest_get_message(client, 101U, 303U, NULL, &request),
        "GET",
        "/channels/101/messages/303",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "edit message",
        capture,
        dcc_rest_edit_message(client, 101U, 303U, &payload, NULL, &request),
        "PATCH",
        "/channels/101/messages/303",
        "application/json",
        "message-route-body"
    );
    static const unsigned char edit_file_bytes[] = {'e', 0U, 'd', 'i', 't'};
    dcc_rest_multipart_file_t edit_file = {
        "files[0]",
        "edit-message.bin",
        "application/octet-stream",
        edit_file_bytes,
        sizeof(edit_file_bytes),
    };
    payload.files = &edit_file;
    payload.file_count = 1U;
    ENDPOINT_EXPECT_CALL(
        "edit message multipart",
        capture,
        dcc_rest_edit_message(client, 101U, 303U, &payload, NULL, &request),
        "PATCH",
        "/channels/101/messages/303",
        "multipart/form-data; boundary=",
        "edit-message.bin"
    );
    if (!endpoint_contains(
            capture->body,
            capture->body_len,
            edit_file_bytes,
            sizeof(edit_file_bytes)
        ) ||
        !endpoint_contains(
            capture->body,
            capture->body_len,
            "message-route-body",
            18U
        )) {
        fprintf(stderr, "edit-message multipart lost payload_json or file bytes\n");
        return 1;
    }
    payload.files = NULL;
    payload.file_count = 0U;
    ENDPOINT_EXPECT_CALL(
        "delete message",
        capture,
        dcc_rest_delete_message(client, 101U, 303U, NULL, &request),
        "DELETE",
        "/channels/101/messages/303",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "crosspost message",
        capture,
        dcc_rest_crosspost_message(client, 101U, 303U, NULL, &request),
        "POST",
        "/channels/101/messages/303/crosspost",
        NULL,
        NULL
    );

    const dcc_snowflake_t bulk_ids[] = {UINT64_C(401), UINT64_C(402)};
    ENDPOINT_EXPECT_CALL(
        "bulk delete messages",
        capture,
        dcc_rest_bulk_delete_messages(
            client,
            101U,
            bulk_ids,
            sizeof(bulk_ids) / sizeof(bulk_ids[0]),
            NULL,
            &request
        ),
        "POST",
        "/channels/101/messages/bulk-delete",
        "application/json",
        "401"
    );
    if (!endpoint_contains(capture->body, capture->body_len, "402", 3U)) {
        fprintf(stderr, "bulk delete body omitted the second snowflake\n");
        return 1;
    }

    const char *reaction = "party:42";
    const char *reaction_path = "/channels/101/messages/303/reactions/party%3A42";
    ENDPOINT_EXPECT_CALL(
        "add own reaction",
        capture,
        dcc_rest_add_message_reaction(
            client, 101U, 303U, reaction, NULL, &request
        ),
        "PUT",
        "/channels/101/messages/303/reactions/party%3A42/@me",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "delete own reaction",
        capture,
        dcc_rest_delete_own_message_reaction(
            client, 101U, 303U, reaction, NULL, &request
        ),
        "DELETE",
        "/channels/101/messages/303/reactions/party%3A42/@me",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "delete user reaction",
        capture,
        dcc_rest_delete_user_message_reaction(
            client, 101U, 303U, reaction, 505U, NULL, &request
        ),
        "DELETE",
        "/channels/101/messages/303/reactions/party%3A42/505",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "delete all reactions",
        capture,
        dcc_rest_delete_all_message_reactions(
            client, 101U, 303U, NULL, &request
        ),
        "DELETE",
        "/channels/101/messages/303/reactions",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "delete all emoji reactions",
        capture,
        dcc_rest_delete_all_message_reactions_for_emoji(
            client, 101U, 303U, reaction, NULL, &request
        ),
        "DELETE",
        reaction_path,
        NULL,
        NULL
    );

    dcc_rest_reaction_query_t reaction_query = DCC_REST_REACTION_QUERY_INIT;
    dcc_rest_reaction_query_init(&reaction_query);
    dcc_rest_reaction_query_init(NULL);
    reaction_query.present = DCC_REST_REACTION_QUERY_PRESENT_TYPE |
        DCC_REST_REACTION_QUERY_PRESENT_AFTER |
        DCC_REST_REACTION_QUERY_PRESENT_LIMIT;
    reaction_query.type = DCC_REST_REACTION_NORMAL;
    reaction_query.after = UINT64_C(606);
    reaction_query.limit = 20U;
    ENDPOINT_EXPECT_CALL(
        "get normal reactions page",
        capture,
        dcc_rest_get_message_reactions(
            client, 101U, 303U, reaction, &reaction_query, NULL, &request
        ),
        "GET",
        "/channels/101/messages/303/reactions/party%3A42?type=0&after=606&limit=20",
        NULL,
        NULL
    );
    reaction_query.type = DCC_REST_REACTION_BURST;
    ENDPOINT_EXPECT_CALL(
        "get burst reactions page",
        capture,
        dcc_rest_get_message_reactions(
            client, 101U, 303U, reaction, &reaction_query, NULL, &request
        ),
        "GET",
        "/channels/101/messages/303/reactions/party%3A42?type=1&after=606&limit=20",
        NULL,
        NULL
    );
    reaction_query.size = offsetof(dcc_rest_reaction_query_t, limit);
    reaction_query.present = DCC_REST_REACTION_QUERY_PRESENT_TYPE |
        DCC_REST_REACTION_QUERY_PRESENT_AFTER;
    reaction_query.type = DCC_REST_REACTION_NORMAL;
    ENDPOINT_EXPECT_CALL(
        "covered historical reaction query prefix",
        capture,
        dcc_rest_get_message_reactions(
            client, 101U, 303U, reaction, &reaction_query, NULL, &request
        ),
        "GET",
        "/channels/101/messages/303/reactions/party%3A42?type=0&after=606",
        NULL,
        NULL
    );

    dcc_rest_id_page_t id_page = DCC_REST_ID_PAGE_INIT;
    dcc_rest_id_page_init(&id_page);
    dcc_rest_id_page_init(NULL);
    id_page.present = DCC_REST_ID_PAGE_PRESENT_AFTER |
        DCC_REST_ID_PAGE_PRESENT_LIMIT;
    id_page.after = UINT64_C(606);
    id_page.limit = 20U;
    ENDPOINT_EXPECT_CALL(
        "get poll answer voters",
        capture,
        dcc_rest_get_poll_answer_voters(
            client, 101U, 303U, 7U, &id_page, NULL, &request
        ),
        "GET",
        "/channels/101/polls/303/answers/7?after=606&limit=20",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "end poll",
        capture,
        dcc_rest_end_poll(client, 101U, 303U, NULL, &request),
        "POST",
        "/channels/101/polls/303/expire",
        NULL,
        NULL
    );

    dcc_rest_pin_page_t pin_page = DCC_REST_PIN_PAGE_INIT;
    dcc_rest_pin_page_init(&pin_page);
    dcc_rest_pin_page_init(NULL);
    pin_page.present = DCC_REST_PIN_PAGE_PRESENT_BEFORE |
        DCC_REST_PIN_PAGE_PRESENT_LIMIT;
    pin_page.before = "2026-08-11T01:02:03+09:00";
    pin_page.limit = 17U;
    ENDPOINT_EXPECT_CALL(
        "get channel pins",
        capture,
        dcc_rest_get_channel_pins(client, 101U, &pin_page, NULL, &request),
        "GET",
        "/channels/101/messages/pins?before=2026-08-11T01%3A02%3A03%2B09%3A00&limit=17",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "pin message",
        capture,
        dcc_rest_pin_message(client, 101U, 303U, NULL, &request),
        "PUT",
        "/channels/101/messages/pins/303",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "unpin message",
        capture,
        dcc_rest_unpin_message(client, 101U, 303U, NULL, &request),
        "DELETE",
        "/channels/101/messages/pins/303",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "legacy pin message",
        capture,
        dcc_rest_legacy_pin_message(client, 101U, 303U, NULL, &request),
        "PUT",
        "/channels/101/pins/303",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "legacy unpin message",
        capture,
        dcc_rest_legacy_unpin_message(client, 101U, 303U, NULL, &request),
        "DELETE",
        "/channels/101/pins/303",
        NULL,
        NULL
    );
    ENDPOINT_EXPECT_CALL(
        "legacy get pins",
        capture,
        dcc_rest_get_legacy_channel_pins(client, 101U, NULL, &request),
        "GET",
        "/channels/101/pins",
        NULL,
        NULL
    );
    return 0;
}


static int endpoint_cancel_destroy_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback
) {
    static const unsigned char file_bytes[] = {'c', 0U, 'a', 'n', 0xffU};
    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&message, "cancel multipart") != DCC_OK) {
        return 1;
    }
    dcc_rest_multipart_file_t file = {
        "files[0]", "cancel.bin", "application/octet-stream",
        file_bytes, sizeof(file_bytes),
    };
    dcc_rest_webhook_execute_t execute = DCC_REST_WEBHOOK_EXECUTE_INIT;
    dcc_rest_webhook_execute_init(&execute, &message);
    execute.files = &file;
    execute.file_count = 1U;

    endpoint_callback_reset(callback);
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    options.callback = endpoint_result_callback;
    options.user_data = callback;
    dcc_rest_request_t *request = NULL;
    callback->request_slot = &request;
    endpoint_capture_reset(capture, 250U);
    uint64_t started = endpoint_now_ms();
    dcc_status_t submit_status = dcc_rest_execute_webhook(
        client,
        950U,
        "cancel-token",
        &execute,
        &options,
        &request
    );
    uint64_t elapsed = endpoint_now_ms() - started;
    int entered = submit_status == DCC_OK && request != NULL &&
        endpoint_wait_for_atomic(&capture->entered, 1U, 1000U);
    dcc_status_t cancel_status = entered
        ? dcc_rest_request_cancel(request)
        : DCC_ERR_STATE;
    dcc_rest_request_destroy(request);
    request = NULL;
    atomic_store_explicit(&capture->capture_release, 1U, memory_order_release);
    dcc_status_t drain_status = dcc_rest_async_wait(client, 3000U);

    int ok = submit_status == DCC_OK && elapsed < 100U && entered &&
        cancel_status == DCC_OK && drain_status == DCC_OK &&
        atomic_load_explicit(&callback->calls, memory_order_acquire) == 1U &&
        callback->result != NULL &&
        dcc_rest_result_status(callback->result) == DCC_ERR_CANCELED &&
        atomic_load_explicit(&capture->calls, memory_order_acquire) == 1U &&
        strcmp(capture->method, "POST") == 0 &&
        strcmp(capture->path, "/webhooks/950/cancel-token") == 0 &&
        strstr(capture->content_type, "multipart/form-data; boundary=") != NULL &&
        endpoint_contains(capture->body, capture->body_len, "cancel.bin", 10U) &&
        endpoint_contains(
            capture->body,
            capture->body_len,
            file_bytes,
            sizeof(file_bytes)
        );
    if (!ok) {
        fprintf(
            stderr,
            "cancel/destroy detail submit=%s elapsed=%llu entered=%d cancel=%s "
            "drain=%s callback=%u result=%s capture=%u method=%s path=%s\n",
            dcc_status_string(submit_status),
            (unsigned long long)elapsed,
            entered,
            dcc_status_string(cancel_status),
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

static int endpoint_admission_rejection_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback,
    endpoint_observer_t *observer
) {
    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&message, "valid after stop") != DCC_OK) {
        return 1;
    }
    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&payload, &message);
    endpoint_callback_reset(callback);
    unsigned capture_calls = atomic_load_explicit(
        &capture->calls,
        memory_order_acquire
    );
    unsigned callback_calls = atomic_load_explicit(
        &callback->calls,
        memory_order_acquire
    );
    unsigned observer_calls = atomic_load_explicit(
        &observer->calls,
        memory_order_acquire
    );
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    options.callback = endpoint_result_callback;
    options.user_data = callback;
    ENDPOINT_EXPECT_REJECTION(
        "stopped-client admission rejection",
        DCC_ERR_STATE,
        capture,
        callback,
        observer,
        dcc_rest_create_message(client, 999U, &payload, &options, &request)
    );
    endpoint_sleep_ms(50U);
    if (atomic_load_explicit(&capture->calls, memory_order_acquire) != capture_calls ||
        atomic_load_explicit(&callback->calls, memory_order_acquire) != callback_calls ||
        atomic_load_explicit(&observer->calls, memory_order_acquire) != observer_calls) {
        fprintf(stderr, "stopped-client rejection produced delayed work\n");
        return 1;
    }
    return 0;
}

int main(void) {
    dcc_client_options_t client_options = {
        .size = sizeof(client_options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .rest_concurrency = 1U,
    };
    dcc_client_t *client = NULL;
    if (dcc_client_create(&client_options, &client) != DCC_OK ||
        dcc_client_start(client) != DCC_OK) {
        dcc_client_destroy(client);
        return 1;
    }

    endpoint_capture_t capture;
    memset(&capture, 0, sizeof(capture));
    atomic_init(&capture.calls, 0U);
    atomic_init(&capture.delay_ms, 0U);
    atomic_init(&capture.entered, 0U);
    atomic_init(&capture.capture_release, 1U);
    dcc_rest_set_interceptor(client, endpoint_intercept, &capture);

    endpoint_callback_t callback;
    memset(&callback, 0, sizeof(callback));
    atomic_init(&callback.calls, 0U);
    atomic_init(&callback.published_handle, 0U);
    endpoint_observer_t observer;
    atomic_init(&observer.calls, 0U);
    if (dcc_client_on_error(client, endpoint_error_observer, &observer) != DCC_OK) {
        dcc_client_destroy(client);
        return 1;
    }

    endpoint_runner_t runner = {.client = client, .status = DCC_ERR_STATE};
    pthread_t runner_thread;
    if (pthread_create(&runner_thread, NULL, endpoint_runner_main, &runner) != 0) {
        dcc_client_destroy(client);
        return 1;
    }

    int failed = endpoint_transition_submission_contract(client, &capture);
    dcc_rest_set_interceptor(client, endpoint_intercept, &capture);
    if (!failed) {
        failed = endpoint_rejection_contract(client, &capture, &callback, &observer);
    }
    if (!failed) {
        failed = endpoint_nested_message_preflight_contract(
            client,
            &capture,
            &callback,
            &observer
        );
    }
    if (!failed) {
        failed = endpoint_multipart_overflow_contract(
            client,
            &capture,
            &callback,
            &observer
        );
    }
    if (!failed) {
        failed = endpoint_preflight_precedence_contract(client);
    }
    if (!failed) {
        failed = endpoint_message_payload_success_contract(client, &capture);
    }
    if (!failed) {
        failed = endpoint_create_message_ownership_contract(client, &capture, &callback);
    }
    if (!failed) {
        failed = endpoint_auto_release_contract(client, &capture, &callback);
    }
    if (!failed) {
        failed = endpoint_message_routes_contract(client, &capture);
    }
    if (!failed) {
        failed = endpoint_interaction_tags_contract(client, &capture);
    }
    if (!failed) {
        failed = endpoint_interaction_crud_contract(client, &capture);
    }
    if (!failed) {
        failed = endpoint_webhook_contract(client, &capture);
    }
    if (!failed) {
        failed = endpoint_versioned_prefix_contract(client, &capture);
    }
    if (!failed) {
        failed = endpoint_cancel_destroy_contract(client, &capture, &callback);
    }
    dcc_status_t stop_status = dcc_client_stop(client);
    (void)pthread_join(runner_thread, NULL);
    if (!failed && stop_status == DCC_OK) {
        failed = endpoint_admission_rejection_contract(
            client,
            &capture,
            &callback,
            &observer
        );
    }
    if (!failed && stop_status == DCC_OK) {
        failed = endpoint_transition_rejection_contract(client);
    }
    dcc_client_destroy(client);
    dcc_rest_result_free(callback.result);
    if (failed || stop_status != DCC_OK || runner.status != DCC_OK) {
        fprintf(stderr, "REST v2 endpoint smoke failed\n");
        return 1;
    }
    return 0;
}

#endif
