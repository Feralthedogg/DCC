#ifndef DCC_REST_TYPES_INVITES_H
#define DCC_REST_TYPES_INVITES_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#define DCC_INVITE_RECORD_INIT_VALUE(type_, initializer_) type_ initializer_
#else
#define DCC_INVITE_RECORD_INIT_VALUE(type_, initializer_) (type_)initializer_
#endif

#define DCC_INVITE_PARAMS_VERSION 1U
#define DCC_REST_INVITE_QUERY_VERSION 1U
#define DCC_REST_INVITE_TARGET_USERS_UPLOAD_VERSION 1U

typedef enum dcc_invite_target_type {
    DCC_INVITE_TARGET_NONE = 0,
    DCC_INVITE_TARGET_STREAM = 1,
    DCC_INVITE_TARGET_EMBEDDED_APPLICATION = 2
} dcc_invite_target_type_t;

enum {
    DCC_INVITE_PARAMS_PRESENT_MAX_AGE = UINT64_C(1),
    DCC_INVITE_PARAMS_PRESENT_MAX_USES = UINT64_C(1) << 1U,
    DCC_INVITE_PARAMS_PRESENT_TEMPORARY = UINT64_C(1) << 2U,
    DCC_INVITE_PARAMS_PRESENT_UNIQUE = UINT64_C(1) << 3U,
    DCC_INVITE_PARAMS_PRESENT_TARGET_TYPE = UINT64_C(1) << 4U,
    DCC_INVITE_PARAMS_PRESENT_TARGET_USER_ID = UINT64_C(1) << 5U,
    DCC_INVITE_PARAMS_PRESENT_TARGET_APPLICATION_ID = UINT64_C(1) << 6U,
    DCC_INVITE_PARAMS_PRESENT_TARGET_USERS_FILE = UINT64_C(1) << 7U,
    DCC_INVITE_PARAMS_PRESENT_ROLE_IDS = UINT64_C(1) << 8U
};

typedef struct dcc_invite_params {
    size_t size;
    uint32_t version;
    uint64_t present;
    uint32_t max_age;
    uint32_t max_uses;
    uint8_t temporary;
    uint8_t unique;
    dcc_invite_target_type_t target_type;
    dcc_snowflake_t target_user_id;
    dcc_snowflake_t target_application_id;
    const char *target_users_filename;
    const void *target_users_data;
    size_t target_users_len;
    const dcc_snowflake_t *role_ids;
    size_t role_id_count;
} dcc_invite_params_t;

#define DCC_INVITE_PARAMS_INIT \
    { sizeof(dcc_invite_params_t), DCC_INVITE_PARAMS_VERSION, UINT64_C(0), \
      0U, 0U, 0U, 0U, DCC_INVITE_TARGET_NONE, 0U, 0U, NULL, NULL, 0U, \
      NULL, 0U }

static inline void dcc_invite_params_init(dcc_invite_params_t *value) {
    if (value != NULL) *value = DCC_INVITE_RECORD_INIT_VALUE(dcc_invite_params_t, DCC_INVITE_PARAMS_INIT);
}

enum {
    DCC_REST_INVITE_QUERY_PRESENT_WITH_COUNTS = UINT64_C(1),
    DCC_REST_INVITE_QUERY_PRESENT_GUILD_SCHEDULED_EVENT_ID = UINT64_C(1) << 1U
};

typedef struct dcc_rest_invite_query {
    size_t size;
    uint32_t version;
    uint64_t present;
    uint8_t with_counts;
    dcc_snowflake_t guild_scheduled_event_id;
} dcc_rest_invite_query_t;

#define DCC_REST_INVITE_QUERY_INIT \
    { sizeof(dcc_rest_invite_query_t), DCC_REST_INVITE_QUERY_VERSION, \
      UINT64_C(0), 0U, 0U }

static inline void dcc_rest_invite_query_init(dcc_rest_invite_query_t *value) {
    if (value != NULL) *value = DCC_INVITE_RECORD_INIT_VALUE(dcc_rest_invite_query_t, DCC_REST_INVITE_QUERY_INIT);
}

typedef struct dcc_rest_invite_target_users_upload {
    size_t size;
    uint32_t version;
    const char *filename;
    const void *data;
    size_t data_len;
} dcc_rest_invite_target_users_upload_t;

#define DCC_REST_INVITE_TARGET_USERS_UPLOAD_INIT \
    { sizeof(dcc_rest_invite_target_users_upload_t), \
      DCC_REST_INVITE_TARGET_USERS_UPLOAD_VERSION, NULL, NULL, 0U }

static inline void dcc_rest_invite_target_users_upload_init(
    dcc_rest_invite_target_users_upload_t *value
) { if (value != NULL) *value = DCC_INVITE_RECORD_INIT_VALUE(dcc_rest_invite_target_users_upload_t, DCC_REST_INVITE_TARGET_USERS_UPLOAD_INIT); }

#ifdef __cplusplus
}
#endif

#undef DCC_INVITE_RECORD_INIT_VALUE

#endif
