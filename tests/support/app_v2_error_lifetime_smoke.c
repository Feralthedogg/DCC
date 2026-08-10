#include "app_v2_error_lifetime_smoke.h"

#include <dcc/app.h>
#include <dcc/client.h>
#include <dcc/error_details.h>
#include <dcc/rest.h>

#include "internal/app/dcc_app_internal.h"
#include "internal/rest/dcc_rest_error_observer_internal.h"
#include "internal/rest/dcc_rest_intercept_internal.h"
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
#else
static int check_app_success_callback_destroy_wait(void) {
    return 0;
}
#endif

int app_v2_error_lifetime_smoke(void) {
    return check_app_success_callback_destroy_wait() ||
        check_composed_dm_terminal_delivery();
}
