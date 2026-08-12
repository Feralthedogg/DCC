#ifndef DCC_TASK9_TEST_LEGACY_SHIMS_H
#define DCC_TASK9_TEST_LEGACY_SHIMS_H

#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_TASK9_TEST_LEGACY_RETURN(callback_, user_data_, endpoint_, ...)     \
  do {                                                                          \
    dcc_rest_call_options_t task9_options_;                                      \
    dcc_rest_request_t *task9_request_ = NULL;                                  \
    void *task9_bridge_ = NULL;                                                  \
    dcc_status_t task9_status_ = dcc_endpoint_legacy_options(                   \
        (callback_), (user_data_), &task9_options_, &task9_bridge_);            \
    if (task9_status_ == DCC_OK)                                                 \
      task9_status_ = endpoint_(__VA_ARGS__, &task9_options_, &task9_request_);  \
    if (task9_status_ != DCC_OK)                                                 \
      dcc_endpoint_legacy_bridge_release(task9_bridge_);                         \
    if (task9_status_ == DCC_OK && task9_request_ != NULL) {                    \
      const dcc_rest_result_t *task9_result_ = NULL;                            \
      task9_status_ = dcc_rest_request_wait(                                    \
          task9_request_, UINT64_C(3000), &task9_result_);                      \
      dcc_rest_request_destroy(task9_request_);                                 \
    }                                                                           \
    return task9_status_;                                                        \
  } while (0)

static dcc_status_t task9_test_create_global_builder(
    dcc_client_t *client, dcc_snowflake_t app,
    const dcc_application_command_builder_t *command,
    dcc_rest_cb callback, void *user_data) {
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_create_global_command, client, app, command);
}
static dcc_status_t task9_test_edit_global_builder(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t command_id,
    const dcc_application_command_builder_t *command,
    dcc_rest_cb callback, void *user_data) {
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_edit_global_command, client, app, command_id, command);
}
static dcc_status_t task9_test_bulk_global_builder(
    dcc_client_t *client, dcc_snowflake_t app,
    const dcc_application_command_builder_t *commands, size_t count,
    dcc_rest_cb callback, void *user_data) {
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_bulk_overwrite_global_commands, client, app, commands, count);
}
static dcc_status_t task9_test_create_guild_builder(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t guild,
    const dcc_application_command_builder_t *command,
    dcc_rest_cb callback, void *user_data) {
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_create_guild_command, client, app, guild, command);
}
static dcc_status_t task9_test_edit_guild_builder(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t guild,
    dcc_snowflake_t command_id,
    const dcc_application_command_builder_t *command,
    dcc_rest_cb callback, void *user_data) {
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_edit_guild_command, client, app, guild, command_id, command);
}
static dcc_status_t task9_test_bulk_guild_builder(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t guild,
    const dcc_application_command_builder_t *commands, size_t count,
    dcc_rest_cb callback, void *user_data) {
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_bulk_overwrite_guild_commands, client, app, guild, commands, count);
}
static dcc_status_t task9_test_create_application_builder(
    dcc_client_t *client, dcc_snowflake_t app,
    const dcc_application_command_registration_options_t *scope,
    const dcc_application_command_builder_t *command,
    dcc_rest_cb callback, void *user_data) {
  if (scope != NULL && scope->guild_id != 0U)
    return task9_test_create_guild_builder(client, app, scope->guild_id,
                                           command, callback, user_data);
  return task9_test_create_global_builder(client, app, command, callback, user_data);
}
static dcc_status_t task9_test_bulk_application_builder(
    dcc_client_t *client, dcc_snowflake_t app,
    const dcc_application_command_registration_options_t *scope,
    const dcc_application_command_builder_t *commands, size_t count,
    dcc_rest_cb callback, void *user_data) {
  if (scope != NULL && scope->guild_id != 0U)
    return task9_test_bulk_guild_builder(client, app, scope->guild_id,
                                         commands, count, callback, user_data);
  return task9_test_bulk_global_builder(client, app, commands, count,
                                        callback, user_data);
}

static dcc_status_t task9_test_raw_request(
    dcc_client_t *client, dcc_rest_method_t method, const char *path,
    const char *query, const char *json, dcc_rest_cb callback, void *user_data) {
  char full_path[320];
  const char *request_path = path;
  if (query != NULL && query[0] != '\0') {
    int n = snprintf(full_path, sizeof(full_path), "%s?%s", path, query);
    if (n < 0 || (size_t)n >= sizeof(full_path)) return DCC_ERR_INVALID_ARG;
    request_path = full_path;
  }
  dcc_rest_call_options_t options;
  dcc_rest_request_t *request = NULL;
  void *bridge = NULL;
  dcc_status_t status = dcc_endpoint_legacy_options(
      callback, user_data, &options, &bridge);
  dcc_rest_request_desc_t desc = DCC_REST_REQUEST_DESC_INIT;
  desc.method = method;
  desc.path = request_path;
  desc.options = &options;
  if (json != NULL) {
    desc.content_type = "application/json";
    desc.body = json;
    desc.body_len = strlen(json);
  }
  if (status == DCC_OK) status = dcc_rest_submit(client, &desc, &request);
  if (status != DCC_OK) dcc_endpoint_legacy_bridge_release(bridge);
  if (status == DCC_OK && request != NULL) {
    const dcc_rest_result_t *result = NULL;
    status = dcc_rest_request_wait(request, UINT64_C(3000), &result);
    dcc_rest_request_destroy(request);
  }
  return status;
}

