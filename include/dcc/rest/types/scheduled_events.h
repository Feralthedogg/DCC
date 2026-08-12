#ifndef DCC_REST_TYPES_SCHEDULED_EVENTS_H
#define DCC_REST_TYPES_SCHEDULED_EVENTS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  DCC_REST_SCHEDULED_EVENT_QUERY_VERSION = 1U,
  DCC_REST_SCHEDULED_EVENT_USERS_QUERY_VERSION = 1U,
  DCC_REST_SCHEDULED_EVENT_QUERY_PRESENT_WITH_USER_COUNT = UINT64_C(1),
  DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_LIMIT = UINT64_C(1),
  DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_WITH_MEMBER = UINT64_C(1) << 1U,
  DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_BEFORE = UINT64_C(1) << 2U,
  DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_AFTER = UINT64_C(1) << 3U,
  DCC_SCHEDULED_EVENT_PARAMS_VERSION = 1U,
  DCC_SCHEDULED_EVENT_PARAMS_PRESENT_CHANNEL_ID = UINT64_C(1),
  DCC_SCHEDULED_EVENT_PARAMS_PRESENT_NAME = UINT64_C(1) << 1U,
  DCC_SCHEDULED_EVENT_PARAMS_PRESENT_DESCRIPTION = UINT64_C(1) << 2U,
  DCC_SCHEDULED_EVENT_PARAMS_PRESENT_IMAGE = UINT64_C(1) << 3U,
  DCC_SCHEDULED_EVENT_PARAMS_PRESENT_SCHEDULED_START_TIME = UINT64_C(1) << 4U,
  DCC_SCHEDULED_EVENT_PARAMS_PRESENT_SCHEDULED_END_TIME = UINT64_C(1) << 5U,
  DCC_SCHEDULED_EVENT_PARAMS_PRESENT_LOCATION = UINT64_C(1) << 6U,
  DCC_SCHEDULED_EVENT_PARAMS_PRESENT_PRIVACY_LEVEL = UINT64_C(1) << 7U,
  DCC_SCHEDULED_EVENT_PARAMS_PRESENT_STATUS = UINT64_C(1) << 8U,
  DCC_SCHEDULED_EVENT_PARAMS_PRESENT_ENTITY_TYPE = UINT64_C(1) << 9U
};

typedef struct dcc_rest_scheduled_event_query {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint8_t with_user_count;
} dcc_rest_scheduled_event_query_t;

typedef struct dcc_rest_scheduled_event_users_query {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint64_t limit;
  uint8_t with_member;
  dcc_snowflake_t before;
  dcc_snowflake_t after;
} dcc_rest_scheduled_event_users_query_t;

#define DCC_REST_SCHEDULED_EVENT_QUERY_INIT                                    \
  {sizeof(dcc_rest_scheduled_event_query_t),                                   \
   DCC_REST_SCHEDULED_EVENT_QUERY_VERSION, UINT64_C(0), 0U}
#define DCC_REST_SCHEDULED_EVENT_USERS_QUERY_INIT                              \
  {sizeof(dcc_rest_scheduled_event_users_query_t),                             \
   DCC_REST_SCHEDULED_EVENT_USERS_QUERY_VERSION, UINT64_C(0), UINT64_C(0),     \
   0U, UINT64_C(0), UINT64_C(0)}

typedef enum dcc_scheduled_event_privacy_level {
    DCC_SCHEDULED_EVENT_PRIVACY_GUILD_ONLY = 2
} dcc_scheduled_event_privacy_level_t;

typedef enum dcc_scheduled_event_entity_type {
    DCC_SCHEDULED_EVENT_ENTITY_STAGE_INSTANCE = 1,
    DCC_SCHEDULED_EVENT_ENTITY_VOICE = 2,
    DCC_SCHEDULED_EVENT_ENTITY_EXTERNAL = 3
} dcc_scheduled_event_entity_type_t;

typedef enum dcc_scheduled_event_status {
    DCC_SCHEDULED_EVENT_STATUS_SCHEDULED = 1,
    DCC_SCHEDULED_EVENT_STATUS_ACTIVE = 2,
    DCC_SCHEDULED_EVENT_STATUS_COMPLETED = 3,
    DCC_SCHEDULED_EVENT_STATUS_CANCELLED = 4
} dcc_scheduled_event_status_t;

typedef struct dcc_scheduled_event_params {
    size_t size;
    uint32_t version;
    uint64_t present;
    uint64_t nulls;
    dcc_snowflake_t channel_id;
    const char *name;
    const char *description;
    const char *image;
    const char *scheduled_start_time;
    const char *scheduled_end_time;
    const char *location;
    dcc_scheduled_event_privacy_level_t privacy_level;
    dcc_scheduled_event_status_t status;
    dcc_scheduled_event_entity_type_t entity_type;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t event_id;
    dcc_snowflake_t creator_id;
    dcc_snowflake_t entity_id;
} dcc_scheduled_event_params_t;

#define DCC_SCHEDULED_EVENT_PARAMS_INIT                                        \
  {sizeof(dcc_scheduled_event_params_t), DCC_SCHEDULED_EVENT_PARAMS_VERSION,   \
   UINT64_C(0), UINT64_C(0), UINT64_C(0), NULL, NULL, NULL, NULL, NULL, NULL, \
   DCC_SCHEDULED_EVENT_PRIVACY_GUILD_ONLY,                                     \
   DCC_SCHEDULED_EVENT_STATUS_SCHEDULED, DCC_SCHEDULED_EVENT_ENTITY_VOICE, \
   UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0)}

static inline void dcc_rest_scheduled_event_query_init(
    dcc_rest_scheduled_event_query_t *query) {
  if (query != NULL) {
    dcc_rest_scheduled_event_query_t value =
        DCC_REST_SCHEDULED_EVENT_QUERY_INIT;
    *query = value;
  }
}

static inline void dcc_rest_scheduled_event_users_query_init(
    dcc_rest_scheduled_event_users_query_t *query) {
  if (query != NULL) {
    dcc_rest_scheduled_event_users_query_t value =
        DCC_REST_SCHEDULED_EVENT_USERS_QUERY_INIT;
    *query = value;
  }
}

static inline void dcc_scheduled_event_params_init(
    dcc_scheduled_event_params_t *params) {
  if (params != NULL) {
    dcc_scheduled_event_params_t value = DCC_SCHEDULED_EVENT_PARAMS_INIT;
    *params = value;
  }
}

#ifdef __cplusplus
}
#endif

#endif
