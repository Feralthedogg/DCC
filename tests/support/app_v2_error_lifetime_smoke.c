#include "app_v2_error_lifetime_smoke.h"

#include <dcc/app.h>
#include <dcc/client.h>
#include <dcc/error_details.h>
#include <dcc/rest.h>

#include "internal/app/dcc_app_internal.h"
#include "internal/client/dcc_client_state_internal.h"
#include "internal/rest/dcc_rest_error_observer_internal.h"
#include "internal/rest/dcc_rest_intercept_internal.h"
#include "internal/runtime/dcc_runtime_internal.h"
#include "http_smoke_server.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>

#if !defined(_WIN32)
#include <pthread.h>
#include <unistd.h>
#endif

typedef struct lifetime_error_seen {
    unsigned count;
    dcc_status_t status;
    uint16_t http_status;
    char operation[128];
} lifetime_error_seen_t;

typedef struct lifetime_legacy_seen {
    unsigned count;
    uint16_t status;
    dcc_status_t error;
} lifetime_legacy_seen_t;

typedef struct lifetime_dm_seen {
    lifetime_error_seen_t error;
    lifetime_legacy_seen_t legacy;
    unsigned log_count;
    unsigned callback_in_terminal_frame;
} lifetime_dm_seen_t;

typedef struct lifetime_intercept {
    dcc_status_t transport_status;
    uint16_t http_status;
    dcc_status_t legacy_error;
    const char *body;
    size_t body_len;
    unsigned request_count;
    unsigned callback_count;
} lifetime_intercept_t;

typedef struct client_managed_destroy_state {
    dcc_client_t *client;
    atomic_uint returned;
    dcc_status_t post_destroy_stop_status;
} client_managed_destroy_state_t;

static void client_managed_destroy_task(void *user_data) {
    client_managed_destroy_state_t *state =
        (client_managed_destroy_state_t *)user_data;
    dcc_client_destroy(state->client);
    state->post_destroy_stop_status = dcc_client_stop(state->client);
    atomic_store_explicit(&state->returned, 1U, memory_order_release);
}

static int check_managed_destroy_defers_to_owner(void) {
    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_client_t *client = NULL;
    if (dcc_client_create(&options, &client) != DCC_OK ||
        dcc_client_start(client) != DCC_OK) {
        dcc_client_destroy(client);
        return 1;
    }

    client_managed_destroy_state_t state = {
        .client = client,
        .post_destroy_stop_status = DCC_ERR_STATE,
    };
    atomic_init(&state.returned, 0U);
    dcc_status_t spawn_status = dcc_runtime_spawn(
        &client->runtime,
        client_managed_destroy_task,
        &state
    );
    dcc_status_t wait_status = spawn_status == DCC_OK
        ? dcc_client_wait(client)
        : spawn_status;
    unsigned returned = atomic_load_explicit(
        &state.returned,
        memory_order_acquire
    );
    if (spawn_status != DCC_OK || wait_status != DCC_OK || returned != 1U ||
        state.post_destroy_stop_status != DCC_OK) {
        fprintf(
            stderr,
            "managed destroy did not preserve ownership: spawn=%d wait=%d "
            "returned=%u stop=%d\n",
            (int)spawn_status,
            (int)wait_status,
            returned,
            (int)state.post_destroy_stop_status
        );
        dcc_client_destroy(client);
        return 1;
    }
    dcc_client_destroy(client);
    return 0;
}

static dcc_status_t lifetime_intercept(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const void *body,
    size_t body_len,
    const char *content_type,
    dcc_rest_cb cb,
    void *user_data,
    void *intercept_user_data
) {
    (void)method;
    (void)path;
    (void)body;
    (void)body_len;
    (void)content_type;
    lifetime_intercept_t *script =
        (lifetime_intercept_t *)intercept_user_data;
    if (script == NULL) {
        return DCC_ERR_STATE;
    }
    script->request_count++;
    if (script->transport_status != DCC_OK) {
        return script->transport_status;
    }
    if (cb != NULL) {
        dcc_rest_response_t response = {
            .size = sizeof(response),
            .status = script->http_status,
            .error = script->legacy_error,
            .body = script->body,
            .body_len = script->body_len,
        };
        script->callback_count++;
        cb(client, &response, user_data);
    }
    return DCC_OK;
}

static void lifetime_copy_error(
    lifetime_error_seen_t *seen,
    const dcc_error_t *error
) {
    if (seen == NULL || error == NULL) {
        return;
    }
    seen->count++;
    seen->status = error->status;
    seen->http_status = error->http_status;
    snprintf(
        seen->operation,
        sizeof(seen->operation),
        "%s",
        error->operation != NULL ? error->operation : ""
    );
}