static dcc_status_t task9_test_bulk_global_raw(
    dcc_client_t *client, dcc_snowflake_t app, const char *json,
    dcc_rest_cb callback, void *user_data) {
  char path[96]; snprintf(path, sizeof(path), "/applications/%llu/commands",
      (unsigned long long)app);
  return task9_test_raw_request(client, DCC_REST_PUT, path, NULL, json,
                                callback, user_data);
}
static dcc_status_t task9_test_bulk_guild_raw(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t guild,
    const char *json, dcc_rest_cb callback, void *user_data) {
  char path[128]; snprintf(path, sizeof(path),
      "/applications/%llu/guilds/%llu/commands",
      (unsigned long long)app, (unsigned long long)guild);
  return task9_test_raw_request(client, DCC_REST_PUT, path, NULL, json,
                                callback, user_data);
}
static dcc_status_t task9_test_get_global_localized(
    dcc_client_t *client, dcc_snowflake_t app, uint8_t enabled,
    dcc_rest_cb callback, void *user_data) {
  char path[96]; snprintf(path, sizeof(path), "/applications/%llu/commands",
      (unsigned long long)app);
  return task9_test_raw_request(client, DCC_REST_GET, path,
      enabled ? "with_localizations=true" : "with_localizations=false",
      NULL, callback, user_data);
}
static dcc_status_t task9_test_get_global_raw(
    dcc_client_t *client, dcc_snowflake_t app, const char *query,
    dcc_rest_cb callback, void *user_data) {
  char path[96]; snprintf(path, sizeof(path), "/applications/%llu/commands",
      (unsigned long long)app);
  return task9_test_raw_request(client, DCC_REST_GET, path, query, NULL,
                                callback, user_data);
}
static dcc_status_t task9_test_get_global_one_raw(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t command,
    dcc_rest_cb callback, void *user_data) {
  char path[128]; snprintf(path, sizeof(path), "/applications/%llu/commands/%llu",
      (unsigned long long)app, (unsigned long long)command);
  return task9_test_raw_request(client, DCC_REST_GET, path, NULL, NULL,
                                callback, user_data);
}
static dcc_status_t task9_test_create_global_raw(
    dcc_client_t *client, dcc_snowflake_t app, const char *json,
    dcc_rest_cb callback, void *user_data) {
  char path[96]; snprintf(path, sizeof(path), "/applications/%llu/commands",
      (unsigned long long)app);
  return task9_test_raw_request(client, DCC_REST_POST, path, NULL, json,
                                callback, user_data);
}
static dcc_status_t task9_test_edit_global_raw(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t command,
    const char *json, dcc_rest_cb callback, void *user_data) {
  char path[128]; snprintf(path, sizeof(path), "/applications/%llu/commands/%llu",
      (unsigned long long)app, (unsigned long long)command);
  return task9_test_raw_request(client, DCC_REST_PATCH, path, NULL, json,
                                callback, user_data);
}
static dcc_status_t task9_test_delete_global_raw(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t command,
    dcc_rest_cb callback, void *user_data) {
  char path[128]; snprintf(path, sizeof(path), "/applications/%llu/commands/%llu",
      (unsigned long long)app, (unsigned long long)command);
  return task9_test_raw_request(client, DCC_REST_DELETE, path, NULL, NULL,
                                callback, user_data);
}
static dcc_status_t task9_test_get_guild_localized(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t guild,
    uint8_t enabled, dcc_rest_cb callback, void *user_data) {
  char path[128]; snprintf(path, sizeof(path),
      "/applications/%llu/guilds/%llu/commands",
      (unsigned long long)app, (unsigned long long)guild);
  return task9_test_raw_request(client, DCC_REST_GET, path,
      enabled ? "with_localizations=true" : "with_localizations=false",
      NULL, callback, user_data);
}
static dcc_status_t task9_test_get_guild_raw(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t guild,
    const char *query, dcc_rest_cb callback, void *user_data) {
  char path[128]; snprintf(path, sizeof(path),
      "/applications/%llu/guilds/%llu/commands",
      (unsigned long long)app, (unsigned long long)guild);
  return task9_test_raw_request(client, DCC_REST_GET, path, query, NULL,
                                callback, user_data);
}
static dcc_status_t task9_test_get_guild_one_raw(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t guild,
    dcc_snowflake_t command, dcc_rest_cb callback, void *user_data) {
  char path[160]; snprintf(path, sizeof(path),
      "/applications/%llu/guilds/%llu/commands/%llu",
      (unsigned long long)app, (unsigned long long)guild,
      (unsigned long long)command);
  return task9_test_raw_request(client, DCC_REST_GET, path, NULL, NULL,
                                callback, user_data);
}
static dcc_status_t task9_test_create_guild_raw(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t guild,
    const char *json, dcc_rest_cb callback, void *user_data) {
  char path[128]; snprintf(path, sizeof(path),
      "/applications/%llu/guilds/%llu/commands",
      (unsigned long long)app, (unsigned long long)guild);
  return task9_test_raw_request(client, DCC_REST_POST, path, NULL, json,
                                callback, user_data);
}
static dcc_status_t task9_test_edit_guild_raw(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t guild,
    dcc_snowflake_t command, const char *json,
    dcc_rest_cb callback, void *user_data) {
  char path[160]; snprintf(path, sizeof(path),
      "/applications/%llu/guilds/%llu/commands/%llu",
      (unsigned long long)app, (unsigned long long)guild,
      (unsigned long long)command);
  return task9_test_raw_request(client, DCC_REST_PATCH, path, NULL, json,
                                callback, user_data);
}
static dcc_status_t task9_test_delete_guild_raw(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t guild,
    dcc_snowflake_t command, dcc_rest_cb callback, void *user_data) {
  char path[160]; snprintf(path, sizeof(path),
      "/applications/%llu/guilds/%llu/commands/%llu",
      (unsigned long long)app, (unsigned long long)guild,
      (unsigned long long)command);
  return task9_test_raw_request(client, DCC_REST_DELETE, path, NULL, NULL,
                                callback, user_data);
}
static dcc_status_t task9_test_get_all_permissions_raw(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t guild,
    dcc_rest_cb callback, void *user_data) {
  char path[160]; snprintf(path, sizeof(path),
      "/applications/%llu/guilds/%llu/commands/permissions",
      (unsigned long long)app, (unsigned long long)guild);
  return task9_test_raw_request(client, DCC_REST_GET, path, NULL, NULL,
                                callback, user_data);
}
static dcc_status_t task9_test_get_permissions_raw(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t guild,
    dcc_snowflake_t command, dcc_rest_cb callback, void *user_data) {
  char path[192]; snprintf(path, sizeof(path),
      "/applications/%llu/guilds/%llu/commands/%llu/permissions",
      (unsigned long long)app, (unsigned long long)guild,
      (unsigned long long)command);
  return task9_test_raw_request(client, DCC_REST_GET, path, NULL, NULL,
                                callback, user_data);
}
static dcc_status_t task9_test_create_application_raw(
    dcc_client_t *client, dcc_snowflake_t app,
    const dcc_application_command_registration_options_t *scope,
    const char *json, dcc_rest_cb callback, void *user_data) {
  char path[128];
  if (scope != NULL && scope->guild_id != 0U)
    snprintf(path, sizeof(path), "/applications/%llu/guilds/%llu/commands",
        (unsigned long long)app, (unsigned long long)scope->guild_id);
  else snprintf(path, sizeof(path), "/applications/%llu/commands",
        (unsigned long long)app);
  return task9_test_raw_request(client, DCC_REST_POST, path, NULL, json,
                                callback, user_data);
}
static dcc_status_t task9_test_bulk_application_raw(
    dcc_client_t *client, dcc_snowflake_t app,
    const dcc_application_command_registration_options_t *scope,
    const char *json, dcc_rest_cb callback, void *user_data) {
  return scope != NULL && scope->guild_id != 0U
      ? task9_test_bulk_guild_raw(client, app, scope->guild_id, json,
                                  callback, user_data)
      : task9_test_bulk_global_raw(client, app, json, callback, user_data);
}
static dcc_status_t task9_test_delete_all_application(
    dcc_client_t *client, dcc_snowflake_t app,
    const dcc_application_command_registration_options_t *scope,
    dcc_rest_cb callback, void *user_data) {
  return task9_test_bulk_application_raw(client, app, scope, "[]", callback,
                                         user_data);
}
static dcc_status_t task9_test_edit_permissions_params(
    dcc_client_t *client, const dcc_guild_command_permissions_params_t *input,
    dcc_rest_cb callback, void *user_data) {
  if (input == NULL) return DCC_ERR_INVALID_ARG;
  dcc_guild_command_permissions_params_t params = *input;
  params.size = sizeof(params);
  params.version = DCC_GUILD_COMMAND_PERMISSIONS_PARAMS_VERSION;
  params.present = DCC_GUILD_COMMAND_PERMISSIONS_PARAMS_PRESENT_PERMISSIONS;
  dcc_rest_call_options_t options;
  dcc_rest_request_t *request = NULL;
  void *bridge = NULL;
  dcc_status_t status = dcc_endpoint_legacy_options(
      callback, user_data, &options, &bridge);
  options.auth_mode = DCC_REST_AUTH_BEARER;
  options.auth_token = "task9-test-bearer";
  if (status == DCC_OK)
    status = dcc_rest_edit_guild_command_permissions(
        client, input->application_id, input->guild_id, input->command_id,
        &params, &options, &request);
  if (status != DCC_OK) dcc_endpoint_legacy_bridge_release(bridge);
  if (status == DCC_OK && request != NULL) {
    const dcc_rest_result_t *result = NULL;
    status = dcc_rest_request_wait(request, UINT64_C(3000), &result);
    dcc_rest_request_destroy(request);
  }
  return status;
}

