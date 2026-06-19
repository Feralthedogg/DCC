#ifndef DCC_REST_DIRECT_MESSAGES_INTERNAL_H
#define DCC_REST_DIRECT_MESSAGES_INTERNAL_H

#include <dcc/dcc.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_parse_channel_id_from_json(
    const char *body,
    size_t body_len,
    dcc_snowflake_t *out
);

#ifdef __cplusplus
}
#endif

#endif
