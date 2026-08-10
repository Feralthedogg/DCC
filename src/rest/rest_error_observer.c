#include "internal/rest/dcc_rest_error_observer_internal.h"

#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"
#include "internal/json/dcc_json_dom_api.h"
#include "internal/rest/dcc_rest_runtime_internal.h"

#include <limits.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

typedef struct dcc_rest_observer_snapshot {
    dcc_client_error_fn public_handler;
    void *public_user_data;
    dcc_client_error_fn app_sink;
    void *app_sink_user_data;
} dcc_rest_observer_snapshot_t;

typedef struct dcc_rest_result_view {
    dcc_rest_result_t result;
    dcc_json_t *json;
} dcc_rest_result_view_t;

typedef struct dcc_rest_terminal_frame {
    dcc_client_t *client;
    struct dcc_rest_terminal_frame *previous;
} dcc_rest_terminal_frame_t;

static _Thread_local dcc_rest_terminal_frame_t *dcc_rest_current_terminal_frame;

static void dcc_rest_terminal_enter(
    dcc_rest_terminal_frame_t *frame,
    dcc_client_t *client
) {
    frame->client = client;
    frame->previous = dcc_rest_current_terminal_frame;
    atomic_fetch_add_explicit(
        &client->rest_terminal_in_flight,
        1U,
        memory_order_acq_rel
    );
    dcc_rest_current_terminal_frame = frame;
}

static void dcc_rest_terminal_leave(dcc_rest_terminal_frame_t *frame) {
    if (frame == NULL || dcc_rest_current_terminal_frame != frame) {
        return;
    }
    dcc_rest_current_terminal_frame = frame->previous;
    atomic_fetch_sub_explicit(
        &frame->client->rest_terminal_in_flight,
        1U,
        memory_order_acq_rel
    );
}

uint8_t dcc_rest_terminal_callback_active(const dcc_client_t *client) {
    for (dcc_rest_terminal_frame_t *frame = dcc_rest_current_terminal_frame;
         frame != NULL;
         frame = frame->previous) {
        if (frame->client == client) {
            return 1U;
        }
    }
    return 0U;
}

void dcc_rest_terminal_wait(dcc_client_t *client) {
    if (client == NULL) {
        return;
    }
    while (atomic_load_explicit(
            &client->rest_terminal_in_flight,
            memory_order_acquire
        ) != 0U) {
        dcc_rest_sleep_ms(1U);
    }
}

static void dcc_rest_error_observer_lock(dcc_client_t *client) {
    while (atomic_flag_test_and_set_explicit(
            &client->rest_error_observer_lock,
            memory_order_acquire
        )) {
    }
}

static void dcc_rest_error_observer_unlock(dcc_client_t *client) {
    atomic_flag_clear_explicit(&client->rest_error_observer_lock, memory_order_release);
}

static dcc_rest_observer_snapshot_t dcc_rest_error_observer_snapshot(
    dcc_client_t *client
) {
    dcc_rest_observer_snapshot_t snapshot;
    memset(&snapshot, 0, sizeof(snapshot));
    dcc_rest_error_observer_lock(client);
    snapshot.public_handler = client->rest_error_observer;
    snapshot.public_user_data = client->rest_error_observer_user_data;
    snapshot.app_sink = client->rest_app_error_sink;
    snapshot.app_sink_user_data = client->rest_app_error_sink_user_data;
    if (snapshot.app_sink != NULL) {
        atomic_fetch_add_explicit(
            &client->rest_app_error_sink_in_flight,
            1U,
            memory_order_acq_rel
        );
    }
    dcc_rest_error_observer_unlock(client);
    return snapshot;
}

void dcc_rest_clear_app_error_sink_and_wait(dcc_client_t *client) {
    if (client == NULL) {
        return;
    }
    dcc_rest_error_observer_lock(client);
    client->rest_app_error_sink = NULL;
    client->rest_app_error_sink_user_data = NULL;
    dcc_rest_error_observer_unlock(client);
    while (atomic_load_explicit(
            &client->rest_app_error_sink_in_flight,
            memory_order_acquire
        ) != 0U) {
        dcc_rest_sleep_ms(1U);
    }
}