static dcc_status_t task9_test_onboarding_params(
    dcc_client_t *client, const dcc_onboarding_params_t *params,
    dcc_rest_cb callback, void *user_data) {
  if (params == NULL) return DCC_ERR_INVALID_ARG;
  char *body = NULL;
  dcc_status_t status = dcc_rest_build_onboarding_body(params, &body);
  if (status == DCC_OK) {
    char path[96]; snprintf(path, sizeof(path), "/guilds/%llu/onboarding",
        (unsigned long long)params->guild_id);
    status = task9_test_raw_request(client, DCC_REST_PUT, path, NULL, body,
                                    callback, user_data);
  }
  free(body);
  return status;
}
static dcc_status_t task9_test_automod_create_params(
    dcc_client_t *client, dcc_snowflake_t guild,
    const dcc_auto_moderation_rule_params_t *params,
    dcc_rest_cb callback, void *user_data) {
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_create_auto_moderation_rule, client, guild, params);
}
static dcc_status_t task9_test_automod_modify_params(
    dcc_client_t *client, dcc_snowflake_t guild,
    const dcc_auto_moderation_rule_params_t *params,
    dcc_rest_cb callback, void *user_data) {
  if (params == NULL || params->rule_id == 0U) return DCC_ERR_INVALID_ARG;
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_modify_auto_moderation_rule, client, guild, params->rule_id, params);
}
static dcc_status_t task9_test_scheduled_list_count(
    dcc_client_t *client, dcc_snowflake_t guild,
    dcc_rest_cb callback, void *user_data) {
  dcc_rest_scheduled_event_query_t query = DCC_REST_SCHEDULED_EVENT_QUERY_INIT;
  query.present = DCC_REST_SCHEDULED_EVENT_QUERY_PRESENT_WITH_USER_COUNT;
  query.with_user_count = 1U;
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_get_guild_scheduled_events, client, guild, &query);
}
static dcc_status_t task9_test_scheduled_get_count(
    dcc_client_t *client, dcc_snowflake_t guild, dcc_snowflake_t event,
    dcc_rest_cb callback, void *user_data) {
  dcc_rest_scheduled_event_query_t query = DCC_REST_SCHEDULED_EVENT_QUERY_INIT;
  query.present = DCC_REST_SCHEDULED_EVENT_QUERY_PRESENT_WITH_USER_COUNT;
  query.with_user_count = 1U;
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_get_guild_scheduled_event, client, guild, event, &query);
}
static dcc_status_t task9_test_scheduled_create_params(
    dcc_client_t *client, const dcc_scheduled_event_params_t *input,
    dcc_rest_cb callback, void *user_data) {
  if (input == NULL) return DCC_ERR_INVALID_ARG;
  dcc_scheduled_event_params_t params = *input;
  params.size = sizeof(params); params.version = DCC_SCHEDULED_EVENT_PARAMS_VERSION;
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_create_guild_scheduled_event, client, input->guild_id, &params);
}
static dcc_status_t task9_test_scheduled_modify_params(
    dcc_client_t *client, const dcc_scheduled_event_params_t *input,
    dcc_rest_cb callback, void *user_data) {
  if (input == NULL) return DCC_ERR_INVALID_ARG;
  dcc_scheduled_event_params_t params = *input;
  params.size = sizeof(params); params.version = DCC_SCHEDULED_EVENT_PARAMS_VERSION;
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_modify_guild_scheduled_event, client, input->guild_id,
      input->event_id, &params);
}
static dcc_status_t task9_test_scheduled_users_page(
    dcc_client_t *client, dcc_snowflake_t guild, dcc_snowflake_t event,
    uint8_t limit, dcc_snowflake_t before, dcc_snowflake_t after,
    dcc_rest_cb callback, void *user_data) {
  dcc_rest_scheduled_event_users_query_t query =
      DCC_REST_SCHEDULED_EVENT_USERS_QUERY_INIT;
  query.present = DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_LIMIT |
      DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_WITH_MEMBER;
  query.limit = limit;
  query.with_member = 1U;
  if (before != 0U) { query.present |= DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_BEFORE; query.before = before; }
  if (after != 0U) { query.present |= DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_AFTER; query.after = after; }
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_get_guild_scheduled_event_users, client, guild, event, &query);
}
static dcc_status_t task9_test_stage_create_params(
    dcc_client_t *client, const dcc_stage_instance_params_t *input,
    dcc_rest_cb callback, void *user_data) {
  if (input == NULL) return DCC_ERR_INVALID_ARG;
  dcc_stage_instance_params_t params = *input;
  params.size = sizeof(params); params.version = DCC_STAGE_INSTANCE_PARAMS_VERSION;
  params.present = DCC_STAGE_INSTANCE_PARAMS_PRESENT_CHANNEL_ID |
      DCC_STAGE_INSTANCE_PARAMS_PRESENT_TOPIC |
      DCC_STAGE_INSTANCE_PARAMS_PRESENT_PRIVACY_LEVEL |
      (params.send_start_notification <= 1U
          ? DCC_STAGE_INSTANCE_PARAMS_PRESENT_SEND_START_NOTIFICATION : 0U) |
      (params.guild_scheduled_event_id != 0U
          ? DCC_STAGE_INSTANCE_PARAMS_PRESENT_GUILD_SCHEDULED_EVENT_ID : 0U);
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_create_stage_instance, client, &params);
}
static dcc_status_t task9_test_stage_modify_params(
    dcc_client_t *client, const dcc_stage_instance_params_t *input,
    dcc_rest_cb callback, void *user_data) {
  if (input == NULL) return DCC_ERR_INVALID_ARG;
  dcc_stage_instance_params_t params = *input;
  params.size = sizeof(params); params.version = DCC_STAGE_INSTANCE_PARAMS_VERSION;
  params.present = DCC_STAGE_INSTANCE_PARAMS_PRESENT_PRIVACY_LEVEL;
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_modify_stage_instance, client, input->channel_id, &params);
}
static dcc_status_t task9_test_template_create_params(
    dcc_client_t *client, const dcc_template_params_t *input,
    dcc_rest_cb callback, void *user_data) {
  if (input == NULL) return DCC_ERR_INVALID_ARG;
  dcc_template_params_t params = *input;
  params.size = sizeof(params); params.version = DCC_TEMPLATE_PARAMS_VERSION;
  params.present = DCC_TEMPLATE_PARAMS_PRESENT_NAME |
      (params.description != NULL ? DCC_TEMPLATE_PARAMS_PRESENT_DESCRIPTION : 0U);
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_create_guild_template, client, input->guild_id, &params);
}
static dcc_status_t task9_test_template_modify_params(
    dcc_client_t *client, const dcc_template_params_t *input,
    dcc_rest_cb callback, void *user_data) {
  if (input == NULL) return DCC_ERR_INVALID_ARG;
  dcc_template_params_t params = *input;
  params.size = sizeof(params); params.version = DCC_TEMPLATE_PARAMS_VERSION;
  params.present = (params.name != NULL ? DCC_TEMPLATE_PARAMS_PRESENT_NAME : 0U) |
      (params.description != NULL ? DCC_TEMPLATE_PARAMS_PRESENT_DESCRIPTION : 0U);
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_modify_guild_template, client, input->guild_id, input->code, &params);
}
static dcc_status_t task9_test_emoji_create(
    dcc_client_t *client, dcc_snowflake_t scope,
    const dcc_emoji_params_t *input, int application,
    dcc_rest_cb callback, void *user_data) {
  if (input == NULL) return DCC_ERR_INVALID_ARG;
  dcc_emoji_params_t params = *input;
  params.size = sizeof(params); params.version = DCC_EMOJI_PARAMS_VERSION;
  params.present = (params.name != NULL ? DCC_EMOJI_PARAMS_PRESENT_NAME : 0U) |
      (params.image != NULL ? DCC_EMOJI_PARAMS_PRESENT_IMAGE : 0U) |
      (!application ? DCC_EMOJI_PARAMS_PRESENT_ROLES : 0U);
  if (application) {
    DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
        dcc_rest_create_application_emoji, client, scope, &params);
  }
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_create_guild_emoji, client, scope, &params);
}
static dcc_status_t task9_test_guild_emoji_create_params(
    dcc_client_t *client, dcc_snowflake_t guild, const dcc_emoji_params_t *params,
    dcc_rest_cb callback, void *user_data) {
  return task9_test_emoji_create(client, guild, params, 0, callback, user_data);
}
static dcc_status_t task9_test_application_emoji_create_params(
    dcc_client_t *client, dcc_snowflake_t app, const dcc_emoji_params_t *params,
    dcc_rest_cb callback, void *user_data) {
  return task9_test_emoji_create(client, app, params, 1, callback, user_data);
}
static dcc_status_t task9_test_guild_emoji_modify_params(
    dcc_client_t *client, dcc_snowflake_t guild, const dcc_emoji_params_t *input,
    dcc_rest_cb callback, void *user_data) {
  if (input == NULL || input->emoji_id == 0U) return DCC_ERR_INVALID_ARG;
  dcc_emoji_params_t params = *input;
  params.size = sizeof(params); params.version = DCC_EMOJI_PARAMS_VERSION;
  params.present = (params.name != NULL ? DCC_EMOJI_PARAMS_PRESENT_NAME : 0U) |
      DCC_EMOJI_PARAMS_PRESENT_ROLES;
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_modify_guild_emoji, client, guild, input->emoji_id, &params);
}
static dcc_status_t task9_test_application_emoji_modify_params(
    dcc_client_t *client, dcc_snowflake_t app, const dcc_emoji_params_t *input,
    dcc_rest_cb callback, void *user_data) {
  if (input == NULL || input->emoji_id == 0U) return DCC_ERR_INVALID_ARG;
  dcc_emoji_params_t params = *input;
  params.size = sizeof(params); params.version = DCC_EMOJI_PARAMS_VERSION;
  params.present = params.name != NULL ? DCC_EMOJI_PARAMS_PRESENT_NAME : 0U;
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_modify_application_emoji, client, app, input->emoji_id, &params);
}
static dcc_status_t task9_test_message_search_params(
    dcc_client_t *client, dcc_snowflake_t guild,
    const dcc_message_search_params_t *params,
    dcc_rest_cb callback, void *user_data) {
  DCC_TASK9_TEST_LEGACY_RETURN(callback, user_data,
      dcc_rest_guild_message_search, client, guild, params);
}
static dcc_status_t task9_test_message_search_raw(
    dcc_client_t *client, dcc_snowflake_t guild, const char *query,
    dcc_rest_cb callback, void *user_data) {
  char path[128]; snprintf(path, sizeof(path), "/guilds/%llu/messages/search",
      (unsigned long long)guild);
  return task9_test_raw_request(client, DCC_REST_GET, path, query, NULL,
                                callback, user_data);
}
static dcc_status_t task9_test_edit_permissions_raw(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t guild,
    dcc_snowflake_t command, const char *json,
    dcc_rest_cb callback, void *user_data) {
  char path[192]; snprintf(path, sizeof(path),
      "/applications/%llu/guilds/%llu/commands/%llu/permissions",
      (unsigned long long)app, (unsigned long long)guild,
      (unsigned long long)command);
  return task9_test_raw_request(client, DCC_REST_PUT, path, NULL, json,
                                callback, user_data);
}
static dcc_status_t task9_test_delete_all_global(
    dcc_client_t *client, dcc_snowflake_t app,
    dcc_rest_cb callback, void *user_data) {
  return task9_test_bulk_global_raw(client, app, "[]", callback, user_data);
}
static dcc_status_t task9_test_delete_all_guild(
    dcc_client_t *client, dcc_snowflake_t app, dcc_snowflake_t guild,
    dcc_rest_cb callback, void *user_data) {
  return task9_test_bulk_guild_raw(client, app, guild, "[]", callback, user_data);
}

