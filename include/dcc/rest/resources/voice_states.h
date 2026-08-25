#ifndef DCC_REST_RESOURCES_VOICE_STATES_H
#define DCC_REST_RESOURCES_VOICE_STATES_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_current_user_voice_state(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_modify_current_user_voice_state(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_current_user_voice_state_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_get_user_voice_state(dcc_client_t *client,
                                                   dcc_snowflake_t guild_id,
                                                   dcc_snowflake_t user_id,
                                                   const dcc_rest_call_options_t *options,
                                                   dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_modify_user_voice_state(
    dcc_client_t *client, dcc_snowflake_t guild_id, dcc_snowflake_t user_id,
    const dcc_user_voice_state_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_get_voice_regions(
    dcc_client_t *client, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_get_guild_voice_regions(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

#ifdef __cplusplus
}
#endif

#endif