static void lifetime_dm_log(
    dcc_log_level_t level,
    const char *message,
    void *user_data
) {
    lifetime_dm_seen_t *seen = (lifetime_dm_seen_t *)user_data;
    if (seen != NULL && level == DCC_LOG_ERROR && message != NULL &&
        strstr(message, "REST failure") != NULL) {
        seen->log_count++;
    }
}

static void lifetime_dm_observer(
    dcc_client_t *client,
    const dcc_error_t *error,
    void *user_data
) {
    (void)client;
    lifetime_dm_seen_t *seen = (lifetime_dm_seen_t *)user_data;
    if (seen != NULL) {
        lifetime_copy_error(&seen->error, error);
    }
}

static void lifetime_dm_legacy(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    lifetime_dm_seen_t *seen = (lifetime_dm_seen_t *)user_data;
    if (seen == NULL || response == NULL) {
        return;
    }
    seen->legacy.count++;
    seen->legacy.status = response->status;
    seen->legacy.error = response->error;
    seen->callback_in_terminal_frame = dcc_rest_terminal_callback_active(client);
}

static int check_composed_dm_terminal_delivery(void) {
    static const char discord_body[] =
        "{\"code\":50007,\"message\":\"Cannot send messages to this user\"}";
    static const char malformed_body[] = "{}";
    lifetime_dm_seen_t seen;
    memset(&seen, 0, sizeof(seen));
    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .log_fn = lifetime_dm_log,
        .log_user_data = &seen,
    };
    dcc_client_t *client = NULL;
    if (dcc_client_create(&options, &client) != DCC_OK ||
        dcc_client_on_error(client, lifetime_dm_observer, &seen) != DCC_OK) {
        dcc_client_destroy(client);
        return 1;
    }

    lifetime_intercept_t script = {
        .transport_status = DCC_OK,
        .http_status = 403U,
        .legacy_error = DCC_OK,
        .body = discord_body,
        .body_len = sizeof(discord_body) - 1U,
    };
    dcc_rest_set_interceptor(client, lifetime_intercept, &script);
    dcc_status_t status = dcc_rest_create_direct_message(
        client,
        42U,
        "{\"content\":\"blocked\"}",
        lifetime_dm_legacy,
        &seen
    );
    if (status != DCC_OK || script.request_count != 1U ||
        script.callback_count != 1U || seen.legacy.count != 1U ||
        seen.legacy.status != 403U || seen.legacy.error != DCC_ERR_DISCORD ||
        seen.callback_in_terminal_frame != 1U || seen.error.count != 1U ||
        seen.error.status != DCC_ERR_DISCORD || seen.error.http_status != 403U ||
        strcmp(seen.error.operation, "/users/@me/channels") != 0 ||
        seen.log_count != 1U) {
        fprintf(
            stderr,
            "composed DM inner failure delivery duplicated or escaped its frame\n"
        );
        dcc_client_destroy(client);
        return 1;
    }

    memset(&seen, 0, sizeof(seen));
    script = (lifetime_intercept_t){
        .transport_status = DCC_OK,
        .http_status = 200U,
        .legacy_error = DCC_OK,
        .body = malformed_body,
        .body_len = sizeof(malformed_body) - 1U,
    };
    dcc_rest_set_interceptor(client, lifetime_intercept, &script);
    status = dcc_rest_create_direct_message(
        client,
        43U,
        "{\"content\":\"malformed\"}",
        lifetime_dm_legacy,
        &seen
    );
    if (status != DCC_ERR_JSON || script.request_count != 1U ||
        script.callback_count != 1U || seen.legacy.count != 1U ||
        seen.legacy.status != 200U || seen.legacy.error != DCC_ERR_JSON ||
        seen.callback_in_terminal_frame != 1U || seen.error.count != 1U ||
        seen.error.status != DCC_ERR_JSON || seen.error.http_status != 200U ||
        strcmp(seen.error.operation, "create direct message") != 0 ||
        seen.log_count != 1U) {
        fprintf(stderr, "composed DM local failure was not one terminal completion\n");
        dcc_client_destroy(client);
        return 1;
    }

    memset(&seen, 0, sizeof(seen));
    script = (lifetime_intercept_t){
        .transport_status = DCC_OK,
        .http_status = 200U,
        .legacy_error = DCC_OK,
        .body = malformed_body,
        .body_len = sizeof(malformed_body) - 1U,
    };
    dcc_rest_set_interceptor(client, lifetime_intercept, &script);
    status = dcc_rest_create_direct_message(
        client,
        44U,
        "{\"content\":\"observer-only\"}",
        NULL,
        NULL
    );
    if (status != DCC_ERR_JSON || script.request_count != 1U ||
        script.callback_count != 1U || seen.error.count != 1U ||
        seen.error.status != DCC_ERR_JSON || seen.log_count != 1U) {
        fprintf(stderr, "composed DM local failure skipped observer-only delivery\n");
        dcc_client_destroy(client);
        return 1;
    }

    dcc_client_destroy(client);
    return 0;
}

