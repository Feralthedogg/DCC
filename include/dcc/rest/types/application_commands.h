#ifndef DCC_REST_TYPES_APPLICATION_COMMANDS_H
#define DCC_REST_TYPES_APPLICATION_COMMANDS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  DCC_REST_COMMAND_LIST_QUERY_VERSION = 1U,
  DCC_GUILD_COMMAND_PERMISSIONS_PARAMS_VERSION = 1U,
  DCC_REST_COMMAND_LIST_QUERY_PRESENT_WITH_LOCALIZATIONS = UINT64_C(1),
  DCC_GUILD_COMMAND_PERMISSIONS_PARAMS_PRESENT_PERMISSIONS = UINT64_C(1)
};

typedef struct dcc_rest_command_list_query {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint8_t with_localizations;
} dcc_rest_command_list_query_t;

#define DCC_REST_COMMAND_LIST_QUERY_INIT                                      \
  {sizeof(dcc_rest_command_list_query_t), DCC_REST_COMMAND_LIST_QUERY_VERSION, \
   UINT64_C(0), 0U}

static inline void
dcc_rest_command_list_query_init(dcc_rest_command_list_query_t *query) {
  if (query != NULL) {
    dcc_rest_command_list_query_t value = DCC_REST_COMMAND_LIST_QUERY_INIT;
    *query = value;
  }
}

typedef struct dcc_guild_command_permissions_params {
    size_t size;
    uint32_t version;
    uint64_t present;
    const dcc_application_command_permission_t *permissions;
    size_t permission_count;
    dcc_snowflake_t application_id;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t command_id;
} dcc_guild_command_permissions_params_t;

#define DCC_GUILD_COMMAND_PERMISSIONS_PARAMS_INIT                              \
  {sizeof(dcc_guild_command_permissions_params_t),                             \
   DCC_GUILD_COMMAND_PERMISSIONS_PARAMS_VERSION, UINT64_C(0), NULL, 0U, \
   UINT64_C(0), UINT64_C(0), UINT64_C(0)}

static inline void dcc_guild_command_permissions_params_init(
    dcc_guild_command_permissions_params_t *params) {
  if (params != NULL) {
    dcc_guild_command_permissions_params_t value =
        DCC_GUILD_COMMAND_PERMISSIONS_PARAMS_INIT;
    *params = value;
  }
}

typedef struct dcc_guild_command_permissions_update_params {
    dcc_snowflake_t command_id;
    const dcc_application_command_permission_t *permissions;
    size_t permission_count;
} dcc_guild_command_permissions_update_params_t;

typedef struct dcc_bulk_guild_command_permissions_params {
    size_t size;
    dcc_snowflake_t application_id;
    dcc_snowflake_t guild_id;
    const dcc_guild_command_permissions_update_params_t *commands;
    size_t command_count;
} dcc_bulk_guild_command_permissions_params_t;

typedef struct dcc_application_command_registration_options {
    size_t size;
    dcc_snowflake_t guild_id;
} dcc_application_command_registration_options_t;

#ifdef __cplusplus
}
#endif

#endif
