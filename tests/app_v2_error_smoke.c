#include <dcc/app.h>
#include <dcc/client.h>
#include <dcc/error_details.h>
#include <dcc/rest.h>
#include <dcc/rest/result.h>
#include <dcc/rest/core/async.h>
#include <dcc/rest/core/request.h>

#include "internal/app/dcc_app_internal.h"
#include "internal/client/dcc_client_state_internal.h"
#include "internal/events/dcc_event_state_internal.h"
#include "internal/rest/dcc_rest_intercept_internal.h"
#include "support/app_v2_error_lifetime_smoke.h"
#include "support/http_smoke_server.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>

#if !defined(_WIN32)
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#endif

typedef struct error_seen {
    unsigned count;
    dcc_error_origin_t origin;
    dcc_status_t status;
    uint16_t http_status;
    int32_t discord_code;
    size_t body_len;
    char operation[128];
    char message[128];
    char body[256];
    uint8_t valid_metadata;
} error_seen_t;

typedef struct legacy_seen {
    unsigned count;
    uint16_t status;
    dcc_status_t error;
} legacy_seen_t;

typedef struct intercept_step {
    dcc_status_t transport_status;
    uint16_t http_status;
    dcc_status_t legacy_error;
    const char *body;
    size_t body_len;
} intercept_step_t;

typedef struct intercept_script {
    intercept_step_t steps[16];
    size_t step_count;
    size_t next_step;
    unsigned callback_count;
    unsigned request_count;
    char last_path[160];
    char last_body[512];
    size_t last_body_len;
} intercept_script_t;

typedef struct app_error_state {
    error_seen_t seen;
    unsigned route_calls;
    unsigned event_calls;
    unsigned task_calls;
    unsigned client_seen;
    unsigned replacement_seen;
} app_error_state_t;

typedef struct reentrant_observer_state {
    unsigned first_count;
    unsigned replacement_count;
    dcc_status_t mutation_status;
} reentrant_observer_state_t;

typedef struct client_terminal_destroy_state {
    dcc_client_t *client;
    unsigned legacy_count;
    unsigned log_count;
    unsigned observer_count;
} client_terminal_destroy_state_t;

typedef struct app_terminal_destroy_state {
    dcc_app_t *app;
    unsigned legacy_count;
    unsigned app_observer_count;
    unsigned client_observer_count;
    unsigned app_sink_in_flight;
    dcc_status_t legacy_destroy_status;
    dcc_status_t app_observer_destroy_status;
    dcc_status_t client_observer_destroy_status;
} app_terminal_destroy_state_t;

typedef struct nested_terminal_state {
    unsigned outer_count;
    unsigned inner_count;
    dcc_status_t nested_status;
} nested_terminal_state_t;

typedef struct adversarial_intercept_state {
    dcc_client_t *foreign_client;
    unsigned callback_attempts;
} adversarial_intercept_state_t;

#if !defined(_WIN32)
typedef struct async_error_seen {
    atomic_uint count;
    atomic_uint legacy_count;
    dcc_status_t status;
    uint16_t http_status;
    int32_t discord_code;
    char operation[160];
} async_error_seen_t;

typedef struct client_runner {
    dcc_client_t *client;
    dcc_status_t status;
} client_runner_t;

typedef struct app_destroy_observer_state {
    dcc_app_t *app;
    dcc_client_t *client;
    atomic_uint observer_entered;
    atomic_uint observer_release;
    atomic_uint destroy_finished;
    error_seen_t seen;
    dcc_status_t request_status;
    dcc_status_t destroy_status;
} app_destroy_observer_state_t;

#endif

static void copy_error(error_seen_t *seen, const dcc_error_t *error) {
    if (seen == NULL || error == NULL) {
        return;
    }
    seen->count++;
    seen->valid_metadata = error->size == sizeof(*error) &&
        error->version == DCC_ERROR_VERSION;
    seen->origin = error->origin;
    seen->status = error->status;
    seen->http_status = error->http_status;
    seen->discord_code = error->discord_code;
    seen->body_len = error->body_len;
    if (error->operation != NULL) {
        snprintf(seen->operation, sizeof(seen->operation), "%s", error->operation);
    } else {
        seen->operation[0] = '\0';
    }
    if (error->message != NULL) {
        snprintf(seen->message, sizeof(seen->message), "%s", error->message);
    } else {
        seen->message[0] = '\0';
    }
    size_t copy_len = error->body_len < sizeof(seen->body)
        ? error->body_len
        : sizeof(seen->body);
    if (copy_len != 0U && error->body != NULL) {
        memcpy(seen->body, error->body, copy_len);
    }
}

static dcc_status_t scripted_intercept(
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
    (void)content_type;
    intercept_script_t *script = (intercept_script_t *)intercept_user_data;
    if (script == NULL || script->next_step >= script->step_count) {
        return DCC_ERR_STATE;
    }
    script->request_count++;
    snprintf(script->last_path, sizeof(script->last_path), "%s", path != NULL ? path : "");
    memset(script->last_body, 0, sizeof(script->last_body));
    script->last_body_len = body_len < sizeof(script->last_body)
        ? body_len
        : sizeof(script->last_body);
    if (script->last_body_len != 0U && body != NULL) {
        memcpy(script->last_body, body, script->last_body_len);
    }

    const intercept_step_t *step = &script->steps[script->next_step++];
    if (step->transport_status != DCC_OK) {
        return step->transport_status;
    }
    if (cb != NULL) {
        dcc_rest_response_t response = {
            .size = sizeof(response),
            .status = step->http_status,
            .error = step->legacy_error,
            .body = step->body,
            .body_len = step->body_len,
        };
        script->callback_count++;
        cb(client, &response, user_data);
    }
    return DCC_OK;
}

static void public_error_observer(
    dcc_client_t *client,
    const dcc_error_t *error,
    void *user_data
) {
    (void)client;
    copy_error((error_seen_t *)user_data, error);
}

static void legacy_response_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    unsigned *count = (unsigned *)user_data;
    if (response != NULL && count != NULL) {
        (*count)++;
    }
}

static void legacy_capture_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    legacy_seen_t *seen = (legacy_seen_t *)user_data;
    if (seen == NULL || response == NULL) {
        return;
    }
    seen->count++;
    seen->status = response->status;
    seen->error = response->error;
}

static void client_terminal_destroy_log(
    dcc_log_level_t level,
    const char *message,
    void *user_data
) {
    client_terminal_destroy_state_t *state =
        (client_terminal_destroy_state_t *)user_data;
    if (state == NULL || state->client == NULL || level != DCC_LOG_ERROR ||
        message == NULL || strstr(message, "REST failure") == NULL) {
        return;
    }
    state->log_count++;
    dcc_client_destroy(state->client);
}

static void client_terminal_destroy_legacy(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    client_terminal_destroy_state_t *state =
        (client_terminal_destroy_state_t *)user_data;
    if (state == NULL || response == NULL) {
        return;
    }
    state->legacy_count++;
    dcc_client_destroy(client);
}

static void client_terminal_destroy_observer(
    dcc_client_t *client,
    const dcc_error_t *error,
    void *user_data
) {
    client_terminal_destroy_state_t *state =
        (client_terminal_destroy_state_t *)user_data;
    if (state == NULL || error == NULL) {
        return;
    }
    state->observer_count++;
    dcc_client_destroy(client);
}

static void app_terminal_destroy_legacy(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    app_terminal_destroy_state_t *state =
        (app_terminal_destroy_state_t *)user_data;
    if (state == NULL || response == NULL) {
        return;
    }
    state->legacy_count++;
    state->app_sink_in_flight = atomic_load_explicit(
        &client->rest_app_error_sink_in_flight,
        memory_order_acquire
    );
    state->legacy_destroy_status = dcc_app_destroy(state->app);
}

static void app_terminal_destroy_observer(
    dcc_app_t *app,
    const dcc_error_t *error,
    void *user_data
) {
    app_terminal_destroy_state_t *state =
        (app_terminal_destroy_state_t *)user_data;
    if (state == NULL || error == NULL) {
        return;
    }
    state->app_observer_count++;
    state->app_observer_destroy_status = dcc_app_destroy(app);
}

static void app_terminal_destroy_client_observer(
    dcc_client_t *client,
    const dcc_error_t *error,
    void *user_data
) {
    (void)client;
    app_terminal_destroy_state_t *state =
        (app_terminal_destroy_state_t *)user_data;
    if (state == NULL || error == NULL) {
        return;
    }
    state->client_observer_count++;
    state->client_observer_destroy_status = dcc_app_destroy(state->app);
}

static void nested_terminal_inner_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    nested_terminal_state_t *state = (nested_terminal_state_t *)user_data;
    if (state != NULL && response != NULL && response->status == 204U) {
        state->inner_count++;
    }
}

