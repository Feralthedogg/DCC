#ifndef DCC_REST_TYPES_GUILD_MEMBERS_H
#define DCC_REST_TYPES_GUILD_MEMBERS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#define DCC_GUILD_MEMBER_RECORD_INIT(type_, value_) type_ value_
#else
#define DCC_GUILD_MEMBER_RECORD_INIT(type_, value_) (type_) value_
#endif

enum {
  DCC_REST_GUILD_MEMBERS_QUERY_VERSION = 1U,
  DCC_REST_GUILD_MEMBER_SEARCH_QUERY_VERSION = 1U,
  DCC_REST_GUILD_MEMBER_ADD_VERSION = 1U,
  DCC_REST_GUILD_MEMBER_UPDATE_VERSION = 1U,
  DCC_REST_CURRENT_GUILD_MEMBER_UPDATE_VERSION = 1U,
  DCC_REST_CURRENT_GUILD_MEMBER_NICKNAME_VERSION = 1U,
  DCC_REST_GUILD_BAN_CREATE_VERSION = 1U,
  DCC_REST_GUILD_BULK_BAN_VERSION = 1U,
  DCC_REST_GUILD_PRUNE_VERSION = 1U
};

enum {
  DCC_REST_GUILD_MEMBERS_QUERY_PRESENT_AFTER = UINT64_C(1),
  DCC_REST_GUILD_MEMBERS_QUERY_PRESENT_LIMIT = UINT64_C(1) << 1U
};

typedef struct dcc_rest_guild_members_query {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_snowflake_t after;
  uint16_t limit;
} dcc_rest_guild_members_query_t;

#define DCC_REST_GUILD_MEMBERS_QUERY_INIT                                      \
  {sizeof(dcc_rest_guild_members_query_t),                                     \
   DCC_REST_GUILD_MEMBERS_QUERY_VERSION, UINT64_C(0), 0U, 0U}

static inline void
dcc_rest_guild_members_query_init(dcc_rest_guild_members_query_t *value) {
  if (value != NULL)
    *value = DCC_GUILD_MEMBER_RECORD_INIT(dcc_rest_guild_members_query_t,
                                          DCC_REST_GUILD_MEMBERS_QUERY_INIT);
}

enum { DCC_REST_GUILD_MEMBER_SEARCH_QUERY_PRESENT_LIMIT = UINT64_C(1) };

typedef struct dcc_rest_guild_member_search_query {
  size_t size;
  uint32_t version;
  uint64_t present;
  const char *query;
  uint16_t limit;
} dcc_rest_guild_member_search_query_t;

#define DCC_REST_GUILD_MEMBER_SEARCH_QUERY_INIT(query_)                        \
  {sizeof(dcc_rest_guild_member_search_query_t),                               \
   DCC_REST_GUILD_MEMBER_SEARCH_QUERY_VERSION, UINT64_C(0), (query_), 0U}

static inline void dcc_rest_guild_member_search_query_init(
    dcc_rest_guild_member_search_query_t *value, const char *query) {
  if (value != NULL)
    *value = DCC_GUILD_MEMBER_RECORD_INIT(
        dcc_rest_guild_member_search_query_t,
        DCC_REST_GUILD_MEMBER_SEARCH_QUERY_INIT(query));
}

enum {
  DCC_REST_GUILD_MEMBER_ADD_PRESENT_NICK = UINT64_C(1),
  DCC_REST_GUILD_MEMBER_ADD_PRESENT_ROLES = UINT64_C(1) << 1U,
  DCC_REST_GUILD_MEMBER_ADD_PRESENT_MUTE = UINT64_C(1) << 2U,
  DCC_REST_GUILD_MEMBER_ADD_PRESENT_DEAF = UINT64_C(1) << 3U
};

typedef struct dcc_rest_guild_member_add {
  size_t size;
  uint32_t version;
  uint64_t present;
  const char *access_token;
  const char *nick;
  const dcc_snowflake_t *roles;
  size_t role_count;
  uint8_t mute;
  uint8_t deaf;
} dcc_rest_guild_member_add_t;

#define DCC_REST_GUILD_MEMBER_ADD_INIT(access_token_)                          \
  {sizeof(dcc_rest_guild_member_add_t),                                        \
   DCC_REST_GUILD_MEMBER_ADD_VERSION,                                          \
   UINT64_C(0),                                                                \
   (access_token_),                                                            \
   NULL,                                                                       \
   NULL,                                                                       \
   0U,                                                                         \
   0U,                                                                         \
   0U}

static inline void
dcc_rest_guild_member_add_init(dcc_rest_guild_member_add_t *value,
                               const char *access_token) {
  if (value != NULL)
    *value = DCC_GUILD_MEMBER_RECORD_INIT(
        dcc_rest_guild_member_add_t,
        DCC_REST_GUILD_MEMBER_ADD_INIT(access_token));
}

