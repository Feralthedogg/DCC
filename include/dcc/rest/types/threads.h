#ifndef DCC_REST_TYPES_THREADS_H
#define DCC_REST_TYPES_THREADS_H

#include <dcc/rest/base.h>
#include <dcc/rest/types/channels.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_thread_member_params {
    size_t size;
    dcc_snowflake_t thread_id;
    dcc_snowflake_t user_id;
} dcc_thread_member_params_t;

typedef struct dcc_thread_params {
    size_t size;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t thread_id;
    dcc_snowflake_t message_id;
    const char *name;
    dcc_channel_auto_archive_duration_t auto_archive_duration;
    dcc_channel_type_t type;
    uint8_t invitable;
    uint16_t rate_limit_per_user;
    uint8_t archived;
    uint8_t locked;
    const dcc_snowflake_t *applied_tags;
    size_t applied_tag_count;
    const char *message_json;
} dcc_thread_params_t;

#ifdef __cplusplus
}
#endif

#endif
