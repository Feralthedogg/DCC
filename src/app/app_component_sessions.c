#include "internal/app/dcc_app_internal.h"

#include <stdint.h>
#include <stdlib.h>

typedef struct dcc_app_component_session_state {
    dcc_app_t *app;
    dcc_app_handler_fn handler;
    void *user_data;
} dcc_app_component_session_state_t;

static dcc_status_t dcc_app_component_session_listeners_reserve(dcc_app_t *app, size_t need) {
    if (need <= app->component_session_listener_cap) {
        return DCC_OK;
    }
    size_t next_cap = app->component_session_listener_cap != 0U
        ? app->component_session_listener_cap
        : 4U;
    while (next_cap < need) {
        if (next_cap > SIZE_MAX / 2U) {
            return DCC_ERR_NOMEM;
        }
        next_cap *= 2U;
    }
    if (next_cap > SIZE_MAX / sizeof(*app->component_session_listeners)) {
        return DCC_ERR_NOMEM;
    }
    dcc_app_component_session_listener_entry_t *next =
        (dcc_app_component_session_listener_entry_t *)realloc(
            app->component_session_listeners,
            next_cap * sizeof(*app->component_session_listeners)
        );
    if (next == NULL) {
        return DCC_ERR_NOMEM;
    }
    app->component_session_listeners = next;
    app->component_session_listener_cap = next_cap;
    return DCC_OK;
}

static void dcc_app_component_session_trampoline(
    dcc_client_t *client,
    const dcc_event_t *event,
    const dcc_component_session_result_t *result,
    void *user_data
) {
    dcc_app_component_session_state_t *state = (dcc_app_component_session_state_t *)user_data;
    const dcc_interaction_t *interaction = dcc_event_interaction(event);
    if (state == NULL || state->app == NULL || state->handler == NULL || interaction == NULL) {
        return;
    }
    (void)dcc_app_dispatch_handler(
        state->app,
        client,
        event,
        interaction,
        result,
        NULL,
        0U,
        state->handler,
        state->user_data
    );
}

dcc_status_t dcc_app_component_session_store(
    dcc_app_t *app,
    const dcc_component_session_store_t *store,
    const dcc_component_session_listener_options_t *options,
    dcc_app_handler_fn handler,
    void *user_data
) {
    if (app == NULL || store == NULL || handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (app->component_session_listener_count == SIZE_MAX) {
        return DCC_ERR_NOMEM;
    }
    dcc_status_t status = dcc_app_component_session_listeners_reserve(
        app,
        app->component_session_listener_count + 1U
    );
    if (status != DCC_OK) {
        return status;
    }

    dcc_app_component_session_state_t *state =
        (dcc_app_component_session_state_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->app = app;
    state->handler = handler;
    state->user_data = user_data;

    dcc_app_component_session_listener_entry_t entry;
    entry.state = state;
    status = dcc_client_on_component_session_store(
        app->client,
        store,
        options,
        dcc_app_component_session_trampoline,
        state,
        &entry.listener
    );
    if (status != DCC_OK) {
        free(state);
        return status;
    }

    app->component_session_listeners[app->component_session_listener_count++] = entry;
    return DCC_OK;
}

dcc_status_t dcc_app_component_session(
    dcc_app_t *app,
    const dcc_component_session_t *session,
    const dcc_component_session_listener_options_t *options,
    dcc_app_handler_fn handler,
    void *user_data
) {
    if (app == NULL || session == NULL || handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_session_store_t store;
    dcc_component_session_store_init(&store);
    dcc_status_t status = dcc_component_session_store_add(&store, session);
    if (status == DCC_OK) {
        status = dcc_app_component_session_store(app, &store, options, handler, user_data);
    }
    dcc_component_session_store_deinit(&store);
    return status;
}