static void nested_terminal_outer_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    nested_terminal_state_t *state = (nested_terminal_state_t *)user_data;
    if (state == NULL || response == NULL || response->status != 400U) {
        return;
    }
    state->outer_count++;
    state->nested_status = dcc_rest_request(
        client,
        "GET",
        "/lifetime/nested-inner",
        NULL,
        nested_terminal_inner_cb,
        state
    );
}

static dcc_status_t adversarial_intercept(
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
    (void)client;
    (void)method;
    (void)path;
    (void)body;
    (void)body_len;
    (void)content_type;
    adversarial_intercept_state_t *state =
        (adversarial_intercept_state_t *)intercept_user_data;
    if (state == NULL || cb == NULL) {
        return DCC_ERR_STATE;
    }
    dcc_rest_response_t response = {
        .size = sizeof(response),
        .status = 204U,
        .error = DCC_OK,
    };
    state->callback_attempts++;
    cb(state->foreign_client, &response, user_data);
    state->callback_attempts++;
    cb(client, &response, user_data);
    return DCC_ERR_STATE;
}

#if !defined(_WIN32)
static void async_error_observer(
    dcc_client_t *client,
    const dcc_error_t *error,
    void *user_data
) {
    (void)client;
    async_error_seen_t *seen = (async_error_seen_t *)user_data;
    if (seen == NULL || error == NULL) {
        return;
    }
    seen->status = error->status;
    seen->http_status = error->http_status;
    seen->discord_code = error->discord_code;
    snprintf(
        seen->operation,
        sizeof(seen->operation),
        "%s",
        error->operation != NULL ? error->operation : ""
    );
    atomic_fetch_add_explicit(&seen->count, 1U, memory_order_release);
}

static void async_legacy_response_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    async_error_seen_t *seen = (async_error_seen_t *)user_data;
    if (seen != NULL && response != NULL) {
        atomic_fetch_add_explicit(&seen->legacy_count, 1U, memory_order_release);
    }
}

static void *client_runner_main(void *user_data) {
    client_runner_t *runner = (client_runner_t *)user_data;
    if (runner != NULL && runner->client != NULL) {
        runner->status = dcc_client_wait(runner->client);
    }
    return NULL;
}

static int wait_for_request(const http_server_t *server, uint64_t timeout_ms) {
    uint64_t start = test_now_ms();
    while (atomic_load_explicit(&server->requests_seen, memory_order_acquire) == 0U &&
           test_now_ms() - start < timeout_ms) {
        usleep(1000U);
    }
    return atomic_load_explicit(&server->requests_seen, memory_order_acquire) != 0U;
}

static int start_client_runner(
    dcc_client_t *client,
    client_runner_t *runner,
    pthread_t *thread
) {
    runner->client = client;
    runner->status = DCC_ERR_STATE;
    return pthread_create(thread, NULL, client_runner_main, runner);
}

static dcc_client_t *create_async_client(async_error_seen_t *seen) {
    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .rest_concurrency = 1U,
    };
    dcc_client_t *client = NULL;
    memset(seen, 0, sizeof(*seen));
    atomic_init(&seen->count, 0U);
    atomic_init(&seen->legacy_count, 0U);
    if (dcc_client_create(&options, &client) != DCC_OK ||
        dcc_client_on_error(client, async_error_observer, seen) != DCC_OK ||
        dcc_client_start(client) != DCC_OK) {
        dcc_client_destroy(client);
        return NULL;
    }
    return client;
}

static int stop_runner_and_client(
    dcc_client_t *client,
    client_runner_t *runner,
    pthread_t thread
) {
    dcc_status_t stop_status = dcc_client_stop(client);
    int join_status = pthread_join(thread, NULL);
    dcc_client_destroy(client);
    return stop_status == DCC_OK && join_status == 0 && runner->status == DCC_OK
        ? 0
        : 1;
}

static int stop_app_runner_and_destroy(
    dcc_app_t *app,
    client_runner_t *runner,
    pthread_t thread
) {
    dcc_client_t *client = dcc_app_client(app);
    dcc_status_t stop_status = dcc_client_stop(client);
    int join_status = pthread_join(thread, NULL);
    dcc_status_t destroy_status = dcc_app_destroy(app);
    return stop_status == DCC_OK && join_status == 0 &&
        runner->status == DCC_OK && destroy_status == DCC_OK
        ? 0
        : 1;
}

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
#endif

static int check_result_values(void) {
    static const char body_bytes[] = { 'a', '\0', 'b', 'c' };
    static const char message_text[] = "Invalid Form Body";
    char *original_body = (char *)malloc(sizeof(body_bytes));
    char *original_message = (char *)malloc(sizeof(message_text));
    if (original_body == NULL || original_message == NULL) {
        free(original_body);
        free(original_message);
        return 1;
    }
    memcpy(original_body, body_bytes, sizeof(body_bytes));
    memcpy(original_message, message_text, sizeof(message_text));
    dcc_rest_result_t source = {
        .size = sizeof(source),
        .version = DCC_REST_RESULT_VERSION,
        .transport_status = DCC_OK,
        .http_status = 400U,
        .discord_code = 50035,
        .discord_message = original_message,
        .body = original_body,
        .body_len = sizeof(body_bytes),
        .retry_after_ms = 25U,
    };
    dcc_rest_result_t *clone = (dcc_rest_result_t *)(uintptr_t)1U;
    if (dcc_rest_result_ok(&source) ||
        dcc_rest_result_status(&source) != DCC_ERR_DISCORD ||
        dcc_rest_result_clone(&source, &clone) != DCC_OK ||
        clone == NULL || clone->size != sizeof(*clone) ||
        clone->version != DCC_REST_RESULT_VERSION ||
        clone->transport_status != DCC_OK || clone->http_status != 400U ||
        clone->discord_code != 50035 || clone->retry_after_ms != 25U ||
        clone->body_len != sizeof(body_bytes) ||
        memcmp(clone->body, original_body, sizeof(body_bytes)) != 0 ||
        strcmp(clone->discord_message, original_message) != 0) {
        fprintf(stderr, "REST result clone/value contract failed\n");
        dcc_rest_result_free(clone);
        free(original_body);
        free(original_message);
        return 1;
    }
    memset(original_body, 'x', sizeof(body_bytes));
    memset(original_message, 'y', sizeof(message_text) - 1U);
    free(original_body);
    free(original_message);
    if (memcmp(clone->body, "a\0bc", 4U) != 0 ||
        strcmp(clone->discord_message, "Invalid Form Body") != 0) {
        fprintf(stderr, "REST result clone did not own exact source bytes\n");
        dcc_rest_result_free(clone);
        return 1;
    }
    dcc_rest_result_free(clone);
    source.body = NULL;
    source.body_len = 0U;
    source.discord_message = NULL;

    dcc_rest_result_t transport = source;
    transport.transport_status = DCC_ERR_NETWORK;
    transport.http_status = 0U;
    dcc_rest_result_t ok = source;
    ok.http_status = 204U;
    dcc_rest_result_t invalid = source;
    invalid.version = DCC_REST_RESULT_VERSION + 1U;
    dcc_rest_result_t short_prefix = source;
    short_prefix.size = offsetof(dcc_rest_result_t, version);
    dcc_rest_result_t short_status = source;
    short_status.size = offsetof(dcc_rest_result_t, http_status);
    dcc_rest_result_t short_clone = source;
    short_clone.size = sizeof(short_clone) - 1U;
    clone = (dcc_rest_result_t *)(uintptr_t)1U;
    if (dcc_rest_result_status(&transport) != DCC_ERR_NETWORK ||
        !dcc_rest_result_ok(&ok) ||
        dcc_rest_result_ok(NULL) ||
        dcc_rest_result_status(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_rest_result_status(&short_prefix) != DCC_ERR_INVALID_ARG ||
        dcc_rest_result_status(&short_status) != DCC_ERR_INVALID_ARG ||
        dcc_rest_result_clone(&invalid, &clone) != DCC_ERR_INVALID_ARG ||
        clone != NULL ||
        ((clone = (dcc_rest_result_t *)(uintptr_t)1U),
         dcc_rest_result_clone(&short_clone, &clone)) != DCC_ERR_INVALID_ARG ||
        clone != NULL ||
        dcc_rest_result_clone(&source, NULL) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "REST result invalid/transport contract failed\n");
        return 1;
    }
    return 0;
}

