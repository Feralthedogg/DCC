#include <dcc/rest.h>

#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "rest_v2_endpoint_smoke_support.h"
#include "rest_v2_task7_sensitive_probe.h"

#include <math.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define DCC_EXPECT_TASK8_SIGNATURE(name_, ...)                                 \
  typedef dcc_status_t (*name_##_task8_signature_t)(__VA_ARGS__);              \
  _Static_assert(_Generic(&(name_), name_##_task8_signature_t: 1, default: 0), \
                 #name_ " does not match the exact Task 8 request signature")

DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_begin_guild_prune, dcc_client_t *,
                           dcc_snowflake_t, const dcc_rest_guild_prune_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_create_guild_ban, dcc_client_t *,
                           dcc_snowflake_t, dcc_snowflake_t,
                           const dcc_rest_guild_ban_create_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_create_guild_soundboard_sound,
                           dcc_client_t *, dcc_snowflake_t,
                           const dcc_rest_guild_soundboard_sound_create_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_delete_guild_ban, dcc_client_t *,
                           dcc_snowflake_t, dcc_snowflake_t,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_delete_guild_integration, dcc_client_t *,
                           dcc_snowflake_t, dcc_snowflake_t,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_delete_guild_soundboard_sound,
                           dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_current_user_guilds, dcc_client_t *,
                           const dcc_rest_current_user_guilds_query_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild, dcc_client_t *, dcc_snowflake_t,
                           const dcc_rest_guild_query_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_audit_log, dcc_client_t *,
                           dcc_snowflake_t,
                           const dcc_rest_guild_audit_log_query_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_ban, dcc_client_t *,
                           dcc_snowflake_t, dcc_snowflake_t,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_bans, dcc_client_t *,
                           dcc_snowflake_t, const dcc_rest_guild_bans_query_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_integrations, dcc_client_t *,
                           dcc_snowflake_t, const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_preview, dcc_client_t *,
                           dcc_snowflake_t, const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_prune_count, dcc_client_t *,
                           dcc_snowflake_t,
                           const dcc_rest_guild_prune_query_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_soundboard_sound, dcc_client_t *,
                           dcc_snowflake_t, dcc_snowflake_t,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_soundboard_sounds, dcc_client_t *,
                           dcc_snowflake_t, const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_vanity_url, dcc_client_t *,
                           dcc_snowflake_t, const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_voice_regions, dcc_client_t *,
                           dcc_snowflake_t, const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_welcome_screen, dcc_client_t *,
                           dcc_snowflake_t, const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_widget, dcc_client_t *,
                           dcc_snowflake_t, const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_widget_json, dcc_client_t *,
                           dcc_snowflake_t, const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_widget_png, dcc_client_t *,
                           dcc_snowflake_t,
                           const dcc_rest_guild_widget_image_query_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_leave_guild, dcc_client_t *,
                           dcc_snowflake_t, const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_guild, dcc_client_t *,
                           dcc_snowflake_t, const dcc_rest_guild_update_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_guild_incident_actions,
                           dcc_client_t *, dcc_snowflake_t,
                           const dcc_rest_guild_incident_actions_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_guild_soundboard_sound,
                           dcc_client_t *, dcc_snowflake_t, dcc_snowflake_t,
                           const dcc_rest_guild_soundboard_sound_update_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_guild_welcome_screen, dcc_client_t *,
                           dcc_snowflake_t,
                           const dcc_rest_guild_welcome_screen_update_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_guild_widget, dcc_client_t *,
                           dcc_snowflake_t,
                           const dcc_rest_guild_widget_update_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);

DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_add_guild_member, dcc_client_t *,
                           dcc_snowflake_t, dcc_snowflake_t,
                           const dcc_rest_guild_member_add_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_add_guild_member_role, dcc_client_t *,
                           dcc_snowflake_t, dcc_snowflake_t, dcc_snowflake_t,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_bulk_ban_guild_members, dcc_client_t *,
                           dcc_snowflake_t, const dcc_rest_guild_bulk_ban_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_current_user_guild_member,
                           dcc_client_t *, dcc_snowflake_t,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_member, dcc_client_t *,
                           dcc_snowflake_t, dcc_snowflake_t,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_list_guild_members, dcc_client_t *,
                           dcc_snowflake_t,
                           const dcc_rest_guild_members_query_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_current_guild_member, dcc_client_t *,
                           dcc_snowflake_t,
                           const dcc_rest_current_guild_member_update_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_guild_member, dcc_client_t *,
                           dcc_snowflake_t, dcc_snowflake_t,
                           const dcc_rest_guild_member_update_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_remove_guild_member, dcc_client_t *,
                           dcc_snowflake_t, dcc_snowflake_t,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_remove_guild_member_role, dcc_client_t *,
                           dcc_snowflake_t, dcc_snowflake_t, dcc_snowflake_t,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_search_guild_members, dcc_client_t *,
                           dcc_snowflake_t,
                           const dcc_rest_guild_member_search_query_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_set_current_guild_member_nickname,
                           dcc_client_t *, dcc_snowflake_t,
                           const dcc_rest_current_guild_member_nickname_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);

DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_create_guild_role, dcc_client_t *,
                           dcc_snowflake_t,
                           const dcc_rest_guild_role_create_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_delete_guild_role, dcc_client_t *,
                           dcc_snowflake_t, dcc_snowflake_t,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_role, dcc_client_t *,
                           dcc_snowflake_t, dcc_snowflake_t,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_role_member_counts,
                           dcc_client_t *, dcc_snowflake_t,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_get_guild_roles, dcc_client_t *,
                           dcc_snowflake_t, const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_guild_role, dcc_client_t *,
                           dcc_snowflake_t, dcc_snowflake_t,
                           const dcc_rest_guild_role_update_t *,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);