enum {
  DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_NICK = UINT64_C(1),
  DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_ROLES = UINT64_C(1) << 1U,
  DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_MUTE = UINT64_C(1) << 2U,
  DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_DEAF = UINT64_C(1) << 3U,
  DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_CHANNEL_ID = UINT64_C(1) << 4U,
  DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_COMMUNICATION_DISABLED_UNTIL =
      UINT64_C(1) << 5U,
  DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_FLAGS = UINT64_C(1) << 6U
};

typedef struct dcc_rest_guild_member_update {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint64_t nulls;
  const char *nick;
  const dcc_snowflake_t *roles;
  size_t role_count;
  uint8_t mute;
  uint8_t deaf;
  dcc_snowflake_t channel_id;
  const char *communication_disabled_until;
  uint64_t flags;
} dcc_rest_guild_member_update_t;

#define DCC_REST_GUILD_MEMBER_UPDATE_INIT                                      \
  {sizeof(dcc_rest_guild_member_update_t),                                     \
   DCC_REST_GUILD_MEMBER_UPDATE_VERSION,                                       \
   UINT64_C(0),                                                                \
   UINT64_C(0),                                                                \
   NULL,                                                                       \
   NULL,                                                                       \
   0U,                                                                         \
   0U,                                                                         \
   0U,                                                                         \
   0U,                                                                         \
   NULL,                                                                       \
   UINT64_C(0)}

static inline void
dcc_rest_guild_member_update_init(dcc_rest_guild_member_update_t *value) {
  if (value != NULL)
    *value = DCC_GUILD_MEMBER_RECORD_INIT(dcc_rest_guild_member_update_t,
                                          DCC_REST_GUILD_MEMBER_UPDATE_INIT);
}

enum {
  DCC_REST_CURRENT_GUILD_MEMBER_UPDATE_PRESENT_NICK = UINT64_C(1),
  DCC_REST_CURRENT_GUILD_MEMBER_UPDATE_PRESENT_BANNER = UINT64_C(1) << 1U,
  DCC_REST_CURRENT_GUILD_MEMBER_UPDATE_PRESENT_AVATAR = UINT64_C(1) << 2U,
  DCC_REST_CURRENT_GUILD_MEMBER_UPDATE_PRESENT_BIO = UINT64_C(1) << 3U
};

typedef struct dcc_rest_current_guild_member_update {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint64_t nulls;
  const char *nick;
  const char *banner;
  const char *avatar;
  const char *bio;
} dcc_rest_current_guild_member_update_t;

#define DCC_REST_CURRENT_GUILD_MEMBER_UPDATE_INIT                              \
  {sizeof(dcc_rest_current_guild_member_update_t),                             \
   DCC_REST_CURRENT_GUILD_MEMBER_UPDATE_VERSION,                               \
   UINT64_C(0),                                                                \
   UINT64_C(0),                                                                \
   NULL,                                                                       \
   NULL,                                                                       \
   NULL,                                                                       \
   NULL}

static inline void dcc_rest_current_guild_member_update_init(
    dcc_rest_current_guild_member_update_t *value) {
  if (value != NULL)
    *value =
        DCC_GUILD_MEMBER_RECORD_INIT(dcc_rest_current_guild_member_update_t,
                                     DCC_REST_CURRENT_GUILD_MEMBER_UPDATE_INIT);
}

enum { DCC_REST_CURRENT_GUILD_MEMBER_NICKNAME_PRESENT_NICK = UINT64_C(1) };

typedef struct dcc_rest_current_guild_member_nickname {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint64_t nulls;
  const char *nick;
} dcc_rest_current_guild_member_nickname_t;

#define DCC_REST_CURRENT_GUILD_MEMBER_NICKNAME_INIT                            \
  {sizeof(dcc_rest_current_guild_member_nickname_t),                           \
   DCC_REST_CURRENT_GUILD_MEMBER_NICKNAME_VERSION, UINT64_C(0), UINT64_C(0),   \
   NULL}

static inline void dcc_rest_current_guild_member_nickname_init(
    dcc_rest_current_guild_member_nickname_t *value) {
  if (value != NULL)
    *value = DCC_GUILD_MEMBER_RECORD_INIT(
        dcc_rest_current_guild_member_nickname_t,
        DCC_REST_CURRENT_GUILD_MEMBER_NICKNAME_INIT);
}

enum { DCC_REST_GUILD_BAN_CREATE_PRESENT_DELETE_MESSAGE_SECONDS = UINT64_C(1) };

typedef struct dcc_rest_guild_ban_create {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint32_t delete_message_seconds;
} dcc_rest_guild_ban_create_t;

#define DCC_REST_GUILD_BAN_CREATE_INIT                                         \
  {sizeof(dcc_rest_guild_ban_create_t), DCC_REST_GUILD_BAN_CREATE_VERSION,     \
   UINT64_C(0), 0U}

