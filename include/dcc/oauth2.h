/**
 * @file include/dcc/oauth2.h
 * @brief Public Discord OAuth2 helper declarations.
 */

#ifndef DCC_OAUTH2_H
#define DCC_OAUTH2_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Callback invoked when an OAuth2 HTTP helper completes.
 *
 * Response body ownership follows dcc_rest_cb: body is valid only for the
 * duration of the callback.
 */
typedef void (*dcc_oauth2_cb)(const dcc_rest_response_t *response, void *user_data);

/**
 * @brief Options for exchanging an OAuth2 authorization code.
 */
typedef struct dcc_oauth2_exchange_code_options {
    size_t size;              /**< Size of this structure in bytes. */
    const char *client_id;    /**< OAuth2 client/application ID. */
    const char *client_secret; /**< OAuth2 client secret. */
    const char *code;         /**< Authorization code received from Discord. */
    const char *redirect_uri; /**< Redirect URI used for the authorization request. */
} dcc_oauth2_exchange_code_options_t;

/**
 * @brief Options for refreshing an OAuth2 token.
 */
typedef struct dcc_oauth2_refresh_token_options {
    size_t size;              /**< Size of this structure in bytes. */
    const char *client_id;    /**< OAuth2 client/application ID. */
    const char *client_secret; /**< OAuth2 client secret. */
    const char *refresh_token; /**< Refresh token to exchange. */
} dcc_oauth2_refresh_token_options_t;

/**
 * @brief Options for revoking an OAuth2 token.
 */
typedef struct dcc_oauth2_revoke_token_options {
    size_t size;              /**< Size of this structure in bytes. */
    const char *client_id;    /**< OAuth2 client/application ID. */
    const char *client_secret; /**< OAuth2 client secret. */
    const char *token;        /**< Access or refresh token to revoke. */
    const char *token_type_hint; /**< Optional `access_token` or `refresh_token` hint. */
} dcc_oauth2_revoke_token_options_t;

/**
 * @brief Submits `GET /oauth2/@me` using Bearer authorization.
 */
DCC_API dcc_status_t dcc_oauth2_get_current_authorization(
    const char *access_token,
    dcc_oauth2_cb cb,
    void *user_data
);

/**
 * @brief Submits `GET /oauth2/applications/@me` using Bot authorization.
 */
DCC_API dcc_status_t dcc_oauth2_get_current_bot_application(
    const char *bot_token,
    dcc_oauth2_cb cb,
    void *user_data
);

/**
 * @brief Exchanges an authorization code using `POST /oauth2/token`.
 */
DCC_API dcc_status_t dcc_oauth2_exchange_code(
    const dcc_oauth2_exchange_code_options_t *options,
    dcc_oauth2_cb cb,
    void *user_data
);

/**
 * @brief Refreshes an access token using `POST /oauth2/token`.
 */
DCC_API dcc_status_t dcc_oauth2_refresh_token(
    const dcc_oauth2_refresh_token_options_t *options,
    dcc_oauth2_cb cb,
    void *user_data
);

/**
 * @brief Revokes an access or refresh token using `POST /oauth2/token/revoke`.
 */
DCC_API dcc_status_t dcc_oauth2_revoke_token(
    const dcc_oauth2_revoke_token_options_t *options,
    dcc_oauth2_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
