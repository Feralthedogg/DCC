#ifndef DCC_REST_RESPONSE_HELPERS_H
#define DCC_REST_RESPONSE_HELPERS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_response_snowflake_field(
    const dcc_rest_response_t *response,
    const char *field_name,
    dcc_snowflake_t *out
);
DCC_API dcc_status_t dcc_rest_response_message_id(
    const dcc_rest_response_t *response,
    dcc_snowflake_t *out_message_id
);
DCC_API dcc_status_t dcc_rest_response_channel_id(
    const dcc_rest_response_t *response,
    dcc_snowflake_t *out_channel_id
);
DCC_API dcc_status_t dcc_rest_response_guild_id(
    const dcc_rest_response_t *response,
    dcc_snowflake_t *out_guild_id
);

#ifdef __cplusplus
}
#endif

#endif