static inline void
dcc_rest_guild_ban_create_init(dcc_rest_guild_ban_create_t *value) {
  if (value != NULL)
    *value = DCC_GUILD_MEMBER_RECORD_INIT(dcc_rest_guild_ban_create_t,
                                          DCC_REST_GUILD_BAN_CREATE_INIT);
}

enum { DCC_REST_GUILD_BULK_BAN_PRESENT_DELETE_MESSAGE_SECONDS = UINT64_C(1) };

typedef struct dcc_rest_guild_bulk_ban {
  size_t size;
  uint32_t version;
  uint64_t present;
  const dcc_snowflake_t *user_ids;
  size_t user_id_count;
  uint32_t delete_message_seconds;
} dcc_rest_guild_bulk_ban_t;

#define DCC_REST_GUILD_BULK_BAN_INIT(user_ids_, user_id_count_)                \
  {sizeof(dcc_rest_guild_bulk_ban_t),                                          \
   DCC_REST_GUILD_BULK_BAN_VERSION,                                            \
   UINT64_C(0),                                                                \
   (user_ids_),                                                                \
   (user_id_count_),                                                           \
   0U}

static inline void
dcc_rest_guild_bulk_ban_init(dcc_rest_guild_bulk_ban_t *value,
                             const dcc_snowflake_t *user_ids,
                             size_t user_id_count) {
  if (value != NULL)
    *value = DCC_GUILD_MEMBER_RECORD_INIT(
        dcc_rest_guild_bulk_ban_t,
        DCC_REST_GUILD_BULK_BAN_INIT(user_ids, user_id_count));
}

enum {
  DCC_REST_GUILD_PRUNE_PRESENT_DAYS = UINT64_C(1),
  DCC_REST_GUILD_PRUNE_PRESENT_COMPUTE_PRUNE_COUNT = UINT64_C(1) << 1U,
  DCC_REST_GUILD_PRUNE_PRESENT_INCLUDE_ROLES = UINT64_C(1) << 2U
};

typedef struct dcc_rest_guild_prune {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint32_t days;
  uint8_t compute_prune_count;
  const dcc_snowflake_t *include_roles;
  size_t include_role_count;
} dcc_rest_guild_prune_t;

#define DCC_REST_GUILD_PRUNE_INIT                                              \
  {sizeof(dcc_rest_guild_prune_t),                                             \
   DCC_REST_GUILD_PRUNE_VERSION,                                               \
   UINT64_C(0),                                                                \
   0U,                                                                         \
   0U,                                                                         \
   NULL,                                                                       \
   0U}

static inline void dcc_rest_guild_prune_init(dcc_rest_guild_prune_t *value) {
  if (value != NULL)
    *value = DCC_GUILD_MEMBER_RECORD_INIT(dcc_rest_guild_prune_t,
                                          DCC_REST_GUILD_PRUNE_INIT);
}

typedef enum dcc_guild_member_update_flag {
  DCC_GUILD_MEMBER_UPDATE_TIMEOUT = 1U << 0,
  DCC_GUILD_MEMBER_UPDATE_NICK = 1U << 1,
  DCC_GUILD_MEMBER_UPDATE_ROLES = 1U << 2,
  DCC_GUILD_MEMBER_UPDATE_VOICE = 1U << 3
} dcc_guild_member_update_flag_t;

typedef struct dcc_guild_member_params {
  size_t size;
  dcc_snowflake_t guild_id;
  dcc_snowflake_t user_id;
  uint32_t flags;
  const char *communication_disabled_until;
  const char *nick;
  const dcc_snowflake_t *roles;
  size_t role_count;
  uint8_t mute;
  uint8_t deaf;
  const char *access_token;
} dcc_guild_member_params_t;

typedef struct dcc_current_guild_member_params {
  size_t size;
  dcc_snowflake_t guild_id;
  const char *nick;
  const char *banner;
  const char *avatar;
  const char *bio;
} dcc_current_guild_member_params_t;

typedef struct dcc_guild_member_nickname_params {
  size_t size;
  dcc_snowflake_t guild_id;
  const char *nick;
} dcc_guild_member_nickname_params_t;

typedef struct dcc_guild_member_role_params {
  size_t size;
  dcc_snowflake_t guild_id;
  dcc_snowflake_t user_id;
  dcc_snowflake_t role_id;
} dcc_guild_member_role_params_t;

typedef struct dcc_guild_ban_params {
  size_t size;
  dcc_snowflake_t guild_id;
  dcc_snowflake_t user_id;
  uint32_t delete_message_seconds;
} dcc_guild_ban_params_t;

typedef struct dcc_guild_prune_params {
  size_t size;
  dcc_snowflake_t guild_id;
  uint32_t days;
  const dcc_snowflake_t *include_roles;
  size_t include_role_count;
  uint8_t compute_prune_count;
} dcc_guild_prune_params_t;

#ifdef __cplusplus
}
#endif

#undef DCC_GUILD_MEMBER_RECORD_INIT

#endif
