#include "app_smoke_response_state.h"

#include <dcc/dcc.h>
#include <dcc/sugar.h>

#include "internal/app/dcc_app_internal.h"
#include "internal/interaction_flow/dcc_interaction_flow_internal.h"
#include "internal/rest/dcc_rest_intercept_internal.h"

#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

#if !defined(_WIN32)
#include <pthread.h>
#include <unistd.h>
#endif

typedef struct response_state_intercept_seen {
    unsigned requests;
} response_state_intercept_seen_t;

#if !defined(_WIN32)
typedef struct response_state_reentrant_seen {
    dcc_ctx_t *ctx;
    dcc_app_auto_defer_t *auto_defer;
    atomic_uint nested_done;
    atomic_uint watchdog_timed_out;
    unsigned outer_callbacks;
    dcc_status_t nested_status;
} response_state_reentrant_seen_t;
#endif

static dcc_status_t response_state_intercept(
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
    response_state_intercept_seen_t *seen =
        (response_state_intercept_seen_t *)intercept_user_data;
    if (seen != NULL) {
        seen->requests++;
    }
    if (cb != NULL) {
        dcc_rest_response_t response = {
            .size = sizeof(response),
            .status = 204U,
            .error = DCC_OK,
        };
        cb(client, &response, user_data);
    }
    return DCC_OK;
}

#if !defined(_WIN32)
static void response_state_reentrant_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    response_state_reentrant_seen_t *seen =
        (response_state_reentrant_seen_t *)user_data;
    if (seen == NULL || response == NULL || response->status != 204U) {
        return;
    }
    seen->outer_callbacks++;
    seen->nested_status = dcc_ctx_reply_text(
        seen->ctx,
        "nested reply",
        NULL,
        NULL
    );
    atomic_store_explicit(&seen->nested_done, 1U, memory_order_release);
}

static void *response_state_reentrant_watchdog(void *user_data) {
    response_state_reentrant_seen_t *seen =
        (response_state_reentrant_seen_t *)user_data;
    for (unsigned elapsed_ms = 0U; elapsed_ms < 100U; ++elapsed_ms) {
        if (atomic_load_explicit(&seen->nested_done, memory_order_acquire) != 0U) {
            return NULL;
        }
        usleep(1000U);
    }
    atomic_store_explicit(&seen->watchdog_timed_out, 1U, memory_order_release);
    atomic_store_explicit(
        &seen->auto_defer->response_state,
        DCC_APP_RESPONSE_READY,
        memory_order_release
    );
    return NULL;
}

static int app_smoke_check_same_context_reentry(void) {
    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_client_t *client = NULL;
    if (dcc_client_create(&options, &client) != DCC_OK) {
        return 0;
    }
    response_state_intercept_seen_t intercept_seen = {0};
    dcc_rest_set_interceptor(client, response_state_intercept, &intercept_seen);
    dcc_interaction_t interaction = {
        .id = 11U,
        .application_id = 12U,
        .token = "reentrant-response-token",
    };
    dcc_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.client = client;
    ctx.interaction = &interaction;
    dcc_flow_init(&ctx.flow, client, &interaction);
    dcc_app_auto_defer_t auto_defer;
    memset(&auto_defer, 0, sizeof(auto_defer));
    atomic_init(&auto_defer.initial_response_admitted, false);
    atomic_init(&auto_defer.response_state, DCC_APP_RESPONSE_READY);
    ctx.auto_defer = &auto_defer;

    response_state_reentrant_seen_t seen = {
        .ctx = &ctx,
        .auto_defer = &auto_defer,
        .nested_status = DCC_OK,
    };
    atomic_init(&seen.nested_done, 0U);
    atomic_init(&seen.watchdog_timed_out, 0U);
    pthread_t watchdog;
    if (pthread_create(
            &watchdog,
            NULL,
            response_state_reentrant_watchdog,
            &seen
        ) != 0) {
        ctx.auto_defer = NULL;
        dcc_client_destroy(client);
        return 0;
    }
    dcc_status_t status = dcc_ctx_reply_text(
        &ctx,
        "outer reply",
        response_state_reentrant_cb,
        &seen
    );
    int join_status = pthread_join(watchdog, NULL);
    int passed = status == DCC_OK && join_status == 0 &&
        atomic_load_explicit(&seen.watchdog_timed_out, memory_order_acquire) == 0U &&
        seen.nested_status == DCC_ERR_STATE && seen.outer_callbacks == 1U &&
        intercept_seen.requests == 1U && dcc_ctx_response_sent(&ctx);
    ctx.auto_defer = NULL;
    dcc_client_destroy(client);
    if (!passed) {
        fprintf(stderr, "same-context synchronous reply reentry did not fail fast\n");
    }
    return passed;
}
#else
static int app_smoke_check_same_context_reentry(void) {
    return 1;
}
#endif

