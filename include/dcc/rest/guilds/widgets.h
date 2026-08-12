#ifndef DCC_REST_GUILDS_WIDGETS_H
#define DCC_REST_GUILDS_WIDGETS_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_guild_widget(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_modify_guild_widget(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_guild_widget_update_t *body,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_get_guild_vanity_url(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

#ifdef __cplusplus
}
#endif

#endif
