#include "internal/events/dcc_interaction_listener_internal.h"

void dcc_client_interaction_listeners_deinit(dcc_client_t *client) {
    if (client == NULL) {
        return;
    }

    dcc_interaction_listeners_lock(client);
    dcc_interaction_listener_ctx_t *ctx = client->interaction_listeners;
    client->interaction_listeners = NULL;
    dcc_interaction_listeners_unlock(client);

    while (ctx != NULL) {
        dcc_interaction_listener_ctx_t *next = ctx->next;
        atomic_store_explicit(&ctx->active, false, memory_order_release);
        dcc_interaction_listener_free(ctx);
        ctx = next;
    }
}

dcc_status_t dcc_client_off_interaction(
    dcc_client_t *client,
    dcc_event_type_t type,
    dcc_listener_id_t id
) {
    if (client == NULL || (type < 0 || type >= DCC_EVENT_MAX) || id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    uint8_t found = 0U;
    dcc_interaction_listeners_lock(client);
    for (dcc_interaction_listener_ctx_t *ctx = client->interaction_listeners; ctx != NULL; ctx = ctx->next) {
        if (ctx->listener_id == id && ctx->type == type &&
            atomic_load_explicit(&ctx->active, memory_order_acquire)) {
            atomic_store_explicit(&ctx->active, false, memory_order_release);
            found = 1U;
            break;
        }
    }
    dcc_interaction_listeners_unlock(client);

    if (!found) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_event_bus_remove(&client->events, type, id);
}
