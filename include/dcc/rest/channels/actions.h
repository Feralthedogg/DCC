#ifndef DCC_REST_CHANNELS_ACTIONS_H
#define DCC_REST_CHANNELS_ACTIONS_H

#include <dcc/rest/types.h>
#include <dcc/rest/request.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_follow_news_channel(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_rest_follow_news_channel_t *follow,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_trigger_channel_typing(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_set_channel_voice_status(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_channel_voice_status_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
