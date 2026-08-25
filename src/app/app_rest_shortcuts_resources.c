#include <dcc/app.h>

#include <dcc/rest/channels/actions.h>
#include <dcc/rest/channels/core.h>
#include <dcc/rest/channels/invites.h>
#include <dcc/rest/channels/permissions.h>
#include <dcc/rest/channels/positions.h>
#include <dcc/rest/emojis_stickers/guild_emojis.h>
#include <dcc/rest/emojis_stickers/guild_stickers.h>
#include <dcc/rest/emojis_stickers/message_search.h>
#include <dcc/rest/emojis_stickers/stickers.h>
#include <dcc/rest/guild_members/lifecycle.h>
#include <dcc/rest/guild_members/query.h>
#include <dcc/rest/guild_members/roles.h>
#include <dcc/rest/guilds/audit_log.h>
#include <dcc/rest/guilds/auto_moderation.h>
#include <dcc/rest/guilds/bans.h>
#include <dcc/rest/guilds/current_member.h>
#include <dcc/rest/guilds/integrations.h>
#include <dcc/rest/guilds/onboarding.h>
#include <dcc/rest/guilds/prune.h>
#include <dcc/rest/guilds/scheduled_events.h>
#include <dcc/rest/guilds/welcome_screen.h>
#include <dcc/rest/guilds/widgets.h>
#include <dcc/rest/invites.h>
#include <dcc/rest/messages/edit.h>
#include <dcc/rest/messages/pins.h>
#include <dcc/rest/messages/reactions.h>
#include <dcc/rest/resources/entitlements.h>
#include <dcc/rest/resources/gateway.h>
#include <dcc/rest/resources/group_dms.h>
#include <dcc/rest/resources/invites.h>
#include <dcc/rest/resources/stage_instances.h>
#include <dcc/rest/resources/templates.h>
#include <dcc/rest/resources/users.h>
#include <dcc/rest/resources/voice_states.h>
#include <dcc/rest/response_helpers.h>
#include <dcc/rest/roles.h>
#include <dcc/rest/threads/create.h>
#include <dcc/rest/threads/lifecycle.h>
#include <dcc/rest/webhooks/execute.h>
#include <dcc/rest/webhooks/management/create.h>
#include <dcc/rest/webhooks/management/delete.h>
#include <dcc/rest/webhooks/management/fetch.h>
#include <dcc/rest/webhooks/management/modify.h>
#include <dcc/rest/webhooks/messages/delete.h>
#include <dcc/rest/webhooks/messages/edit.h>
#include <dcc/rest/webhooks/messages/fetch.h>

#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task9_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dcc_app_direct_message_state {
  dcc_client_t *client;
  char *json_body;
  dcc_rest_cb callback;
  void *user_data;
} dcc_app_direct_message_state_t;

static void dcc_app_direct_message_result(
    dcc_client_t *client, const dcc_rest_result_t *result, void *user_data);

static void dcc_app_direct_message_finish(dcc_app_direct_message_state_t *state,
                                          const dcc_rest_result_t *result,
                                          dcc_status_t local_status) {
  if (state == NULL) {
    return;
  }
  dcc_rest_response_t response = {
      .size = sizeof(response),
      .status = result != NULL ? result->http_status : 0U,
      .error = result != NULL ? dcc_rest_result_status(result) : local_status,
      .body = result != NULL ? result->body : NULL,
      .body_len = result != NULL ? result->body_len : 0U,
  };
  if (state->callback != NULL) {
    state->callback(state->client, &response, state->user_data);
  }
  free(state->json_body);
  free(state);
}

static void dcc_app_direct_message_created(dcc_client_t *client,
                                           const dcc_rest_result_t *result,
                                           void *user_data) {
  (void)client;
  dcc_app_direct_message_state_t *state =
      (dcc_app_direct_message_state_t *)user_data;
  dcc_status_t status = dcc_rest_result_status(result);
  dcc_snowflake_t channel_id = 0U;
  if (status == DCC_OK) {
    dcc_rest_response_t response = {
        .size = sizeof(response),
        .status = result->http_status,
        .error = DCC_OK,
        .body = result->body,
        .body_len = result->body_len,
    };
    status = dcc_rest_response_snowflake_field(&response, "id", &channel_id);
  }
  if (status != DCC_OK) {
    dcc_app_direct_message_finish(state, result, status);
    return;
  }

  char path[80];
  int length = snprintf(path, sizeof(path), "/channels/%llu/messages",
                        (unsigned long long)channel_id);
  if (length < 0 || (size_t)length >= sizeof(path)) {
    dcc_app_direct_message_finish(state, NULL, DCC_ERR_INVALID_ARG);
    return;
  }
  dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
  options.callback = dcc_app_direct_message_result;
  options.user_data = state;
  dcc_rest_request_desc_t description = DCC_REST_REQUEST_DESC_INIT;
  description.method = DCC_REST_POST;
  description.path = path;
  description.content_type = "application/json";
  description.body = state->json_body;
  description.body_len = strlen(state->json_body);
  description.options = &options;
  status = dcc_rest_submit(state->client, &description, NULL);
  if (status != DCC_OK) {
    dcc_app_direct_message_finish(state, NULL, status);
  }
}

