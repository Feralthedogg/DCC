#ifndef DCC_REST_TYPES_ROLES_H
#define DCC_REST_TYPES_ROLES_H

#include <dcc/permissions.h>
#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#define DCC_ROLE_RECORD_INIT(type_, value_) type_ value_
#else
#define DCC_ROLE_RECORD_INIT(type_, value_) (type_) value_
#endif

enum {
  DCC_REST_ROLE_COLORS_VERSION = 1U,
  DCC_REST_GUILD_ROLE_CREATE_VERSION = 1U,
  DCC_REST_GUILD_ROLE_UPDATE_VERSION = 1U,
  DCC_REST_GUILD_ROLE_POSITION_VERSION = 1U
};

enum {
  DCC_REST_ROLE_COLORS_PRESENT_SECONDARY_COLOR = UINT64_C(1),
  DCC_REST_ROLE_COLORS_PRESENT_TERTIARY_COLOR = UINT64_C(1) << 1U
};

typedef struct dcc_rest_role_colors {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint64_t nulls;
  uint32_t primary_color;
  uint32_t secondary_color;
  uint32_t tertiary_color;
} dcc_rest_role_colors_t;

#define DCC_REST_ROLE_COLORS_INIT(primary_color_)                              \
  {sizeof(dcc_rest_role_colors_t),                                             \
   DCC_REST_ROLE_COLORS_VERSION,                                               \
   UINT64_C(0),                                                                \
   UINT64_C(0),                                                                \
   (primary_color_),                                                           \
   0U,                                                                         \
   0U}

static inline void dcc_rest_role_colors_init(dcc_rest_role_colors_t *value,
                                             uint32_t primary_color) {
  if (value != NULL)
    *value = DCC_ROLE_RECORD_INIT(dcc_rest_role_colors_t,
                                  DCC_REST_ROLE_COLORS_INIT(primary_color));
}

enum {
  DCC_REST_GUILD_ROLE_CREATE_PRESENT_NAME = UINT64_C(1),
  DCC_REST_GUILD_ROLE_CREATE_PRESENT_PERMISSIONS = UINT64_C(1) << 1U,
  DCC_REST_GUILD_ROLE_CREATE_PRESENT_COLOR = UINT64_C(1) << 2U,
  DCC_REST_GUILD_ROLE_CREATE_PRESENT_COLORS = UINT64_C(1) << 3U,
  DCC_REST_GUILD_ROLE_CREATE_PRESENT_HOIST = UINT64_C(1) << 4U,
  DCC_REST_GUILD_ROLE_CREATE_PRESENT_ICON = UINT64_C(1) << 5U,
  DCC_REST_GUILD_ROLE_CREATE_PRESENT_UNICODE_EMOJI = UINT64_C(1) << 6U,
  DCC_REST_GUILD_ROLE_CREATE_PRESENT_MENTIONABLE = UINT64_C(1) << 7U
};

typedef struct dcc_rest_guild_role_create {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint64_t nulls;
  const char *name;
  dcc_permission_t permissions;
  uint32_t color;
  const dcc_rest_role_colors_t *colors;
  uint8_t hoist;
  const char *icon;
  const char *unicode_emoji;
  uint8_t mentionable;
} dcc_rest_guild_role_create_t;

#define DCC_REST_GUILD_ROLE_CREATE_INIT                                        \
  {sizeof(dcc_rest_guild_role_create_t),                                       \
   DCC_REST_GUILD_ROLE_CREATE_VERSION,                                         \
   UINT64_C(0),                                                                \
   UINT64_C(0),                                                                \
   NULL,                                                                       \
   UINT64_C(0),                                                                \
   0U,                                                                         \
   NULL,                                                                       \
   0U,                                                                         \
   NULL,                                                                       \
   NULL,                                                                       \
   0U}

static inline void
dcc_rest_guild_role_create_init(dcc_rest_guild_role_create_t *value) {
  if (value != NULL)
    *value = DCC_ROLE_RECORD_INIT(dcc_rest_guild_role_create_t,
                                  DCC_REST_GUILD_ROLE_CREATE_INIT);
}

