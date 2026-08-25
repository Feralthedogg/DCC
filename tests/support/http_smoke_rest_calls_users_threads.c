#include "http_smoke_support.h"

#if !defined(_WIN32)

dcc_status_t call_rest_leave_guild(dcc_client_t *client, dcc_rest_cb cb,
                                   void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_leave_guild(client, 333, &options, NULL);
}
dcc_status_t call_rest_add_group_dm_recipient(dcc_client_t *client,
                                              dcc_rest_cb cb, void *user_data) {
  dcc_group_dm_recipient_params_t params = DCC_GROUP_DM_RECIPIENT_PARAMS_INIT;
  params.access_token = "tok";
  params.nick = "feral";
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  options.auth_mode = DCC_REST_AUTH_BEARER;
  options.auth_token = "group-dm-token";
  return dcc_rest_add_group_dm_recipient(client, 222, 444, &params, &options,
                                         NULL);
}
dcc_status_t call_rest_add_group_dm_recipient_params(dcc_client_t *client,
                                                     dcc_rest_cb cb,
                                                     void *user_data) {
  const dcc_group_dm_recipient_params_t params = {
      .size = sizeof(params),
      .version = DCC_GROUP_DM_RECIPIENT_PARAMS_VERSION,
      .present = UINT64_C(0),
      .channel_id = 222,
      .user_id = 444,
      .access_token = "tok2",
      .nick = "feral2"};
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  options.auth_mode = DCC_REST_AUTH_BEARER;
  options.auth_token = "group-dm-token";
  return dcc_rest_add_group_dm_recipient(client, 222, 444, &params, &options,
                                         NULL);
}
dcc_status_t call_rest_remove_group_dm_recipient(dcc_client_t *client,
                                                 dcc_rest_cb cb,
                                                 void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  options.auth_mode = DCC_REST_AUTH_BEARER;
  options.auth_token = "group-dm-token";
  return dcc_rest_remove_group_dm_recipient(client, 222, 444, &options, NULL);
}
dcc_status_t call_rest_get_current_user_voice_state(dcc_client_t *client,
                                                    dcc_rest_cb cb,
                                                    void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_get_current_user_voice_state(client, 333, &options, NULL);
}
dcc_status_t call_rest_modify_current_user_voice_state(dcc_client_t *client,
                                                       dcc_rest_cb cb,
                                                       void *user_data) {
  dcc_current_user_voice_state_params_t params =
      DCC_CURRENT_USER_VOICE_STATE_PARAMS_INIT;
  params.present =
      DCC_VOICE_STATE_PRESENT_CHANNEL_ID | DCC_VOICE_STATE_PRESENT_SUPPRESS;
  params.channel_id = 222;
  params.suppress = 0;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_modify_current_user_voice_state(client, 333, &params,
                                                  &options, NULL);
}
dcc_status_t call_rest_modify_current_user_voice_state_params(
    dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
  const dcc_current_user_voice_state_params_t params = {
      .size = sizeof(params),
      .version = DCC_VOICE_STATE_PARAMS_VERSION,
      .present = DCC_VOICE_STATE_PRESENT_CHANNEL_ID |
                 DCC_VOICE_STATE_PRESENT_SUPPRESS |
                 DCC_VOICE_STATE_PRESENT_REQUEST_TO_SPEAK_TIMESTAMP,
      .channel_id = 222,
      .suppress = 0,
      .request_to_speak_timestamp = "2026-06-16T00:00:00.000000+00:00"};
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_modify_current_user_voice_state(client, 333, &params,
                                                  &options, NULL);
}
dcc_status_t call_rest_get_user_voice_state(dcc_client_t *client,
                                            dcc_rest_cb cb, void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_get_user_voice_state(client, 333, 444, &options, NULL);
}
dcc_status_t call_rest_modify_user_voice_state(dcc_client_t *client,
                                               dcc_rest_cb cb,
                                               void *user_data) {
  dcc_user_voice_state_params_t params = DCC_USER_VOICE_STATE_PARAMS_INIT;
  params.present =
      DCC_VOICE_STATE_PRESENT_CHANNEL_ID | DCC_VOICE_STATE_PRESENT_SUPPRESS;
  params.channel_id = 222;
  params.suppress = 1;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_modify_user_voice_state(client, 333, 444, &params, &options,
                                          NULL);
}
dcc_status_t call_rest_modify_user_voice_state_params(dcc_client_t *client,
                                                      dcc_rest_cb cb,
                                                      void *user_data) {
  const dcc_user_voice_state_params_t params = {
      .size = sizeof(params),
      .version = DCC_VOICE_STATE_PARAMS_VERSION,
      .present =
          DCC_VOICE_STATE_PRESENT_CHANNEL_ID | DCC_VOICE_STATE_PRESENT_SUPPRESS,
      .channel_id = 222,
      .suppress = 1};
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_modify_user_voice_state(client, 333, 444, &params, &options,
                                          NULL);
}
dcc_status_t call_rest_get_voice_regions(dcc_client_t *client, dcc_rest_cb cb,
                                         void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_get_voice_regions(client, &options, NULL);
}
dcc_status_t call_rest_get_guild_voice_regions(dcc_client_t *client,
                                               dcc_rest_cb cb,
                                               void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_get_guild_voice_regions(client, 333, &options, NULL);
}
dcc_status_t call_rest_get_skus(dcc_client_t *client, dcc_rest_cb cb,
                                void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_get_skus(client, 123, &options, NULL);
}
dcc_status_t call_rest_get_entitlements(dcc_client_t *client, dcc_rest_cb cb,
                                        void *user_data) {
  dcc_rest_entitlement_query_t query = DCC_REST_ENTITLEMENT_QUERY_INIT;
  query.present = DCC_REST_ENTITLEMENT_QUERY_PRESENT_LIMIT |
                  DCC_REST_ENTITLEMENT_QUERY_PRESENT_USER_ID;
  query.limit = 2;
  query.user_id = 444;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_get_entitlements(client, 123, &query, &options, NULL);
}
dcc_status_t call_rest_get_entitlements_page(dcc_client_t *client,
                                             dcc_rest_cb cb, void *user_data) {
  dcc_snowflake_t sku_ids[] = {555, 556};
  dcc_rest_entitlement_query_t query = DCC_REST_ENTITLEMENT_QUERY_INIT;
  query.present = UINT64_C(127);
  query.user_id = 444;
  query.sku_ids = sku_ids;
  query.sku_id_count = 2;
  query.before = 111;
  query.after = 222;
  query.limit = 25;
  query.guild_id = 333;
  query.exclude_ended = 1;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_get_entitlements(client, 123, &query, &options, NULL);
}
dcc_status_t call_rest_create_test_entitlement(dcc_client_t *client,
                                               dcc_rest_cb cb,
                                               void *user_data) {
  dcc_test_entitlement_params_t params = DCC_TEST_ENTITLEMENT_PARAMS_INIT;
  params.sku_id = 555;
  params.owner_id = 444;
  params.owner_type = 2;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_create_test_entitlement(client, 123, &params, &options, NULL);
}
dcc_status_t call_rest_create_test_entitlement_params(dcc_client_t *client,
                                                      dcc_rest_cb cb,
                                                      void *user_data) {
  const dcc_test_entitlement_params_t params = {
      .size = sizeof(params),
      .version = DCC_TEST_ENTITLEMENT_PARAMS_VERSION,
      .application_id = 123,
      .sku_id = 555,
      .owner_id = 444,
      .owner_type = 2};
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_create_test_entitlement(client, 123, &params, &options, NULL);
}
dcc_status_t call_rest_delete_test_entitlement(dcc_client_t *client,
                                               dcc_rest_cb cb,
                                               void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_delete_test_entitlement(client, 123, 666, &options, NULL);
}
dcc_status_t call_rest_consume_entitlement(dcc_client_t *client, dcc_rest_cb cb,
                                           void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_consume_entitlement(client, 123, 666, &options, NULL);
}
dcc_status_t call_rest_get_gateway_bot(dcc_client_t *client, dcc_rest_cb cb,
                                       void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_get_gateway_bot(client, &options, NULL);
}
dcc_status_t call_rest_get_guild_member(dcc_client_t *client, dcc_rest_cb cb,
                                        void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_get_guild_member(client, 333, 444, &options, NULL);
}
dcc_status_t call_rest_list_guild_members(dcc_client_t *client, dcc_rest_cb cb,
                                          void *user_data) {
  dcc_rest_guild_members_query_t query = DCC_REST_GUILD_MEMBERS_QUERY_INIT;
  query.present = DCC_REST_GUILD_MEMBERS_QUERY_PRESENT_LIMIT |
                  DCC_REST_GUILD_MEMBERS_QUERY_PRESENT_AFTER;
  query.limit = 2;
  query.after = 444;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_list_guild_members(client, 333, &query, &options, NULL);
}
dcc_status_t call_rest_list_guild_members_page(dcc_client_t *client,
                                               dcc_rest_cb cb,
                                               void *user_data) {
  return call_rest_list_guild_members(client, cb, user_data);
}
dcc_status_t call_rest_search_guild_members(dcc_client_t *client,
                                            dcc_rest_cb cb, void *user_data) {
  dcc_rest_guild_member_search_query_t query =
      DCC_REST_GUILD_MEMBER_SEARCH_QUERY_INIT("feral");
  query.present = DCC_REST_GUILD_MEMBER_SEARCH_QUERY_PRESENT_LIMIT;
  query.limit = 1;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_search_guild_members(client, 333, &query, &options, NULL);
}
dcc_status_t call_rest_search_guild_members_page(dcc_client_t *client,
                                                 dcc_rest_cb cb,
                                                 void *user_data) {
  dcc_rest_guild_member_search_query_t query =
      DCC_REST_GUILD_MEMBER_SEARCH_QUERY_INIT("feral ops");
  query.present = DCC_REST_GUILD_MEMBER_SEARCH_QUERY_PRESENT_LIMIT;
  query.limit = 1;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_search_guild_members(client, 333, &query, &options, NULL);
}
dcc_status_t call_rest_add_guild_member(dcc_client_t *client, dcc_rest_cb cb,
                                        void *user_data) {
  dcc_rest_guild_member_add_t body = DCC_REST_GUILD_MEMBER_ADD_INIT("tok");
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_add_guild_member(client, 333, 444, &body, &options, NULL);
}
dcc_status_t call_rest_add_guild_member_params(dcc_client_t *client,
                                               dcc_rest_cb cb,
                                               void *user_data) {
  const dcc_snowflake_t roles[] = {555, 556};
  dcc_rest_guild_member_add_t body = DCC_REST_GUILD_MEMBER_ADD_INIT("tok2");
  body.present = DCC_REST_GUILD_MEMBER_ADD_PRESENT_NICK |
                 DCC_REST_GUILD_MEMBER_ADD_PRESENT_ROLES;
  body.nick = "feral typed";
  body.roles = roles;
  body.role_count = 2;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_add_guild_member(client, 333, 444, &body, &options, NULL);
}
dcc_status_t call_rest_modify_guild_member(dcc_client_t *client, dcc_rest_cb cb,
                                           void *user_data) {
  dcc_rest_guild_member_update_t body = DCC_REST_GUILD_MEMBER_UPDATE_INIT;
  body.present = DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_NICK;
  body.nick = "feral";
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_modify_guild_member(client, 333, 444, &body, &options, NULL);
}
dcc_status_t call_rest_modify_guild_member_params(dcc_client_t *client,
                                                  dcc_rest_cb cb,
                                                  void *user_data) {
  const dcc_snowflake_t roles[] = {555, 556};
  dcc_rest_guild_member_update_t body = DCC_REST_GUILD_MEMBER_UPDATE_INIT;
  body.present =
      DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_COMMUNICATION_DISABLED_UNTIL |
      DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_NICK |
      DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_ROLES |
      DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_MUTE |
      DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_DEAF;
  body.nulls = DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_NICK;
  body.communication_disabled_until = "2026-06-15T03:00:00.000000+00:00";
  body.roles = roles;
  body.role_count = 2;
  body.mute = 1;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_modify_guild_member(client, 333, 444, &body, &options, NULL);
}
dcc_status_t call_rest_set_guild_member_timeout(dcc_client_t *client,
                                                dcc_rest_cb cb,
                                                void *user_data) {
  dcc_rest_guild_member_update_t body = DCC_REST_GUILD_MEMBER_UPDATE_INIT;
  body.present =
      DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_COMMUNICATION_DISABLED_UNTIL;
  body.communication_disabled_until = "2026-06-15T03:00:00.000000+00:00";
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_modify_guild_member(client, 333, 444, &body, &options, NULL);
}
dcc_status_t call_rest_clear_guild_member_timeout(dcc_client_t *client,
                                                  dcc_rest_cb cb,
                                                  void *user_data) {
  dcc_rest_guild_member_update_t body = DCC_REST_GUILD_MEMBER_UPDATE_INIT;
  body.present =
      DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_COMMUNICATION_DISABLED_UNTIL;
  body.nulls =
      DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_COMMUNICATION_DISABLED_UNTIL;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_modify_guild_member(client, 333, 444, &body, &options, NULL);
}
dcc_status_t call_rest_move_guild_member(dcc_client_t *client, dcc_rest_cb cb,
                                         void *user_data) {
  dcc_rest_guild_member_update_t body = DCC_REST_GUILD_MEMBER_UPDATE_INIT;
  body.present = DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_CHANNEL_ID;
  body.channel_id = 222;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_modify_guild_member(client, 333, 444, &body, &options, NULL);
}
dcc_status_t call_rest_disconnect_guild_member_voice(dcc_client_t *client,
                                                     dcc_rest_cb cb,
                                                     void *user_data) {
  dcc_rest_guild_member_update_t body = DCC_REST_GUILD_MEMBER_UPDATE_INIT;
  body.present = DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_CHANNEL_ID;
  body.nulls = DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_CHANNEL_ID;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_modify_guild_member(client, 333, 444, &body, &options, NULL);
}
dcc_status_t call_rest_remove_guild_member(dcc_client_t *client, dcc_rest_cb cb,
                                           void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_remove_guild_member(client, 333, 444, &options, NULL);
}
dcc_status_t call_rest_add_guild_member_role(dcc_client_t *client,
                                             dcc_rest_cb cb, void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_add_guild_member_role(client, 333, 444, 555, &options, NULL);
}
dcc_status_t call_rest_add_guild_member_role_params(dcc_client_t *client,
                                                    dcc_rest_cb cb,
                                                    void *user_data) {
  return call_rest_add_guild_member_role(client, cb, user_data);
}
dcc_status_t call_rest_remove_guild_member_role(dcc_client_t *client,
                                                dcc_rest_cb cb,
                                                void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_remove_guild_member_role(client, 333, 444, 555, &options,
                                           NULL);
}
dcc_status_t call_rest_get_guild_roles(dcc_client_t *client, dcc_rest_cb cb,
                                       void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_get_guild_roles(client, 333, &options, NULL);
}
dcc_status_t call_rest_create_guild_role(dcc_client_t *client, dcc_rest_cb cb,
                                         void *user_data) {
  dcc_rest_guild_role_create_t body = DCC_REST_GUILD_ROLE_CREATE_INIT;
  body.present = DCC_REST_GUILD_ROLE_CREATE_PRESENT_NAME;
  body.name = "ops";
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_create_guild_role(client, 333, &body, &options, NULL);
}
dcc_status_t call_rest_create_guild_role_params(dcc_client_t *client,
                                                dcc_rest_cb cb,
                                                void *user_data) {
  dcc_rest_guild_role_create_t body = DCC_REST_GUILD_ROLE_CREATE_INIT;
  body.present = DCC_REST_GUILD_ROLE_CREATE_PRESENT_NAME |
                 DCC_REST_GUILD_ROLE_CREATE_PRESENT_COLOR |
                 DCC_REST_GUILD_ROLE_CREATE_PRESENT_PERMISSIONS |
                 DCC_REST_GUILD_ROLE_CREATE_PRESENT_HOIST |
                 DCC_REST_GUILD_ROLE_CREATE_PRESENT_MENTIONABLE |
                 DCC_REST_GUILD_ROLE_CREATE_PRESENT_ICON;
  body.name = "ops typed";
  body.color = 1122867;
  body.permissions = 2048;
  body.hoist = 1;
  body.icon = "data:image/png;base64,AAAA";
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_create_guild_role(client, 333, &body, &options, NULL);
}
dcc_status_t call_rest_modify_guild_role(dcc_client_t *client, dcc_rest_cb cb,
                                         void *user_data) {
  dcc_rest_guild_role_update_t body = DCC_REST_GUILD_ROLE_UPDATE_INIT;
  body.present = DCC_REST_GUILD_ROLE_UPDATE_PRESENT_NAME;
  body.name = "ops2";
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_modify_guild_role(client, 333, 555, &body, &options, NULL);
}
dcc_status_t call_rest_modify_guild_role_params(dcc_client_t *client,
                                                dcc_rest_cb cb,
                                                void *user_data) {
  dcc_rest_guild_role_update_t body = DCC_REST_GUILD_ROLE_UPDATE_INIT;
  body.present = DCC_REST_GUILD_ROLE_UPDATE_PRESENT_NAME |
                 DCC_REST_GUILD_ROLE_UPDATE_PRESENT_COLOR |
                 DCC_REST_GUILD_ROLE_UPDATE_PRESENT_PERMISSIONS |
                 DCC_REST_GUILD_ROLE_UPDATE_PRESENT_HOIST |
                 DCC_REST_GUILD_ROLE_UPDATE_PRESENT_MENTIONABLE;
  body.name = "ops edited";
  body.color = 4478310;
  body.permissions = 3072;
  body.mentionable = 1;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_modify_guild_role(client, 333, 555, &body, &options, NULL);
}
dcc_status_t call_rest_modify_guild_role_positions(dcc_client_t *client,
                                                   dcc_rest_cb cb,
                                                   void *user_data) {
  dcc_rest_guild_role_position_t position =
      DCC_REST_GUILD_ROLE_POSITION_INIT(555);
  position.present = DCC_REST_GUILD_ROLE_POSITION_PRESENT_POSITION;
  position.position = 1;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_modify_guild_role_positions(client, 333, &position, 1,
                                              &options, NULL);
}
dcc_status_t call_rest_modify_guild_role_positions_params(dcc_client_t *client,
                                                          dcc_rest_cb cb,
                                                          void *user_data) {
  dcc_rest_guild_role_position_t positions[] = {
      DCC_REST_GUILD_ROLE_POSITION_INIT(555),
      DCC_REST_GUILD_ROLE_POSITION_INIT(556)};
  positions[0].present = DCC_REST_GUILD_ROLE_POSITION_PRESENT_POSITION;
  positions[0].position = 1;
  positions[1].present = DCC_REST_GUILD_ROLE_POSITION_PRESENT_POSITION;
  positions[1].position = 2;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_modify_guild_role_positions(client, 333, positions, 2,
                                              &options, NULL);
}
dcc_status_t call_rest_delete_guild_role(dcc_client_t *client, dcc_rest_cb cb,
                                         void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_delete_guild_role(client, 333, 555, &options, NULL);
}
dcc_status_t call_rest_get_application_role_connection_metadata(
    dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_get_application_role_connection_metadata(client, 123,
                                                           &options, NULL);
}
dcc_status_t call_rest_update_application_role_connection_metadata(
    dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
  dcc_application_role_connection_metadata_params_t metadata =
      DCC_APPLICATION_ROLE_CONNECTION_METADATA_PARAMS_INIT;
  metadata.type =
      DCC_APPLICATION_ROLE_CONNECTION_METADATA_INTEGER_LESS_THAN_OR_EQUAL;
  metadata.key = "score";
  metadata.name = "Score";
  metadata.description = "Rank score";
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_update_application_role_connection_metadata(
      client, 123, &metadata, 1, &options, NULL);
}
dcc_status_t call_rest_update_application_role_connection_metadata_params(
    dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
  dcc_localization_t name_localizations[] = {{"ko", "Score KO"}};
  dcc_localization_t description_localizations[] = {{"ko", "Rank score KO"}};
  dcc_application_role_connection_metadata_params_t metadata[] = {
      DCC_APPLICATION_ROLE_CONNECTION_METADATA_PARAMS_INIT};
  metadata[0].present =
      DCC_ROLE_CONNECTION_METADATA_PRESENT_NAME_LOCALIZATIONS |
      DCC_ROLE_CONNECTION_METADATA_PRESENT_DESCRIPTION_LOCALIZATIONS;
  metadata[0].type =
      DCC_APPLICATION_ROLE_CONNECTION_METADATA_INTEGER_LESS_THAN_OR_EQUAL;
  metadata[0].key = "score";
  metadata[0].name = "Score";
  metadata[0].description = "Rank score";
  metadata[0].name_localizations = name_localizations;
  metadata[0].name_localization_count = 1;
  metadata[0].description_localizations = description_localizations;
  metadata[0].description_localization_count = 1;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_update_application_role_connection_metadata(
      client, 123, metadata, 1, &options, NULL);
}
dcc_status_t call_rest_get_current_user_application_role_connection(
    dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  options.auth_mode = DCC_REST_AUTH_BEARER;
  options.auth_token = "user-token";
  return dcc_rest_get_current_user_application_role_connection(client, 123,
                                                               &options, NULL);
}
dcc_status_t call_rest_update_current_user_application_role_connection(
    dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
  dcc_rest_string_map_entry_t entries[] = {{"score", "42"}};
  dcc_rest_string_map_t metadata = DCC_REST_STRING_MAP_INIT;
  metadata.entries = entries;
  metadata.entry_count = 1;
  dcc_application_role_connection_params_t params =
      DCC_APPLICATION_ROLE_CONNECTION_PARAMS_INIT;
  params.present = DCC_ROLE_CONNECTION_PRESENT_PLATFORM_NAME |
                   DCC_ROLE_CONNECTION_PRESENT_METADATA;
  params.platform_name = "dcc";
  params.metadata = &metadata;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  options.auth_mode = DCC_REST_AUTH_BEARER;
  options.auth_token = "user-token";
  return dcc_rest_update_current_user_application_role_connection(
      client, 123, &params, &options, NULL);
}
dcc_status_t call_rest_update_current_user_application_role_connection_params(
    dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
  dcc_rest_string_map_entry_t entries[] = {{"score", "42"}, {"tier", "gold"}};
  dcc_rest_string_map_t metadata = DCC_REST_STRING_MAP_INIT;
  metadata.entries = entries;
  metadata.entry_count = 2;
  dcc_application_role_connection_params_t params =
      DCC_APPLICATION_ROLE_CONNECTION_PARAMS_INIT;
  params.present = DCC_ROLE_CONNECTION_PRESENT_PLATFORM_NAME |
                   DCC_ROLE_CONNECTION_PRESENT_PLATFORM_USERNAME |
                   DCC_ROLE_CONNECTION_PRESENT_METADATA;
  params.platform_name = "dcc";
  params.platform_username = "octo";
  params.metadata = &metadata;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  options.auth_mode = DCC_REST_AUTH_BEARER;
  options.auth_token = "user-token";
  return dcc_rest_update_current_user_application_role_connection(
      client, 123, &params, &options, NULL);
}
dcc_status_t call_rest_get_thread(dcc_client_t *client, dcc_rest_cb cb,
                                  void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_get_channel(client, 999, &options, NULL);
}
dcc_status_t call_rest_create_thread(dcc_client_t *client, dcc_rest_cb cb,
                                     void *user_data) {
  dcc_thread_params_t params = DCC_THREAD_PARAMS_INIT;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  params.present = DCC_THREAD_PARAMS_PRESENT_NAME |
                   DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION |
                   DCC_THREAD_PARAMS_PRESENT_TYPE;
  params.name = "ops";
  params.auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_HOUR;
  params.type = DCC_CHANNEL_PUBLIC_THREAD;
  return dcc_rest_create_thread(client, 222, &params, &options, NULL);
}
dcc_status_t call_rest_create_thread_params(dcc_client_t *client,
                                            dcc_rest_cb cb, void *user_data) {
  dcc_thread_params_t params = DCC_THREAD_PARAMS_INIT;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  params.present = DCC_THREAD_PARAMS_PRESENT_NAME |
                   DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION |
                   DCC_THREAD_PARAMS_PRESENT_TYPE |
                   DCC_THREAD_PARAMS_PRESENT_INVITABLE |
                   DCC_THREAD_PARAMS_PRESENT_RATE_LIMIT_PER_USER;
  params.name = "ops typed";
  params.auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_HOUR;
  params.type = DCC_CHANNEL_PRIVATE_THREAD;
  params.invitable = 1;
  params.rate_limit_per_user = 5;
  return dcc_rest_create_thread(client, 222, &params, &options, NULL);
}
dcc_status_t call_rest_create_forum_thread_params(dcc_client_t *client,
                                                  dcc_rest_cb cb,
                                                  void *user_data) {
  const dcc_snowflake_t tags[] = {901, 902};
  dcc_message_builder_t message;
  dcc_message_builder_init(&message);
  dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
  dcc_thread_params_t params = DCC_THREAD_PARAMS_INIT;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  dcc_status_t status =
      dcc_message_builder_set_content(&message, "forum starter");
  payload.message = &message;
  params.present = DCC_THREAD_PARAMS_PRESENT_NAME |
                   DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION |
                   DCC_THREAD_PARAMS_PRESENT_RATE_LIMIT_PER_USER |
                   DCC_THREAD_PARAMS_PRESENT_APPLIED_TAGS |
                   DCC_THREAD_PARAMS_PRESENT_MESSAGE;
  params.name = "forum typed";
  params.auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_DAY;
  params.rate_limit_per_user = 3;
  params.applied_tags = tags;
  params.applied_tag_count = sizeof(tags) / sizeof(tags[0]);
  params.message = &payload;
  return status == DCC_OK ? dcc_rest_create_forum_thread(client, 222, &params,
                                                         &options, NULL)
                          : status;
}
dcc_status_t call_rest_create_thread_from_message(dcc_client_t *client,
                                                  dcc_rest_cb cb,
                                                  void *user_data) {
  dcc_thread_params_t params = DCC_THREAD_PARAMS_INIT;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  params.present = DCC_THREAD_PARAMS_PRESENT_NAME |
                   DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION;
  params.name = "ops-msg";
  params.auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_HOUR;
  return dcc_rest_create_thread_from_message(client, 222, 777, &params,
                                             &options, NULL);
}
dcc_status_t call_rest_create_thread_from_message_params(dcc_client_t *client,
                                                         dcc_rest_cb cb,
                                                         void *user_data) {
  dcc_thread_params_t params = DCC_THREAD_PARAMS_INIT;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  params.present = DCC_THREAD_PARAMS_PRESENT_NAME |
                   DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION |
                   DCC_THREAD_PARAMS_PRESENT_RATE_LIMIT_PER_USER;
  params.name = "ops-msg typed";
  params.auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_3_DAYS;
  params.rate_limit_per_user = 4;
  return dcc_rest_create_thread_from_message(client, 222, 777, &params,
                                             &options, NULL);
}
dcc_status_t call_rest_modify_thread(dcc_client_t *client, dcc_rest_cb cb,
                                     void *user_data) {
  dcc_channel_params_t params = DCC_CHANNEL_PARAMS_INIT;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  params.kind = DCC_CHANNEL_PARAMS_THREAD;
  params.payload.thread.present = DCC_CHANNEL_THREAD_PRESENT_ARCHIVED;
  params.payload.thread.archived = 1;
  return dcc_rest_modify_channel(client, 999, &params, &options, NULL);
}
dcc_status_t call_rest_modify_thread_params(dcc_client_t *client,
                                            dcc_rest_cb cb, void *user_data) {
  const dcc_snowflake_t tags[] = {901};
  dcc_channel_params_t params = DCC_CHANNEL_PARAMS_INIT;
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  params.kind = DCC_CHANNEL_PARAMS_THREAD;
  params.payload.thread.present =
      DCC_CHANNEL_THREAD_PRESENT_NAME | DCC_CHANNEL_THREAD_PRESENT_ARCHIVED |
      DCC_CHANNEL_THREAD_PRESENT_AUTO_ARCHIVE_DURATION |
      DCC_CHANNEL_THREAD_PRESENT_LOCKED | DCC_CHANNEL_THREAD_PRESENT_INVITABLE |
      DCC_CHANNEL_THREAD_PRESENT_RATE_LIMIT_PER_USER |
      DCC_CHANNEL_THREAD_PRESENT_APPLIED_TAGS;
  params.payload.thread.name = "renamed";
  params.payload.thread.auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_WEEK;
  params.payload.thread.invitable = 0;
  params.payload.thread.rate_limit_per_user = 6;
  params.payload.thread.archived = 1;
  params.payload.thread.locked = 1;
  params.payload.thread.applied_tags = tags;
  params.payload.thread.applied_tag_count = sizeof(tags) / sizeof(tags[0]);
  return dcc_rest_modify_channel(client, 999, &params, &options, NULL);
}
dcc_status_t call_rest_join_thread(dcc_client_t *client, dcc_rest_cb cb,
                                   void *user_data) {
  dcc_rest_call_options_t options =
      rest_call_options_from_legacy(cb, user_data);
  return dcc_rest_join_thread(client, 999, &options, NULL);
}

#endif