DCC_EXPECT_TASK8_SIGNATURE(dcc_rest_modify_guild_role_positions, dcc_client_t *,
                           dcc_snowflake_t,
                           const dcc_rest_guild_role_position_t *, size_t,
                           const dcc_rest_call_options_t *,
                           dcc_rest_request_t **);

#define TASK8_EXPECT_CALL(label_, capture_, call_, method_, path_,             \
                          content_type_, body_)                                \
  do {                                                                         \
    dcc_rest_request_t *request = NULL;                                        \
    endpoint_capture_reset((capture_), 0U);                                    \
    atomic_store_explicit(&(capture_)->capture_release, 1U,                    \
                          memory_order_release);                               \
    dcc_status_t submit_status = (call_);                                      \
    const char *body_fragment = (body_);                                       \
    if (endpoint_completed_contract(                                           \
            (label_), (capture_), submit_status, request, (method_), (path_),  \
            (content_type_), body_fragment,                                    \
            body_fragment != NULL ? strlen(body_fragment) : 0U) != 0) {        \
      return 1;                                                                \
    }                                                                          \
  } while (0)

#define TASK8_EXPECT_REJECTION(label_, capture_, callback_, observer_, call_)  \
  do {                                                                         \
    unsigned capture_before =                                                  \
        atomic_load_explicit(&(capture_)->calls, memory_order_acquire);        \
    unsigned callback_before =                                                 \
        atomic_load_explicit(&(callback_)->calls, memory_order_acquire);       \
    unsigned observer_before =                                                 \
        atomic_load_explicit(&(observer_)->calls, memory_order_acquire);       \
    dcc_rest_request_t *request = (dcc_rest_request_t *)(uintptr_t)1U;         \
    dcc_endpoint_test_allocation_probe_begin(0U);                              \
    dcc_status_t rejected = (call_);                                           \
    size_t allocations = dcc_endpoint_test_allocation_probe_end();             \
    if (rejected != DCC_ERR_INVALID_ARG || request != NULL ||                  \
        allocations != 0U ||                                                   \
        atomic_load_explicit(&(capture_)->calls, memory_order_acquire) !=      \
            capture_before ||                                                  \
        atomic_load_explicit(&(callback_)->calls, memory_order_acquire) !=     \
            callback_before ||                                                 \
        atomic_load_explicit(&(observer_)->calls, memory_order_acquire) !=     \
            observer_before) {                                                 \
      fprintf(                                                                 \
          stderr,                                                              \
          "%s rejection status=%s request=%p allocations=%zu "                 \
          "capture=%u/%u callback=%u/%u observer=%u/%u\n",                     \
          (label_), dcc_status_string(rejected), (void *)request, allocations, \
          capture_before,                                                      \
          atomic_load_explicit(&(capture_)->calls, memory_order_acquire),      \
          callback_before,                                                     \
          atomic_load_explicit(&(callback_)->calls, memory_order_acquire),     \
          observer_before,                                                     \
          atomic_load_explicit(&(observer_)->calls, memory_order_acquire));    \
      dcc_rest_request_destroy(request);                                       \
      return 1;                                                                \
    }                                                                          \
  } while (0)

