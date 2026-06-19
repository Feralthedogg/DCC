#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_resume_internal.h"

#include <stdlib.h>

bool dcc_gateway_can_resume(const dcc_client_t *client) {
    return client != NULL &&
           client->gateway_can_resume &&
           client->gateway_session_id != NULL &&
           client->gateway_session_id[0] != '\0' &&
           client->gateway_has_seq;
}

static dcc_status_t dcc_gateway_replace_string(char **slot, const char *value) {
    char *copy = dcc_strdup(value != NULL ? value : "");
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    free(*slot);
    *slot = copy;
    return DCC_OK;
}

void dcc_gateway_clear_resume(dcc_client_t *client) {
    if (client == NULL) {
        return;
    }
    free(client->gateway_session_id);
    free(client->gateway_resume_url);
    client->gateway_session_id = NULL;
    client->gateway_resume_url = NULL;
    client->gateway_seq = 0;
    client->gateway_has_seq = false;
    client->gateway_can_resume = false;
}

dcc_status_t dcc_gateway_store_ready(
    dcc_gateway_session_t *session,
    const char *session_id,
    const char *resume_url
) {
    if (session_id == NULL || session_id[0] == '\0') {
        return DCC_ERR_DISCORD;
    }

    dcc_status_t status = dcc_gateway_replace_string(&session->client->gateway_session_id, session_id);
    if (status != DCC_OK) {
        return status;
    }

    if (resume_url != NULL && resume_url[0] != '\0') {
        status = dcc_gateway_replace_string(&session->client->gateway_resume_url, resume_url);
        if (status != DCC_OK) {
            return status;
        }
    }

    session->client->gateway_can_resume = session->client->gateway_has_seq;
    return DCC_OK;
}
