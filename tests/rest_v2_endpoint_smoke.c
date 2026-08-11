#include <dcc/autocomplete.h>
#include <dcc/client.h>
#include <dcc/message.h>
#include <dcc/modal.h>
#include <dcc/rest/interactions.h>
#include <dcc/rest/messages.h>
#include <dcc/rest/request.h>
#include <dcc/rest/webhooks.h>

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

typedef struct endpoint_capture {
    atomic_uint calls;
    atomic_uint delay_ms;
    atomic_uint entered;
    atomic_uint capture_release;
    char method[16];
    char path[512];
    char content_type[256];
    unsigned char body[65536];
    size_t body_len;
} endpoint_capture_t;

typedef struct endpoint_callback {
    atomic_uint calls;
    atomic_uint published_handle;
    dcc_rest_request_t **request_slot;
    dcc_rest_result_t *result;
} endpoint_callback_t;

typedef struct endpoint_observer {
    atomic_uint calls;
} endpoint_observer_t;

typedef struct endpoint_runner {
    dcc_client_t *client;
    dcc_status_t status;
} endpoint_runner_t;

static uint64_t endpoint_now_ms(void) {
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

static void endpoint_result_callback(
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

static int endpoint_contains(
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

static void endpoint_capture_reset(endpoint_capture_t *capture, uint32_t delay_ms) {
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

static int endpoint_wait_for_atomic(
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

static void endpoint_callback_reset(endpoint_callback_t *callback) {
    dcc_rest_result_free(callback->result);
    callback->result = NULL;
    callback->request_slot = NULL;
    atomic_store_explicit(&callback->calls, 0U, memory_order_release);
    atomic_store_explicit(&callback->published_handle, 0U, memory_order_release);
}

static int endpoint_completed_contract(
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
        dcc_status_t rejected_status = (call_); \
        if (rejected_status != (expected_status_) || request != NULL || \
            atomic_load_explicit(&(capture_)->calls, memory_order_acquire) != \
                capture_calls_before || \
            atomic_load_explicit(&(callback_)->calls, memory_order_acquire) != \
                callback_calls_before || \
            atomic_load_explicit(&(observer_)->calls, memory_order_acquire) != \
                observer_calls_before) { \
            fprintf( \
                stderr, \
                "%s rejection detail status=%s request=%p capture=%u/%u " \
                "callback=%u/%u observer=%u/%u\n", \
                (label_), \
                dcc_status_string(rejected_status), \
                (void *)request, \
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

static int endpoint_create_message_ownership_contract(
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

static int endpoint_interaction_tags_contract(
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

static int endpoint_auto_release_contract(
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

static int endpoint_interaction_crud_contract(
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

static int endpoint_webhook_contract(
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

    int failed = endpoint_rejection_contract(client, &capture, &callback, &observer);
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
    dcc_client_destroy(client);
    dcc_rest_result_free(callback.result);
    if (failed || stop_status != DCC_OK || runner.status != DCC_OK) {
        fprintf(stderr, "REST v2 endpoint smoke failed\n");
        return 1;
    }
    return 0;
}

#endif
