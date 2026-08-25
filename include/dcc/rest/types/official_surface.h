#ifndef DCC_REST_TYPES_OFFICIAL_SURFACE_H
#define DCC_REST_TYPES_OFFICIAL_SURFACE_H

#include <dcc/application_types.h>
#include <dcc/rest/base.h>
#include <dcc/rest/types/string_map.h>

#define DCC_REST_TASK10_RECORD_VERSION 1U

typedef enum dcc_application_event_webhook_status {
  DCC_APPLICATION_EVENT_WEBHOOK_DISABLED = 1,
  DCC_APPLICATION_EVENT_WEBHOOK_ENABLED = 2
} dcc_application_event_webhook_status_t;

typedef struct dcc_application_install_params {
  size_t size;
  uint32_t version;
  uint64_t present;
  const char *const *scopes;
  size_t scope_count;
  const char *permissions;
} dcc_application_install_params_t;

typedef struct dcc_application_integration_type_config {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_application_integration_type_t integration_type;
  const dcc_application_install_params_t *oauth2_install_params;
} dcc_application_integration_type_config_t;

typedef struct dcc_application_modify_params {
  size_t size;
  uint32_t version;
  uint64_t present;
  const char *custom_install_url;
  const char *description;
  const char *role_connections_verification_url;
  const dcc_application_install_params_t *install_params;
  const dcc_application_integration_type_config_t *integration_types_config;
  size_t integration_type_config_count;
  uint64_t flags;
  const char *icon;
  const char *cover_image;
  const char *interactions_endpoint_url;
  const char *const *tags;
  size_t tag_count;
  const char *event_webhooks_url;
  dcc_application_event_webhook_status_t event_webhooks_status;
  const char *const *event_webhooks_types;
  size_t event_webhooks_type_count;
} dcc_application_modify_params_t;

enum {
  DCC_APPLICATION_INSTALL_PRESENT_SCOPES = UINT64_C(1),
  DCC_APPLICATION_INSTALL_PRESENT_PERMISSIONS = UINT64_C(1) << 1U,
  DCC_APPLICATION_INTEGRATION_CONFIG_PRESENT_OAUTH2_INSTALL_PARAMS = UINT64_C(1)
};
enum {
  DCC_APPLICATION_MODIFY_PRESENT_CUSTOM_INSTALL_URL = UINT64_C(1),
  DCC_APPLICATION_MODIFY_PRESENT_DESCRIPTION = UINT64_C(1) << 1U,
  DCC_APPLICATION_MODIFY_PRESENT_ROLE_CONNECTIONS_VERIFICATION_URL = UINT64_C(1)
                                                                     << 2U,
  DCC_APPLICATION_MODIFY_PRESENT_INSTALL_PARAMS = UINT64_C(1) << 3U,
  DCC_APPLICATION_MODIFY_PRESENT_INTEGRATION_TYPES_CONFIG = UINT64_C(1) << 4U,
  DCC_APPLICATION_MODIFY_PRESENT_FLAGS = UINT64_C(1) << 5U,
  DCC_APPLICATION_MODIFY_PRESENT_ICON = UINT64_C(1) << 6U,
  DCC_APPLICATION_MODIFY_PRESENT_COVER_IMAGE = UINT64_C(1) << 7U,
  DCC_APPLICATION_MODIFY_PRESENT_INTERACTIONS_ENDPOINT_URL = UINT64_C(1) << 8U,
  DCC_APPLICATION_MODIFY_PRESENT_TAGS = UINT64_C(1) << 9U,
  DCC_APPLICATION_MODIFY_PRESENT_EVENT_WEBHOOKS_URL = UINT64_C(1) << 10U,
  DCC_APPLICATION_MODIFY_PRESENT_EVENT_WEBHOOKS_STATUS = UINT64_C(1) << 11U,
  DCC_APPLICATION_MODIFY_PRESENT_EVENT_WEBHOOKS_TYPES = UINT64_C(1) << 12U
};

#define DCC_APPLICATION_INSTALL_PARAMS_INIT                                    \
  {sizeof(dcc_application_install_params_t),                                   \
   DCC_REST_TASK10_RECORD_VERSION,                                             \
   UINT64_C(0),                                                                \
   NULL,                                                                       \
   0U,                                                                         \
   NULL}
#define DCC_APPLICATION_INTEGRATION_TYPE_CONFIG_INIT                           \
  {sizeof(dcc_application_integration_type_config_t),                          \
   DCC_REST_TASK10_RECORD_VERSION, UINT64_C(0),                                \
   DCC_APPLICATION_INTEGRATION_TYPE_GUILD_INSTALL, NULL}