#define TASK9_RAW_SIMPLE3(name_, method_, format_)                              \
static dcc_status_t name_(dcc_client_t *client, dcc_snowflake_t a,             \
    dcc_rest_cb callback, void *user_data) {                                    \
  char path[192]; snprintf(path, sizeof(path), format_, (unsigned long long)a); \
  return task9_test_raw_request(client, method_, path, NULL, NULL, callback, user_data); \
}
#define TASK9_RAW_SIMPLE4(name_, method_, format_)                              \
static dcc_status_t name_(dcc_client_t *client, dcc_snowflake_t a,             \
    dcc_snowflake_t b, dcc_rest_cb callback, void *user_data) {                 \
  char path[192]; snprintf(path, sizeof(path), format_,                         \
      (unsigned long long)a, (unsigned long long)b);                            \
  return task9_test_raw_request(client, method_, path, NULL, NULL, callback, user_data); \
}
#define TASK9_RAW_BODY3(name_, method_, format_)                                \
static dcc_status_t name_(dcc_client_t *client, dcc_snowflake_t a,             \
    const char *json, dcc_rest_cb callback, void *user_data) {                  \
  char path[192]; snprintf(path, sizeof(path), format_, (unsigned long long)a); \
  return task9_test_raw_request(client, method_, path, NULL, json, callback, user_data); \
}
#define TASK9_RAW_BODY4(name_, method_, format_)                                \
static dcc_status_t name_(dcc_client_t *client, dcc_snowflake_t a,             \
    dcc_snowflake_t b, const char *json, dcc_rest_cb callback, void *user_data) { \
  char path[192]; snprintf(path, sizeof(path), format_,                         \
      (unsigned long long)a, (unsigned long long)b);                            \
  return task9_test_raw_request(client, method_, path, NULL, json, callback, user_data); \
}

