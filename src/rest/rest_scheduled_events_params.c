#include "internal/rest/dcc_rest_builders_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_create_guild_scheduled_event_params(
    dcc_client_t *client,
    const dcc_scheduled_event_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_scheduled_event_body(params, 0, &body);
    if (status == DCC_OK) {
        status = dcc_rest_create_guild_scheduled_event(client, params->guild_id, body, cb, user_data);
    }
    free(body);
    return status;
}

dcc_status_t dcc_rest_modify_guild_scheduled_event_params(
    dcc_client_t *client,
    const dcc_scheduled_event_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_scheduled_event_body(params, 1, &body);
    if (status == DCC_OK) {
        status = dcc_rest_modify_guild_scheduled_event(client, params->guild_id, params->event_id, body, cb, user_data);
    }
    free(body);
    return status;
}