static int task8_wire_contract(dcc_client_t *client,
                               endpoint_capture_t *capture) {
  dcc_rest_guild_query_t guild_query = DCC_REST_GUILD_QUERY_INIT;
  guild_query.present = DCC_REST_GUILD_QUERY_PRESENT_WITH_COUNTS;
  guild_query.with_counts = 0U;
  TASK8_EXPECT_CALL(
      "Task 8 Get Guild explicit false", capture,
      dcc_rest_get_guild(client, 810U, &guild_query, NULL, &request), "GET",
      "/guilds/810?with_counts=false", NULL, NULL);

  dcc_rest_current_user_guilds_query_t user_guilds =
      DCC_REST_CURRENT_USER_GUILDS_QUERY_INIT;
  user_guilds.present = DCC_REST_CURRENT_USER_GUILDS_QUERY_PRESENT_LIMIT |
                        DCC_REST_CURRENT_USER_GUILDS_QUERY_PRESENT_WITH_COUNTS;
  user_guilds.limit = 200U;
  dcc_rest_call_options_t bearer = DCC_REST_CALL_OPTIONS_INIT;
  bearer.auth_mode = DCC_REST_AUTH_BEARER;
  bearer.auth_token = "task8-user-token";
  TASK8_EXPECT_CALL(
      "Task 8 current guilds Bearer", capture,
      dcc_rest_get_current_user_guilds(client, &user_guilds, &bearer, &request),
      "GET", "/users/@me/guilds?limit=200&with_counts=false", NULL, NULL);

  dcc_rest_guild_audit_log_query_t audit = DCC_REST_GUILD_AUDIT_LOG_QUERY_INIT;
  audit.present = DCC_REST_GUILD_AUDIT_LOG_QUERY_PRESENT_AFTER;
  audit.after = 0U;
  TASK8_EXPECT_CALL(
      "Task 8 audit after zero", capture,
      dcc_rest_get_guild_audit_log(client, 811U, &audit, NULL, &request), "GET",
      "/guilds/811/audit-logs?after=0", NULL, NULL);

  dcc_rest_guild_member_search_query_t search =
      DCC_REST_GUILD_MEMBER_SEARCH_QUERY_INIT("한 글/ops");
  search.present = DCC_REST_GUILD_MEMBER_SEARCH_QUERY_PRESENT_LIMIT;
  search.limit = 1000U;
  TASK8_EXPECT_CALL(
      "Task 8 member search encoding", capture,
      dcc_rest_search_guild_members(client, 812U, &search, NULL, &request),
      "GET",
      "/guilds/812/members/search?query=%ED%95%9C%20%EA%B8%80%2Fops&limit=1000",
      NULL, NULL);

  dcc_snowflake_t prune_roles[] = {901U, 902U};
  dcc_rest_guild_prune_t prune = DCC_REST_GUILD_PRUNE_INIT;
  prune.present = DCC_REST_GUILD_PRUNE_PRESENT_DAYS |
                  DCC_REST_GUILD_PRUNE_PRESENT_COMPUTE_PRUNE_COUNT |
                  DCC_REST_GUILD_PRUNE_PRESENT_INCLUDE_ROLES;
  prune.days = 7U;
  prune.compute_prune_count = 0U;
  prune.include_roles = prune_roles;
  prune.include_role_count = 2U;
  TASK8_EXPECT_CALL(
      "Task 8 prune body", capture,
      dcc_rest_begin_guild_prune(client, 813U, &prune, NULL, &request), "POST",
      "/guilds/813/prune", "application/json",
      "\"include_roles\":[\"901\",\"902\"]");

  dcc_rest_guild_widget_image_query_t widget =
      DCC_REST_GUILD_WIDGET_IMAGE_QUERY_INIT;
  widget.present = DCC_REST_GUILD_WIDGET_IMAGE_QUERY_PRESENT_STYLE;
  widget.style = DCC_REST_GUILD_WIDGET_STYLE_BANNER4;
  TASK8_EXPECT_CALL(
      "Task 8 public widget", capture,
      dcc_rest_get_guild_widget_png(client, 814U, &widget, NULL, &request),
      "GET", "/guilds/814/widget.png?style=banner4", NULL, NULL);

  TASK8_EXPECT_CALL(
      "Task 8 current guild member Bearer", capture,
      dcc_rest_get_current_user_guild_member(client, 815U, &bearer, &request),
      "GET", "/users/@me/guilds/815/member", NULL, NULL);

  dcc_rest_guild_role_create_t role = DCC_REST_GUILD_ROLE_CREATE_INIT;
  role.present = DCC_REST_GUILD_ROLE_CREATE_PRESENT_NAME |
                 DCC_REST_GUILD_ROLE_CREATE_PRESENT_PERMISSIONS |
                 DCC_REST_GUILD_ROLE_CREATE_PRESENT_COLOR |
                 DCC_REST_GUILD_ROLE_CREATE_PRESENT_HOIST |
                 DCC_REST_GUILD_ROLE_CREATE_PRESENT_MENTIONABLE;
  role.name = "operators";
  role.permissions = UINT64_C(2048);
  role.color = UINT32_C(0x123456);
  role.hoist = 1U;
  role.mentionable = 0U;
  TASK8_EXPECT_CALL(
      "Task 8 role body", capture,
      dcc_rest_create_guild_role(client, 816U, &role, NULL, &request), "POST",
      "/guilds/816/roles", "application/json", "\"permissions\":\"2048\"");

  dcc_rest_guild_role_update_t role_update = DCC_REST_GUILD_ROLE_UPDATE_INIT;
  role_update.present = UINT64_C(0xF7);
  role_update.nulls = UINT64_C(0xF7);
  TASK8_EXPECT_CALL("Task 8 role update nullable fields", capture,
                    dcc_rest_modify_guild_role(client, 816U, 817U, &role_update,
                                               NULL, &request),
                    "PATCH", "/guilds/816/roles/817", "application/json",
                    "{\"name\":null,\"permissions\":null,\"color\":null,"
                    "\"hoist\":null,\"icon\":null,\"unicode_emoji\":null,"
                    "\"mentionable\":null}");
  role_update = (dcc_rest_guild_role_update_t)DCC_REST_GUILD_ROLE_UPDATE_INIT;
  role_update.present = DCC_REST_GUILD_ROLE_UPDATE_PRESENT_COLORS;
  role_update.nulls = DCC_REST_GUILD_ROLE_UPDATE_PRESENT_COLORS;
  TASK8_EXPECT_CALL("Task 8 role update nullable colors", capture,
                    dcc_rest_modify_guild_role(client, 816U, 817U, &role_update,
                                               NULL, &request),
                    "PATCH", "/guilds/816/roles/817", "application/json",
                    "{\"colors\":null}");

  dcc_rest_guild_soundboard_sound_create_t sound =
      DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_INIT("bell",
                                                  "data:audio/ogg;base64,AAAA");
  sound.present = DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_PRESENT_VOLUME;
  sound.volume = 0.5;
  TASK8_EXPECT_CALL("Task 8 sound JSON data URI", capture,
                    dcc_rest_create_guild_soundboard_sound(client, 817U, &sound,
                                                           NULL, &request),
                    "POST", "/guilds/817/soundboard-sounds", "application/json",
                    "\"sound\":\"data:audio/ogg;base64,AAAA\"");

  sound.nulls = DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_PRESENT_VOLUME;
  sound.volume = NAN;
  TASK8_EXPECT_CALL("Task 8 sound create nullable volume", capture,
                    dcc_rest_create_guild_soundboard_sound(client, 817U, &sound,
                                                           NULL, &request),
                    "POST", "/guilds/817/soundboard-sounds", "application/json",
                    "\"volume\":null");

  dcc_rest_guild_soundboard_sound_update_t sound_update =
      DCC_REST_GUILD_SOUNDBOARD_SOUND_UPDATE_INIT;
  sound_update.present = DCC_REST_GUILD_SOUNDBOARD_SOUND_UPDATE_PRESENT_VOLUME;
  sound_update.nulls = DCC_REST_GUILD_SOUNDBOARD_SOUND_UPDATE_PRESENT_VOLUME;
  sound_update.volume = INFINITY;
  TASK8_EXPECT_CALL("Task 8 sound update nullable volume", capture,
                    dcc_rest_modify_guild_soundboard_sound(
                        client, 817U, 818U, &sound_update, NULL, &request),
                    "PATCH", "/guilds/817/soundboard-sounds/818",
                    "application/json", "{\"volume\":null}");

  dcc_rest_guild_welcome_screen_update_t welcome_update =
      DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_INIT;
  welcome_update.present = DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_ENABLED;
  welcome_update.nulls = DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_ENABLED;
  welcome_update.enabled = 2U;
  TASK8_EXPECT_CALL("Task 8 welcome enabled nullable", capture,
                    dcc_rest_modify_guild_welcome_screen(
                        client, 819U, &welcome_update, NULL, &request),
                    "PATCH", "/guilds/819/welcome-screen", "application/json",
                    "{\"enabled\":null}");

  TASK8_EXPECT_CALL("Task 8 null modify guild body", capture,
                    dcc_rest_modify_guild(client, 830U, NULL, NULL, &request),
                    "PATCH", "/guilds/830", "application/json", "{}");
  TASK8_EXPECT_CALL(
      "Task 8 null ban body", capture,
      dcc_rest_create_guild_ban(client, 830U, 831U, NULL, NULL, &request),
      "PUT", "/guilds/830/bans/831", "application/json", "{}");
  TASK8_EXPECT_CALL(
      "Task 8 null prune body", capture,
      dcc_rest_begin_guild_prune(client, 830U, NULL, NULL, &request), "POST",
      "/guilds/830/prune", "application/json", "{}");
  TASK8_EXPECT_CALL(
      "Task 8 null create role body", capture,
      dcc_rest_create_guild_role(client, 830U, NULL, NULL, &request), "POST",
      "/guilds/830/roles", "application/json", "{}");
  TASK8_EXPECT_CALL(
      "Task 8 null update role body", capture,
      dcc_rest_modify_guild_role(client, 830U, 832U, NULL, NULL, &request),
      "PATCH", "/guilds/830/roles/832", "application/json", "{}");
  TASK8_EXPECT_CALL(
      "Task 8 null member update body", capture,
      dcc_rest_modify_guild_member(client, 830U, 833U, NULL, NULL, &request),
      "PATCH", "/guilds/830/members/833", "application/json", "{}");
  TASK8_EXPECT_CALL(
      "Task 8 null current member body", capture,
      dcc_rest_modify_current_guild_member(client, 830U, NULL, NULL, &request),
      "PATCH", "/guilds/830/members/@me", "application/json", "{}");
  TASK8_EXPECT_CALL("Task 8 null nickname body", capture,
                    dcc_rest_set_current_guild_member_nickname(
                        client, 830U, NULL, NULL, &request),
                    "PATCH", "/guilds/830/members/@me/nick", "application/json",
                    "{}");
  TASK8_EXPECT_CALL("Task 8 null incident body", capture,
                    dcc_rest_modify_guild_incident_actions(client, 830U, NULL,
                                                           NULL, &request),
                    "PUT", "/guilds/830/incident-actions", "application/json",
                    "{}");
  TASK8_EXPECT_CALL(
      "Task 8 null widget body", capture,
      dcc_rest_modify_guild_widget(client, 830U, NULL, NULL, &request), "PATCH",
      "/guilds/830/widget", "application/json", "{}");
  TASK8_EXPECT_CALL(
      "Task 8 null welcome body", capture,
      dcc_rest_modify_guild_welcome_screen(client, 830U, NULL, NULL, &request),
      "PATCH", "/guilds/830/welcome-screen", "application/json", "{}");
  TASK8_EXPECT_CALL("Task 8 null sound update body", capture,
                    dcc_rest_modify_guild_soundboard_sound(
                        client, 830U, 834U, NULL, NULL, &request),
                    "PATCH", "/guilds/830/soundboard-sounds/834",
                    "application/json", "{}");
  return 0;
}