#if !defined(_WIN32)
typedef struct app_success_destroy_control {
    atomic_uint callback_entered;
    atomic_uint callback_release;
    atomic_uint cleanup_count;
    atomic_uint destroy_finished;
    atomic_uint callback_read_safe;
} app_success_destroy_control_t;

typedef struct app_success_owned_state {
    app_success_destroy_control_t *control;
    uint64_t magic;
} app_success_owned_state_t;

typedef struct app_success_destroy_state {
    dcc_app_t *app;
    dcc_client_t *client;
    app_success_owned_state_t *owned;
    app_success_destroy_control_t *control;
    dcc_status_t request_status;
    dcc_status_t destroy_status;
} app_success_destroy_state_t;

typedef struct app_admission_control {
    atomic_uint first_callback_entered;
    atomic_uint first_callback_release;
    atomic_uint cleanup_count;
    atomic_uint destroy_finished;
    atomic_uint request_count;
    atomic_uint late_callback_count;
    atomic_uint late_observer_count;
} app_admission_control_t;

typedef struct app_admission_destroy_state {
    dcc_app_t *app;
    dcc_client_t *client;
    app_admission_control_t *control;
    dcc_status_t request_status;
    dcc_status_t destroy_status;
} app_admission_destroy_state_t;

typedef struct app_async_destroy_control {
    atomic_uint live;
    atomic_uint cleanup_count;
    atomic_uint callback_count;
    atomic_uint callback_live_count;
    atomic_uint runtime_finished;
    atomic_uint destroy_finished;
} app_async_destroy_control_t;

typedef struct app_async_owned_state {
    app_async_destroy_control_t *control;
    uint64_t magic;
} app_async_owned_state_t;

typedef struct app_async_destroy_state {
    dcc_app_t *app;
    dcc_client_t *client;
    app_async_destroy_control_t *control;
    dcc_status_t runtime_status;
    dcc_status_t destroy_status;
} app_async_destroy_state_t;

static int wait_for_atomic_nonzero(atomic_uint *value, uint64_t timeout_ms) {
    uint64_t start = test_now_ms();
    while (atomic_load_explicit(value, memory_order_acquire) == 0U &&
           test_now_ms() - start < timeout_ms) {
        usleep(1000U);
    }
    return atomic_load_explicit(value, memory_order_acquire) != 0U;
}

static int wait_for_app_teardown(dcc_app_t *app, uint64_t timeout_ms) {
    uint64_t start = test_now_ms();
    for (;;) {
        dcc_app_listener_lock(app);
        int tearing_down = app->tearing_down != 0U;
        dcc_app_listener_unlock(app);
        if (tearing_down) {
            return 1;
        }
        if (test_now_ms() - start >= timeout_ms) {
            return 0;
        }
        usleep(1000U);
    }
}

static int wait_for_start_admission_close(
    dcc_client_t *client,
    uint64_t timeout_ms
) {
    uint64_t start = test_now_ms();
    while ((atomic_load_explicit(
                &client->start_admission,
                memory_order_acquire
            ) & DCC_CLIENT_LIFECYCLE_ADMISSION_CLOSED) == 0U &&
           test_now_ms() - start < timeout_ms) {
        usleep(1000U);
    }
    return (atomic_load_explicit(
                &client->start_admission,
                memory_order_acquire
            ) & DCC_CLIENT_LIFECYCLE_ADMISSION_CLOSED) != 0U;
}

static void app_success_owned_cleanup(void *user_data) {
    app_success_owned_state_t *owned = (app_success_owned_state_t *)user_data;
    if (owned == NULL) {
        return;
    }
    atomic_fetch_add_explicit(
        &owned->control->cleanup_count,
        1U,
        memory_order_release
    );
    owned->magic = 0U;
    free(owned);
}

static void app_success_blocking_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    app_success_owned_state_t *owned = (app_success_owned_state_t *)user_data;
    if (owned == NULL || response == NULL || response->status != 204U) {
        return;
    }
    app_success_destroy_control_t *control = owned->control;
    atomic_store_explicit(&control->callback_entered, 1U, memory_order_release);
    while (atomic_load_explicit(
            &control->callback_release,
            memory_order_acquire
        ) == 0U) {
        usleep(1000U);
    }
    atomic_store_explicit(
        &control->callback_read_safe,
        owned->magic == UINT64_C(0xDCC15A5E) ? 1U : 0U,
        memory_order_release
    );
}

static void *app_success_request_main(void *user_data) {
    app_success_destroy_state_t *state =
        (app_success_destroy_state_t *)user_data;
    state->request_status = dcc_rest_request(
        state->client,
        "GET",
        "/app/destroy-success-in-flight",
        NULL,
        app_success_blocking_cb,
        state->owned
    );
    return NULL;
}