static int app_smoke_check_auto_defer_local_retry(void) {
    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_client_t *client = NULL;
    if (dcc_client_create(&options, &client) != DCC_OK) {
        return 0;
    }
    response_state_intercept_seen_t seen = {0};
    dcc_rest_set_interceptor(client, response_state_intercept, &seen);

    dcc_interaction_t interaction = {
        .id = 1U,
        .application_id = 2U,
        .token = "response-state-token",
    };
    dcc_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.client = client;
    ctx.interaction = &interaction;
    dcc_flow_init(&ctx.flow, client, &interaction);

    dcc_app_auto_defer_t auto_defer;
    memset(&auto_defer, 0, sizeof(auto_defer));
    atomic_init(&auto_defer.initial_response_admitted, false);
    atomic_init(&auto_defer.response_state, DCC_APP_RESPONSE_READY);
    ctx.auto_defer = &auto_defer;

    dcc_message_builder_t invalid;
    dcc_message_builder_init(&invalid);
    invalid.present |= DCC_MESSAGE_BUILDER_PRESENT_CONTENT;
    dcc_status_t status = dcc_ctx_reply(&ctx, &invalid, NULL, NULL);
    int local_failure_ok = status == DCC_ERR_INVALID_ARG &&
        dcc_ctx_response_state(&ctx) == DCC_INTERACTION_FLOW_FAILED &&
        !dcc_ctx_response_sent(&ctx) && seen.requests == 0U;

    dcc_message_builder_t valid;
    dcc_message_builder_init(&valid);
    status = dcc_message_builder_set_content(&valid, "recovered");
    if (status == DCC_OK) {
        status = dcc_ctx_reply(&ctx, &valid, NULL, NULL);
    }
    int retry_ok = status == DCC_OK && seen.requests == 1U &&
        dcc_ctx_response_state(&ctx) == DCC_INTERACTION_FLOW_REPLIED &&
        dcc_ctx_response_sent(&ctx) && dcc_ctx_replied(&ctx);

    dcc_flow_init(&ctx.flow, client, &interaction);
    atomic_store_explicit(
        &auto_defer.initial_response_admitted,
        false,
        memory_order_release
    );
    atomic_store_explicit(
        &auto_defer.response_state,
        DCC_APP_RESPONSE_READY,
        memory_order_release
    );
    dcc_autocomplete_choice_t choice;
    dcc_autocomplete_choice_init(&choice, "Recovered");
    status = dcc_autocomplete_choice_set_string_value(&choice, "recovered");
    dcc_autocomplete_builder_t autocomplete;
    dcc_autocomplete_builder_init(&autocomplete);
    if (status == DCC_OK) {
        status = dcc_autocomplete_builder_set_choices(
            &autocomplete,
            &choice,
            1U
        );
    }
    if (status == DCC_OK) {
        status = dcc_ctx_reply_autocomplete(&ctx, &autocomplete, NULL, NULL);
    }
    int autocomplete_ok = status == DCC_OK && seen.requests == 2U &&
        atomic_load_explicit(
            &auto_defer.initial_response_admitted,
            memory_order_acquire
        ) && atomic_load_explicit(
            &auto_defer.response_state,
            memory_order_acquire
        ) == DCC_APP_RESPONSE_REPLIED &&
        dcc_ctx_response_sent(&ctx) && dcc_ctx_replied(&ctx);

    ctx.auto_defer = NULL;
    dcc_client_destroy(client);
    if (!local_failure_ok || !retry_ok || !autocomplete_ok) {
        fprintf(stderr, "ctx auto-defer local admission retry mismatch\n");
        return 0;
    }
    return 1;
}

