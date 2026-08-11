#ifndef DCC_REST_TYPES_THREADS_H
#define DCC_REST_TYPES_THREADS_H

#include <dcc/rest/base.h>
#include <dcc/rest/types/channels.h>
#include <dcc/rest/types/messages.h>

#ifdef __cplusplus
extern "C" {
#define DCC_THREAD_RECORD_INIT_VALUE(type_, initializer_) type_ initializer_
#else
#define DCC_THREAD_RECORD_INIT_VALUE(type_, initializer_) (type_)initializer_
#endif

#define DCC_THREAD_PARAMS_VERSION 1U
#define DCC_REST_THREAD_ARCHIVE_QUERY_VERSION 1U
#define DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_VERSION 1U
#define DCC_REST_THREAD_MEMBER_QUERY_VERSION 1U

enum {
    DCC_THREAD_PARAMS_PRESENT_NAME = UINT64_C(1),
    DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION = UINT64_C(1) << 1U,
    DCC_THREAD_PARAMS_PRESENT_TYPE = UINT64_C(1) << 2U,
    DCC_THREAD_PARAMS_PRESENT_INVITABLE = UINT64_C(1) << 3U,
    DCC_THREAD_PARAMS_PRESENT_RATE_LIMIT_PER_USER = UINT64_C(1) << 4U,
    DCC_THREAD_PARAMS_PRESENT_MESSAGE = UINT64_C(1) << 5U,
    DCC_THREAD_PARAMS_PRESENT_APPLIED_TAGS = UINT64_C(1) << 6U
};

typedef struct dcc_thread_params {
    size_t size;
    uint32_t version;
    uint64_t present;
    const char *name;
    dcc_channel_auto_archive_duration_t auto_archive_duration;
    dcc_channel_type_t type;
    uint8_t invitable;
    uint32_t rate_limit_per_user;
    const dcc_rest_message_payload_t *message;
    const dcc_snowflake_t *applied_tags;
    size_t applied_tag_count;
} dcc_thread_params_t;

#define DCC_THREAD_PARAMS_INIT \
    { sizeof(dcc_thread_params_t), DCC_THREAD_PARAMS_VERSION, UINT64_C(0), \
      NULL, DCC_CHANNEL_AUTO_ARCHIVE_1_DAY, DCC_CHANNEL_PUBLIC_THREAD, 0U, \
      0U, NULL, NULL, 0U }

static inline void dcc_thread_params_init(dcc_thread_params_t *value) {
    if (value != NULL) *value = DCC_THREAD_RECORD_INIT_VALUE(dcc_thread_params_t, DCC_THREAD_PARAMS_INIT);
}

enum {
    DCC_REST_THREAD_ARCHIVE_QUERY_PRESENT_BEFORE = UINT64_C(1),
    DCC_REST_THREAD_ARCHIVE_QUERY_PRESENT_LIMIT = UINT64_C(1) << 1U
};

typedef struct dcc_rest_thread_archive_query {
    size_t size;
    uint32_t version;
    uint64_t present;
    const char *before;
    uint32_t limit;
} dcc_rest_thread_archive_query_t;

#define DCC_REST_THREAD_ARCHIVE_QUERY_INIT \
    { sizeof(dcc_rest_thread_archive_query_t), \
      DCC_REST_THREAD_ARCHIVE_QUERY_VERSION, UINT64_C(0), NULL, 0U }

static inline void dcc_rest_thread_archive_query_init(
    dcc_rest_thread_archive_query_t *value
) { if (value != NULL) *value = DCC_THREAD_RECORD_INIT_VALUE(dcc_rest_thread_archive_query_t, DCC_REST_THREAD_ARCHIVE_QUERY_INIT); }

enum {
    DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_PRESENT_BEFORE = UINT64_C(1),
    DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_PRESENT_LIMIT = UINT64_C(1) << 1U
};

typedef struct dcc_rest_joined_thread_archive_query {
    size_t size;
    uint32_t version;
    uint64_t present;
    dcc_snowflake_t before;
    uint32_t limit;
} dcc_rest_joined_thread_archive_query_t;

#define DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_INIT \
    { sizeof(dcc_rest_joined_thread_archive_query_t), \
      DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_VERSION, UINT64_C(0), 0U, 0U }

static inline void dcc_rest_joined_thread_archive_query_init(
    dcc_rest_joined_thread_archive_query_t *value
) { if (value != NULL) *value = DCC_THREAD_RECORD_INIT_VALUE(dcc_rest_joined_thread_archive_query_t, DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_INIT); }

enum {
    DCC_REST_THREAD_MEMBER_QUERY_PRESENT_WITH_MEMBER = UINT64_C(1),
    DCC_REST_THREAD_MEMBER_QUERY_PRESENT_AFTER = UINT64_C(1) << 1U,
    DCC_REST_THREAD_MEMBER_QUERY_PRESENT_LIMIT = UINT64_C(1) << 2U
};

typedef struct dcc_rest_thread_member_query {
    size_t size;
    uint32_t version;
    uint64_t present;
    uint8_t with_member;
    dcc_snowflake_t after;
    uint32_t limit;
} dcc_rest_thread_member_query_t;

#define DCC_REST_THREAD_MEMBER_QUERY_INIT \
    { sizeof(dcc_rest_thread_member_query_t), \
      DCC_REST_THREAD_MEMBER_QUERY_VERSION, UINT64_C(0), 0U, 0U, 0U }

static inline void dcc_rest_thread_member_query_init(
    dcc_rest_thread_member_query_t *value
) { if (value != NULL) *value = DCC_THREAD_RECORD_INIT_VALUE(dcc_rest_thread_member_query_t, DCC_REST_THREAD_MEMBER_QUERY_INIT); }

#ifdef __cplusplus
}
#endif

#undef DCC_THREAD_RECORD_INIT_VALUE

#endif