static void *app_success_destroy_main(void *user_data) {
    app_success_destroy_state_t *state =
        (app_success_destroy_state_t *)user_data;
    state->destroy_status = dcc_app_destroy(state->app);
    atomic_store_explicit(
        &state->control->destroy_finished,
        1U,
        memory_order_release
    );
    return NULL;
}

static int check_app_success_callback_destroy_wait(void) {
    dcc_app_options_t options;
    dcc_app_options_init(&options);
    options.client.token = "";
    options.client.intents = DCC_INTENT_GUILDS;
    dcc_app_t *app = NULL;
    if (dcc_app_create(&options, &app) != DCC_OK) {
        return 1;
    }

    app_success_destroy_control_t control;
    atomic_init(&control.callback_entered, 0U);
    atomic_init(&control.callback_release, 0U);
    atomic_init(&control.cleanup_count, 0U);
    atomic_init(&control.destroy_finished, 0U);
    atomic_init(&control.callback_read_safe, 0U);
    app_success_owned_state_t *owned =
        (app_success_owned_state_t *)malloc(sizeof(*owned));
    if (owned == NULL) {
        (void)dcc_app_destroy(app);
        return 1;
    }
    owned->control = &control;
    owned->magic = UINT64_C(0xDCC15A5E);
    if (dcc_app_set_state(app, owned, app_success_owned_cleanup) != DCC_OK) {
        free(owned);
        (void)dcc_app_destroy(app);
        return 1;
    }

    lifetime_intercept_t script = {
        .transport_status = DCC_OK,
        .http_status = 204U,
        .legacy_error = DCC_OK,
    };
    dcc_rest_set_interceptor(dcc_app_client(app), lifetime_intercept, &script);
    app_success_destroy_state_t state = {
        .app = app,
        .client = dcc_app_client(app),
        .owned = owned,
        .control = &control,
        .request_status = DCC_ERR_STATE,
        .destroy_status = DCC_ERR_STATE,
    };
    pthread_t request_thread;
    if (pthread_create(&request_thread, NULL, app_success_request_main, &state) != 0) {
        (void)dcc_app_destroy(app);
        return 1;
    }
    if (!wait_for_atomic_nonzero(&control.callback_entered, 2000U)) {
        atomic_store_explicit(&control.callback_release, 1U, memory_order_release);
        (void)pthread_join(request_thread, NULL);
        (void)dcc_app_destroy(app);
        fprintf(stderr, "success terminal callback did not enter\n");
        return 1;
    }

    pthread_t destroy_thread;
    if (pthread_create(&destroy_thread, NULL, app_success_destroy_main, &state) != 0) {
        atomic_store_explicit(&control.callback_release, 1U, memory_order_release);
        (void)pthread_join(request_thread, NULL);
        (void)dcc_app_destroy(app);
        return 1;
    }
    int teardown_started = wait_for_app_teardown(app, 2000U);
    int cleanup_ran_early = wait_for_atomic_nonzero(&control.cleanup_count, 100U);
    int destroy_waited = atomic_load_explicit(
        &control.destroy_finished,
        memory_order_acquire
    ) == 0U;
    atomic_store_explicit(&control.callback_release, 1U, memory_order_release);
    int request_join = pthread_join(request_thread, NULL);
    int destroy_join = pthread_join(destroy_thread, NULL);
    if (!teardown_started || cleanup_ran_early || !destroy_waited ||
        request_join != 0 || destroy_join != 0 ||
        state.request_status != DCC_OK || state.destroy_status != DCC_OK ||
        atomic_load_explicit(&control.callback_read_safe, memory_order_acquire) != 1U ||
        atomic_load_explicit(&control.cleanup_count, memory_order_acquire) != 1U ||
        atomic_load_explicit(&control.destroy_finished, memory_order_acquire) != 1U ||
        script.request_count != 1U || script.callback_count != 1U) {
        fprintf(
            stderr,
            "App success terminal did not hold owned state through callback drain\n"
        );
        return 1;
    }
    return 0;
}

static dcc_status_t app_admission_intercept(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const void *body,
    size_t body_len,
    const char *content_type,
    dcc_rest_cb cb,
    void *user_data,
    void *intercept_user_data
) {
    (void)method;
    (void)path;
    (void)body;
    (void)body_len;
    (void)content_type;
    app_admission_control_t *control =
        (app_admission_control_t *)intercept_user_data;
    if (control == NULL) {
        return DCC_ERR_STATE;
    }
    unsigned request_number = atomic_fetch_add_explicit(
        &control->request_count,
        1U,
        memory_order_acq_rel
    ) + 1U;
    if (cb != NULL) {
        static const char late_body[] = "{\"message\":\"too late\"}";
        dcc_rest_response_t response = {
            .size = sizeof(response),
            .status = request_number == 1U ? 204U : 400U,
            .error = DCC_OK,
            .body = request_number == 1U ? NULL : late_body,
            .body_len = request_number == 1U ? 0U : sizeof(late_body) - 1U,
        };
        cb(client, &response, user_data);
    }
    return DCC_OK;
}