TASK9_RAW_SIMPLE3(task9_get_onboarding_raw, DCC_REST_GET, "/guilds/%llu/onboarding")
TASK9_RAW_BODY3(task9_modify_onboarding_raw, DCC_REST_PUT, "/guilds/%llu/onboarding")
TASK9_RAW_SIMPLE3(task9_get_automod_rules_raw, DCC_REST_GET, "/guilds/%llu/auto-moderation/rules")
TASK9_RAW_SIMPLE4(task9_get_automod_rule_raw, DCC_REST_GET, "/guilds/%llu/auto-moderation/rules/%llu")
TASK9_RAW_BODY3(task9_create_automod_raw, DCC_REST_POST, "/guilds/%llu/auto-moderation/rules")
TASK9_RAW_BODY4(task9_modify_automod_raw, DCC_REST_PATCH, "/guilds/%llu/auto-moderation/rules/%llu")
TASK9_RAW_SIMPLE4(task9_delete_automod_raw, DCC_REST_DELETE, "/guilds/%llu/auto-moderation/rules/%llu")
TASK9_RAW_BODY3(task9_create_scheduled_raw, DCC_REST_POST, "/guilds/%llu/scheduled-events")
TASK9_RAW_BODY4(task9_modify_scheduled_raw, DCC_REST_PATCH, "/guilds/%llu/scheduled-events/%llu")
TASK9_RAW_SIMPLE4(task9_delete_scheduled_raw, DCC_REST_DELETE, "/guilds/%llu/scheduled-events/%llu")
TASK9_RAW_SIMPLE3(task9_get_stage_raw, DCC_REST_GET, "/stage-instances/%llu")
TASK9_RAW_SIMPLE3(task9_delete_stage_raw, DCC_REST_DELETE, "/stage-instances/%llu")
TASK9_RAW_SIMPLE3(task9_get_guild_emojis_raw, DCC_REST_GET, "/guilds/%llu/emojis")
TASK9_RAW_SIMPLE4(task9_get_guild_emoji_raw, DCC_REST_GET, "/guilds/%llu/emojis/%llu")
TASK9_RAW_BODY3(task9_create_guild_emoji_raw, DCC_REST_POST, "/guilds/%llu/emojis")
TASK9_RAW_BODY4(task9_modify_guild_emoji_raw, DCC_REST_PATCH, "/guilds/%llu/emojis/%llu")
TASK9_RAW_SIMPLE4(task9_delete_guild_emoji_raw, DCC_REST_DELETE, "/guilds/%llu/emojis/%llu")
TASK9_RAW_SIMPLE3(task9_get_guild_stickers_raw, DCC_REST_GET, "/guilds/%llu/stickers")
TASK9_RAW_SIMPLE4(task9_get_guild_sticker_raw, DCC_REST_GET, "/guilds/%llu/stickers/%llu")
TASK9_RAW_BODY4(task9_modify_guild_sticker_raw, DCC_REST_PATCH, "/guilds/%llu/stickers/%llu")
TASK9_RAW_SIMPLE4(task9_delete_guild_sticker_raw, DCC_REST_DELETE, "/guilds/%llu/stickers/%llu")
TASK9_RAW_SIMPLE3(task9_get_sticker_raw, DCC_REST_GET, "/stickers/%llu")
TASK9_RAW_SIMPLE3(task9_get_app_emojis_raw, DCC_REST_GET, "/applications/%llu/emojis")
TASK9_RAW_SIMPLE4(task9_get_app_emoji_raw, DCC_REST_GET, "/applications/%llu/emojis/%llu")
TASK9_RAW_BODY3(task9_create_app_emoji_raw, DCC_REST_POST, "/applications/%llu/emojis")
TASK9_RAW_BODY4(task9_modify_app_emoji_raw, DCC_REST_PATCH, "/applications/%llu/emojis/%llu")
TASK9_RAW_SIMPLE4(task9_delete_app_emoji_raw, DCC_REST_DELETE, "/applications/%llu/emojis/%llu")

