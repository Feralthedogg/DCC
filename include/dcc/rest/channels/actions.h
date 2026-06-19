#ifndef DCC_REST_CHANNELS_ACTIONS_H
#define DCC_REST_CHANNELS_ACTIONS_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_follow_news_channel(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_trigger_channel_typing(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_set_channel_voice_status(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_set_channel_voice_status_params(
    dcc_client_t *client,
    const dcc_channel_voice_status_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
