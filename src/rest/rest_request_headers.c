#include "internal/rest/dcc_rest_request_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_rest_request_headers_init(
    dcc_rest_request_headers_t *headers,
    const dcc_client_t *client,
    int absolute_url,
    size_t body_len,
    const char *content_type
) {
    if (headers == NULL || client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    headers->header_count = 0;
    headers->authorization = NULL;

    if (client->token != NULL && client->token[0] != '\0' && !absolute_url) {
        size_t auth_len = strlen("Bot ") + strlen(client->token);
        headers->authorization = (char *)malloc(auth_len + 1U);
        if (headers->authorization == NULL) {
            return DCC_ERR_NOMEM;
        }
        snprintf(headers->authorization, auth_len + 1U, "Bot %s", client->token);
        headers->headers[headers->header_count++] = (dcc_http_header_t){
            .name = "Authorization",
            .value = headers->authorization,
        };
    }

    if (body_len != 0 && content_type != NULL && content_type[0] != '\0') {
        headers->headers[headers->header_count++] = (dcc_http_header_t){
            .name = "Content-Type",
            .value = content_type,
        };
    }

    return DCC_OK;
}

void dcc_rest_request_headers_deinit(dcc_rest_request_headers_t *headers) {
    if (headers == NULL) {
        return;
    }
    free(headers->authorization);
    headers->authorization = NULL;
    headers->header_count = 0;
}