static int task8_rejection_contract(dcc_client_t *client,
                                    endpoint_capture_t *capture,
                                    endpoint_callback_t *callback,
                                    endpoint_observer_t *observer) {
  endpoint_callback_reset(callback);
  dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
  options.callback = endpoint_result_callback;
  options.user_data = callback;

  TASK8_EXPECT_REJECTION(
      "Task 8 zero path ID", capture, callback, observer,
      dcc_rest_get_guild(client, 0U, NULL, &options, &request));

  dcc_rest_guild_query_t bad_query = DCC_REST_GUILD_QUERY_INIT;
  bad_query.version = 2U;
  TASK8_EXPECT_REJECTION(
      "Task 8 bad query version", capture, callback, observer,
      dcc_rest_get_guild(client, 820U, &bad_query, &options, &request));
  bad_query = (dcc_rest_guild_query_t)DCC_REST_GUILD_QUERY_INIT;
  bad_query.present = UINT64_C(1) << 63U;
  TASK8_EXPECT_REJECTION(
      "Task 8 unknown query bit", capture, callback, observer,
      dcc_rest_get_guild(client, 820U, &bad_query, &options, &request));

  dcc_rest_guild_members_query_t members = DCC_REST_GUILD_MEMBERS_QUERY_INIT;
  members.present = DCC_REST_GUILD_MEMBERS_QUERY_PRESENT_LIMIT;
  members.limit = 1001U;
  TASK8_EXPECT_REJECTION(
      "Task 8 member limit", capture, callback, observer,
      dcc_rest_list_guild_members(client, 820U, &members, &options, &request));

  dcc_rest_guild_ban_create_t ban = DCC_REST_GUILD_BAN_CREATE_INIT;
  ban.present = DCC_REST_GUILD_BAN_CREATE_PRESENT_DELETE_MESSAGE_SECONDS;
  ban.delete_message_seconds = 604801U;
  TASK8_EXPECT_REJECTION(
      "Task 8 ban seconds are rejected, not clamped", capture, callback,
      observer,
      dcc_rest_create_guild_ban(client, 820U, 821U, &ban, &options, &request));

  dcc_snowflake_t users[] = {821U, 0U};
  dcc_rest_guild_bulk_ban_t bulk = DCC_REST_GUILD_BULK_BAN_INIT(users, 2U);
  TASK8_EXPECT_REJECTION(
      "Task 8 zero bulk user", capture, callback, observer,
      dcc_rest_bulk_ban_guild_members(client, 820U, &bulk, &options, &request));

  dcc_rest_guild_member_update_t member = DCC_REST_GUILD_MEMBER_UPDATE_INIT;
  member.nulls = DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_NICK;
  TASK8_EXPECT_REJECTION("Task 8 null without present", capture, callback,
                         observer,
                         dcc_rest_modify_guild_member(
                             client, 820U, 821U, &member, &options, &request));

  member = (dcc_rest_guild_member_update_t)DCC_REST_GUILD_MEMBER_UPDATE_INIT;
  member.present =
      DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_COMMUNICATION_DISABLED_UNTIL;
  member.communication_disabled_until = "2026-02-29T00:00:00Z";
  TASK8_EXPECT_REJECTION("Task 8 member impossible calendar date", capture,
                         callback, observer,
                         dcc_rest_modify_guild_member(
                             client, 820U, 821U, &member, &options, &request));

  dcc_snowflake_t one_role = 821U;
  member = (dcc_rest_guild_member_update_t)DCC_REST_GUILD_MEMBER_UPDATE_INIT;
  member.present = DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_ROLES;
  member.roles = &one_role;
  member.role_count = SIZE_MAX;
  TASK8_EXPECT_REJECTION("Task 8 member role span overflow", capture, callback,
                         observer,
                         dcc_rest_modify_guild_member(
                             client, 820U, 821U, &member, &options, &request));

  dcc_rest_guild_prune_query_t prune_query = DCC_REST_GUILD_PRUNE_QUERY_INIT;
  prune_query.present = DCC_REST_GUILD_PRUNE_QUERY_PRESENT_INCLUDE_ROLES;
  prune_query.include_roles = &one_role;
  prune_query.include_role_count = SIZE_MAX;
  TASK8_EXPECT_REJECTION("Task 8 prune query role span overflow", capture,
                         callback, observer,
                         dcc_rest_get_guild_prune_count(
                             client, 820U, &prune_query, &options, &request));

  const char *one_feature = "COMMUNITY";
  dcc_rest_guild_update_t guild = DCC_REST_GUILD_UPDATE_INIT;
  guild.present = DCC_REST_GUILD_UPDATE_PRESENT_FEATURES;
  guild.features = &one_feature;
  guild.feature_count = SIZE_MAX;
  TASK8_EXPECT_REJECTION(
      "Task 8 guild feature span overflow", capture, callback, observer,
      dcc_rest_modify_guild(client, 820U, &guild, &options, &request));
  guild = (dcc_rest_guild_update_t)DCC_REST_GUILD_UPDATE_INIT;
  guild.present = DCC_REST_GUILD_UPDATE_PRESENT_NAME;
  guild.name = "\nvalid guild";
  TASK8_EXPECT_REJECTION(
      "Task 8 guild leading ASCII whitespace", capture, callback, observer,
      dcc_rest_modify_guild(client, 820U, &guild, &options, &request));
  guild.name = "valid guild\xE3\x80\x80";
  TASK8_EXPECT_REJECTION(
      "Task 8 guild trailing Unicode whitespace", capture, callback, observer,
      dcc_rest_modify_guild(client, 820U, &guild, &options, &request));

  dcc_rest_guild_incident_actions_t incident =
      DCC_REST_GUILD_INCIDENT_ACTIONS_INIT;
  incident.present =
      DCC_REST_GUILD_INCIDENT_ACTIONS_PRESENT_INVITES_DISABLED_UNTIL;
  incident.invites_disabled_until = "2026-04-31T00:00:00Z";
  TASK8_EXPECT_REJECTION("Task 8 incident impossible calendar date", capture,
                         callback, observer,
                         dcc_rest_modify_guild_incident_actions(
                             client, 820U, &incident, &options, &request));

  dcc_rest_guild_role_create_t role = DCC_REST_GUILD_ROLE_CREATE_INIT;
  role.present = DCC_REST_GUILD_ROLE_CREATE_PRESENT_COLOR;
  role.color = UINT32_C(0x1000000);
  TASK8_EXPECT_REJECTION(
      "Task 8 role RGB range", capture, callback, observer,
      dcc_rest_create_guild_role(client, 820U, &role, &options, &request));
  role = (dcc_rest_guild_role_create_t)DCC_REST_GUILD_ROLE_CREATE_INIT;
  role.present = DCC_REST_GUILD_ROLE_CREATE_PRESENT_ICON |
                 DCC_REST_GUILD_ROLE_CREATE_PRESENT_UNICODE_EMOJI;
  role.icon = "data:image/png;base64,AAAA";
  role.unicode_emoji = "⭐";
  TASK8_EXPECT_REJECTION(
      "Task 8 role icon conflict", capture, callback, observer,
      dcc_rest_create_guild_role(client, 820U, &role, &options, &request));

  role = (dcc_rest_guild_role_create_t)DCC_REST_GUILD_ROLE_CREATE_INIT;
  role.present = DCC_REST_GUILD_ROLE_CREATE_PRESENT_COLORS;
  role.nulls = DCC_REST_GUILD_ROLE_CREATE_PRESENT_COLORS;
  TASK8_EXPECT_REJECTION(
      "Task 8 create role colors are not nullable", capture, callback, observer,
      dcc_rest_create_guild_role(client, 820U, &role, &options, &request));

  dcc_rest_role_colors_t invalid_colors = DCC_REST_ROLE_COLORS_INIT(1U);
  invalid_colors.present = DCC_REST_ROLE_COLORS_PRESENT_SECONDARY_COLOR |
                           DCC_REST_ROLE_COLORS_PRESENT_TERTIARY_COLOR;
  invalid_colors.secondary_color = 2U;
  invalid_colors.tertiary_color = 3U;
  role = (dcc_rest_guild_role_create_t)DCC_REST_GUILD_ROLE_CREATE_INIT;
  role.present = DCC_REST_GUILD_ROLE_CREATE_PRESENT_COLORS;
  role.colors = &invalid_colors;
  TASK8_EXPECT_REJECTION(
      "Task 8 invalid nested role colors allocate nothing", capture, callback,
      observer,
      dcc_rest_create_guild_role(client, 820U, &role, &options, &request));

  dcc_rest_current_guild_member_update_t current_member =
      DCC_REST_CURRENT_GUILD_MEMBER_UPDATE_INIT;
  current_member.present = DCC_REST_CURRENT_GUILD_MEMBER_UPDATE_PRESENT_NICK;
  current_member.nick = "";
  TASK8_EXPECT_REJECTION(
      "Task 8 current member nickname is nonempty", capture, callback, observer,
      dcc_rest_modify_current_guild_member(client, 820U, &current_member,
                                           &options, &request));

  dcc_rest_current_guild_member_nickname_t nickname =
      DCC_REST_CURRENT_GUILD_MEMBER_NICKNAME_INIT;
  nickname.present = DCC_REST_CURRENT_GUILD_MEMBER_NICKNAME_PRESENT_NICK;
  nickname.nick = "123456789012345678901234567890123";
  TASK8_EXPECT_REJECTION("Task 8 nickname scalar limit", capture, callback,
                         observer,
                         dcc_rest_set_current_guild_member_nickname(
                             client, 820U, &nickname, &options, &request));

  dcc_rest_guild_role_position_t position =
      DCC_REST_GUILD_ROLE_POSITION_INIT(821U);
  position.present = DCC_REST_GUILD_ROLE_POSITION_PRESENT_POSITION;
  position.position = -1;
  TASK8_EXPECT_REJECTION("Task 8 negative role position", capture, callback,
                         observer,
                         dcc_rest_modify_guild_role_positions(
                             client, 820U, &position, 1U, &options, &request));

  dcc_rest_welcome_channel_t channels[6];
  for (size_t index = 0U; index < 6U; ++index) {
    channels[index] = (dcc_rest_welcome_channel_t)DCC_REST_WELCOME_CHANNEL_INIT(
        900U + index, "welcome");
  }
  dcc_rest_guild_welcome_screen_update_t welcome =
      DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_INIT;
  welcome.present =
      DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_WELCOME_CHANNELS;
  welcome.welcome_channels = channels;
  welcome.welcome_channel_count = 6U;
  TASK8_EXPECT_REJECTION("Task 8 welcome channel count", capture, callback,
                         observer,
                         dcc_rest_modify_guild_welcome_screen(
                             client, 820U, &welcome, &options, &request));

  dcc_rest_guild_soundboard_sound_create_t sound =
      DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_INIT("bell", "not-a-data-uri");
  TASK8_EXPECT_REJECTION("Task 8 sound data URI", capture, callback, observer,
                         dcc_rest_create_guild_soundboard_sound(
                             client, 820U, &sound, &options, &request));
  sound = (dcc_rest_guild_soundboard_sound_create_t)
      DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_INIT("bell",
                                                  "data:audio/ogg;base64,AAAA");
  sound.present = DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_PRESENT_VOLUME;
  sound.volume = NAN;
  TASK8_EXPECT_REJECTION("Task 8 sound finite volume", capture, callback,
                         observer,
                         dcc_rest_create_guild_soundboard_sound(
                             client, 820U, &sound, &options, &request));

  options.auth_mode = DCC_REST_AUTH_BOT;
  TASK8_EXPECT_REJECTION(
      "Task 8 public widget rejects Bot", capture, callback, observer,
      dcc_rest_get_guild_widget_json(client, 820U, &options, &request));
  options.auth_mode = DCC_REST_AUTH_DEFAULT;
  TASK8_EXPECT_REJECTION(
      "Task 8 current member requires Bearer", capture, callback, observer,
      dcc_rest_get_current_user_guild_member(client, 820U, &options, &request));
  options.auth_mode = DCC_REST_AUTH_BOT;
  options.audit_log_reason = "not allowed";
  TASK8_EXPECT_REJECTION(
      "Task 8 ineligible audit reason", capture, callback, observer,
      dcc_rest_get_guild(client, 820U, NULL, &options, &request));
  return 0;
}