#define DCC_APPLICATION_MODIFY_PARAMS_INIT                                     \
  {sizeof(dcc_application_modify_params_t),                                    \
   DCC_REST_TASK10_RECORD_VERSION,                                             \
   UINT64_C(0),                                                                \
   NULL,                                                                       \
   NULL,                                                                       \
   NULL,                                                                       \
   NULL,                                                                       \
   NULL,                                                                       \
   0U,                                                                         \
   UINT64_C(0),                                                                \
   NULL,                                                                       \
   NULL,                                                                       \
   NULL,                                                                       \
   NULL,                                                                       \
   0U,                                                                         \
   NULL,                                                                       \
   DCC_APPLICATION_EVENT_WEBHOOK_DISABLED,                                     \
   NULL,                                                                       \
   0U}

typedef struct dcc_soundboard_send_params {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_snowflake_t sound_id;
  dcc_snowflake_t source_guild_id;
} dcc_soundboard_send_params_t;
enum { DCC_SOUNDBOARD_SEND_PRESENT_SOURCE_GUILD_ID = UINT64_C(1) };
#define DCC_SOUNDBOARD_SEND_PARAMS_INIT                                        \
  {sizeof(dcc_soundboard_send_params_t), DCC_REST_TASK10_RECORD_VERSION,       \
   UINT64_C(0), 0U, 0U}

typedef struct dcc_rest_sku_subscription_query {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_snowflake_t before;
  dcc_snowflake_t after;
  uint8_t limit;
  dcc_snowflake_t user_id;
} dcc_rest_sku_subscription_query_t;
enum {
  DCC_REST_SKU_SUBSCRIPTION_QUERY_PRESENT_BEFORE = UINT64_C(1),
  DCC_REST_SKU_SUBSCRIPTION_QUERY_PRESENT_AFTER = UINT64_C(1) << 1U,
  DCC_REST_SKU_SUBSCRIPTION_QUERY_PRESENT_LIMIT = UINT64_C(1) << 2U,
  DCC_REST_SKU_SUBSCRIPTION_QUERY_PRESENT_USER_ID = UINT64_C(1) << 3U
};
#define DCC_REST_SKU_SUBSCRIPTION_QUERY_INIT                                   \
  {sizeof(dcc_rest_sku_subscription_query_t),                                  \
   DCC_REST_TASK10_RECORD_VERSION,                                             \
   UINT64_C(0),                                                                \
   0U,                                                                         \
   0U,                                                                         \
   0U,                                                                         \
   0U}

typedef struct dcc_lobby_create_or_join_params {
  size_t size;
  uint32_t version;
  uint64_t present;
  const char *secret;
  uint32_t idle_timeout_seconds;
  const dcc_rest_string_map_t *lobby_metadata;
  const dcc_rest_string_map_t *member_metadata;
} dcc_lobby_create_or_join_params_t;
typedef struct dcc_lobby_member_spec {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_snowflake_t user_id;
  const dcc_rest_string_map_t *metadata;
  uint64_t flags;
} dcc_lobby_member_spec_t;
typedef struct dcc_lobby_params {
  size_t size;
  uint32_t version;
  uint64_t present;
  const dcc_rest_string_map_t *metadata;
  const dcc_lobby_member_spec_t *members;
  size_t member_count;
  uint32_t idle_timeout_seconds;
} dcc_lobby_params_t;
typedef struct dcc_lobby_channel_link_params {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_snowflake_t channel_id;
} dcc_lobby_channel_link_params_t;
typedef struct dcc_lobby_member_params {
  size_t size;
  uint32_t version;
  uint64_t present;
  const dcc_rest_string_map_t *metadata;
  uint64_t flags;
  const char *additional_name;
} dcc_lobby_member_params_t;
typedef struct dcc_lobby_member_bulk_item {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_snowflake_t user_id;
  const dcc_rest_string_map_t *metadata;
  uint64_t flags;
  const char *additional_name;
  uint8_t remove_member;
} dcc_lobby_member_bulk_item_t;
typedef struct dcc_lobby_member_bulk_params {
  size_t size;
  uint32_t version;
  uint64_t present;
  const dcc_lobby_member_bulk_item_t *items;
  size_t item_count;
} dcc_lobby_member_bulk_params_t;
typedef struct dcc_lobby_message_params {
  size_t size;
  uint32_t version;
  uint64_t present;
  const char *content;
  const dcc_rest_string_map_t *metadata;
  uint64_t flags;
} dcc_lobby_message_params_t;
typedef struct dcc_lobby_message_moderation_metadata_item {
  const char *key;
  const char *value;
} dcc_lobby_message_moderation_metadata_item_t;
typedef struct dcc_lobby_message_moderation_metadata_params {
  size_t size;
  uint32_t version;
  uint64_t present;
  const dcc_lobby_message_moderation_metadata_item_t *items;
  size_t item_count;
} dcc_lobby_message_moderation_metadata_params_t;
typedef struct dcc_rest_lobby_message_query {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint16_t limit;
} dcc_rest_lobby_message_query_t;