static dcc_status_t task9_get_scheduled_raw(dcc_client_t *client,
    dcc_snowflake_t guild, const char *query, dcc_rest_cb cb, void *data) {
  char path[128]; snprintf(path, sizeof(path), "/guilds/%llu/scheduled-events",
      (unsigned long long)guild);
  return task9_test_raw_request(client, DCC_REST_GET, path, query, NULL, cb, data);
}
static dcc_status_t task9_get_scheduled_one_raw(dcc_client_t *client,
    dcc_snowflake_t guild, dcc_snowflake_t event, const char *query,
    dcc_rest_cb cb, void *data) {
  char path[160]; snprintf(path, sizeof(path), "/guilds/%llu/scheduled-events/%llu",
      (unsigned long long)guild, (unsigned long long)event);
  return task9_test_raw_request(client, DCC_REST_GET, path, query, NULL, cb, data);
}
static dcc_status_t task9_get_scheduled_users_raw(dcc_client_t *client,
    dcc_snowflake_t guild, dcc_snowflake_t event, const char *query,
    dcc_rest_cb cb, void *data) {
  char path[192]; snprintf(path, sizeof(path),
      "/guilds/%llu/scheduled-events/%llu/users",
      (unsigned long long)guild, (unsigned long long)event);
  return task9_test_raw_request(client, DCC_REST_GET, path, query, NULL, cb, data);
}
static dcc_status_t task9_create_stage_raw(dcc_client_t *client,
    const char *json, dcc_rest_cb cb, void *data) {
  return task9_test_raw_request(client, DCC_REST_POST, "/stage-instances",
                                NULL, json, cb, data);
}
static dcc_status_t task9_modify_stage_raw(dcc_client_t *client,
    dcc_snowflake_t channel, const char *json, dcc_rest_cb cb, void *data) {
  char path[96]; snprintf(path, sizeof(path), "/stage-instances/%llu",
      (unsigned long long)channel);
  return task9_test_raw_request(client, DCC_REST_PATCH, path, NULL, json, cb, data);
}
static dcc_status_t task9_get_template_raw(dcc_client_t *client,
    const char *code, dcc_rest_cb cb, void *data) {
  char *escaped = NULL; char path[448];
  dcc_status_t status = dcc_rest_escape_path_segment(code, &escaped);
  if (status != DCC_OK) return status;
  snprintf(path, sizeof(path), "/guilds/templates/%s", escaped);
  status = task9_test_raw_request(client, DCC_REST_GET, path, NULL, NULL, cb, data);
  free(escaped);
  return status;
}
static dcc_status_t task9_get_guild_templates_raw(dcc_client_t *client,
    dcc_snowflake_t guild, dcc_rest_cb cb, void *data) {
  char path[96]; snprintf(path, sizeof(path), "/guilds/%llu/templates",
      (unsigned long long)guild);
  return task9_test_raw_request(client, DCC_REST_GET, path, NULL, NULL, cb, data);
}
static dcc_status_t task9_create_template_raw(dcc_client_t *client,
    dcc_snowflake_t guild, const char *json, dcc_rest_cb cb, void *data) {
  char path[96]; snprintf(path, sizeof(path), "/guilds/%llu/templates",
      (unsigned long long)guild);
  return task9_test_raw_request(client, DCC_REST_POST, path, NULL, json, cb, data);
}
static dcc_status_t task9_template_code_raw(dcc_client_t *client,
    dcc_snowflake_t guild, const char *code, const char *json,
    dcc_rest_method_t method, dcc_rest_cb cb, void *data) {
  char *escaped = NULL; char path[512];
  dcc_status_t status = dcc_rest_escape_path_segment(code, &escaped);
  if (status != DCC_OK) return status;
  snprintf(path, sizeof(path), "/guilds/%llu/templates/%s",
      (unsigned long long)guild, escaped);
  status = task9_test_raw_request(client, method, path, NULL, json, cb, data);
  free(escaped);
  return status;
}
static dcc_status_t task9_modify_template_raw(dcc_client_t *client,
    dcc_snowflake_t guild, const char *code, const char *json,
    dcc_rest_cb cb, void *data) {
  return task9_template_code_raw(client, guild, code, json, DCC_REST_PATCH, cb, data);
}
static dcc_status_t task9_delete_template_raw(dcc_client_t *client,
    dcc_snowflake_t guild, const char *code, dcc_rest_cb cb, void *data) {
  return task9_template_code_raw(client, guild, code, NULL, DCC_REST_DELETE, cb, data);
}
static dcc_status_t task9_get_sticker_packs_raw(dcc_client_t *client,
    dcc_rest_cb cb, void *data) {
  return task9_test_raw_request(client, DCC_REST_GET, "/sticker-packs",
                                NULL, NULL, cb, data);
}
static dcc_status_t task9_sync_template_raw(dcc_client_t *client,
    dcc_snowflake_t guild, const char *code, dcc_rest_cb cb, void *data) {
  return task9_template_code_raw(client, guild, code, NULL, DCC_REST_PUT, cb, data);
}

