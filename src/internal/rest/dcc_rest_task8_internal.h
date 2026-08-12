#ifndef DCC_REST_TASK8_INTERNAL_H
#define DCC_REST_TASK8_INTERNAL_H

#include "internal/rest/dcc_rest_endpoint_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_task8_prepare(dcc_client_t *client,
                               const dcc_rest_call_options_t *options,
                               dcc_endpoint_auth_policy_t auth,
                               dcc_endpoint_audit_policy_t audit,
                               dcc_rest_request_t **out_request,
                               dcc_rest_call_options_t *resolved);

dcc_status_t dcc_task8_submit(dcc_client_t *client, const char *operation,
                              dcc_rest_method_t method, const char *path,
                              char *json,
                              const dcc_rest_call_options_t *resolved,
                              dcc_rest_request_t **out_request);

dcc_status_t dcc_task8_submit_built(dcc_status_t build_status,
                                    dcc_client_t *client, const char *operation,
                                    dcc_rest_method_t method, const char *path,
                                    char *json,
                                    const dcc_rest_call_options_t *resolved,
                                    dcc_rest_request_t **out_request);

dcc_status_t dcc_task8_empty_object(char **out);
int dcc_task8_iso8601_valid(const char *value);
int dcc_task8_text_has_edge_whitespace(const char *value);

dcc_status_t dcc_task8_query_guild(const dcc_rest_guild_query_t *query,
                                   dcc_rest_buffer_t *out);
dcc_status_t dcc_task8_query_current_user_guilds(
    const dcc_rest_current_user_guilds_query_t *query, dcc_rest_buffer_t *out);
dcc_status_t
dcc_task8_query_audit_log(const dcc_rest_guild_audit_log_query_t *query,
                          dcc_rest_buffer_t *out);
dcc_status_t dcc_task8_query_bans(const dcc_rest_guild_bans_query_t *query,
                                  dcc_rest_buffer_t *out);
dcc_status_t
dcc_task8_query_members(const dcc_rest_guild_members_query_t *query,
                        dcc_rest_buffer_t *out);
dcc_status_t
dcc_task8_query_member_search(const dcc_rest_guild_member_search_query_t *query,
                              dcc_rest_buffer_t *out);
dcc_status_t dcc_task8_query_prune(const dcc_rest_guild_prune_query_t *query,
                                   dcc_rest_buffer_t *out);
dcc_status_t
dcc_task8_query_widget_image(const dcc_rest_guild_widget_image_query_t *query,
                             dcc_rest_buffer_t *out);

dcc_status_t
dcc_task8_build_member_add(const dcc_rest_guild_member_add_t *value,
                           char **out);
dcc_status_t
dcc_task8_build_member_update(const dcc_rest_guild_member_update_t *value,
                              char **out);
dcc_status_t dcc_task8_build_current_member_update(
    const dcc_rest_current_guild_member_update_t *value, char **out);
dcc_status_t dcc_task8_build_current_member_nickname(
    const dcc_rest_current_guild_member_nickname_t *value, char **out);
dcc_status_t
dcc_task8_build_ban_create(const dcc_rest_guild_ban_create_t *value,
                           char **out);
dcc_status_t dcc_task8_build_bulk_ban(const dcc_rest_guild_bulk_ban_t *value,
                                      char **out);
dcc_status_t dcc_task8_build_prune(const dcc_rest_guild_prune_t *value,
                                   char **out);
dcc_status_t
dcc_task8_build_role_create(const dcc_rest_guild_role_create_t *value,
                            char **out);
dcc_status_t
dcc_task8_build_role_update(const dcc_rest_guild_role_update_t *value,
                            char **out);
dcc_status_t
dcc_task8_build_role_positions(const dcc_rest_guild_role_position_t *values,
                               size_t count, char **out);
dcc_status_t dcc_task8_build_guild_update(const dcc_rest_guild_update_t *value,
                                          char **out);
dcc_status_t
dcc_task8_build_incident_actions(const dcc_rest_guild_incident_actions_t *value,
                                 char **out);
dcc_status_t
dcc_task8_build_widget_update(const dcc_rest_guild_widget_update_t *value,
                              char **out);
dcc_status_t dcc_task8_build_welcome_screen(
    const dcc_rest_guild_welcome_screen_update_t *value, char **out);
dcc_status_t dcc_task8_build_sound_create(
    const dcc_rest_guild_soundboard_sound_create_t *value, char **out);
dcc_status_t dcc_task8_build_sound_update(
    const dcc_rest_guild_soundboard_sound_update_t *value, char **out);

#ifdef __cplusplus
}
#endif

#endif