static void dcc_app_direct_message_result(dcc_client_t *client,
                                          const dcc_rest_result_t *result,
                                          void *user_data) {
  (void)client;
  dcc_app_direct_message_finish(
      (dcc_app_direct_message_state_t *)user_data, result,
      result != NULL ? dcc_rest_result_status(result) : DCC_ERR_RUNTIME);
}

static dcc_status_t dcc_app_direct_message_submit(dcc_client_t *client,
                                                  dcc_snowflake_t user_id,
                                                  const char *json_body,
                                                  dcc_rest_cb callback,
                                                  void *user_data) {
  if (client == NULL || user_id == 0U || json_body == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  dcc_app_direct_message_state_t *state =
      (dcc_app_direct_message_state_t *)calloc(1U, sizeof(*state));
  if (state == NULL) {
    return DCC_ERR_NOMEM;
  }
  size_t body_len = strlen(json_body);
  state->json_body = (char *)malloc(body_len + 1U);
  if (state->json_body == NULL) {
    free(state);
    return DCC_ERR_NOMEM;
  }
  memcpy(state->json_body, json_body, body_len + 1U);
  state->client = client;
  state->callback = callback;
  state->user_data = user_data;

  dcc_dm_channel_params_t params = DCC_DM_CHANNEL_PARAMS_INIT;
  params.recipient_id = user_id;
  dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
  options.callback = dcc_app_direct_message_created;
  options.user_data = state;
  dcc_status_t status =
      dcc_rest_create_dm_channel(client, &params, &options, NULL);
  if (status != DCC_OK) {
    free(state->json_body);
    free(state);
  }
  return status;
}

static int dcc_app_string_invalid(const char *value) {
  return value == NULL || value[0] == '\0';
}

static dcc_status_t dcc_app_submit_json_path(
    dcc_app_t *app, dcc_rest_method_t method, const char *path,
    const char *json_body, dcc_rest_cb cb, void *user_data) {
  if (app == NULL || path == NULL || json_body == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  return dcc_endpoint_submit_legacy_raw(
      dcc_app_client(app), method, path, NULL, "application/json", json_body,
      strlen(json_body), cb, user_data);
}

dcc_status_t dcc_app_get_current_application(dcc_app_t *app, dcc_rest_cb cb,
                                             void *user_data) {
  if (app == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_current_application,
                             dcc_app_client(app));
}

dcc_status_t dcc_app_get_gateway_bot(dcc_app_t *app, dcc_rest_cb cb,
                                     void *user_data) {
  if (app == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_gateway_bot,
                             dcc_app_client(app));
}

dcc_status_t dcc_app_get_skus(dcc_app_t *app, dcc_snowflake_t application_id,
                              dcc_rest_cb cb, void *user_data) {
  if (app == NULL || application_id == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_skus,
                             dcc_app_client(app), application_id);
}

dcc_status_t dcc_app_get_entitlements(dcc_app_t *app,
                                      dcc_snowflake_t application_id,
                                      const char *query, dcc_rest_cb cb,
                                      void *user_data) {
  if (app == NULL || application_id == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  char path[256];
  int length = snprintf(path, sizeof(path), "/applications/%llu/entitlements%s",
                        (unsigned long long)application_id,
                        query != NULL ? query : "");
  return length >= 0 && (size_t)length < sizeof(path)
             ? dcc_endpoint_submit_legacy_raw(
                   dcc_app_client(app), DCC_REST_GET, path, NULL, NULL, NULL,
                   0U, cb, user_data)
             : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_app_get_entitlements_page(
    dcc_app_t *app, dcc_snowflake_t application_id, dcc_snowflake_t user_id,
    const dcc_snowflake_t *sku_ids, size_t sku_id_count,
    dcc_snowflake_t before_id, dcc_snowflake_t after_id, uint8_t limit,
    dcc_snowflake_t guild_id, uint8_t exclude_ended, dcc_rest_cb cb,
    void *user_data) {
  if (app == NULL || application_id == 0U ||
      (sku_ids == NULL && sku_id_count > 0U)) {
    return DCC_ERR_INVALID_ARG;
  }
  dcc_rest_entitlement_query_t query = DCC_REST_ENTITLEMENT_QUERY_INIT;
  if (user_id != 0U) {
    query.present |= DCC_REST_ENTITLEMENT_QUERY_PRESENT_USER_ID;
    query.user_id = user_id;
  }
  if (sku_id_count != 0U) {
    query.present |= DCC_REST_ENTITLEMENT_QUERY_PRESENT_SKU_IDS;
    query.sku_ids = sku_ids;
    query.sku_id_count = sku_id_count;
  }
  if (before_id != 0U) {
    query.present |= DCC_REST_ENTITLEMENT_QUERY_PRESENT_BEFORE;
    query.before = before_id;
  }
  if (after_id != 0U) {
    query.present |= DCC_REST_ENTITLEMENT_QUERY_PRESENT_AFTER;
    query.after = after_id;
  }
  if (limit != 0U) {
    query.present |= DCC_REST_ENTITLEMENT_QUERY_PRESENT_LIMIT;
    query.limit = limit;
  }
  if (guild_id != 0U) {
    query.present |= DCC_REST_ENTITLEMENT_QUERY_PRESENT_GUILD_ID;
    query.guild_id = guild_id;
  }
  query.present |= DCC_REST_ENTITLEMENT_QUERY_PRESENT_EXCLUDE_ENDED;
  query.exclude_ended = exclude_ended;
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_entitlements,
                             dcc_app_client(app), application_id, &query);
}

dcc_status_t dcc_app_create_test_entitlement(dcc_app_t *app,
                                             dcc_snowflake_t application_id,
                                             const char *json_body,
                                             dcc_rest_cb cb, void *user_data) {
  if (app == NULL || application_id == 0U || json_body == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  char path[96];
  dcc_status_t status = dcc_rest_format_path(
      path, sizeof(path), "/applications/%llu/entitlements",
      (unsigned long long)application_id);
  return status == DCC_OK
             ? dcc_app_submit_json_path(
                   app, DCC_REST_POST, path, json_body, cb, user_data)
             : status;
}

dcc_status_t dcc_app_create_test_entitlement_params(
    dcc_app_t *app, const dcc_test_entitlement_params_t *params, dcc_rest_cb cb,
    void *user_data) {
  if (app == NULL || params == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_create_test_entitlement,
                             dcc_app_client(app), params->application_id,
                             params);
}

dcc_status_t dcc_app_delete_test_entitlement(dcc_app_t *app,
                                             dcc_snowflake_t application_id,
                                             dcc_snowflake_t entitlement_id,
                                             dcc_rest_cb cb, void *user_data) {
  if (app == NULL || application_id == 0U || entitlement_id == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_delete_test_entitlement,
                             dcc_app_client(app), application_id,
                             entitlement_id);
}

dcc_status_t dcc_app_consume_entitlement(dcc_app_t *app,
                                         dcc_snowflake_t application_id,
                                         dcc_snowflake_t entitlement_id,
                                         dcc_rest_cb cb, void *user_data) {
  if (app == NULL || application_id == 0U || entitlement_id == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_consume_entitlement,
                             dcc_app_client(app), application_id,
                             entitlement_id);
}

dcc_status_t dcc_app_get_current_user(dcc_app_t *app, dcc_rest_cb cb,
                                      void *user_data) {
  if (app == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_current_user,
                             dcc_app_client(app));
}

dcc_status_t dcc_app_modify_current_user(dcc_app_t *app, const char *json_body,
                                         dcc_rest_cb cb, void *user_data) {
  if (app == NULL || json_body == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  return dcc_app_submit_json_path(app, DCC_REST_PATCH, "/users/@me", json_body,
                                  cb, user_data);
}

dcc_status_t dcc_app_modify_current_user_params(dcc_app_t *app,
                                   const dcc_current_user_params_t *params,
                                   dcc_rest_cb cb, void *user_data) {
  if (app == NULL || params == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_modify_current_user,
                             dcc_app_client(app), params);
}

dcc_status_t dcc_app_get_user(dcc_app_t *app, dcc_snowflake_t user_id,
                              dcc_rest_cb cb, void *user_data) {
  if (app == NULL || user_id == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_user,
                             dcc_app_client(app), user_id);
}

dcc_status_t dcc_app_get_current_user_connections(dcc_app_t *app,
                                                  dcc_rest_cb cb,
                                                  void *user_data) {
  if (app == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data,
                             dcc_rest_get_current_user_connections,
                             dcc_app_client(app));
}

dcc_status_t dcc_app_get_current_user_guilds(dcc_app_t *app, const char *query,
                                             dcc_rest_cb cb, void *user_data) {
  if (app == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  return dcc_endpoint_submit_legacy_raw(dcc_app_client(app), DCC_REST_GET,
                                        "/users/@me/guilds", query, NULL, NULL,
                                        0U, cb, user_data);
}

dcc_status_t dcc_app_get_current_user_dms(dcc_app_t *app, dcc_rest_cb cb,
                                          void *user_data) {
  (void)app;
  (void)cb;
  (void)user_data;
  return DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_app_create_dm_channel(dcc_app_t *app, const char *json_body,
                                       dcc_rest_cb cb, void *user_data) {
  if (app == NULL || json_body == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  return dcc_endpoint_submit_legacy_raw(
      dcc_app_client(app), DCC_REST_POST, "/users/@me/channels", NULL,
      "application/json", json_body, strlen(json_body), cb, user_data);
}

dcc_status_t dcc_app_create_dm_channel_params(dcc_app_t *app,
                                 const dcc_dm_channel_params_t *params,
                                 dcc_rest_cb cb, void *user_data) {
  if (app == NULL || params == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_create_dm_channel,
                             dcc_app_client(app), params);
}

dcc_status_t dcc_app_add_group_dm_recipient(dcc_app_t *app,
                                            dcc_snowflake_t channel_id,
                                            dcc_snowflake_t user_id,
                                            const char *json_body,
                                            dcc_rest_cb cb, void *user_data) {
  if (app == NULL || channel_id == 0U || user_id == 0U || json_body == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  char path[96];
  dcc_status_t status = dcc_rest_format_path(
      path, sizeof(path), "/channels/%llu/recipients/%llu",
      (unsigned long long)channel_id, (unsigned long long)user_id);
  return status == DCC_OK
             ? dcc_app_submit_json_path(
                   app, DCC_REST_PUT, path, json_body, cb, user_data)
             : status;
}

dcc_status_t dcc_app_add_group_dm_recipient_params(
    dcc_app_t *app, const dcc_group_dm_recipient_params_t *params,
    dcc_rest_cb cb, void *user_data) {
  if (app == NULL || params == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_add_group_dm_recipient,
                             dcc_app_client(app), params->channel_id,
                             params->user_id, params);
}

dcc_status_t dcc_app_remove_group_dm_recipient(dcc_app_t *app,
                                               dcc_snowflake_t channel_id,
                                               dcc_snowflake_t user_id,
                                               dcc_rest_cb cb,
                                               void *user_data) {
  if (app == NULL || channel_id == 0U || user_id == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data,
                             dcc_rest_remove_group_dm_recipient,
                             dcc_app_client(app), channel_id, user_id);
}

dcc_status_t dcc_app_create_direct_message(dcc_app_t *app,
                                           dcc_snowflake_t user_id,
                                           const char *json_body,
                                           dcc_rest_cb cb, void *user_data) {
  if (app == NULL || user_id == 0U || json_body == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  return dcc_app_direct_message_submit(dcc_app_client(app), user_id, json_body,
                                       cb, user_data);
}

dcc_status_t dcc_app_create_direct_message_builder(dcc_app_t *app, dcc_snowflake_t user_id,
                                      const dcc_message_builder_t *message,
                                      dcc_rest_cb cb, void *user_data) {
  if (app == NULL || user_id == 0U || message == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  char *json = NULL;
  dcc_status_t status = dcc_message_builder_build_json(message, &json);
  if (status == DCC_OK) {
    status = dcc_app_direct_message_submit(dcc_app_client(app), user_id, json,
                                           cb, user_data);
  }
  dcc_message_builder_json_free(json);
  return status;
}

dcc_status_t dcc_app_create_direct_message_text(dcc_app_t *app,
                                                dcc_snowflake_t user_id,
                                                const char *content,
                                                dcc_rest_cb cb,
                                                void *user_data) {
  if (content == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  dcc_message_builder_t message = {
      .size = sizeof(message),
      .version = DCC_MESSAGE_BUILDER_VERSION,
      .present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT,
      .content = content,
  };
  return dcc_app_create_direct_message_builder(app, user_id, &message, cb,
                                               user_data);
}

dcc_status_t dcc_app_leave_guild(dcc_app_t *app, dcc_snowflake_t guild_id,
                                 dcc_rest_cb cb, void *user_data) {
  if (app == NULL || guild_id == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_leave_guild,
                             dcc_app_client(app), guild_id);
}

dcc_status_t dcc_app_get_guild_invites(dcc_app_t *app, dcc_snowflake_t guild_id,
                                       dcc_rest_cb cb, void *user_data) {
  if (app == NULL || guild_id == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_guild_invites,
                             dcc_app_client(app), guild_id);
}

dcc_status_t dcc_app_get_invite(dcc_app_t *app, const char *invite_code,
                                const char *query, dcc_rest_cb cb,
                                void *user_data) {
  if (app == NULL || dcc_app_string_invalid(invite_code)) {
    return DCC_ERR_INVALID_ARG;
  }
  (void)query;
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_invite,
                             dcc_app_client(app), invite_code, NULL);
}

dcc_status_t dcc_app_get_invite_full(dcc_app_t *app, const char *invite_code,
                                     dcc_rest_cb cb, void *user_data) {
  if (app == NULL || dcc_app_string_invalid(invite_code)) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_invite,
                             dcc_app_client(app), invite_code, NULL);
}

dcc_status_t dcc_app_delete_invite(dcc_app_t *app, const char *invite_code,
                                   dcc_rest_cb cb, void *user_data) {
  if (app == NULL || dcc_app_string_invalid(invite_code)) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_delete_invite,
                             dcc_app_client(app), invite_code);
}

dcc_status_t dcc_app_create_stage_instance(dcc_app_t *app,
                                           const char *json_body,
                                           dcc_rest_cb cb, void *user_data) {
  if (app == NULL || json_body == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  return dcc_app_submit_json_path(app, DCC_REST_POST, "/stage-instances",
                                  json_body, cb, user_data);
}

dcc_status_t dcc_app_create_stage_instance_params(dcc_app_t *app,
                                     const dcc_stage_instance_params_t *params,
                                     dcc_rest_cb cb, void *user_data) {
  if (app == NULL || params == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_create_stage_instance,
                             dcc_app_client(app), params);
}

dcc_status_t dcc_app_get_stage_instance(dcc_app_t *app,
                                        dcc_snowflake_t channel_id,
                                        dcc_rest_cb cb, void *user_data) {
  if (app == NULL || channel_id == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_stage_instance,
                             dcc_app_client(app), channel_id);
}

dcc_status_t dcc_app_modify_stage_instance(dcc_app_t *app,
                                           dcc_snowflake_t channel_id,
                                           const char *json_body,
                                           dcc_rest_cb cb, void *user_data) {
  if (app == NULL || channel_id == 0U || json_body == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  char path[80];
  dcc_status_t status = dcc_rest_format_path(
      path, sizeof(path), "/stage-instances/%llu",
      (unsigned long long)channel_id);
  return status == DCC_OK
             ? dcc_app_submit_json_path(
                   app, DCC_REST_PATCH, path, json_body, cb, user_data)
             : status;
}

dcc_status_t dcc_app_modify_stage_instance_params(dcc_app_t *app,
                                     const dcc_stage_instance_params_t *params,
                                     dcc_rest_cb cb, void *user_data) {
  if (app == NULL || params == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  if (params->channel_id == 0U)
    return DCC_ERR_INVALID_ARG;
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_modify_stage_instance,
                             dcc_app_client(app), params->channel_id, params);
}

dcc_status_t dcc_app_delete_stage_instance(dcc_app_t *app,
                                           dcc_snowflake_t channel_id,
                                           dcc_rest_cb cb, void *user_data) {
  if (app == NULL || channel_id == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_delete_stage_instance,
                             dcc_app_client(app), channel_id);
}

dcc_status_t dcc_app_get_template(dcc_app_t *app, const char *code,
                                  dcc_rest_cb cb, void *user_data) {
  if (app == NULL || dcc_app_string_invalid(code)) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_template,
                             dcc_app_client(app), code);
}

dcc_status_t dcc_app_create_guild_from_template(dcc_app_t *app,
                                                const char *code,
                                                const char *json_body,
                                                dcc_rest_cb cb,
                                                void *user_data) {
  if (app == NULL || dcc_app_string_invalid(code) || json_body == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  char *escaped = NULL;
  dcc_status_t status = dcc_rest_escape_path_segment(code, &escaped);
  char path[192];
  if (status == DCC_OK) {
    status = dcc_rest_format_path(path, sizeof(path), "/guilds/templates/%s",
                                  escaped);
  }
  free(escaped);
  return status == DCC_OK
             ? dcc_app_submit_json_path(
                   app, DCC_REST_POST, path, json_body, cb, user_data)
             : status;
}

dcc_status_t dcc_app_create_guild_from_template_params(dcc_app_t *app,
                                          const dcc_template_params_t *params,
                                          dcc_rest_cb cb, void *user_data) {
  if (app == NULL || params == NULL || dcc_app_string_invalid(params->code)) {
    return DCC_ERR_INVALID_ARG;
  }
  char *json = NULL;
  dcc_status_t status = dcc_task9_template_body(params, 1, &json);
  if (status == DCC_OK) {
    status = dcc_app_create_guild_from_template(
        app, params->code, json, cb, user_data);
  }
  free(json);
  return status;
}

dcc_status_t dcc_app_get_guild_templates(dcc_app_t *app,
                                         dcc_snowflake_t guild_id,
                                         dcc_rest_cb cb, void *user_data) {
  if (app == NULL || guild_id == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_get_guild_templates,
                             dcc_app_client(app), guild_id);
}

dcc_status_t dcc_app_create_guild_template(dcc_app_t *app,
                                           dcc_snowflake_t guild_id,
                                           const char *json_body,
                                           dcc_rest_cb cb, void *user_data) {
  if (app == NULL || guild_id == 0U || json_body == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  char path[96];
  dcc_status_t status = dcc_rest_format_path(
      path, sizeof(path), "/guilds/%llu/templates",
      (unsigned long long)guild_id);
  return status == DCC_OK
             ? dcc_app_submit_json_path(
                   app, DCC_REST_POST, path, json_body, cb, user_data)
             : status;
}

dcc_status_t dcc_app_create_guild_template_params(dcc_app_t *app,
                                     const dcc_template_params_t *params,
                                     dcc_rest_cb cb, void *user_data) {
  if (app == NULL || params == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  if (params->guild_id == 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_create_guild_template,
                             dcc_app_client(app), params->guild_id, params);
}

dcc_status_t dcc_app_modify_guild_template(dcc_app_t *app,
                                           dcc_snowflake_t guild_id,
                                           const char *code,
                                           const char *json_body,
                                           dcc_rest_cb cb, void *user_data) {
  if (app == NULL || guild_id == 0U || dcc_app_string_invalid(code) ||
      json_body == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  char *escaped = NULL;
  dcc_status_t status = dcc_rest_escape_path_segment(code, &escaped);
  char path[192];
  if (status == DCC_OK) {
    status = dcc_rest_format_path(
        path, sizeof(path), "/guilds/%llu/templates/%s",
        (unsigned long long)guild_id, escaped);
  }
  free(escaped);
  return status == DCC_OK
             ? dcc_app_submit_json_path(
                   app, DCC_REST_PATCH, path, json_body, cb, user_data)
             : status;
}

dcc_status_t dcc_app_modify_guild_template_params(dcc_app_t *app,
                                     const dcc_template_params_t *params,
                                     dcc_rest_cb cb, void *user_data) {
  if (app == NULL || params == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  if (params->guild_id == 0U || dcc_app_string_invalid(params->code)) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_modify_guild_template,
                             dcc_app_client(app), params->guild_id,
                             params->code, params);
}

dcc_status_t dcc_app_sync_guild_template(dcc_app_t *app,
                                         dcc_snowflake_t guild_id,
                                         const char *code, dcc_rest_cb cb,
                                         void *user_data) {
  if (app == NULL || guild_id == 0U || dcc_app_string_invalid(code)) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_sync_guild_template,
                             dcc_app_client(app), guild_id, code);
}

dcc_status_t dcc_app_delete_guild_template(dcc_app_t *app,
                                           dcc_snowflake_t guild_id,
                                           const char *code, dcc_rest_cb cb,
                                           void *user_data) {
  if (app == NULL || guild_id == 0U || dcc_app_string_invalid(code)) {
    return DCC_ERR_INVALID_ARG;
  }
  DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_delete_guild_template,
                             dcc_app_client(app), guild_id, code);
}
