#ifndef DCC_SNOWFLAKE_H
#define DCC_SNOWFLAKE_H

#include <dcc/error.h>

#include <stddef.h>
#include <stdint.h>

typedef uint64_t dcc_snowflake_t;

#define DCC_SNOWFLAKE_DISCORD_EPOCH_MS UINT64_C(1420070400000)

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_snowflake_parse_len(
    const char *text,
    size_t text_len,
    dcc_snowflake_t *out
);
DCC_API dcc_status_t dcc_snowflake_parse(const char *text, dcc_snowflake_t *out);
DCC_API dcc_status_t dcc_snowflake_parse_user_mention(
    const char *text,
    dcc_snowflake_t *out
);
DCC_API dcc_status_t dcc_snowflake_parse_member_mention(
    const char *text,
    dcc_snowflake_t *out
);
DCC_API dcc_status_t dcc_snowflake_parse_role_mention(
    const char *text,
    dcc_snowflake_t *out
);
DCC_API dcc_status_t dcc_snowflake_parse_channel_mention(
    const char *text,
    dcc_snowflake_t *out
);
DCC_API dcc_status_t dcc_snowflake_parse_any_mention(
    const char *text,
    dcc_snowflake_t *out
);
DCC_API dcc_status_t dcc_snowflake_parse_any(
    const char *text,
    dcc_snowflake_t *out
);
DCC_API dcc_status_t dcc_snowflake_created_at_ms(
    dcc_snowflake_t snowflake,
    uint64_t *out_unix_ms
);
DCC_API dcc_status_t dcc_snowflake_created_at_unix_seconds(
    dcc_snowflake_t snowflake,
    int64_t *out_unix_seconds
);

#ifdef __cplusplus
}
#endif

#endif
