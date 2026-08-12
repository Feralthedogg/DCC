#ifndef DCC_REST_EMOJIS_STICKERS_STICKERS_H
#define DCC_REST_EMOJIS_STICKERS_STICKERS_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_sticker(
    dcc_client_t *client,
    dcc_snowflake_t sticker_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_sticker_packs(
    dcc_client_t *client,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
