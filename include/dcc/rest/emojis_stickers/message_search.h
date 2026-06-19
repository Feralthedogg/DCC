#ifndef DCC_REST_EMOJIS_STICKERS_MESSAGE_SEARCH_H
#define DCC_REST_EMOJIS_STICKERS_MESSAGE_SEARCH_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_guild_message_search(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_guild_message_search_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_message_search_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
