#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void dcc_set_error(dcc_client_t *client, const char *message) {
    if (client == NULL) {
        return;
    }
    if (message == NULL) {
        client->last_error[0] = '\0';
        return;
    }
    snprintf(client->last_error, sizeof(client->last_error), "%s", message);
}

char *dcc_strdup(const char *value) {
    if (value == NULL) {
        return NULL;
    }
    size_t len = strlen(value);
    char *copy = (char *)malloc(len + 1U);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, value, len + 1U);
    return copy;
}

const char *dcc_version_string(void) {
    return "1.2.3";
}

const char *dcc_status_string(dcc_status_t status) {
    switch (status) {
        case DCC_OK:
            return "ok";
        case DCC_ERR_INVALID_ARG:
            return "invalid argument";
        case DCC_ERR_NOMEM:
            return "out of memory";
        case DCC_ERR_RUNTIME:
            return "runtime error";
        case DCC_ERR_NETWORK:
            return "network error";
        case DCC_ERR_TLS:
            return "tls error";
        case DCC_ERR_JSON:
            return "json error";
        case DCC_ERR_DISCORD:
            return "discord error";
        case DCC_ERR_RATE_LIMITED:
            return "rate limited";
        case DCC_ERR_CANCELED:
            return "canceled";
        case DCC_ERR_STATE:
            return "invalid state";
        case DCC_ERR_TIMEOUT:
            return "timeout";
        case DCC_ERR_NOT_FOUND:
            return "not found";
        default:
            return "unknown error";
    }
}

dcc_status_t dcc_client_runtime_stats(dcc_client_t *client, dcc_runtime_stats_t *out) {
    if (client == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }
    out->tasks_spawned = client->runtime.tasks_spawned;
    out->events_dispatched = client->events.events_dispatched;
    return DCC_OK;
}

const char *dcc_client_last_error(const dcc_client_t *client) {
    if (client == NULL) {
        return "";
    }
    return client->last_error;
}
