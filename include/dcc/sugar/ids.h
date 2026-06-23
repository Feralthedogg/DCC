#ifndef DCC_SUGAR_IDS_H
#define DCC_SUGAR_IDS_H

#include <dcc/snowflake.h>
#include <dcc/sugar/format.h>

#include <stdint.h>

static inline const char *dcc_sugar_snowflake_timestamp(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t snowflake,
    char style
) {
    int64_t unix_seconds = 0;
    if (dcc_snowflake_created_at_unix_seconds(snowflake, &unix_seconds) != DCC_OK) {
        if (buffer != NULL && buffer_size != 0U) {
            buffer[0] = '\0';
        }
        return "";
    }
    return dcc_sugar_timestamp(buffer, buffer_size, unix_seconds, style);
}

#define DCC_PARSE_SNOWFLAKE(text_, out_) \
    dcc_snowflake_parse((text_), (out_))

#define DCC_PARSE_ID(text_, out_) \
    dcc_snowflake_parse_any((text_), (out_))

#define DCC_PARSE_MENTION(text_, out_) \
    dcc_snowflake_parse_any_mention((text_), (out_))

#define DCC_PARSE_USER_MENTION(text_, out_) \
    dcc_snowflake_parse_user_mention((text_), (out_))

#define DCC_PARSE_MEMBER_MENTION(text_, out_) \
    dcc_snowflake_parse_member_mention((text_), (out_))

#define DCC_PARSE_ROLE_MENTION(text_, out_) \
    dcc_snowflake_parse_role_mention((text_), (out_))

#define DCC_PARSE_CHANNEL_MENTION(text_, out_) \
    dcc_snowflake_parse_channel_mention((text_), (out_))

#define DCC_PARSE_USER_ID(text_, out_) DCC_PARSE_ID((text_), (out_))
#define DCC_PARSE_ROLE_ID(text_, out_) DCC_PARSE_ID((text_), (out_))
#define DCC_PARSE_CHANNEL_ID(text_, out_) DCC_PARSE_ID((text_), (out_))
#define DCC_PARSE_GUILD_ID(text_, out_) DCC_PARSE_ID((text_), (out_))

#define DCC_SNOWFLAKE_CREATED_AT_MS(snowflake_, out_) \
    dcc_snowflake_created_at_ms((snowflake_), (out_))

#define DCC_SNOWFLAKE_CREATED_AT_UNIX_SECONDS(snowflake_, out_) \
    dcc_snowflake_created_at_unix_seconds((snowflake_), (out_))

#define DCC_SNOWFLAKE_TIMESTAMP(snowflake_, style_) \
    dcc_sugar_snowflake_timestamp( \
        (char[DCC_FORMAT_TIMESTAMP_MAX]){ 0 }, \
        DCC_FORMAT_TIMESTAMP_MAX, \
        (snowflake_), \
        (style_) \
    )

#define DCC_SNOWFLAKE_RELATIVE(snowflake_) DCC_SNOWFLAKE_TIMESTAMP((snowflake_), 'R')
#define DCC_ID_CREATED_AT(snowflake_, style_) DCC_SNOWFLAKE_TIMESTAMP((snowflake_), (style_))
#define DCC_ID_CREATED_RELATIVE(snowflake_) DCC_SNOWFLAKE_RELATIVE((snowflake_))

#endif
