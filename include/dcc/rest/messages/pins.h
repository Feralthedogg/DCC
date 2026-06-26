/**
 * @file include/dcc/rest/messages/pins.h
 * @brief Public DCC rest messages pins declarations.
 */

#ifndef DCC_REST_MESSAGES_PINS_H
#define DCC_REST_MESSAGES_PINS_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Submits a REST request to pin message.
 */
DCC_API dcc_status_t dcc_rest_pin_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits a REST request to unpin message.
 */
DCC_API dcc_status_t dcc_rest_unpin_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits a REST request to get channel pins.
 */
DCC_API dcc_status_t dcc_rest_get_channel_pins(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits a REST request to get channel pins page.
 */
DCC_API dcc_status_t dcc_rest_get_channel_pins_page(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *before_iso8601,
    uint64_t limit,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits a REST request to pin message using the legacy pins route.
 */
DCC_API dcc_status_t dcc_rest_legacy_pin_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits a REST request to unpin message using the legacy pins route.
 */
DCC_API dcc_status_t dcc_rest_legacy_unpin_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);

/**
 * @brief Submits a REST request to get channel pins using the legacy pins route.
 */
DCC_API dcc_status_t dcc_rest_get_legacy_channel_pins(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
