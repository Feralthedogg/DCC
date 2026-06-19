#ifndef DCC_REST_EMOJIS_STICKERS_GUILD_STICKERS_H
#define DCC_REST_EMOJIS_STICKERS_GUILD_STICKERS_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_guild_stickers(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_guild_sticker(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_guild_sticker_multipart(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_rest_multipart_field_t *fields,
    size_t field_count,
    const dcc_rest_multipart_file_t *file,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_guild_sticker_params(
    dcc_client_t *client,
    const dcc_guild_sticker_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_guild_sticker(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_delete_guild_sticker(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