enum {
  DCC_LOBBY_CREATE_OR_JOIN_PRESENT_IDLE_TIMEOUT_SECONDS = UINT64_C(1),
  DCC_LOBBY_CREATE_OR_JOIN_PRESENT_LOBBY_METADATA = UINT64_C(1) << 1U,
  DCC_LOBBY_CREATE_OR_JOIN_PRESENT_MEMBER_METADATA = UINT64_C(1) << 2U
};
enum {
  DCC_LOBBY_MEMBER_SPEC_PRESENT_METADATA = UINT64_C(1),
  DCC_LOBBY_MEMBER_SPEC_PRESENT_FLAGS = UINT64_C(1) << 1U,
  DCC_LOBBY_PRESENT_METADATA = UINT64_C(1),
  DCC_LOBBY_PRESENT_MEMBERS = UINT64_C(1) << 1U,
  DCC_LOBBY_PRESENT_IDLE_TIMEOUT_SECONDS = UINT64_C(1) << 2U,
  DCC_LOBBY_CHANNEL_LINK_PRESENT_CHANNEL_ID = UINT64_C(1),
  DCC_LOBBY_MEMBER_PRESENT_METADATA = UINT64_C(1),
  DCC_LOBBY_MEMBER_PRESENT_FLAGS = UINT64_C(1) << 1U,
  DCC_LOBBY_MEMBER_PRESENT_ADDITIONAL_NAME = UINT64_C(1) << 2U,
  DCC_LOBBY_BULK_ITEM_PRESENT_METADATA = UINT64_C(1),
  DCC_LOBBY_BULK_ITEM_PRESENT_FLAGS = UINT64_C(1) << 1U,
  DCC_LOBBY_BULK_ITEM_PRESENT_ADDITIONAL_NAME = UINT64_C(1) << 2U,
  DCC_LOBBY_BULK_ITEM_PRESENT_REMOVE_MEMBER = UINT64_C(1) << 3U,
  DCC_LOBBY_MESSAGE_PRESENT_METADATA = UINT64_C(1),
  DCC_LOBBY_MESSAGE_PRESENT_FLAGS = UINT64_C(1) << 1U,
  DCC_REST_LOBBY_MESSAGE_QUERY_PRESENT_LIMIT = UINT64_C(1)
};

#define DCC_LOBBY_CREATE_OR_JOIN_PARAMS_INIT                                   \
  {sizeof(dcc_lobby_create_or_join_params_t), 1U, 0U, NULL, 0U, NULL, NULL}
#define DCC_LOBBY_MEMBER_SPEC_INIT                                             \
  {sizeof(dcc_lobby_member_spec_t), 1U, 0U, 0U, NULL, 0U}
#define DCC_LOBBY_PARAMS_INIT                                                  \
  {sizeof(dcc_lobby_params_t), 1U, 0U, NULL, NULL, 0U, 0U}
#define DCC_LOBBY_CHANNEL_LINK_PARAMS_INIT                                     \
  {sizeof(dcc_lobby_channel_link_params_t), 1U, 0U, 0U}
#define DCC_LOBBY_MEMBER_PARAMS_INIT                                           \
  {sizeof(dcc_lobby_member_params_t), 1U, 0U, NULL, 0U, NULL}
#define DCC_LOBBY_MEMBER_BULK_ITEM_INIT                                        \
  {sizeof(dcc_lobby_member_bulk_item_t), 1U, 0U, 0U, NULL, 0U, NULL, 0U}
#define DCC_LOBBY_MEMBER_BULK_PARAMS_INIT                                      \
  {sizeof(dcc_lobby_member_bulk_params_t), 1U, 0U, NULL, 0U}
#define DCC_LOBBY_MESSAGE_PARAMS_INIT                                          \
  {sizeof(dcc_lobby_message_params_t), 1U, 0U, NULL, NULL, 0U}
#define DCC_LOBBY_MESSAGE_MODERATION_METADATA_PARAMS_INIT                      \
  {sizeof(dcc_lobby_message_moderation_metadata_params_t), 1U, 0U, NULL, 0U}
#define DCC_REST_LOBBY_MESSAGE_QUERY_INIT                                      \
  {sizeof(dcc_rest_lobby_message_query_t), 1U, 0U, 0U}

#endif
