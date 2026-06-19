#include "internal/events/dcc_interaction_listener_internal.h"

#include <stdlib.h>

dcc_status_t dcc_client_on_interaction_filtered(
    dcc_client_t *client,
    dcc_event_type_t type,
    const char *key,
    uint8_t match_custom_id,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
) {
    if (client == NULL ||
        key == NULL ||
        key[0] == '\0' ||
        cb == NULL ||
        (match_custom_id
            ? !dcc_interaction_custom_id_event_type(type)
            : !dcc_interaction_name_event_type(type))) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_interaction_listener_ctx_t *ctx = (dcc_interaction_listener_ctx_t *)calloc(1, sizeof(*ctx));
    if (ctx == NULL) {
        return DCC_ERR_NOMEM;
    }
    ctx->key = dcc_strdup(key);
    if (ctx->key == NULL) {
        dcc_interaction_listener_free(ctx);
        return DCC_ERR_NOMEM;
    }
    ctx->type = type;
    ctx->cb = cb;
    ctx->user_data = user_data;
    ctx->match_custom_id = match_custom_id ? 1U : 0U;
    atomic_init(&ctx->active, true);

    dcc_listener_id_t id = 0;
    dcc_status_t status = dcc_event_bus_add(
        &client->events,
        type,
        dcc_interaction_listener_trampoline,
        ctx,
        &id
    );
    if (status != DCC_OK) {
        atomic_store_explicit(&ctx->active, false, memory_order_release);
        dcc_interaction_listener_free(ctx);
        return status;
    }
    ctx->listener_id = id;

    dcc_interaction_listeners_lock(client);
    ctx->next = client->interaction_listeners;
    client->interaction_listeners = ctx;
    dcc_interaction_listeners_unlock(client);

    if (out_id != NULL) {
        *out_id = id;
    }
    return DCC_OK;
}