static void app_admission_cleanup(void *user_data) {
    app_admission_control_t *control = (app_admission_control_t *)user_data;
    if (control != NULL) {
        atomic_fetch_add_explicit(
            &control->cleanup_count,
            1U,
            memory_order_release
        );
    }
}

static void app_admission_first_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    app_admission_control_t *control = (app_admission_control_t *)user_data;
    if (control == NULL || response == NULL || response->status != 204U) {
        return;
    }
    atomic_store_explicit(
        &control->first_callback_entered,
        1U,
        memory_order_release
    );
    while (atomic_load_explicit(
            &control->first_callback_release,
            memory_order_acquire
        ) == 0U) {
        usleep(1000U);
    }
}

static void app_admission_late_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    (void)response;
    app_admission_control_t *control = (app_admission_control_t *)user_data;
    if (control != NULL) {
        atomic_fetch_add_explicit(
            &control->late_callback_count,
            1U,
            memory_order_release
        );
    }
}

static void app_admission_late_observer(
    dcc_client_t *client,
    const dcc_error_t *error,
    void *user_data
) {
    (void)client;
    app_admission_control_t *control = (app_admission_control_t *)user_data;
    if (control != NULL && error != NULL) {
        atomic_fetch_add_explicit(
            &control->late_observer_count,
            1U,
            memory_order_release
        );
    }
}

static void *app_admission_request_main(void *user_data) {
    app_admission_destroy_state_t *state =
        (app_admission_destroy_state_t *)user_data;
    state->request_status = dcc_rest_request(
        state->client,
        "GET",
        "/app/admission-in-flight",
        NULL,
        app_admission_first_cb,
        state->control
    );
    return NULL;
}

static void *app_admission_destroy_main(void *user_data) {
    app_admission_destroy_state_t *state =
        (app_admission_destroy_state_t *)user_data;
    state->destroy_status = dcc_app_destroy(state->app);
    atomic_store_explicit(
        &state->control->destroy_finished,
        1U,
        memory_order_release
    );
    return NULL;
}

static int check_app_destroy_closes_rest_before_start_drain(void) {
    dcc_app_options_t options;
    dcc_app_options_init(&options);
    options.client.token = "";
    options.client.intents = DCC_INTENT_GUILDS;
    dcc_app_t *app = NULL;
    if (dcc_app_create(&options, &app) != DCC_OK) {
        return 1;
    }

    app_admission_control_t control;
    atomic_init(&control.first_callback_entered, 0U);
    atomic_init(&control.first_callback_release, 0U);
    atomic_init(&control.cleanup_count, 0U);
    atomic_init(&control.destroy_finished, 0U);
    atomic_init(&control.request_count, 0U);
    atomic_init(&control.late_callback_count, 0U);
    atomic_init(&control.late_observer_count, 0U);
    dcc_client_t *client = dcc_app_client(app);
    if (dcc_client_on_error(
            client,
            app_admission_late_observer,
            &control
        ) != DCC_OK) {
        (void)dcc_app_destroy(app);
        return 1;
    }
    dcc_rest_set_interceptor(client, app_admission_intercept, &control);

    /* Hold a synthetic already-admitted start call so prepare_destroy() must
     * pause in its start-admission drain. REST must already be closed while
     * that pause is observable. */
    if (dcc_client_lifecycle_admission_enter(
            &client->start_admission
        ) != DCC_OK) {
        (void)dcc_app_destroy(app);
        return 1;
    }

    app_admission_destroy_state_t state = {
        .app = app,
        .client = client,
        .control = &control,
        .request_status = DCC_ERR_STATE,
        .destroy_status = DCC_ERR_STATE,
    };
    pthread_t destroy_thread;
    if (pthread_create(
            &destroy_thread,
            NULL,
            app_admission_destroy_main,
            &state
        ) != 0) {
        dcc_client_lifecycle_admission_leave(&client->start_admission);
        (void)dcc_app_destroy(app);
        return 1;
    }

    int start_drain_entered = wait_for_start_admission_close(client, 2000U);
    dcc_status_t sync_status = DCC_ERR_RUNTIME;
    dcc_status_t async_status = DCC_ERR_RUNTIME;
    if (start_drain_entered) {
        sync_status = dcc_rest_request(
            client,
            "GET",
            "/app/rejected-during-start-drain-sync",
            NULL,
            app_admission_late_cb,
            &control
        );
        async_status = dcc_rest_request_async(
            client,
            "GET",
            "/app/rejected-during-start-drain-async",
            NULL,
            app_admission_late_cb,
            &control
        );
    }
    unsigned destroy_waited = atomic_load_explicit(
        &control.destroy_finished,
        memory_order_acquire
    ) == 0U;
    dcc_client_lifecycle_admission_leave(&client->start_admission);
    int destroy_join = pthread_join(destroy_thread, NULL);

    if (!start_drain_entered || !destroy_waited || destroy_join != 0 ||
        state.destroy_status != DCC_OK || sync_status != DCC_ERR_STATE ||
        async_status != DCC_ERR_STATE ||
        atomic_load_explicit(&control.request_count, memory_order_acquire) != 0U ||
        atomic_load_explicit(
            &control.late_callback_count,
            memory_order_acquire
        ) != 0U ||
        atomic_load_explicit(
            &control.late_observer_count,
            memory_order_acquire
        ) != 0U) {
        fprintf(
            stderr,
            "REST opened during start drain: entered=%d waited=%u sync=%d "
            "async=%d destroy=%d requests=%u callbacks=%u observers=%u\n",
            start_drain_entered,
            destroy_waited,
            (int)sync_status,
            (int)async_status,
            (int)state.destroy_status,
            atomic_load_explicit(&control.request_count, memory_order_acquire),
            atomic_load_explicit(
                &control.late_callback_count,
                memory_order_acquire
            ),
            atomic_load_explicit(
                &control.late_observer_count,
                memory_order_acquire
            )
        );
        return 1;
    }
    return 0;
}

