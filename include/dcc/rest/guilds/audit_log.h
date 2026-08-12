#ifndef DCC_REST_GUILDS_AUDIT_LOG_H
#define DCC_REST_GUILDS_AUDIT_LOG_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_guild_audit_log(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_guild_audit_log_query_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

#ifdef __cplusplus
}
#endif

#endif
