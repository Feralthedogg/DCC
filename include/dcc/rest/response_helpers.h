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
DCC_API dcc_status_t dcc_rest_response_u64_field(
    const dcc_rest_response_t *response,
    const char *field_name,
    uint64_t *out
);
DCC_API dcc_status_t dcc_rest_response_i64_field(
    const dcc_rest_response_t *response,
    const char *field_name,
    int64_t *out
);
DCC_API dcc_status_t dcc_rest_response_bool_field(
    const dcc_rest_response_t *response,
    const char *field_name,
    int *out
);
DCC_API dcc_status_t dcc_rest_response_string_field(
    const dcc_rest_response_t *response,
    const char *field_name,
    char *out,
    size_t out_cap
);
DCC_API dcc_status_t dcc_rest_response_string_field_len(
    const dcc_rest_response_t *response,
    const char *field_name,
    char *out,
    size_t out_cap,
    size_t *out_len
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
