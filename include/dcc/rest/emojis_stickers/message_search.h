#ifndef DCC_REST_EMOJIS_STICKERS_MESSAGE_SEARCH_H
#define DCC_REST_EMOJIS_STICKERS_MESSAGE_SEARCH_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_guild_message_search(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_message_search_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