static int check_app_destroy_closes_rest_admission(void) {
    dcc_app_options_t options;
    dcc_app_options_init(&options);
    options.client.token = "";
    options.client.intents = DCC_INTENT_GUILDS;
    dcc_app_t *app = NULL;
    if (dcc_app_create(&options, &app) != DCC_OK) {
        return 1;
    }

    app_admission_control_t control;
    atomic_init(&control.first_callback_entered, 0U);
    atomic_init(&control.first_callback_release, 0U);
    atomic_init(&control.cleanup_count, 0U);
    atomic_init(&control.destroy_finished, 0U);
    atomic_init(&control.request_count, 0U);
    atomic_init(&control.late_callback_count, 0U);
    atomic_init(&control.late_observer_count, 0U);
    dcc_client_t *client = dcc_app_client(app);
    if (dcc_app_set_state(app, &control, app_admission_cleanup) != DCC_OK ||
        dcc_client_on_error(client, app_admission_late_observer, &control) != DCC_OK) {
        (void)dcc_app_destroy(app);
        return 1;
    }
    dcc_rest_set_interceptor(client, app_admission_intercept, &control);

    app_admission_destroy_state_t state = {
        .app = app,
        .client = client,
        .control = &control,
        .request_status = DCC_ERR_STATE,
        .destroy_status = DCC_ERR_STATE,
    };
    pthread_t request_thread;
    if (pthread_create(&request_thread, NULL, app_admission_request_main, &state) != 0) {
        (void)dcc_app_destroy(app);
        return 1;
    }
    if (!wait_for_atomic_nonzero(&control.first_callback_entered, 2000U)) {
        atomic_store_explicit(
            &control.first_callback_release,
            1U,
            memory_order_release
        );
        (void)pthread_join(request_thread, NULL);
        (void)dcc_app_destroy(app);
        fprintf(stderr, "admission probe callback did not enter\n");
        return 1;
    }

    pthread_t destroy_thread;
    if (pthread_create(&destroy_thread, NULL, app_admission_destroy_main, &state) != 0) {
        atomic_store_explicit(
            &control.first_callback_release,
            1U,
            memory_order_release
        );
        (void)pthread_join(request_thread, NULL);
        (void)dcc_app_destroy(app);
        return 1;
    }
    int teardown_started = wait_for_app_teardown(app, 2000U);
    dcc_status_t sync_status = dcc_rest_request(
        client,
        "GET",
        "/app/rejected-sync",
        NULL,
        app_admission_late_cb,
        &control
    );
    dcc_status_t async_status = dcc_rest_request_async(
        client,
        "GET",
        "/app/rejected-async",
        NULL,
        app_admission_late_cb,
        &control
    );
    dcc_status_t wait_status = dcc_client_wait(client);
    dcc_status_t start_status = dcc_client_start(client);
    unsigned destroy_waited = atomic_load_explicit(
        &control.destroy_finished,
        memory_order_acquire
    ) == 0U;
    atomic_store_explicit(
        &control.first_callback_release,
        1U,
        memory_order_release
    );
    int request_join = pthread_join(request_thread, NULL);
    int destroy_join = pthread_join(destroy_thread, NULL);
    if (!teardown_started || !destroy_waited || request_join != 0 ||
        destroy_join != 0 || state.request_status != DCC_OK ||
        state.destroy_status != DCC_OK || sync_status != DCC_ERR_STATE ||
        async_status != DCC_ERR_STATE || wait_status != DCC_ERR_STATE ||
        start_status != DCC_ERR_STATE ||
        atomic_load_explicit(&control.request_count, memory_order_acquire) != 1U ||
        atomic_load_explicit(
            &control.late_callback_count,
            memory_order_acquire
        ) != 0U ||
        atomic_load_explicit(
            &control.late_observer_count,
            memory_order_acquire
        ) != 0U ||
        atomic_load_explicit(&control.cleanup_count, memory_order_acquire) != 1U) {
        fprintf(
            stderr,
            "teardown admission remained open: sync=%d async=%d wait=%d start=%d "
            "requests=%u callbacks=%u observers=%u\n",
            (int)sync_status,
            (int)async_status,
            (int)wait_status,
            (int)start_status,
            atomic_load_explicit(&control.request_count, memory_order_acquire),
            atomic_load_explicit(
                &control.late_callback_count,
                memory_order_acquire
            ),
            atomic_load_explicit(
                &control.late_observer_count,
                memory_order_acquire
            )
        );
        return 1;
    }
    return 0;
}

