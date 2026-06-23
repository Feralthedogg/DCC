#ifndef DCC_MESSAGE_LINK_H
#define DCC_MESSAGE_LINK_H

#include <dcc/error.h>
#include <dcc/snowflake.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DCC_MESSAGE_LINK_MAX 128U

typedef struct dcc_message_link {
    dcc_snowflake_t guild_id;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t message_id;
    uint8_t is_dm;
} dcc_message_link_t;

DCC_API dcc_status_t dcc_message_link_format(
    char *buffer,
    size_t buffer_size,
    const dcc_message_link_t *link
);
DCC_API dcc_status_t dcc_message_link_parse(
    const char *text,
    dcc_message_link_t *out
);

#ifdef __cplusplus
}
#endif

#endif
