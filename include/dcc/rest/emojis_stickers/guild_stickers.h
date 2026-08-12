#ifndef DCC_REST_EMOJIS_STICKERS_GUILD_STICKERS_H
#define DCC_REST_EMOJIS_STICKERS_GUILD_STICKERS_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_create_guild_sticker(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_guild_sticker_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_delete_guild_sticker(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_guild_sticker(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_guild_stickers(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_modify_guild_sticker(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    const dcc_guild_sticker_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