dcc_status_t dcc_client_on_error(
    dcc_client_t *client,
    dcc_client_error_fn handler,
    void *user_data
) {
    if (client == NULL || (handler == NULL && user_data != NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_rest_error_observer_lock(client);
    client->rest_error_observer = handler;
    client->rest_error_observer_user_data = user_data;
    dcc_rest_error_observer_unlock(client);
    return DCC_OK;
}

dcc_status_t dcc_rest_set_app_error_sink(
    dcc_client_t *client,
    dcc_client_error_fn handler,
    void *user_data
) {
    if (client == NULL || (handler == NULL && user_data != NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_rest_error_observer_lock(client);
    client->rest_app_error_sink = handler;
    client->rest_app_error_sink_user_data = user_data;
    dcc_rest_error_observer_unlock(client);
    return DCC_OK;
}

static void dcc_rest_result_view_init(
    dcc_rest_result_view_t *view,
    const dcc_rest_terminal_completion_t *completion
) {
    memset(view, 0, sizeof(*view));
    view->result = (dcc_rest_result_t){
        .size = sizeof(view->result),
        .version = DCC_REST_RESULT_VERSION,
        .transport_status = completion->transport_status,
        .http_status = completion->http_status,
        .body = completion->body,
        .body_len = completion->body_len,
        .retry_after_ms = completion->retry_after_ms,
    };
    if (completion->body == NULL || completion->body_len == 0U ||
        dcc_json_parse(completion->body, completion->body_len, &view->json) != DCC_OK ||
        dcc_json_typeof(view->json) != DCC_JSON_OBJECT) {
        dcc_json_free(view->json);
        view->json = NULL;
        return;
    }

    int64_t code = 0;
    if (dcc_json_i64(dcc_json_object_get(view->json, "code"), &code) == 0 &&
        code >= INT32_MIN && code <= INT32_MAX) {
        view->result.discord_code = (int32_t)code;
    }
    view->result.discord_message = dcc_json_string(
        dcc_json_object_get(view->json, "message")
    );
}

static void dcc_rest_result_view_deinit(dcc_rest_result_view_t *view) {
    dcc_json_free(view->json);
    view->json = NULL;
}

static void dcc_rest_log_error(
    dcc_client_t *client,
    const dcc_error_t *error
) {
    char message[512];
    (void)snprintf(
        message,
        sizeof(message),
        "REST failure operation=%s status=%s http=%u discord=%d message=%s",
        error->operation != NULL ? error->operation : "(unknown)",
        dcc_status_string(error->status),
        (unsigned)error->http_status,
        (int)error->discord_code,
        error->message != NULL ? error->message : "(none)"
    );
    dcc_emit_log(client, DCC_LOG_ERROR, message);
}

void dcc_rest_deliver_terminal(
    dcc_client_t *client,
    const dcc_rest_terminal_completion_t *completion,
    dcc_rest_cb callback,
    void *callback_user_data
) {
    if (client == NULL || completion == NULL) {
        return;
    }

    dcc_rest_terminal_frame_t terminal_frame;
    dcc_rest_terminal_enter(&terminal_frame, client);
    dcc_rest_result_view_t view;
    dcc_rest_result_view_init(&view, completion);
    dcc_status_t mapped_status = dcc_rest_result_status(&view.result);
    dcc_rest_observer_snapshot_t observers;
    memset(&observers, 0, sizeof(observers));
    if (mapped_status != DCC_OK) {
        observers = dcc_rest_error_observer_snapshot(client);
    }

    if (callback != NULL) {
        dcc_rest_response_t response = {
            .size = sizeof(response),
            .status = completion->http_status,
            .error = completion->legacy_error,
            .body = completion->body,
            .body_len = completion->body_len,
        };
        callback(client, &response, callback_user_data);
    }

    if (mapped_status != DCC_OK) {
        dcc_error_t error = {
            .size = sizeof(error),
            .version = DCC_ERROR_VERSION,
            .origin = DCC_ERROR_REST,
            .status = mapped_status,
            .http_status = view.result.http_status,
            .discord_code = view.result.discord_code,
            .operation = completion->operation,
            .message = view.result.discord_message,
            .body = view.result.body,
            .body_len = view.result.body_len,
        };
        dcc_rest_log_error(client, &error);
        if (observers.app_sink != NULL) {
            observers.app_sink(client, &error, observers.app_sink_user_data);
        }
        if (observers.public_handler != NULL) {
            observers.public_handler(client, &error, observers.public_user_data);
        }
    }
    dcc_rest_result_view_deinit(&view);
    if (observers.app_sink != NULL) {
        atomic_fetch_sub_explicit(
            &client->rest_app_error_sink_in_flight,
            1U,
            memory_order_acq_rel
        );
    }
    dcc_rest_terminal_leave(&terminal_frame);
}
