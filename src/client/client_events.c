#include "internal/client/dcc_client_state_internal.h"

#include <string.h>

void dcc_emit_log(dcc_client_t *client, dcc_log_level_t level, const char *message) {
    if (client == NULL || message == NULL) {
        return;
    }

    if (client->log_fn != NULL) {
        client->log_fn(level, message, client->log_user_data);
    }

    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_LOG;
    event.raw.type = DCC_EVENT_LOG;
    event.raw.name = dcc_event_type_name(DCC_EVENT_LOG);
    event.data.log.level = level;
    event.data.log.message = message;
    (void)dcc_event_bus_dispatch(&client->events, client, &event);
}

void dcc_emit_rate_limited(dcc_client_t *client, const dcc_rate_limited_event_t *limited) {
    if (client == NULL || limited == NULL) {
        return;
    }

    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_RATE_LIMITED;
    event.data.rate_limited = *limited;
    (void)dcc_event_bus_dispatch(&client->events, client, &event);
}
