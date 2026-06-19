#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_bot_internal.h"
#include "internal/gateway/dcc_gateway_lifecycle_internal.h"
#include "internal/http/dcc_http.h"
#include "internal/json/dcc_json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_gateway_fetch_bot_info(dcc_client_t *client) {
    if (client == NULL || client->token == NULL || client->token[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    if (client->gateway_cached_url != NULL && client->gateway_cached_url[0] != '\0') {
        return DCC_OK;
    }

    const char *api_base = getenv("DCC_DISCORD_API_BASE");
    if (api_base == NULL || api_base[0] == '\0') {
        api_base = "https://discord.com/api/v10";
    }

    char *url = NULL;
    dcc_status_t status = dcc_gateway_build_url(api_base, "/gateway/bot", &url);
    if (status != DCC_OK) {
        return status;
    }

    size_t auth_len = strlen("Bot ") + strlen(client->token);
    char *authorization = (char *)malloc(auth_len + 1U);
    if (authorization == NULL) {
        free(url);
        return DCC_ERR_NOMEM;
    }
    snprintf(authorization, auth_len + 1U, "Bot %s", client->token);

    dcc_http_header_t headers[] = {
        {.name = "Authorization", .value = authorization},
        {.name = "Accept", .value = "application/json"},
    };
    dcc_http_request_t request = {
        .method = "GET",
        .url = url,
        .headers = headers,
        .header_count = sizeof(headers) / sizeof(headers[0]),
        .timeout_ms = 30000,
    };
    dcc_http_response_t response;
    status = dcc_http_perform(&request, &response);
    free(authorization);
    free(url);
    if (status != DCC_OK) {
        dcc_set_error(client, "gateway bot REST request failed");
        return status;
    }

    if (response.status < 200 || response.status >= 300) {
        dcc_set_error(client, response.status == 401 || response.status == 403
            ? "gateway bot REST authentication failed"
            : "gateway bot REST returned non-success");
        dcc_http_response_deinit(&response);
        return DCC_ERR_DISCORD;
    }

    dcc_json_t *root = NULL;
    status = dcc_json_parse(response.body, response.body_len, &root);
    if (status != DCC_OK) {
        dcc_set_error(client, "gateway bot REST returned invalid JSON");
        dcc_http_response_deinit(&response);
        return status;
    }

    status = dcc_gateway_apply_bot_info(client, root);

    dcc_json_free(root);
    dcc_http_response_deinit(&response);
    if (status == DCC_OK) {
        dcc_emit_log(client, DCC_LOG_INFO, "gateway bot information fetched");
    }
    return status;
}
