#include "internal/app/dcc_app_internal.h"

#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum dcc_app_event_listener_kind {
    DCC_APP_EVENT_LISTENER_GENERIC = 0,
    DCC_APP_EVENT_LISTENER_READY,
    DCC_APP_EVENT_LISTENER_MESSAGE
} dcc_app_event_listener_kind_t;

typedef struct dcc_app_event_listener_state {
    dcc_app_t *app;
    dcc_app_event_listener_kind_t kind;
    dcc_app_event_fn event_handler;
    dcc_app_ready_fn ready_handler;
    dcc_app_message_fn message_handler;
    void *user_data;
    uint8_t once;
    atomic_bool fired;
} dcc_app_event_listener_state_t;

static dcc_status_t dcc_app_event_listeners_reserve(dcc_app_t *app, size_t need) {
    if (need <= app->event_listener_cap) {
        return DCC_OK;
    }
    size_t next_cap = app->event_listener_cap != 0U ? app->event_listener_cap : 4U;
    while (next_cap < need) {
        if (next_cap > SIZE_MAX / 2U) {
            return DCC_ERR_NOMEM;
        }
        next_cap *= 2U;
    }
    if (next_cap > SIZE_MAX / sizeof(*app->event_listeners)) {
        return DCC_ERR_NOMEM;
    }
    dcc_app_event_listener_entry_t *next =
        (dcc_app_event_listener_entry_t *)realloc(
            app->event_listeners,
            next_cap * sizeof(*app->event_listeners)
        );
    if (next == NULL) {
        return DCC_ERR_NOMEM;
    }
    app->event_listeners = next;
    app->event_listener_cap = next_cap;
    return DCC_OK;
}

static void dcc_app_event_listener_dispatch(
    dcc_client_t *client,
    const dcc_event_t *event,
    void *user_data
) {
    (void)client;
    dcc_app_event_listener_state_t *state = (dcc_app_event_listener_state_t *)user_data;
    if (state == NULL || state->app == NULL || event == NULL) {
        return;
    }

    switch (state->kind) {
        case DCC_APP_EVENT_LISTENER_GENERIC:
            if (state->event_handler != NULL) {
                state->event_handler(state->app, event, state->user_data);
            }
            return;

        case DCC_APP_EVENT_LISTENER_READY:
            if (state->ready_handler != NULL) {
                const dcc_ready_event_t *ready = dcc_event_ready(event);
                if (ready != NULL) {
                    if (state->once && atomic_exchange(&state->fired, true)) {
                        return;
                    }
                    state->ready_handler(state->app, ready, state->user_data);
                }
            }
            return;

        case DCC_APP_EVENT_LISTENER_MESSAGE:
            if (state->message_handler != NULL) {
                const dcc_message_t *message = dcc_event_message(event);
                if (message != NULL) {
                    state->message_handler(state->app, message, event, state->user_data);
                }
            }
            return;
    }
}

dcc_status_t dcc_app_on_event_listener_internal(
    dcc_app_t *app,
    dcc_event_type_t type,
    dcc_event_cb cb,
    void *state
) {
    if (app == NULL || cb == NULL || state == NULL) {
        free(state);
        return DCC_ERR_INVALID_ARG;
    }
    if (app->event_listener_count == SIZE_MAX) {
        free(state);
        return DCC_ERR_NOMEM;
    }
    dcc_status_t status =
        dcc_app_event_listeners_reserve(app, app->event_listener_count + 1U);
    if (status != DCC_OK) {
        free(state);
        return status;
    }

    dcc_listener_id_t id = 0U;
    status = dcc_client_on(app->client, type, cb, state, &id);
    if (status != DCC_OK) {
        free(state);
        return status;
    }

    app->event_listeners[app->event_listener_count].type = type;
    app->event_listeners[app->event_listener_count].id = id;
    app->event_listeners[app->event_listener_count].state = state;
    app->event_listener_count++;
    return DCC_OK;
}

static dcc_status_t dcc_app_on_internal(
    dcc_app_t *app,
    dcc_event_type_t type,
    dcc_app_event_listener_state_t *state
) {
    return dcc_app_on_event_listener_internal(app, type, dcc_app_event_listener_dispatch, state);
}

dcc_status_t dcc_app_on(
    dcc_app_t *app,
    dcc_event_type_t type,
    dcc_app_event_fn handler,
    void *user_data
) {
    if (app == NULL || handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_event_listener_state_t *state =
        (dcc_app_event_listener_state_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->app = app;
    state->kind = DCC_APP_EVENT_LISTENER_GENERIC;
    state->event_handler = handler;
    state->user_data = user_data;
    return dcc_app_on_internal(app, type, state);
}

static dcc_status_t dcc_app_on_ready_internal(
    dcc_app_t *app,
    dcc_app_ready_fn handler,
    void *user_data,
    uint8_t once
) {
    if (app == NULL || handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_event_listener_state_t *state =
        (dcc_app_event_listener_state_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->app = app;
    state->kind = DCC_APP_EVENT_LISTENER_READY;
    state->ready_handler = handler;
    state->user_data = user_data;
    state->once = once != 0U;
    atomic_init(&state->fired, false);
    return dcc_app_on_internal(app, DCC_EVENT_READY, state);
}

dcc_status_t dcc_app_on_ready(
    dcc_app_t *app,
    dcc_app_ready_fn handler,
    void *user_data
) {
    return dcc_app_on_ready_internal(app, handler, user_data, 0U);
}

dcc_status_t dcc_app_on_ready_once(
    dcc_app_t *app,
    dcc_app_ready_fn handler,
    void *user_data
) {
    return dcc_app_on_ready_internal(app, handler, user_data, 1U);
}

static dcc_status_t dcc_app_on_message_event(
    dcc_app_t *app,
    dcc_event_type_t type,
    dcc_app_message_fn handler,
    void *user_data
) {
    if (app == NULL || handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_event_listener_state_t *state =
        (dcc_app_event_listener_state_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->app = app;
    state->kind = DCC_APP_EVENT_LISTENER_MESSAGE;
    state->message_handler = handler;
    state->user_data = user_data;
    return dcc_app_on_internal(app, type, state);
}

dcc_status_t dcc_app_on_message_create(
    dcc_app_t *app,
    dcc_app_message_fn handler,
    void *user_data
) {
    return dcc_app_on_message_event(app, DCC_EVENT_MESSAGE_CREATE, handler, user_data);
}

dcc_status_t dcc_app_on_message_update(
    dcc_app_t *app,
    dcc_app_message_fn handler,
    void *user_data
) {
    return dcc_app_on_message_event(app, DCC_EVENT_MESSAGE_UPDATE, handler, user_data);
}

dcc_status_t dcc_app_on_message_delete(
    dcc_app_t *app,
    dcc_app_message_fn handler,
    void *user_data
) {
    return dcc_app_on_message_event(app, DCC_EVENT_MESSAGE_DELETE, handler, user_data);
}