static int check_rest_terminal_matrix(void) {
    static const char discord_body[] =
        "{\"code\":50035,\"message\":\"Invalid Form Body\"}";
    static const char malformed[] = "{\"code\":oops}";
    static const char truncated[] = { '{', '"', 'c', 'o', 'd', 'e', '"', ':', '5' };
    static const char non_object[] = "[50035]";
    static const char non_nul_object[] = {
        '{', '"', 'd', 'e', 't', 'a', 'i', 'l', '"', ':', '"', 'x', '"', '}'
    };
    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_client_t *client = NULL;
    if (dcc_client_create(&options, &client) != DCC_OK) {
        fprintf(stderr, "REST matrix client creation failed\n");
        return 1;
    }

    intercept_script_t script;
    memset(&script, 0, sizeof(script));
    script.steps[0] = (intercept_step_t){
        .transport_status = DCC_OK,
        .http_status = 400U,
        .legacy_error = DCC_OK,
        .body = discord_body,
        .body_len = sizeof(discord_body) - 1U,
    };
    script.steps[1] = (intercept_step_t){
        .transport_status = DCC_OK,
        .http_status = 500U,
        .legacy_error = DCC_OK,
        .body = "server",
        .body_len = 6U,
    };
    script.steps[2] = (intercept_step_t){ .transport_status = DCC_ERR_NETWORK };
    script.steps[3] = (intercept_step_t){
        .transport_status = DCC_OK,
        .http_status = 400U,
        .legacy_error = DCC_OK,
        .body = malformed,
        .body_len = sizeof(malformed) - 1U,
    };
    script.steps[4] = (intercept_step_t){
        .transport_status = DCC_OK,
        .http_status = 400U,
        .legacy_error = DCC_OK,
        .body = truncated,
        .body_len = sizeof(truncated),
    };
    script.steps[5] = (intercept_step_t){
        .transport_status = DCC_OK,
        .http_status = 400U,
        .legacy_error = DCC_OK,
        .body = non_object,
        .body_len = sizeof(non_object) - 1U,
    };
    script.steps[6] = (intercept_step_t){
        .transport_status = DCC_OK,
        .http_status = 400U,
        .legacy_error = DCC_OK,
        .body = non_nul_object,
        .body_len = sizeof(non_nul_object),
    };
    script.step_count = 7U;
    dcc_rest_set_interceptor(client, scripted_intercept, &script);

    error_seen_t seen;
    memset(&seen, 0, sizeof(seen));
    if (dcc_client_on_error(NULL, public_error_observer, &seen) != DCC_ERR_INVALID_ARG ||
        dcc_client_on_error(client, NULL, &seen) != DCC_ERR_INVALID_ARG ||
        dcc_client_on_error(client, public_error_observer, &seen) != DCC_OK) {
        fprintf(stderr, "client observer registration contract failed\n");
        dcc_client_destroy(client);
        return 1;
    }

    if (dcc_rest_request(client, "POST", "/matrix/400", NULL, NULL, NULL) != DCC_OK ||
        seen.count != 1U || !seen.valid_metadata || seen.origin != DCC_ERROR_REST ||
        seen.status != DCC_ERR_DISCORD || seen.http_status != 400U ||
        seen.discord_code != 50035 || strcmp(seen.message, "Invalid Form Body") != 0 ||
        strcmp(seen.operation, "/matrix/400") != 0 ||
        seen.body_len != sizeof(discord_body) - 1U ||
        memcmp(seen.body, discord_body, sizeof(discord_body) - 1U) != 0) {
        fprintf(stderr, "structured Discord REST observation failed\n");
        dcc_client_destroy(client);
        return 1;
    }

    unsigned legacy_count = 0U;
    if (dcc_rest_request(
            client, "GET", "/matrix/500", NULL, legacy_response_cb, &legacy_count
        ) != DCC_OK || legacy_count != 1U || seen.count != 2U ||
        seen.status != DCC_ERR_DISCORD || seen.http_status != 500U) {
        fprintf(stderr, "legacy callback plus observer delivery failed\n");
        dcc_client_destroy(client);
        return 1;
    }
    if (dcc_rest_request(client, "GET", "/matrix/network", NULL, NULL, NULL) != DCC_ERR_NETWORK ||
        seen.count != 3U || seen.status != DCC_ERR_NETWORK || seen.http_status != 0U) {
        fprintf(stderr, "intercepted transport observation failed\n");
        dcc_client_destroy(client);
        return 1;
    }
    if (dcc_rest_request(client, "GET", "/matrix/malformed", NULL, NULL, NULL) != DCC_OK ||
        seen.count != 4U || seen.discord_code != 0 || seen.message[0] != '\0' ||
        dcc_rest_request(client, "GET", "/matrix/truncated", NULL, NULL, NULL) != DCC_OK ||
        seen.count != 5U || seen.discord_code != 0 || seen.message[0] != '\0' ||
        dcc_rest_request(client, "GET", "/matrix/non-object", NULL, NULL, NULL) != DCC_OK ||
        seen.count != 6U || seen.discord_code != 0 || seen.message[0] != '\0' ||
        dcc_rest_request(client, "GET", "/matrix/non-nul", NULL, NULL, NULL) != DCC_OK ||
        seen.count != 7U || seen.discord_code != 0 || seen.message[0] != '\0') {
        fprintf(stderr, "bounded malformed JSON observation failed\n");
        dcc_client_destroy(client);
        return 1;
    }

    unsigned before = seen.count;
    dcc_rest_set_interceptor(client, NULL, NULL);
    if (dcc_rest_request(client, NULL, "/invalid", NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_rest_request_async(client, "GET", "/not-started", NULL, NULL, NULL) != DCC_ERR_STATE ||
        seen.count != before) {
        fprintf(stderr, "admission/validation error was observed as a completion\n");
        dcc_client_destroy(client);
        return 1;
    }
    if (dcc_client_on_error(client, NULL, NULL) != DCC_OK) {
        fprintf(stderr, "client observer clear failed\n");
        dcc_client_destroy(client);
        return 1;
    }
    dcc_client_destroy(client);
    return 0;
}

static int check_interceptor_transport_mapping(void) {
    static const char http_body[] = "{\"code\":9,\"message\":\"http\"}";
    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_client_t *client = NULL;
    if (dcc_client_create(&options, &client) != DCC_OK) {
        return 1;
    }

    intercept_script_t script;
    memset(&script, 0, sizeof(script));
    script.steps[0] = (intercept_step_t){
        .transport_status = DCC_OK,
        .legacy_error = DCC_ERR_NETWORK,
    };
    script.steps[1] = (intercept_step_t){
        .transport_status = DCC_OK,
        .legacy_error = DCC_ERR_TIMEOUT,
    };
    script.steps[2] = (intercept_step_t){
        .transport_status = DCC_OK,
        .legacy_error = DCC_ERR_CANCELED,
    };
    script.steps[3] = (intercept_step_t){
        .transport_status = DCC_OK,
        .http_status = 400U,
        .legacy_error = DCC_ERR_NETWORK,
        .body = http_body,
        .body_len = sizeof(http_body) - 1U,
    };
    script.steps[4] = (intercept_step_t){
        .transport_status = DCC_ERR_NETWORK,
    };
    script.step_count = 5U;
    dcc_rest_set_interceptor(client, scripted_intercept, &script);

    error_seen_t seen;
    legacy_seen_t legacy;
    memset(&seen, 0, sizeof(seen));
    memset(&legacy, 0, sizeof(legacy));
    if (dcc_client_on_error(client, public_error_observer, &seen) != DCC_OK ||
        dcc_rest_request(client, "GET", "/mapping/network", NULL,
            legacy_capture_cb, &legacy) != DCC_OK ||
        seen.count != 1U || seen.status != DCC_ERR_NETWORK ||
        seen.http_status != 0U || legacy.count != 1U ||
        legacy.status != 0U || legacy.error != DCC_ERR_NETWORK ||
        dcc_rest_request(client, "GET", "/mapping/timeout", NULL,
            legacy_capture_cb, &legacy) != DCC_OK ||
        seen.count != 2U || seen.status != DCC_ERR_TIMEOUT ||
        seen.http_status != 0U || legacy.count != 2U ||
        legacy.error != DCC_ERR_TIMEOUT ||
        dcc_rest_request(client, "GET", "/mapping/canceled", NULL,
            legacy_capture_cb, &legacy) != DCC_OK ||
        seen.count != 3U || seen.status != DCC_ERR_CANCELED ||
        seen.http_status != 0U || legacy.count != 3U ||
        legacy.error != DCC_ERR_CANCELED ||
        dcc_rest_request(client, "GET", "/mapping/http", NULL,
            legacy_capture_cb, &legacy) != DCC_OK ||
        seen.count != 4U || seen.status != DCC_ERR_DISCORD ||
        seen.http_status != 400U || seen.discord_code != 9 ||
        legacy.count != 4U || legacy.status != 400U ||
        legacy.error != DCC_ERR_NETWORK ||
        dcc_rest_request(client, "GET", "/mapping/no-callback", NULL,
            legacy_capture_cb, &legacy) != DCC_ERR_NETWORK ||
        seen.count != 5U || seen.status != DCC_ERR_NETWORK ||
        seen.http_status != 0U || legacy.count != 4U) {
        fprintf(stderr, "interceptor transport/HTTP mapping failed\n");
        dcc_client_destroy(client);
        return 1;
    }
    script.steps[script.next_step] = (intercept_step_t){
        .transport_status = DCC_ERR_NETWORK,
    };
    script.step_count = script.next_step + 1U;
    if (dcc_rest_request_async(client, "GET", "/mapping/async-admission", NULL,
            legacy_capture_cb, &legacy) != DCC_ERR_NETWORK ||
        seen.count != 5U || legacy.count != 4U) {
        fprintf(stderr, "async interceptor admission failure was observed\n");
        dcc_client_destroy(client);
        return 1;
    }
    dcc_client_destroy(client);

    dcc_client_t *foreign = NULL;
    client = NULL;
    if (dcc_client_create(&options, &client) != DCC_OK ||
        dcc_client_create(&options, &foreign) != DCC_OK) {
        dcc_client_destroy(client);
        dcc_client_destroy(foreign);
        return 1;
    }
    error_seen_t original_seen;
    error_seen_t foreign_seen;
    memset(&original_seen, 0, sizeof(original_seen));
    memset(&foreign_seen, 0, sizeof(foreign_seen));
    memset(&legacy, 0, sizeof(legacy));
    adversarial_intercept_state_t adversarial = {
        .foreign_client = foreign,
    };
    dcc_rest_set_interceptor(client, adversarial_intercept, &adversarial);
    if (dcc_client_on_error(client, public_error_observer, &original_seen) != DCC_OK ||
        dcc_client_on_error(foreign, public_error_observer, &foreign_seen) != DCC_OK ||
        dcc_rest_request(client, "GET", "/mapping/mismatch", NULL,
            legacy_capture_cb, &legacy) != DCC_ERR_STATE ||
        adversarial.callback_attempts != 2U || legacy.count != 1U ||
        legacy.status != 0U || legacy.error != DCC_ERR_RUNTIME ||
        original_seen.count != 1U || original_seen.status != DCC_ERR_RUNTIME ||
        foreign_seen.count != 0U) {
        fprintf(stderr, "interceptor mismatch/duplicate fail-closed contract failed\n");
        dcc_client_destroy(client);
        dcc_client_destroy(foreign);
        return 1;
    }
    dcc_client_destroy(client);
    dcc_client_destroy(foreign);
    return 0;
}

static int check_terminal_destroy_lifetime(void) {
    static const char body[] = "{\"code\":17,\"message\":\"terminal\"}";
    client_terminal_destroy_state_t client_state;
    memset(&client_state, 0, sizeof(client_state));
    dcc_client_options_t client_options = {
        .size = sizeof(client_options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .log_fn = client_terminal_destroy_log,
        .log_user_data = &client_state,
    };
    dcc_client_t *client = NULL;
    if (dcc_client_create(&client_options, &client) != DCC_OK) {
        return 1;
    }
    client_state.client = client;
    intercept_script_t script;
    memset(&script, 0, sizeof(script));
    script.steps[0] = (intercept_step_t){
        .transport_status = DCC_OK,
        .http_status = 400U,
        .legacy_error = DCC_OK,
        .body = body,
        .body_len = sizeof(body) - 1U,
    };
    script.step_count = 1U;
    dcc_rest_set_interceptor(client, scripted_intercept, &script);
    if (dcc_client_on_error(client, client_terminal_destroy_observer, &client_state) != DCC_OK ||
        dcc_rest_request(client, "GET", "/lifetime/client", NULL,
            client_terminal_destroy_legacy, &client_state) != DCC_OK ||
        script.callback_count != 1U || client_state.legacy_count != 1U ||
        client_state.log_count != 1U || client_state.observer_count != 1U) {
        fprintf(stderr, "terminal client destroy deferral failed\n");
        return 1;
    }
    dcc_client_destroy(client);

    for (unsigned error_case = 0U; error_case < 2U; ++error_case) {
        dcc_app_options_t app_options;
        dcc_app_options_init(&app_options);
        app_options.client.token = "";
        app_options.client.intents = DCC_INTENT_GUILDS;
        dcc_app_t *app = NULL;
        if (dcc_app_create(&app_options, &app) != DCC_OK) {
            return 1;
        }
        app_terminal_destroy_state_t app_state;
        memset(&app_state, 0, sizeof(app_state));
        app_state.app = app;
        app_state.legacy_destroy_status = DCC_OK;
        app_state.app_observer_destroy_status = DCC_OK;
        app_state.client_observer_destroy_status = DCC_OK;
        memset(&script, 0, sizeof(script));
        script.steps[0] = (intercept_step_t){
            .transport_status = DCC_OK,
            .http_status = error_case ? 400U : 204U,
            .legacy_error = DCC_OK,
            .body = error_case ? body : NULL,
            .body_len = error_case ? sizeof(body) - 1U : 0U,
        };
        script.step_count = 1U;
        client = dcc_app_client(app);
        dcc_rest_set_interceptor(client, scripted_intercept, &script);
        if (dcc_app_on_error(app, app_terminal_destroy_observer, &app_state) != DCC_OK ||
            dcc_client_on_error(client, app_terminal_destroy_client_observer, &app_state) != DCC_OK ||
            dcc_rest_request(client, "GET", error_case ? "/lifetime/app-error" :
                "/lifetime/app-success", NULL, app_terminal_destroy_legacy,
                &app_state) != DCC_OK ||
            script.callback_count != 1U || app_state.legacy_count != 1U ||
            app_state.legacy_destroy_status != DCC_ERR_STATE ||
            (!error_case && (app_state.app_sink_in_flight != 0U ||
                app_state.app_observer_count != 0U ||
                app_state.client_observer_count != 0U)) ||
            (error_case && (app_state.app_sink_in_flight != 1U ||
                app_state.app_observer_count != 1U ||
                app_state.client_observer_count != 1U ||
                app_state.app_observer_destroy_status != DCC_ERR_STATE ||
                app_state.client_observer_destroy_status != DCC_ERR_STATE))) {
            fprintf(stderr, "terminal App %s destroy rejection failed\n",
                error_case ? "error" : "success");
            return 1;
        }
        if (dcc_app_destroy(app) != DCC_OK) {
            fprintf(stderr, "owner App destroy after terminal callback failed\n");
            return 1;
        }
    }
    return 0;
}

static int check_terminal_nested_rest(void) {
    static const char body[] = "{\"code\":17,\"message\":\"outer\"}";
    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_client_t *client = NULL;
    if (dcc_client_create(&options, &client) != DCC_OK) {
        return 1;
    }

    intercept_script_t script;
    memset(&script, 0, sizeof(script));
    script.steps[0] = (intercept_step_t){
        .transport_status = DCC_OK,
        .http_status = 400U,
        .legacy_error = DCC_OK,
        .body = body,
        .body_len = sizeof(body) - 1U,
    };
    script.steps[1] = (intercept_step_t){
        .transport_status = DCC_OK,
        .http_status = 204U,
        .legacy_error = DCC_OK,
    };
    script.step_count = 2U;
    nested_terminal_state_t nested = {
        .nested_status = DCC_ERR_STATE,
    };
    error_seen_t seen;
    memset(&seen, 0, sizeof(seen));
    dcc_rest_set_interceptor(client, scripted_intercept, &script);
    if (dcc_client_on_error(client, public_error_observer, &seen) != DCC_OK ||
        dcc_rest_request(client, "GET", "/lifetime/nested-outer", NULL,
            nested_terminal_outer_cb, &nested) != DCC_OK ||
        nested.nested_status != DCC_OK || nested.outer_count != 1U ||
        nested.inner_count != 1U || script.request_count != 2U ||
        script.callback_count != 2U || script.next_step != 2U ||
        seen.count != 1U || seen.status != DCC_ERR_DISCORD ||
        seen.http_status != 400U) {
        fprintf(stderr, "nested terminal REST delivery failed\n");
        dcc_client_destroy(client);
        return 1;
    }
    dcc_client_destroy(client);
    return 0;
}

#if !defined(_WIN32)
static int check_async_rate_limit_case(int terminal) {
    http_server_t server;
    pthread_t server_thread;
    int server_status = terminal
        ? start_server_mode(&server, &server_thread, 8)
        : start_rate_limited_server(&server, &server_thread);
    if (server_status != 0) {
        fprintf(stderr, "async rate-limit server creation failed\n");
        return 1;
    }

    async_error_seen_t seen;
    dcc_client_t *client = create_async_client(&seen);
    if (client == NULL) {
        close(server.fd);
        (void)pthread_join(server_thread, NULL);
        fprintf(stderr, "async rate-limit client creation failed\n");
        return 1;
    }
    char url[160];
    snprintf(
        url,
        sizeof(url),
        "http://127.0.0.1:%u/channels/7/messages/9",
        (unsigned)server.port
    );
    dcc_status_t status = dcc_rest_request_async(
        client,
        "GET",
        url,
        NULL,
        async_legacy_response_cb,
        &seen
    );
    client_runner_t runner;
    pthread_t runner_thread;
    int runner_started = status == DCC_OK &&
        start_client_runner(client, &runner, &runner_thread) == 0;
    if (!runner_started) {
        dcc_client_destroy(client);
        close(server.fd);
        (void)pthread_join(server_thread, NULL);
        fprintf(stderr, "async rate-limit runner creation failed\n");
        return 1;
    }
    status = dcc_rest_async_wait(client, 5000U);
    int stop_failed = stop_runner_and_client(
        client,
        &runner,
        runner_thread
    );
    (void)pthread_join(server_thread, NULL);
    close(server.fd);

    unsigned observed = atomic_load_explicit(&seen.count, memory_order_acquire);
    unsigned legacy = atomic_load_explicit(&seen.legacy_count, memory_order_acquire);
    if (status != DCC_OK || stop_failed || legacy != 1U ||
        server.request_count != (terminal ? 4U : 2U) ||
        (!terminal && observed != 0U) ||
        (terminal && (observed != 1U || seen.status != DCC_ERR_DISCORD ||
            seen.http_status != 429U || seen.discord_code != 20028 ||
            strcmp(seen.operation, url) != 0))) {
        fprintf(
            stderr,
            "async %s 429 completion mismatch: status=%s observed=%u legacy=%u requests=%u\n",
            terminal ? "terminal" : "transient",
            dcc_status_string(status),
            observed,
            legacy,
            server.request_count
        );
        return 1;
    }
    return 0;
}

static int check_async_pending_cancel(void) {
    http_server_t server;
    pthread_t server_thread;
    if (start_server(&server, &server_thread) != 0) {
        fprintf(stderr, "pending-cancel server creation failed\n");
        return 1;
    }
    server.delay_ms = 200U;

    async_error_seen_t seen;
    dcc_client_t *client = create_async_client(&seen);
    if (client == NULL) {
        close(server.fd);
        (void)pthread_join(server_thread, NULL);
        return 1;
    }
    char active_url[160];
    const char pending_url[] = "http://127.0.0.1:9/pending-cancel";
    snprintf(
        active_url,
        sizeof(active_url),
        "http://127.0.0.1:%u/active-before-pending-cancel",
        (unsigned)server.port
    );
    dcc_status_t status = dcc_rest_request_async(
        client, "GET", active_url, NULL, NULL, NULL
    );
    if (status == DCC_OK) {
        status = dcc_rest_request_async(
            client, "GET", pending_url, NULL, NULL, NULL
        );
    }
    client_runner_t runner;
    pthread_t runner_thread;
    int runner_started = status == DCC_OK &&
        start_client_runner(client, &runner, &runner_thread) == 0;
    if (!runner_started) {
        dcc_client_destroy(client);
        close(server.fd);
        (void)pthread_join(server_thread, NULL);
        return 1;
    }

    size_t canceled = 0U;
    if (!wait_for_request(&server, 1000U)) {
        status = DCC_ERR_TIMEOUT;
    } else {
        status = dcc_rest_async_cancel_pending(client, &canceled);
    }
    if (status == DCC_OK) {
        status = dcc_rest_async_wait(client, 3000U);
    }
    int stop_failed = stop_runner_and_client(client, &runner, runner_thread);
    (void)pthread_join(server_thread, NULL);
    close(server.fd);

    unsigned observed = atomic_load_explicit(&seen.count, memory_order_acquire);
    if (status != DCC_OK || stop_failed || canceled != 1U || observed != 1U ||
        seen.status != DCC_ERR_CANCELED || seen.http_status != 0U ||
        strcmp(seen.operation, pending_url) != 0) {
        fprintf(
            stderr,
            "pending cancellation observation mismatch: status=%s canceled=%zu observed=%u\n",
            dcc_status_string(status),
            canceled,
            observed
        );
        return 1;
    }
    return 0;
}

static int check_async_active_cancel(void) {
    http_server_t server;
    pthread_t server_thread;
    if (start_server(&server, &server_thread) != 0) {
        fprintf(stderr, "active-cancel server creation failed\n");
        return 1;
    }
    server.delay_ms = 300U;

    async_error_seen_t seen;
    dcc_client_t *client = create_async_client(&seen);
    if (client == NULL) {
        close(server.fd);
        (void)pthread_join(server_thread, NULL);
        return 1;
    }
    char url[160];
    snprintf(
        url,
        sizeof(url),
        "http://127.0.0.1:%u/active-cancel",
        (unsigned)server.port
    );
    dcc_status_t status = dcc_rest_request_async(
        client, "GET", url, NULL, NULL, NULL
    );
    client_runner_t runner;
    pthread_t runner_thread;
    int runner_started = status == DCC_OK &&
        start_client_runner(client, &runner, &runner_thread) == 0;
    if (!runner_started) {
        dcc_client_destroy(client);
        close(server.fd);
        (void)pthread_join(server_thread, NULL);
        return 1;
    }

    size_t canceled = 0U;
    if (!wait_for_request(&server, 1000U)) {
        status = DCC_ERR_TIMEOUT;
    } else {
        status = dcc_rest_async_cancel_active(client, &canceled);
    }
    if (status == DCC_OK) {
        status = dcc_rest_async_wait(client, 3000U);
    }
    int stop_failed = stop_runner_and_client(client, &runner, runner_thread);
    (void)pthread_join(server_thread, NULL);
    close(server.fd);

    unsigned observed = atomic_load_explicit(&seen.count, memory_order_acquire);
    if (status != DCC_OK || stop_failed || canceled != 1U || observed != 1U ||
        seen.status != DCC_ERR_CANCELED || strcmp(seen.operation, url) != 0) {
        fprintf(
            stderr,
            "active cancellation observation mismatch: status=%s canceled=%zu observed=%u\n",
            dcc_status_string(status),
            canceled,
            observed
        );
        return 1;
    }
    return 0;
}

static int check_async_shutdown_drain(void) {
    async_error_seen_t seen;
    dcc_client_t *client = create_async_client(&seen);
    if (client == NULL) {
        return 1;
    }
    if (dcc_rest_request_async(
            client,
            "GET",
            "http://127.0.0.1:9/shutdown-active",
            NULL,
            NULL,
            NULL
        ) != DCC_OK ||
        dcc_rest_request_async(
            client,
            "GET",
            "http://127.0.0.1:9/shutdown-pending",
            NULL,
            NULL,
            NULL
        ) != DCC_OK) {
        dcc_client_destroy(client);
        return 1;
    }
    dcc_client_destroy(client);
    unsigned observed = atomic_load_explicit(&seen.count, memory_order_acquire);
    if (observed != 2U || seen.status != DCC_ERR_CANCELED) {
        fprintf(stderr, "shutdown drain observation mismatch: observed=%u\n", observed);
        return 1;
    }
    return 0;
}

static int check_async_terminal_matrix(void) {
    return check_async_rate_limit_case(0) ||
        check_async_rate_limit_case(1) ||
        check_async_pending_cancel() ||
        check_async_active_cancel() ||
        check_async_shutdown_drain();
}
#else
static int check_async_terminal_matrix(void) {
    return 0;
}
#endif

static void app_error_observer(dcc_app_t *app, const dcc_error_t *error, void *user_data) {
    (void)app;
    app_error_state_t *state = (app_error_state_t *)user_data;
    if (state != NULL) {
        copy_error(&state->seen, error);
    }
}

#if !defined(_WIN32)
static void blocking_app_error_observer(
    dcc_app_t *app,
    const dcc_error_t *error,
    void *user_data
) {
    (void)app;
    app_destroy_observer_state_t *state =
        (app_destroy_observer_state_t *)user_data;
    if (state == NULL || error == NULL) {
        return;
    }
    copy_error(&state->seen, error);
    atomic_store_explicit(&state->observer_entered, 1U, memory_order_release);
    while (atomic_load_explicit(&state->observer_release, memory_order_acquire) == 0U) {
        usleep(1000U);
    }
}

static void *app_error_request_main(void *user_data) {
    app_destroy_observer_state_t *state =
        (app_destroy_observer_state_t *)user_data;
    state->request_status = dcc_rest_request(
        state->client,
        "GET",
        "/app/destroy-in-flight",
        NULL,
        NULL,
        NULL
    );
    return NULL;
}

static void *app_destroy_main(void *user_data) {
    app_destroy_observer_state_t *state =
        (app_destroy_observer_state_t *)user_data;
    state->destroy_status = dcc_app_destroy(state->app);
    atomic_store_explicit(&state->destroy_finished, 1U, memory_order_release);
    return NULL;
}

#endif

static void app_client_observer(dcc_client_t *client, const dcc_error_t *error, void *user_data) {
    (void)client;
    app_error_state_t *state = (app_error_state_t *)user_data;
    if (state != NULL && error != NULL) {
        state->client_seen++;
    }
}

static void reentrant_client_replacement(
    dcc_client_t *client,
    const dcc_error_t *error,
    void *user_data
) {
    reentrant_observer_state_t *state = (reentrant_observer_state_t *)user_data;
    if (client != NULL && error != NULL && state != NULL) {
        state->replacement_count++;
        state->mutation_status = dcc_client_on_error(client, NULL, NULL);
    }
}

static void reentrant_client_first(
    dcc_client_t *client,
    const dcc_error_t *error,
    void *user_data
) {
    reentrant_observer_state_t *state = (reentrant_observer_state_t *)user_data;
    if (client != NULL && error != NULL && state != NULL) {
        state->first_count++;
        state->mutation_status = dcc_client_on_error(
            client,
            reentrant_client_replacement,
            state
        );
    }
}

static void reentrant_app_replacement(
    dcc_app_t *app,
    const dcc_error_t *error,
    void *user_data
) {
    reentrant_observer_state_t *state = (reentrant_observer_state_t *)user_data;
    if (app != NULL && error != NULL && state != NULL) {
        state->replacement_count++;
        state->mutation_status = dcc_app_on_error(app, NULL, NULL);
    }
}

static void reentrant_app_first(
    dcc_app_t *app,
    const dcc_error_t *error,
    void *user_data
) {
    reentrant_observer_state_t *state = (reentrant_observer_state_t *)user_data;
    if (app != NULL && error != NULL && state != NULL) {
        state->first_count++;
        state->mutation_status = dcc_app_on_error(
            app,
            reentrant_app_replacement,
            state
        );
    }
}

static dcc_status_t failing_route(dcc_ctx_t *ctx, void *user_data) {
    app_error_state_t *state = (app_error_state_t *)user_data;
    if (ctx == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    state->route_calls++;
    return DCC_ERR_STATE;
}

static dcc_status_t responded_route(dcc_ctx_t *ctx, void *user_data) {
    app_error_state_t *state = (app_error_state_t *)user_data;
    if (ctx == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    state->route_calls++;
    if (dcc_ctx_reply_ephemeral_text(ctx, "explicit response", NULL, NULL) != DCC_OK) {
        return DCC_ERR_RUNTIME;
    }
    return DCC_ERR_STATE;
}

static dcc_message_builder_t invalid_message_builder(void) {
    dcc_message_builder_t message;
    dcc_message_builder_init(&message);
    message.present |= DCC_MESSAGE_BUILDER_PRESENT_CONTENT;
    message.content = NULL;
    return message;
}

static dcc_status_t invalid_initial_reply_route(dcc_ctx_t *ctx, void *user_data) {
    app_error_state_t *state = (app_error_state_t *)user_data;
    if (ctx == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    state->route_calls++;
    dcc_message_builder_t invalid = invalid_message_builder();
    return dcc_ctx_reply(ctx, &invalid, NULL, NULL);
}

static dcc_status_t failed_initial_admission_route(dcc_ctx_t *ctx, void *user_data) {
    app_error_state_t *state = (app_error_state_t *)user_data;
    if (ctx == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    state->route_calls++;
    return dcc_ctx_reply_text(ctx, "initial admission", NULL, NULL);
}

static dcc_status_t admitted_then_local_failure_route(dcc_ctx_t *ctx, void *user_data) {
    app_error_state_t *state = (app_error_state_t *)user_data;
    if (ctx == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    state->route_calls++;
    dcc_status_t status = dcc_ctx_reply_text(ctx, "admitted initial", NULL, NULL);
    if (status != DCC_OK) {
        return status;
    }
    dcc_message_builder_t invalid = invalid_message_builder();
    return dcc_ctx_edit_original(ctx, &invalid, NULL, NULL);
}

static dcc_status_t deferred_then_local_failure_route(dcc_ctx_t *ctx, void *user_data) {
    app_error_state_t *state = (app_error_state_t *)user_data;
    if (ctx == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    state->route_calls++;
    dcc_status_t status = dcc_ctx_defer_ephemeral(ctx, NULL, NULL);
    if (status != DCC_OK) {
        return status;
    }
    dcc_message_builder_t invalid = invalid_message_builder();
    return dcc_ctx_edit_original(ctx, &invalid, NULL, NULL);
}

static dcc_status_t failing_event(
    dcc_app_t *app,
    const dcc_event_t *event,
    void *user_data
) {
    app_error_state_t *state = (app_error_state_t *)user_data;
    if (app == NULL || event == NULL || state == NULL ||
        dcc_event_type(event) != DCC_EVENT_LOG) {
        return DCC_ERR_INVALID_ARG;
    }
    state->event_calls++;
    return DCC_ERR_STATE;
}

static dcc_status_t failing_task(dcc_app_t *app, void *user_data) {
    app_error_state_t *state = (app_error_state_t *)user_data;
    if (app == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    state->task_calls++;
    return DCC_ERR_STATE;
}

static dcc_listener_t route_listener(
    const char *name,
    dcc_app_handler_fn handler,
    void *user_data
) {
    dcc_listener_t listener;
    dcc_listener_init(&listener, DCC_LISTENER_SLASH);
    listener.handler.plain = handler;
    listener.user_data = user_data;
    listener.target.route.name = name;
    listener.target.route.description = "error smoke";
    return listener;
}

static dcc_status_t dispatch_slash(dcc_app_t *app, const char *name, const char *token) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_SLASH_COMMAND;
    event.data.interaction.name = name;
    event.data.interaction.token = token;
    event.data.interaction.id = 1U;
    event.data.interaction.application_id = 2U;
    return dcc_event_bus_dispatch(&dcc_app_client(app)->events, dcc_app_client(app), &event);
}

static dcc_status_t dispatch_slash_and_drain(
    dcc_app_t *app,
    const char *name,
    const char *token
) {
    dcc_status_t status = dispatch_slash(app, name, token);
    return status == DCC_OK
        ? dcc_rest_async_wait(dcc_app_client(app), 5000U)
        : status;
}

static dcc_status_t dispatch_log(dcc_app_t *app) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_LOG;
    return dcc_event_bus_dispatch(&dcc_app_client(app)->events, dcc_app_client(app), &event);
}

static int check_observer_reentrancy(void) {
    static const char body[] = "{\"code\":1,\"message\":\"failure\"}";
    dcc_client_options_t client_options = {
        .size = sizeof(client_options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_client_t *client = NULL;
    if (dcc_client_create(&client_options, &client) != DCC_OK) {
        return 1;
    }
    intercept_script_t script;
    memset(&script, 0, sizeof(script));
    for (size_t i = 0U; i < 3U; ++i) {
        script.steps[i] = (intercept_step_t){
            .transport_status = DCC_OK,
            .http_status = 400U,
            .legacy_error = DCC_OK,
            .body = body,
            .body_len = sizeof(body) - 1U,
        };
    }
    script.step_count = 3U;
    dcc_rest_set_interceptor(client, scripted_intercept, &script);
    reentrant_observer_state_t client_state;
    memset(&client_state, 0, sizeof(client_state));
    client_state.mutation_status = DCC_ERR_STATE;
    if (dcc_client_on_error(client, reentrant_client_first, &client_state) != DCC_OK ||
        dcc_rest_request(client, "GET", "/replace/one", NULL, NULL, NULL) != DCC_OK ||
        dcc_rest_request(client, "GET", "/replace/two", NULL, NULL, NULL) != DCC_OK ||
        dcc_rest_request(client, "GET", "/replace/three", NULL, NULL, NULL) != DCC_OK ||
        client_state.first_count != 1U || client_state.replacement_count != 1U ||
        client_state.mutation_status != DCC_OK) {
        fprintf(stderr, "client observer reentrant replace/clear failed\n");
        dcc_client_destroy(client);
        return 1;
    }
    dcc_client_destroy(client);

    dcc_app_options_t app_options;
    dcc_app_options_init(&app_options);
    app_options.client.token = "";
    app_options.client.intents = DCC_INTENT_GUILDS;
    dcc_app_t *app = NULL;
    if (dcc_app_create(&app_options, &app) != DCC_OK) {
        return 1;
    }
    reentrant_observer_state_t app_state;
    memset(&app_state, 0, sizeof(app_state));
    app_state.mutation_status = DCC_ERR_STATE;
    app_error_state_t route_state;
    memset(&route_state, 0, sizeof(route_state));
    dcc_listener_t listener = route_listener("reentrant", failing_route, &route_state);
    if (dcc_app_on_error(app, reentrant_app_first, &app_state) != DCC_OK ||
        dcc_app_listen(app, &listener, NULL) != DCC_OK ||
        dispatch_slash(app, "reentrant", "one") != DCC_OK ||
        dispatch_slash(app, "reentrant", "two") != DCC_OK ||
        dispatch_slash(app, "reentrant", "three") != DCC_OK ||
        route_state.route_calls != 3U || app_state.first_count != 1U ||
        app_state.replacement_count != 1U || app_state.mutation_status != DCC_OK) {
        fprintf(stderr, "App observer reentrant replace/clear failed\n");
        (void)dcc_app_destroy(app);
        return 1;
    }
    return dcc_app_destroy(app) == DCC_OK ? 0 : 1;
}

#if !defined(_WIN32)
static int check_app_sink_destroy_wait(void) {
    static const char body[] = "{\"code\":2,\"message\":\"in flight\"}";
    dcc_app_options_t options;
    dcc_app_options_init(&options);
    options.client.token = "";
    options.client.intents = DCC_INTENT_GUILDS;
    dcc_app_t *app = NULL;
    if (dcc_app_create(&options, &app) != DCC_OK) {
        return 1;
    }

    intercept_script_t script;
    memset(&script, 0, sizeof(script));
    script.steps[0] = (intercept_step_t){
        .transport_status = DCC_OK,
        .http_status = 400U,
        .legacy_error = DCC_OK,
        .body = body,
        .body_len = sizeof(body) - 1U,
    };
    script.step_count = 1U;
    dcc_rest_set_interceptor(dcc_app_client(app), scripted_intercept, &script);

    app_destroy_observer_state_t state;
    memset(&state, 0, sizeof(state));
    state.app = app;
    state.client = dcc_app_client(app);
    state.request_status = DCC_ERR_STATE;
    state.destroy_status = DCC_ERR_STATE;
    atomic_init(&state.observer_entered, 0U);
    atomic_init(&state.observer_release, 0U);
    atomic_init(&state.destroy_finished, 0U);
    if (dcc_app_on_error(app, blocking_app_error_observer, &state) != DCC_OK) {
        (void)dcc_app_destroy(app);
        return 1;
    }

    pthread_t request_thread;
    if (pthread_create(&request_thread, NULL, app_error_request_main, &state) != 0) {
        (void)dcc_app_destroy(app);
        return 1;
    }
    if (!wait_for_atomic_nonzero(&state.observer_entered, 2000U)) {
        atomic_store_explicit(&state.observer_release, 1U, memory_order_release);
        (void)pthread_join(request_thread, NULL);
        (void)dcc_app_destroy(app);
        fprintf(stderr, "App sink callback did not enter\n");
        return 1;
    }

    app_error_state_t replacement_state;
    memset(&replacement_state, 0, sizeof(replacement_state));
    if (dcc_app_on_error(app, app_error_observer, &replacement_state) != DCC_OK ||
        dcc_app_on_error(app, NULL, NULL) != DCC_OK) {
        atomic_store_explicit(&state.observer_release, 1U, memory_order_release);
        (void)pthread_join(request_thread, NULL);
        (void)dcc_app_destroy(app);
        fprintf(stderr, "App sink observer replace/clear while in flight failed\n");
        return 1;
    }

    pthread_t destroy_thread;
    if (pthread_create(&destroy_thread, NULL, app_destroy_main, &state) != 0) {
        atomic_store_explicit(&state.observer_release, 1U, memory_order_release);
        (void)pthread_join(request_thread, NULL);
        (void)dcc_app_destroy(app);
        return 1;
    }
    int teardown_started = wait_for_app_teardown(app, 2000U);
    int destroy_waited = atomic_load_explicit(
        &state.destroy_finished,
        memory_order_acquire
    ) == 0U;
    atomic_store_explicit(&state.observer_release, 1U, memory_order_release);
    int request_join = pthread_join(request_thread, NULL);
    int destroy_join = pthread_join(destroy_thread, NULL);
    if (!teardown_started || !destroy_waited || request_join != 0 || destroy_join != 0 ||
        state.request_status != DCC_OK || state.destroy_status != DCC_OK ||
        state.seen.count != 1U || !state.seen.valid_metadata ||
        state.seen.origin != DCC_ERROR_REST || state.seen.status != DCC_ERR_DISCORD ||
        strcmp(state.seen.operation, "/app/destroy-in-flight") != 0) {
        fprintf(stderr, "App sink in-flight destroy wait failed\n");
        return 1;
    }
    return 0;
}

#else
static int check_app_sink_destroy_wait(void) {
    return 0;
}
#endif

#if defined(_WIN32)
static int check_app_error_policy(void) {
    return 0;
}
#else
static int check_app_error_policy(void) {
    dcc_app_options_t options;
    dcc_app_options_init(&options);
    options.client.token = "";
    options.client.intents = DCC_INTENT_GUILDS;
    dcc_app_t *app = NULL;
    if (dcc_app_create(&options, &app) != DCC_OK) {
        fprintf(stderr, "App error-policy creation failed\n");
        return 1;
    }
    dcc_client_t *client = dcc_app_client(app);
    client_runner_t runner;
    pthread_t runner_thread;
    if (dcc_client_start(client) != DCC_OK ||
        start_client_runner(client, &runner, &runner_thread) != 0) {
        (void)dcc_app_destroy(app);
        fprintf(stderr, "App error-policy runtime start failed\n");
        return 1;
    }

    app_error_state_t state;
    memset(&state, 0, sizeof(state));
    intercept_script_t script;
    memset(&script, 0, sizeof(script));
    for (size_t i = 0U; i < 4U; ++i) {
        script.steps[i] = (intercept_step_t){
            .transport_status = DCC_OK,
            .http_status = 204U,
            .legacy_error = DCC_OK,
        };
    }
    script.step_count = 4U;
    dcc_rest_set_interceptor(dcc_app_client(app), scripted_intercept, &script);
    if (dcc_app_on_error(NULL, app_error_observer, &state) != DCC_ERR_INVALID_ARG ||
        dcc_app_on_error(app, NULL, &state) != DCC_ERR_INVALID_ARG ||
        dcc_app_on_error(app, app_error_observer, &state) != DCC_OK ||
        dcc_client_on_error(dcc_app_client(app), app_client_observer, &state) != DCC_OK ||
        dcc_app_use_default_error_responses(app) != DCC_OK) {
        fprintf(stderr, "App observer/default policy registration failed\n");
        (void)stop_app_runner_and_destroy(app, &runner, runner_thread);
        return 1;
    }

    dcc_listener_t listener = route_listener("fails", failing_route, &state);
    dcc_listener_t responded = route_listener("responded", responded_route, &state);
    dcc_listener_t invalid_initial = route_listener(
        "invalid-initial",
        invalid_initial_reply_route,
        &state
    );
    dcc_listener_t failed_admission = route_listener(
        "failed-admission",
        failed_initial_admission_route,
        &state
    );
    dcc_listener_t admitted_local = route_listener(
        "admitted-local",
        admitted_then_local_failure_route,
        &state
    );
    dcc_listener_t deferred_local = route_listener(
        "deferred-local",
        deferred_then_local_failure_route,
        &state
    );
    dcc_listener_t generic_failure = route_listener(
        "generic-failure",
        invalid_initial_reply_route,
        &state
    );
    if (dcc_app_listen(app, &listener, NULL) != DCC_OK ||
        dcc_app_listen(app, &responded, NULL) != DCC_OK ||
        dcc_app_listen(app, &invalid_initial, NULL) != DCC_OK ||
        dcc_app_listen(app, &failed_admission, NULL) != DCC_OK ||
        dcc_app_listen(app, &admitted_local, NULL) != DCC_OK ||
        dcc_app_listen(app, &deferred_local, NULL) != DCC_OK ||
        dcc_app_listen(app, &generic_failure, NULL) != DCC_OK ||
        dispatch_slash_and_drain(app, "fails", "secret-token") != DCC_OK ||
        state.route_calls != 1U || state.seen.count != 1U ||
        state.seen.origin != DCC_ERROR_HANDLER || state.seen.status != DCC_ERR_STATE ||
        strcmp(state.seen.operation, "fails") != 0 ||
        script.request_count != 1U || script.last_body_len == 0U ||
        strstr(script.last_body, "Something went wrong. Please try again.") == NULL ||
        strstr(script.last_body, "secret-token") != NULL ||
        strstr(script.last_body, "invalid state") != NULL ||
        strstr(script.last_body, "\"flags\":64") == NULL) {
        fprintf(stderr, "safe generic App failure policy failed\n");
        (void)stop_app_runner_and_destroy(app, &runner, runner_thread);
        return 1;
    }

    unsigned previous_errors = state.seen.count;
    if (dispatch_slash_and_drain(app, "responded", "second-secret") != DCC_OK ||
        state.route_calls != 2U || state.seen.count != previous_errors + 1U ||
        script.request_count != 2U ||
        strstr(script.last_body, "explicit response") == NULL) {
        fprintf(stderr, "response-started failure emitted a second response\n");
        (void)stop_app_runner_and_destroy(app, &runner, runner_thread);
        return 1;
    }

    size_t previous_requests = script.request_count;
    script.step_count = script.next_step + 1U;
    if (dispatch_slash_and_drain(app, "invalid-initial", "invalid-secret") != DCC_OK ||
        script.request_count != previous_requests + 1U ||
        strstr(script.last_body, "Something went wrong. Please try again.") == NULL ||
        strstr(script.last_body, "invalid-secret") != NULL) {
        fprintf(stderr, "invalid initial reply did not emit one safe fallback\n");
        (void)stop_app_runner_and_destroy(app, &runner, runner_thread);
        return 1;
    }

    previous_requests = script.request_count;
    unsigned previous_app_errors = state.seen.count;
    unsigned previous_client_errors = state.client_seen;
    script.steps[script.next_step] = (intercept_step_t){
        .transport_status = DCC_ERR_NETWORK,
    };
    script.step_count = script.next_step + 1U;
    if (dispatch_slash_and_drain(app, "failed-admission", "admission-secret") != DCC_OK ||
        script.request_count != previous_requests + 1U ||
        state.seen.count != previous_app_errors + 1U ||
        state.client_seen != previous_client_errors + 1U ||
        strstr(script.last_body, "initial admission") == NULL ||
        strstr(script.last_body, "Something went wrong. Please try again.") != NULL) {
        fprintf(stderr, "terminal initial-response failure violated admission semantics\n");
        (void)stop_app_runner_and_destroy(app, &runner, runner_thread);
        return 1;
    }

    previous_requests = script.request_count;
    script.steps[script.next_step] = (intercept_step_t){
        .transport_status = DCC_OK,
        .http_status = 204U,
        .legacy_error = DCC_OK,
    };
    script.step_count = script.next_step + 1U;
    if (dispatch_slash_and_drain(app, "admitted-local", "admitted-secret") != DCC_OK ||
        script.request_count != previous_requests + 1U ||
        strstr(script.last_body, "admitted initial") == NULL) {
        fprintf(stderr, "admitted initial local failure emitted fallback\n");
        (void)stop_app_runner_and_destroy(app, &runner, runner_thread);
        return 1;
    }

    previous_requests = script.request_count;
    script.steps[script.next_step] = (intercept_step_t){
        .transport_status = DCC_OK,
        .http_status = 204U,
        .legacy_error = DCC_OK,
    };
    script.step_count = script.next_step + 1U;
    if (dispatch_slash_and_drain(app, "deferred-local", "deferred-secret") != DCC_OK ||
        script.request_count != previous_requests + 1U ||
        strstr(script.last_body, "\"type\":5") == NULL) {
        fprintf(stderr, "admitted defer local failure emitted fallback\n");
        (void)stop_app_runner_and_destroy(app, &runner, runner_thread);
        return 1;
    }

    previous_requests = script.request_count;
    previous_app_errors = state.seen.count;
    previous_client_errors = state.client_seen;
    script.steps[script.next_step] = (intercept_step_t){
        .transport_status = DCC_ERR_NETWORK,
    };
    script.step_count = script.next_step + 1U;
    if (dispatch_slash_and_drain(app, "generic-failure", "generic-secret") != DCC_OK ||
        script.request_count != previous_requests + 1U ||
        state.seen.count != previous_app_errors + 2U ||
        state.client_seen != previous_client_errors + 1U ||
        strstr(script.last_body, "Something went wrong. Please try again.") == NULL ||
        strstr(script.last_body, "generic-secret") != NULL ||
        strstr(script.last_body, "invalid argument") != NULL) {
        fprintf(stderr, "failing generic fallback recursed, duplicated, or leaked data\n");
        (void)stop_app_runner_and_destroy(app, &runner, runner_thread);
        return 1;
    }

    static const char rest_body[] = "{\"code\":7,\"message\":\"app rest\"}";
    script.steps[script.next_step] = (intercept_step_t){
        .transport_status = DCC_OK,
        .http_status = 400U,
        .legacy_error = DCC_OK,
        .body = rest_body,
        .body_len = sizeof(rest_body) - 1U,
    };
    script.step_count = script.next_step + 1U;
    previous_errors = state.seen.count;
    previous_client_errors = state.client_seen;
    if (dcc_rest_request(dcc_app_client(app), "GET", "/app/rest", NULL, NULL, NULL) != DCC_OK ||
        state.client_seen != previous_client_errors + 1U ||
        state.seen.count != previous_errors + 1U ||
        state.seen.origin != DCC_ERROR_REST) {
        fprintf(stderr, "public client observer and private App sink did not coexist\n");
        (void)stop_app_runner_and_destroy(app, &runner, runner_thread);
        return 1;
    }

    dcc_listener_t event_listener;
    dcc_listener_init(&event_listener, DCC_LISTENER_EVENT);
    event_listener.handler.event = failing_event;
    event_listener.user_data = &state;
    event_listener.target.event.type = DCC_EVENT_LOG;
    dcc_listener_id_t event_id = 0U;
    previous_errors = state.seen.count;
    if (dcc_app_listen(app, &event_listener, &event_id) != DCC_OK ||
        event_id == 0U || dispatch_log(app) != DCC_OK || state.event_calls != 1U ||
        state.seen.count != previous_errors + 1U ||
        state.seen.origin != DCC_ERROR_HANDLER ||
        strcmp(state.seen.operation, dcc_event_type_name(DCC_EVENT_LOG)) != 0 ||
        dcc_app_unlisten(app, event_id) != DCC_OK) {
        fprintf(stderr, "real event failure observation failed\n");
        (void)stop_app_runner_and_destroy(app, &runner, runner_thread);
        return 1;
    }

    dcc_listener_t task_listener;
    dcc_listener_init(&task_listener, DCC_LISTENER_TASK);
    task_listener.handler.task = failing_task;
    task_listener.user_data = &state;
    task_listener.target.schedule.kind = DCC_LISTENER_SCHEDULE_INTERVAL;
    task_listener.target.schedule.interval_ms = 1U;
    dcc_listener_id_t task_id = 0U;
    previous_errors = state.seen.count;
    if (dcc_app_listen(app, &task_listener, &task_id) != DCC_OK ||
        task_id == 0U || app->listener_count == 0U || app->schedule_count == 0U) {
        fprintf(stderr, "real task listener registration failed\n");
        (void)stop_app_runner_and_destroy(app, &runner, runner_thread);
        return 1;
    }
    dcc_app_listener_entry_t *task_entry = app->listeners[app->listener_count - 1U];
    dcc_app_schedule_t *task_schedule = app->schedules[app->schedule_count - 1U];
    if (task_schedule->canonical_fn != dcc_app_listener_run_task ||
        task_schedule->listener_state != task_entry ||
        dcc_app_listener_run_task(task_entry, app) != DCC_ERR_STATE ||
        state.task_calls != 1U || state.seen.count != previous_errors + 1U ||
        state.seen.origin != DCC_ERROR_HANDLER ||
        strcmp(state.seen.operation, "scheduled task") != 0 ||
        dcc_app_unlisten(app, task_id) != DCC_OK) {
        fprintf(stderr, "real task failure observation failed\n");
        (void)stop_app_runner_and_destroy(app, &runner, runner_thread);
        return 1;
    }
    if (dcc_app_on_error(app, NULL, NULL) != DCC_OK ||
        dcc_client_on_error(dcc_app_client(app), NULL, NULL) != DCC_OK) {
        fprintf(stderr, "observer clear contract failed\n");
        (void)stop_app_runner_and_destroy(app, &runner, runner_thread);
        return 1;
    }
    return stop_app_runner_and_destroy(app, &runner, runner_thread);
}
#endif

int main(void) {
#if !defined(_WIN32)
    (void)signal(SIGPIPE, SIG_IGN);
#endif
    if (check_result_values() != 0 ||
        check_rest_terminal_matrix() != 0 ||
        check_terminal_destroy_lifetime() != 0 ||
        check_terminal_nested_rest() != 0 ||
        app_v2_error_lifetime_smoke() != 0 ||
        check_interceptor_transport_mapping() != 0 ||
        check_async_terminal_matrix() != 0 ||
        check_observer_reentrancy() != 0 ||
        check_app_sink_destroy_wait() != 0 ||
        check_app_error_policy() != 0) {
        return 1;
    }
    return 0;
}