#undef TASK9_RAW_SIMPLE3
#undef TASK9_RAW_SIMPLE4
#undef TASK9_RAW_BODY3
#undef TASK9_RAW_BODY4

#define dcc_rest_create_global_command_builder task9_test_create_global_builder
#define dcc_rest_edit_global_command_builder task9_test_edit_global_builder
#define dcc_rest_bulk_overwrite_global_commands_builder task9_test_bulk_global_builder
#define dcc_rest_create_guild_command_builder task9_test_create_guild_builder
#define dcc_rest_edit_guild_command_builder task9_test_edit_guild_builder
#define dcc_rest_bulk_overwrite_guild_commands_builder task9_test_bulk_guild_builder
#define dcc_rest_create_application_command_builder task9_test_create_application_builder
#define dcc_rest_bulk_overwrite_application_commands_builder task9_test_bulk_application_builder
#define dcc_rest_bulk_overwrite_global_commands task9_test_bulk_global_raw
#define dcc_rest_bulk_overwrite_guild_commands task9_test_bulk_guild_raw
#define dcc_rest_get_global_commands task9_test_get_global_raw
#define dcc_rest_get_global_command task9_test_get_global_one_raw
#define dcc_rest_create_global_command task9_test_create_global_raw
#define dcc_rest_edit_global_command task9_test_edit_global_raw
#define dcc_rest_delete_global_command task9_test_delete_global_raw
#define dcc_rest_get_guild_commands task9_test_get_guild_raw
#define dcc_rest_get_guild_command task9_test_get_guild_one_raw
#define dcc_rest_create_guild_command task9_test_create_guild_raw
#define dcc_rest_edit_guild_command task9_test_edit_guild_raw
#define dcc_rest_delete_guild_command task9_test_delete_guild_raw
#define dcc_rest_get_all_guild_command_permissions task9_test_get_all_permissions_raw
#define dcc_rest_get_guild_command_permissions task9_test_get_permissions_raw
#define dcc_rest_get_global_commands_with_localizations task9_test_get_global_localized
#define dcc_rest_get_guild_commands_with_localizations task9_test_get_guild_localized
#define dcc_rest_create_application_command task9_test_create_application_raw
#define dcc_rest_bulk_overwrite_application_commands task9_test_bulk_application_raw
#define dcc_rest_delete_all_application_commands task9_test_delete_all_application
#define dcc_rest_edit_guild_command_permissions_params task9_test_edit_permissions_params
#define dcc_rest_modify_guild_onboarding_params task9_test_onboarding_params
#define dcc_rest_create_auto_moderation_rule_params task9_test_automod_create_params
#define dcc_rest_modify_auto_moderation_rule_params task9_test_automod_modify_params
#define dcc_rest_get_guild_scheduled_events_with_user_count task9_test_scheduled_list_count
#define dcc_rest_get_guild_scheduled_event_with_user_count task9_test_scheduled_get_count
#define dcc_rest_create_guild_scheduled_event_params task9_test_scheduled_create_params
#define dcc_rest_modify_guild_scheduled_event_params task9_test_scheduled_modify_params
#define dcc_rest_get_guild_scheduled_event_users_page task9_test_scheduled_users_page
#define dcc_rest_create_stage_instance_params task9_test_stage_create_params
#define dcc_rest_modify_stage_instance_params task9_test_stage_modify_params
#define dcc_rest_create_guild_template_params task9_test_template_create_params
#define dcc_rest_modify_guild_template_params task9_test_template_modify_params
#define dcc_rest_create_guild_emoji_params task9_test_guild_emoji_create_params
#define dcc_rest_modify_guild_emoji_params task9_test_guild_emoji_modify_params
#define dcc_rest_create_application_emoji_params task9_test_application_emoji_create_params
#define dcc_rest_modify_application_emoji_params task9_test_application_emoji_modify_params
#define dcc_rest_guild_message_search_params task9_test_message_search_params
#define dcc_rest_guild_message_search task9_test_message_search_raw
#define dcc_rest_edit_guild_command_permissions task9_test_edit_permissions_raw
#define dcc_rest_delete_all_global_commands task9_test_delete_all_global
#define dcc_rest_delete_all_guild_commands task9_test_delete_all_guild
#define dcc_rest_get_guild_onboarding task9_get_onboarding_raw
#define dcc_rest_modify_guild_onboarding task9_modify_onboarding_raw
#define dcc_rest_get_auto_moderation_rules task9_get_automod_rules_raw
#define dcc_rest_get_auto_moderation_rule task9_get_automod_rule_raw
#define dcc_rest_create_auto_moderation_rule task9_create_automod_raw
#define dcc_rest_modify_auto_moderation_rule task9_modify_automod_raw
#define dcc_rest_delete_auto_moderation_rule task9_delete_automod_raw
#define dcc_rest_get_guild_scheduled_events task9_get_scheduled_raw
#define dcc_rest_get_guild_scheduled_event task9_get_scheduled_one_raw
#define dcc_rest_create_guild_scheduled_event task9_create_scheduled_raw
#define dcc_rest_modify_guild_scheduled_event task9_modify_scheduled_raw
#define dcc_rest_delete_guild_scheduled_event task9_delete_scheduled_raw
#define dcc_rest_get_guild_scheduled_event_users task9_get_scheduled_users_raw
#define dcc_rest_create_stage_instance task9_create_stage_raw
#define dcc_rest_get_stage_instance task9_get_stage_raw
#define dcc_rest_modify_stage_instance task9_modify_stage_raw
#define dcc_rest_delete_stage_instance task9_delete_stage_raw
#define dcc_rest_get_template task9_get_template_raw
#define dcc_rest_get_guild_templates task9_get_guild_templates_raw
#define dcc_rest_create_guild_template task9_create_template_raw
#define dcc_rest_modify_guild_template task9_modify_template_raw
#define dcc_rest_delete_guild_template task9_delete_template_raw
#define dcc_rest_sync_guild_template task9_sync_template_raw
#define dcc_rest_get_guild_emojis task9_get_guild_emojis_raw
#define dcc_rest_get_guild_emoji task9_get_guild_emoji_raw
#define dcc_rest_create_guild_emoji task9_create_guild_emoji_raw
#define dcc_rest_modify_guild_emoji task9_modify_guild_emoji_raw
#define dcc_rest_delete_guild_emoji task9_delete_guild_emoji_raw
#define dcc_rest_get_guild_stickers task9_get_guild_stickers_raw
#define dcc_rest_get_guild_sticker task9_get_guild_sticker_raw
#define dcc_rest_modify_guild_sticker task9_modify_guild_sticker_raw
#define dcc_rest_delete_guild_sticker task9_delete_guild_sticker_raw
#define dcc_rest_get_sticker task9_get_sticker_raw
#define dcc_rest_get_sticker_packs task9_get_sticker_packs_raw
#define dcc_rest_get_application_emojis task9_get_app_emojis_raw
#define dcc_rest_get_application_emoji task9_get_app_emoji_raw
#define dcc_rest_create_application_emoji task9_create_app_emoji_raw
#define dcc_rest_modify_application_emoji task9_modify_app_emoji_raw
#define dcc_rest_delete_application_emoji task9_delete_app_emoji_raw

#endif