enum {
  DCC_REST_GUILD_ROLE_UPDATE_PRESENT_NAME = UINT64_C(1),
  DCC_REST_GUILD_ROLE_UPDATE_PRESENT_PERMISSIONS = UINT64_C(1) << 1U,
  DCC_REST_GUILD_ROLE_UPDATE_PRESENT_COLOR = UINT64_C(1) << 2U,
  DCC_REST_GUILD_ROLE_UPDATE_PRESENT_COLORS = UINT64_C(1) << 3U,
  DCC_REST_GUILD_ROLE_UPDATE_PRESENT_HOIST = UINT64_C(1) << 4U,
  DCC_REST_GUILD_ROLE_UPDATE_PRESENT_ICON = UINT64_C(1) << 5U,
  DCC_REST_GUILD_ROLE_UPDATE_PRESENT_UNICODE_EMOJI = UINT64_C(1) << 6U,
  DCC_REST_GUILD_ROLE_UPDATE_PRESENT_MENTIONABLE = UINT64_C(1) << 7U
};

typedef struct dcc_rest_guild_role_update {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint64_t nulls;
  const char *name;
  dcc_permission_t permissions;
  uint32_t color;
  const dcc_rest_role_colors_t *colors;
  uint8_t hoist;
  const char *icon;
  const char *unicode_emoji;
  uint8_t mentionable;
} dcc_rest_guild_role_update_t;

#define DCC_REST_GUILD_ROLE_UPDATE_INIT                                        \
  {sizeof(dcc_rest_guild_role_update_t),                                       \
   DCC_REST_GUILD_ROLE_UPDATE_VERSION,                                         \
   UINT64_C(0),                                                                \
   UINT64_C(0),                                                                \
   NULL,                                                                       \
   UINT64_C(0),                                                                \
   0U,                                                                         \
   NULL,                                                                       \
   0U,                                                                         \
   NULL,                                                                       \
   NULL,                                                                       \
   0U}

static inline void
dcc_rest_guild_role_update_init(dcc_rest_guild_role_update_t *value) {
  if (value != NULL)
    *value = DCC_ROLE_RECORD_INIT(dcc_rest_guild_role_update_t,
                                  DCC_REST_GUILD_ROLE_UPDATE_INIT);
}

enum { DCC_REST_GUILD_ROLE_POSITION_PRESENT_POSITION = UINT64_C(1) };

typedef struct dcc_rest_guild_role_position {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint64_t nulls;
  dcc_snowflake_t role_id;
  int32_t position;
} dcc_rest_guild_role_position_t;

#define DCC_REST_GUILD_ROLE_POSITION_INIT(role_id_)                            \
  {sizeof(dcc_rest_guild_role_position_t),                                     \
   DCC_REST_GUILD_ROLE_POSITION_VERSION,                                       \
   UINT64_C(0),                                                                \
   UINT64_C(0),                                                                \
   (role_id_),                                                                 \
   0}

static inline void
dcc_rest_guild_role_position_init(dcc_rest_guild_role_position_t *value,
                                  dcc_snowflake_t role_id) {
  if (value != NULL)
    *value = DCC_ROLE_RECORD_INIT(dcc_rest_guild_role_position_t,
                                  DCC_REST_GUILD_ROLE_POSITION_INIT(role_id));
}

typedef struct dcc_role_params {
  size_t size;
  dcc_snowflake_t guild_id;
  dcc_snowflake_t role_id;
  const char *name;
  uint32_t color;
  uint8_t position;
  uint64_t permissions;
  uint8_t hoist;
  uint8_t mentionable;
  const char *icon;
  const char *unicode_emoji;
} dcc_role_params_t;

typedef struct dcc_role_position {
  dcc_snowflake_t role_id;
  uint8_t position;
} dcc_role_position_t;

#ifdef __cplusplus
}
#endif

#undef DCC_ROLE_RECORD_INIT

#endif
