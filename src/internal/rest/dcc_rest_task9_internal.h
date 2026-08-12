#ifndef DCC_REST_TASK9_INTERNAL_H
#define DCC_REST_TASK9_INTERNAL_H

#include "internal/rest/dcc_rest_endpoint_internal.h"

dcc_status_t dcc_task9_submit_owned(
    dcc_client_t *client, const char *operation, dcc_rest_method_t method,
    const char *path, char *body, size_t body_len, const char *content_type,
    const dcc_rest_call_options_t *options, dcc_endpoint_auth_policy_t auth,
    dcc_endpoint_audit_policy_t audit, dcc_rest_request_t **out_request);

dcc_status_t dcc_task9_command_body(
    const dcc_application_command_builder_t *command, int create,
    int guild_scope, char **out);
dcc_status_t dcc_task9_command_array_body(
    const dcc_application_command_builder_t *commands, size_t count,
    int guild_scope, char **out);
dcc_status_t dcc_task9_command_query(
    const dcc_rest_command_list_query_t *query, dcc_rest_buffer_t *out);
dcc_status_t dcc_task9_scheduled_query(
    const dcc_rest_scheduled_event_query_t *query, dcc_rest_buffer_t *out);
dcc_status_t dcc_task9_scheduled_users_query(
    const dcc_rest_scheduled_event_users_query_t *query,
    dcc_rest_buffer_t *out);
dcc_status_t dcc_task9_emoji_body(
    const dcc_emoji_params_t *params, uint64_t allowed, uint64_t required,
    char **out);
dcc_status_t dcc_task9_template_body(
    const dcc_template_params_t *params, int create, char **out);
dcc_status_t dcc_task9_stage_body(
    const dcc_stage_instance_params_t *params, int create, char **out);
dcc_status_t dcc_task9_scheduled_body(
    const dcc_scheduled_event_params_t *params, int create, char **out);
dcc_status_t dcc_task9_sticker_modify_body(
    const dcc_guild_sticker_params_t *params, char **out);
dcc_status_t dcc_task9_sticker_multipart(
    const dcc_guild_sticker_params_t *params, char **out, size_t *out_len,
    const char **out_content_type);
dcc_status_t dcc_task9_permissions_body(
    const dcc_guild_command_permissions_params_t *params, char **out);

#endif