static void app_async_owned_cleanup(void *user_data) {
    app_async_owned_state_t *owned = (app_async_owned_state_t *)user_data;
    if (owned == NULL) {
        return;
    }
    atomic_store_explicit(&owned->control->live, 0U, memory_order_release);
    atomic_fetch_add_explicit(
        &owned->control->cleanup_count,
        1U,
        memory_order_release
    );
}

static void app_async_terminal_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    (void)response;
    app_async_owned_state_t *owned = (app_async_owned_state_t *)user_data;
    if (owned == NULL) {
        return;
    }
    atomic_fetch_add_explicit(
        &owned->control->callback_count,
        1U,
        memory_order_release
    );
    if (atomic_load_explicit(&owned->control->live, memory_order_acquire) != 0U &&
        owned->magic == UINT64_C(0xDCC2A51C)) {
        atomic_fetch_add_explicit(
            &owned->control->callback_live_count,
            1U,
            memory_order_release
        );
    }
}

static void *app_async_runtime_main(void *user_data) {
    app_async_destroy_state_t *state = (app_async_destroy_state_t *)user_data;
    state->runtime_status = dcc_client_wait(state->client);
    atomic_store_explicit(
        &state->control->runtime_finished,
        1U,
        memory_order_release
    );
    return NULL;
}

static void *app_async_destroy_main(void *user_data) {
    app_async_destroy_state_t *state = (app_async_destroy_state_t *)user_data;
    state->destroy_status = dcc_app_destroy(state->app);
    atomic_store_explicit(
        &state->control->destroy_finished,
        1U,
        memory_order_release
    );
    return NULL;
}

