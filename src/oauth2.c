#include "internal/http/dcc_http.h"
#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_url_internal.h"

#include <dcc/oauth2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static dcc_status_t dcc_oauth2_append_form_value(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const char *value
) {
    if (key == NULL || key[0] == '\0' || value == NULL || value[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    char *encoded_key = NULL;
    char *encoded_value = NULL;
    dcc_status_t status = dcc_rest_escape_path_segment(key, &encoded_key);
    if (status == DCC_OK) {
        status = dcc_rest_escape_path_segment(value, &encoded_value);
    }
    if (status == DCC_OK && !*first) {
        status = dcc_rest_buffer_append_cstr(body, "&");
    }
    if (status == DCC_OK) {
        *first = 0;
        status = dcc_rest_buffer_append_cstr(body, encoded_key);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "=");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, encoded_value);
    }
    free(encoded_key);
    free(encoded_value);
    return status;
}

static dcc_status_t dcc_oauth2_perform(
    const char *method,
    const char *path,
    const char *authorization,
    const void *body,
    size_t body_len,
    const char *content_type,
    dcc_oauth2_cb cb,
    void *user_data
) {
    char *url = NULL;
    int absolute = 0;
    dcc_status_t status = dcc_rest_build_url(path, &url, &absolute);
    (void)absolute;
    if (status != DCC_OK) {
        return status;
    }

    dcc_http_header_t headers[2];
    size_t header_count = 0;
    if (authorization != NULL && authorization[0] != '\0') {
        headers[header_count++] = (dcc_http_header_t){
            .name = "Authorization",
            .value = authorization,
        };
    }
    if (body_len != 0 && content_type != NULL && content_type[0] != '\0') {
        headers[header_count++] = (dcc_http_header_t){
            .name = "Content-Type",
            .value = content_type,
        };
    }

    dcc_http_request_t request = {
        .method = method,
        .url = url,
        .headers = headers,
        .header_count = header_count,
        .body = body,
        .body_len = body_len,
        .timeout_ms = 30000,
    };
    dcc_http_response_t http_response;
    status = dcc_http_perform(&request, &http_response);
    if (cb != NULL) {
        dcc_rest_response_t response = {
            .size = sizeof(response),
            .status = status == DCC_OK ? http_response.status : 0,
            .error = status,
            .body = status == DCC_OK ? http_response.body : NULL,
            .body_len = status == DCC_OK ? http_response.body_len : 0,
        };
        cb(&response, user_data);
    }
    if (status == DCC_OK) {
        dcc_http_response_deinit(&http_response);
    }
    free(url);
    return status;
}

dcc_status_t dcc_oauth2_get_current_authorization(
    const char *access_token,
    dcc_oauth2_cb cb,
    void *user_data
) {
    if (access_token == NULL || access_token[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    size_t auth_len = strlen("Bearer ") + strlen(access_token);
    char *authorization = (char *)malloc(auth_len + 1U);
    if (authorization == NULL) {
        return DCC_ERR_NOMEM;
    }
    snprintf(authorization, auth_len + 1U, "Bearer %s", access_token);
    dcc_status_t status = dcc_oauth2_perform(
        "GET",
        "/oauth2/@me",
        authorization,
        NULL,
        0,
        NULL,
        cb,
        user_data
    );
    free(authorization);
    return status;
}

dcc_status_t dcc_oauth2_get_current_bot_application(
    const char *bot_token,
    dcc_oauth2_cb cb,
    void *user_data
) {
    if (bot_token == NULL || bot_token[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    size_t auth_len = strlen("Bot ") + strlen(bot_token);
    char *authorization = (char *)malloc(auth_len + 1U);
    if (authorization == NULL) {
        return DCC_ERR_NOMEM;
    }
    snprintf(authorization, auth_len + 1U, "Bot %s", bot_token);
    dcc_status_t status = dcc_oauth2_perform(
        "GET",
        "/oauth2/applications/@me",
        authorization,
        NULL,
        0,
        NULL,
        cb,
        user_data
    );
    free(authorization);
    return status;
}

dcc_status_t dcc_oauth2_exchange_code(
    const dcc_oauth2_exchange_code_options_t *options,
    dcc_oauth2_cb cb,
    void *user_data
) {
    if (options == NULL || options->size < sizeof(*options)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    int first = 1;
    dcc_status_t status = dcc_oauth2_append_form_value(&body, &first, "grant_type", "authorization_code");
    if (status == DCC_OK) {
        status = dcc_oauth2_append_form_value(&body, &first, "client_id", options->client_id);
    }
    if (status == DCC_OK) {
        status = dcc_oauth2_append_form_value(&body, &first, "client_secret", options->client_secret);
    }
    if (status == DCC_OK) {
        status = dcc_oauth2_append_form_value(&body, &first, "code", options->code);
    }
    if (status == DCC_OK) {
        status = dcc_oauth2_append_form_value(&body, &first, "redirect_uri", options->redirect_uri);
    }
    if (status == DCC_OK) {
        status = dcc_oauth2_perform(
            "POST",
            "/oauth2/token",
            NULL,
            body.data,
            body.len,
            "application/x-www-form-urlencoded",
            cb,
            user_data
        );
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_oauth2_refresh_token(
    const dcc_oauth2_refresh_token_options_t *options,
    dcc_oauth2_cb cb,
    void *user_data
) {
    if (options == NULL || options->size < sizeof(*options)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    int first = 1;
    dcc_status_t status = dcc_oauth2_append_form_value(&body, &first, "grant_type", "refresh_token");
    if (status == DCC_OK) {
        status = dcc_oauth2_append_form_value(&body, &first, "client_id", options->client_id);
    }
    if (status == DCC_OK) {
        status = dcc_oauth2_append_form_value(&body, &first, "client_secret", options->client_secret);
    }
    if (status == DCC_OK) {
        status = dcc_oauth2_append_form_value(&body, &first, "refresh_token", options->refresh_token);
    }
    if (status == DCC_OK) {
        status = dcc_oauth2_perform(
            "POST",
            "/oauth2/token",
            NULL,
            body.data,
            body.len,
            "application/x-www-form-urlencoded",
            cb,
            user_data
        );
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_oauth2_revoke_token(
    const dcc_oauth2_revoke_token_options_t *options,
    dcc_oauth2_cb cb,
    void *user_data
) {
    if (options == NULL || options->size < sizeof(*options)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    int first = 1;
    dcc_status_t status = dcc_oauth2_append_form_value(&body, &first, "client_id", options->client_id);
    if (status == DCC_OK) {
        status = dcc_oauth2_append_form_value(&body, &first, "client_secret", options->client_secret);
    }
    if (status == DCC_OK) {
        status = dcc_oauth2_append_form_value(&body, &first, "token", options->token);
    }
    if (status == DCC_OK && options->token_type_hint != NULL && options->token_type_hint[0] != '\0') {
        status = dcc_oauth2_append_form_value(&body, &first, "token_type_hint", options->token_type_hint);
    }
    if (status == DCC_OK) {
        status = dcc_oauth2_perform(
            "POST",
            "/oauth2/token/revoke",
            NULL,
            body.data,
            body.len,
            "application/x-www-form-urlencoded",
            cb,
            user_data
        );
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}
