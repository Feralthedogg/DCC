#ifndef DCC_REST_EMOJIS_STICKERS_STICKERS_H
#define DCC_REST_EMOJIS_STICKERS_STICKERS_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_sticker(
    dcc_client_t *client,
    dcc_snowflake_t sticker_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_sticker_packs(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