static int check_app_async_terminals_precede_owned_cleanup(void) {
    http_server_t server;
    pthread_t server_thread;
    if (start_server(&server, &server_thread) != 0) {
        fprintf(stderr, "App teardown lifetime server creation failed\n");
        return 1;
    }
    server.delay_ms = 300U;
    set_api_base_for_server(&server);

    dcc_app_options_t options;
    dcc_app_options_init(&options);
    options.client.token = "";
    options.client.intents = DCC_INTENT_GUILDS;
    options.client.rest_concurrency = 1U;
    dcc_app_t *app = NULL;
    if (dcc_app_create(&options, &app) != DCC_OK) {
        close(server.fd);
        (void)pthread_join(server_thread, NULL);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        return 1;
    }

    app_async_destroy_control_t control;
    atomic_init(&control.live, 1U);
    atomic_init(&control.cleanup_count, 0U);
    atomic_init(&control.callback_count, 0U);
    atomic_init(&control.callback_live_count, 0U);
    atomic_init(&control.runtime_finished, 0U);
    atomic_init(&control.destroy_finished, 0U);
    app_async_owned_state_t *owned =
        (app_async_owned_state_t *)malloc(sizeof(*owned));
    if (owned == NULL) {
        (void)dcc_app_destroy(app);
        close(server.fd);
        (void)pthread_join(server_thread, NULL);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        return 1;
    }
    owned->control = &control;
    owned->magic = UINT64_C(0xDCC2A51C);
    if (dcc_app_set_state(app, owned, app_async_owned_cleanup) != DCC_OK ||
        dcc_app_start(app) != DCC_OK) {
        (void)dcc_app_destroy(app);
        free(owned);
        close(server.fd);
        (void)pthread_join(server_thread, NULL);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        return 1;
    }

    /* Keep a second client-specific runtime reference alive so App teardown
     * cannot rely on being the last global LLAM owner to reap its REST worker. */
    dcc_client_options_t peer_options = {
        .size = sizeof(peer_options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_client_t *peer = NULL;
    if (dcc_client_create(&peer_options, &peer) != DCC_OK ||
        dcc_runtime_init(&peer->runtime) != DCC_OK) {
        dcc_client_destroy(peer);
        (void)dcc_app_destroy(app);
        free(owned);
        close(server.fd);
        (void)pthread_join(server_thread, NULL);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        return 1;
    }

    dcc_client_t *client = dcc_app_client(app);
    app_async_destroy_state_t state = {
        .app = app,
        .client = client,
        .control = &control,
        .runtime_status = DCC_ERR_STATE,
        .destroy_status = DCC_ERR_STATE,
    };
    dcc_status_t submit_status = dcc_rest_request_async(
        client,
        "GET",
        "/app/teardown-active",
        NULL,
        app_async_terminal_cb,
        owned
    );
    pthread_t runtime_thread;
    int runtime_started = submit_status == DCC_OK &&
        pthread_create(&runtime_thread, NULL, app_async_runtime_main, &state) == 0;
    if (!runtime_started || !wait_for_atomic_nonzero(&server.requests_seen, 2000U)) {
        (void)dcc_app_stop(app);
        if (runtime_started) {
            (void)pthread_join(runtime_thread, NULL);
        }
        (void)dcc_app_destroy(app);
        dcc_runtime_shutdown(&peer->runtime);
        dcc_client_destroy(peer);
        free(owned);
        close(server.fd);
        (void)pthread_join(server_thread, NULL);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        fprintf(stderr, "App teardown active request did not start\n");
        return 1;
    }
    dcc_status_t pending_submit_status = dcc_rest_request_async(
        client,
        "GET",
        "/app/teardown-pending",
        NULL,
        app_async_terminal_cb,
        owned
    );
    size_t pending = 0U;
    size_t active = 0U;
    dcc_status_t pending_status = dcc_rest_async_pending(
        client,
        &pending,
        &active
    );

    pthread_t destroy_thread;
    int destroy_started = pending_submit_status == DCC_OK &&
        pending_status == DCC_OK && pending == 1U && active == 1U &&
        pthread_create(&destroy_thread, NULL, app_async_destroy_main, &state) == 0;
    if (!destroy_started) {
        (void)dcc_app_stop(app);
    }
    int destroy_join = destroy_started ? pthread_join(destroy_thread, NULL) : -1;
    int runtime_join = pthread_join(runtime_thread, NULL);
    if (!destroy_started) {
        (void)dcc_app_destroy(app);
    }
    dcc_runtime_shutdown(&peer->runtime);
    dcc_client_destroy(peer);
    (void)pthread_join(server_thread, NULL);
    close(server.fd);
    (void)unsetenv("DCC_DISCORD_API_BASE");

    unsigned callbacks = atomic_load_explicit(
        &control.callback_count,
        memory_order_acquire
    );
    unsigned callbacks_live = atomic_load_explicit(
        &control.callback_live_count,
        memory_order_acquire
    );
    unsigned cleanups = atomic_load_explicit(
        &control.cleanup_count,
        memory_order_acquire
    );
    unsigned destroy_finished = atomic_load_explicit(
        &control.destroy_finished,
        memory_order_acquire
    );
    unsigned runtime_finished = atomic_load_explicit(
        &control.runtime_finished,
        memory_order_acquire
    );
    free(owned);
    if (!destroy_started || destroy_join != 0 || runtime_join != 0 ||
        state.destroy_status != DCC_OK || state.runtime_status != DCC_OK ||
        callbacks != 2U || callbacks_live != 2U || cleanups != 1U ||
        destroy_finished != 1U || runtime_finished != 1U ||
        server.request_count != 1U) {
        fprintf(
            stderr,
            "App cleanup preceded REST terminal drain: destroy=%d runtime=%d "
            "callbacks=%u live=%u cleanups=%u pending=%zu active=%zu requests=%u\n",
            (int)state.destroy_status,
            (int)state.runtime_status,
            callbacks,
            callbacks_live,
            cleanups,
            pending,
            active,
            server.request_count
        );
        return 1;
    }
    return 0;
}
#else
static int check_app_success_callback_destroy_wait(void) {
    return 0;
}

static int check_app_destroy_closes_rest_admission(void) {
    return 0;
}

static int check_app_destroy_closes_rest_before_start_drain(void) {
    return 0;
}

static int check_app_async_terminals_precede_owned_cleanup(void) {
    return 0;
}
#endif

int app_v2_error_lifetime_smoke(void) {
    return check_managed_destroy_defers_to_owner() ||
        check_app_success_callback_destroy_wait() ||
        check_app_destroy_closes_rest_before_start_drain() ||
        check_app_destroy_closes_rest_admission() ||
        check_app_async_terminals_precede_owned_cleanup() ||
        check_composed_dm_terminal_delivery();
}