static int task8_sensitive_body_wipe_contract(dcc_client_t *client) {
  static const char access_token[] = "task8-add-member-oauth-secret";
  static const char serialized[] =
      "{\"access_token\":\"task8-add-member-oauth-secret\"}";
  dcc_rest_guild_member_add_t body =
      DCC_REST_GUILD_MEMBER_ADD_INIT(access_token);
  dcc_endpoint_sensitive_probe_snapshot_t before =
      DCC_ENDPOINT_SENSITIVE_PROBE_SNAPSHOT_INIT;
  dcc_endpoint_sensitive_probe_snapshot_t after =
      DCC_ENDPOINT_SENSITIVE_PROBE_SNAPSHOT_INIT;
  dcc_rest_request_t *request = (dcc_rest_request_t *)(uintptr_t)1U;

  dcc_endpoint_test_sensitive_probe_begin(access_token,
                                          sizeof(access_token) - 1U);
  dcc_status_t before_status =
      dcc_endpoint_test_sensitive_probe_snapshot(&before);
  dcc_endpoint_test_sensitive_force_failure_once(
      DCC_ENDPOINT_SENSITIVE_FAILURE_ADMISSION);
  dcc_status_t submit =
      dcc_rest_add_guild_member(client, 840U, 841U, &body, NULL, &request);
  dcc_status_t after_status =
      dcc_endpoint_test_sensitive_probe_snapshot(&after);
  dcc_endpoint_test_sensitive_probe_end();

  size_t expected_wipe = sizeof(serialized);
  if (before_status != DCC_OK || after_status != DCC_OK ||
      submit != DCC_ERR_STATE || request != NULL ||
      after.secure_wipe_calls != before.secure_wipe_calls + 1U ||
      after.secure_wipe_bytes < before.secure_wipe_bytes + expected_wipe) {
    fprintf(stderr,
            "Task 8 sensitive body wipe before=%s after=%s submit=%s "
            "request=%p wipes=%zu/%zu bytes=%zu/%zu expected=%zu\n",
            dcc_status_string(before_status), dcc_status_string(after_status),
            dcc_status_string(submit), (void *)request,
            before.secure_wipe_calls, after.secure_wipe_calls,
            before.secure_wipe_bytes, after.secure_wipe_bytes, expected_wipe);
    dcc_rest_request_destroy(request);
    return 1;
  }
  return 0;
}

int endpoint_task8_contract(dcc_client_t *client, endpoint_capture_t *capture,
                            endpoint_callback_t *callback,
                            endpoint_observer_t *observer) {
  if (task8_wire_contract(client, capture) != 0 ||
      task8_rejection_contract(client, capture, callback, observer) != 0 ||
      task8_sensitive_body_wipe_contract(client) != 0) {
    return 1;
  }
  return 0;
}

#undef TASK8_EXPECT_REJECTION
#undef TASK8_EXPECT_CALL