int app_smoke_check_response_state(void) {
    dcc_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    dcc_flow_init(&ctx.flow, NULL, NULL);

    if (dcc_ctx_response_state(NULL) != DCC_INTERACTION_FLOW_FAILED ||
        strcmp(dcc_ctx_response_state_string(NULL), "failed") != 0 ||
        dcc_ctx_response_sent(NULL) ||
        dcc_ctx_deferred(NULL) ||
        dcc_ctx_replied(NULL) ||
        dcc_ctx_followed_up(NULL) ||
        !dcc_ctx_response_failed(NULL) ||
        dcc_ctx_can_followup(NULL) ||
        dcc_ctx_can_edit_original(NULL) ||
        DCC_CTX_RESPONSE_STATE(&ctx) != DCC_INTERACTION_FLOW_READY ||
        DCC_CTX_RESPONDED(&ctx) ||
        DCC_CTX_DEFERRED(&ctx) ||
        DCC_CTX_REPLIED(&ctx) ||
        DCC_CTX_FOLLOWED_UP(&ctx) ||
        DCC_CTX_RESPONSE_FAILED(&ctx) ||
        DCC_CTX_CAN_FOLLOWUP(&ctx) ||
        DCC_CTX_CAN_EDIT_ORIGINAL(&ctx) ||
        strcmp(DCC_CTX_RESPONSE_STATE_NAME(&ctx), "ready") != 0) {
        fprintf(stderr, "ctx response ready/null state mismatch\n");
        return 0;
    }

    if (dcc_flow_mark_initial(
            &ctx.flow,
            DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL,
            DCC_OK
        ) != DCC_OK ||
        dcc_ctx_response_state(&ctx) != DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL ||
        !dcc_ctx_response_sent(&ctx) ||
        !dcc_ctx_deferred(&ctx) ||
        dcc_ctx_replied(&ctx) ||
        !dcc_ctx_can_followup(&ctx) ||
        !dcc_ctx_can_edit_original(&ctx) ||
        strcmp(dcc_ctx_response_state_string(&ctx), "deferred_ephemeral") != 0) {
        fprintf(stderr, "ctx response deferred state mismatch\n");
        return 0;
    }

    if (dcc_flow_mark_initial(
            &ctx.flow,
            DCC_INTERACTION_FLOW_REPLIED,
            DCC_OK
        ) != DCC_OK ||
        dcc_ctx_response_state(&ctx) != DCC_INTERACTION_FLOW_REPLIED ||
        !dcc_ctx_response_sent(&ctx) ||
        dcc_ctx_deferred(&ctx) ||
        !dcc_ctx_replied(&ctx) ||
        !dcc_ctx_can_followup(&ctx) ||
        !dcc_ctx_can_edit_original(&ctx)) {
        fprintf(stderr, "ctx response replied state mismatch\n");
        return 0;
    }

    dcc_flow_mark(
        &ctx.flow,
        DCC_INTERACTION_FLOW_FOLLOWED_UP,
        DCC_OK
    );
    if (dcc_ctx_response_state(&ctx) != DCC_INTERACTION_FLOW_FOLLOWED_UP ||
        !dcc_ctx_replied(&ctx) ||
        !dcc_ctx_followed_up(&ctx) ||
        !dcc_ctx_can_followup(&ctx) ||
        dcc_ctx_can_edit_original(&ctx)) {
        fprintf(stderr, "ctx response followup state mismatch\n");
        return 0;
    }

    dcc_flow_init(&ctx.flow, NULL, NULL);
    if (dcc_flow_mark_initial(
            &ctx.flow,
            DCC_INTERACTION_FLOW_REPLIED,
            DCC_ERR_INVALID_ARG
        ) != DCC_ERR_INVALID_ARG ||
        dcc_ctx_response_state(&ctx) != DCC_INTERACTION_FLOW_FAILED ||
        !dcc_ctx_response_failed(&ctx) ||
        dcc_ctx_response_sent(&ctx) ||
        dcc_ctx_can_followup(&ctx)) {
        fprintf(stderr, "ctx response local admission failure mismatch\n");
        return 0;
    }

    dcc_app_auto_defer_t auto_defer;
    memset(&auto_defer, 0, sizeof(auto_defer));
    atomic_init(&auto_defer.initial_response_admitted, true);
    atomic_init(&auto_defer.response_state, DCC_APP_RESPONSE_DEFERRED);
    auto_defer.ephemeral = 1U;
    dcc_flow_init(&ctx.flow, NULL, NULL);
    ctx.auto_defer = &auto_defer;
    if (dcc_ctx_response_state(&ctx) != DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL ||
        !dcc_ctx_response_sent(&ctx) ||
        !dcc_ctx_deferred(&ctx) ||
        !dcc_ctx_can_edit_original(&ctx)) {
        fprintf(stderr, "ctx response auto defer state mismatch\n");
        return 0;
    }

    atomic_store(&auto_defer.response_state, DCC_APP_RESPONSE_REPLIED);
    if (dcc_ctx_response_state(&ctx) != DCC_INTERACTION_FLOW_REPLIED ||
        !dcc_ctx_replied(&ctx) ||
        !dcc_ctx_can_followup(&ctx)) {
        fprintf(stderr, "ctx response auto replied state mismatch\n");
        return 0;
    }

    atomic_store(&auto_defer.response_state, DCC_APP_RESPONSE_FAILED);
    if (dcc_ctx_response_state(&ctx) != DCC_INTERACTION_FLOW_FAILED ||
        !dcc_ctx_response_failed(&ctx) ||
        !dcc_ctx_response_sent(&ctx)) {
        fprintf(stderr, "ctx response post-admission failure mismatch\n");
        return 0;
    }

    atomic_store(&auto_defer.initial_response_admitted, false);
    dcc_flow_init(&ctx.flow, NULL, NULL);
    if (dcc_ctx_response_state(&ctx) != DCC_INTERACTION_FLOW_FAILED ||
        !dcc_ctx_response_failed(&ctx) ||
        dcc_ctx_response_sent(&ctx)) {
        fprintf(stderr, "ctx response auto admission failure mismatch\n");
        return 0;
    }

    return app_smoke_check_auto_defer_local_retry() &&
        app_smoke_check_same_context_reentry();
}
