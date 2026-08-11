#include "internal/rest/dcc_rest_request_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_sensitive_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_rest_request_headers_init(
    dcc_rest_request_headers_t *headers,
    const dcc_client_t *client,
    int absolute_url,
    size_t body_len,
    const char *content_type,
    dcc_rest_auth_mode_t auth_mode,
    const char *auth_token,
    const char *audit_log_reason
) {
    if (headers == NULL || client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    headers->header_count = 0;
    headers->authorization = NULL;
    headers->audit_log_reason = NULL;

    const char *prefix = NULL;
    const char *token = NULL;
    if (!absolute_url && auth_mode == DCC_REST_AUTH_BOT) {
        prefix = "Bot ";
        token = client->token;
    } else if (!absolute_url && auth_mode == DCC_REST_AUTH_BEARER) {
        prefix = "Bearer ";
        token = auth_token;
    }
    if (prefix != NULL && token != NULL && token[0] != '\0') {
        size_t auth_len = strlen(prefix) + strlen(token);
        headers->authorization = (char *)malloc(auth_len + 1U);
        if (headers->authorization == NULL) {
            return DCC_ERR_NOMEM;
        }
        snprintf(headers->authorization, auth_len + 1U, "%s%s", prefix, token);
        headers->headers[headers->header_count++] = (dcc_http_header_t){
            .name = "Authorization",
            .value = headers->authorization,
        };
    }

    if (!absolute_url && audit_log_reason != NULL) {
        dcc_status_t status = dcc_rest_escape_path_segment(
            audit_log_reason, &headers->audit_log_reason
        );
        if (status != DCC_OK) {
            dcc_rest_sensitive_free(
                headers->authorization,
                headers->authorization != NULL
                    ? strlen(headers->authorization) + 1U
                    : 0U
            );
            headers->authorization = NULL;
            return status;
        }
        headers->headers[headers->header_count++] = (dcc_http_header_t){
            .name = "X-Audit-Log-Reason",
            .value = headers->audit_log_reason,
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
    dcc_rest_sensitive_free(
        headers->authorization,
        headers->authorization != NULL
            ? strlen(headers->authorization) + 1U
            : 0U
    );
    dcc_rest_sensitive_free(
        headers->audit_log_reason,
        headers->audit_log_reason != NULL
            ? strlen(headers->audit_log_reason) + 1U
            : 0U
    );
    headers->authorization = NULL;
    headers->audit_log_reason = NULL;
    headers->header_count = 0;
}
