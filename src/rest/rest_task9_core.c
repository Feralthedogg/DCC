#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_multipart_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"
#include "internal/rest/dcc_rest_task9_internal.h"
#include "internal/objects/dcc_application_command_builder_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

#include <stdlib.h>
#include <string.h>

static dcc_status_t dcc_task9_record(
    const void *value, uint32_t version, uint64_t known,
    dcc_endpoint_record_view_t *out) {
  return dcc_endpoint_record_read(value, offsetof(dcc_endpoint_record_view_t, version),
                                  offsetof(dcc_endpoint_record_view_t, present),
                                  sizeof(dcc_endpoint_record_view_t), version,
                                  known, out);
}

dcc_status_t dcc_task9_submit_owned(
    dcc_client_t *client, const char *operation, dcc_rest_method_t method,
    const char *path, char *body, size_t body_len, const char *content_type,
    const dcc_rest_call_options_t *options, dcc_endpoint_auth_policy_t auth,
    dcc_endpoint_audit_policy_t audit, dcc_rest_request_t **out_request) {
  dcc_rest_call_options_t resolved;
  dcc_status_t status = dcc_endpoint_prepare_policy(
      options, out_request, &resolved, auth, audit);
  if (status == DCC_OK && (client == NULL || path == NULL || path[0] == '\0'))
    status = DCC_ERR_INVALID_ARG;
  if (status == DCC_OK) {
    dcc_endpoint_body_t payload = {body, body_len, content_type};
    status = dcc_endpoint_submit_named(
        client, operation, method, path, body != NULL ? &payload : NULL,
        &resolved, DCC_ENDPOINT_PATH_PUBLIC, out_request);
  }
  free(body);
  return status;
}

dcc_status_t dcc_task9_command_body(
    const dcc_application_command_builder_t *command, int create,
    int guild_scope, char **out) {
  if (out == NULL) return DCC_ERR_INVALID_ARG;
  *out = NULL;
  dcc_builder_abi_view_t view;
  dcc_status_t status = dcc_application_command_builder_abi_validate(command, &view);
  if (status != DCC_OK) return status;
  if (create) {
    status = dcc_application_command_builder_validate_create(command);
    if (status != DCC_OK) return status;
  } else if (view.present == 0U ||
             (view.present & DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE) != 0U) {
    return DCC_ERR_INVALID_ARG;
  }
  if (guild_scope && (view.present &
      (DCC_APPLICATION_COMMAND_BUILDER_PRESENT_CONTEXTS |
       DCC_APPLICATION_COMMAND_BUILDER_PRESENT_CONTEXTS_JSON |
       DCC_APPLICATION_COMMAND_BUILDER_PRESENT_INTEGRATION_TYPES |
       DCC_APPLICATION_COMMAND_BUILDER_PRESENT_INTEGRATION_TYPES_JSON)) != 0U)
    return DCC_ERR_INVALID_ARG;
  return dcc_application_command_builder_build_json(command, out);
}

dcc_status_t dcc_task9_command_array_body(
    const dcc_application_command_builder_t *commands, size_t count,
    int guild_scope, char **out) {
  if (out == NULL || (count != 0U && commands == NULL))
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (count != 0U) {
    size_t stride = 0U;
    dcc_status_t status = dcc_application_command_builder_array_begin(
        commands, count, &stride);
    if (status != DCC_OK) return status;
    for (size_t i = 0U; i < count; ++i) {
      const dcc_application_command_builder_t *command =
          (const dcc_application_command_builder_t *)
              dcc_builder_abi_array_at(commands, stride, i);
      dcc_builder_abi_view_t view;
      status = dcc_application_command_builder_abi_validate(command, &view);
      if (status != DCC_OK || view.size != stride ||
          dcc_application_command_builder_validate_create(command) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
      if (guild_scope && (view.present &
          (DCC_APPLICATION_COMMAND_BUILDER_PRESENT_CONTEXTS |
           DCC_APPLICATION_COMMAND_BUILDER_PRESENT_CONTEXTS_JSON |
           DCC_APPLICATION_COMMAND_BUILDER_PRESENT_INTEGRATION_TYPES |
           DCC_APPLICATION_COMMAND_BUILDER_PRESENT_INTEGRATION_TYPES_JSON)) != 0U)
        return DCC_ERR_INVALID_ARG;
    }
  }
  return dcc_application_command_builder_build_array_json(commands, count, out);
}

dcc_status_t dcc_task9_command_query(
    const dcc_rest_command_list_query_t *query, dcc_rest_buffer_t *out) {
  if (out == NULL) return DCC_ERR_INVALID_ARG;
  if (query == NULL) return DCC_OK;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = dcc_task9_record(query, DCC_REST_COMMAND_LIST_QUERY_VERSION,
      DCC_REST_COMMAND_LIST_QUERY_PRESENT_WITH_LOCALIZATIONS, &view);
  if (status != DCC_OK) return status;
  if ((view.present & DCC_REST_COMMAND_LIST_QUERY_PRESENT_WITH_LOCALIZATIONS) != 0U) {
    if (query->with_localizations > 1U) return DCC_ERR_INVALID_ARG;
    status = dcc_rest_query_append_bool(out, "with_localizations", query->with_localizations);
  }
  return status;
}

dcc_status_t dcc_task9_scheduled_query(
    const dcc_rest_scheduled_event_query_t *query, dcc_rest_buffer_t *out) {
  if (out == NULL) return DCC_ERR_INVALID_ARG;
  if (query == NULL) return DCC_OK;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = dcc_task9_record(query,
      DCC_REST_SCHEDULED_EVENT_QUERY_VERSION,
      DCC_REST_SCHEDULED_EVENT_QUERY_PRESENT_WITH_USER_COUNT, &view);
  if (status != DCC_OK) return status;
  if ((view.present & DCC_REST_SCHEDULED_EVENT_QUERY_PRESENT_WITH_USER_COUNT) != 0U) {
    if (query->with_user_count > 1U) return DCC_ERR_INVALID_ARG;
    status = dcc_rest_query_append_bool(out, "with_user_count", query->with_user_count);
  }
  return status;
}

dcc_status_t dcc_task9_scheduled_users_query(
    const dcc_rest_scheduled_event_users_query_t *query,
    dcc_rest_buffer_t *out) {
  if (out == NULL) return DCC_ERR_INVALID_ARG;
  if (query == NULL) return DCC_OK;
  const uint64_t known = DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_LIMIT |
      DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_WITH_MEMBER |
      DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_BEFORE |
      DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_AFTER;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = dcc_task9_record(query,
      DCC_REST_SCHEDULED_EVENT_USERS_QUERY_VERSION, known, &view);
  if (status != DCC_OK ||
      ((view.present & DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_BEFORE) != 0U &&
       (view.present & DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_AFTER) != 0U))
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_LIMIT) != 0U) {
    if (query->limit < 1U || query->limit > 100U) return DCC_ERR_INVALID_ARG;
    status = dcc_rest_query_append_u64_value(out, "limit", query->limit);
  }
  if (status == DCC_OK && (view.present & DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_WITH_MEMBER) != 0U) {
    if (query->with_member > 1U) return DCC_ERR_INVALID_ARG;
    status = dcc_rest_query_append_bool(out, "with_member", query->with_member);
  }
  if (status == DCC_OK && (view.present & DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_BEFORE) != 0U)
    status = query->before == 0U ? DCC_ERR_INVALID_ARG : dcc_rest_query_append_u64(out, "before", query->before);
  if (status == DCC_OK && (view.present & DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_AFTER) != 0U)
    status = query->after == 0U ? DCC_ERR_INVALID_ARG : dcc_rest_query_append_u64(out, "after", query->after);
  return status;
}

static dcc_status_t dcc_task9_json_finish(dcc_rest_buffer_t *body, char **out,
                                          dcc_status_t status) {
  if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(body, "}");
  if (status == DCC_OK) { *out = body->data; return DCC_OK; }
  dcc_rest_buffer_deinit(body);
  return status;
}

dcc_status_t dcc_task9_emoji_body(
    const dcc_emoji_params_t *params, uint64_t allowed, uint64_t required,
    char **out) {
  if (out == NULL) return DCC_ERR_INVALID_ARG;
  *out = NULL;
  dcc_endpoint_record_view_t view;
  const uint64_t known = DCC_EMOJI_PARAMS_PRESENT_NAME |
      DCC_EMOJI_PARAMS_PRESENT_IMAGE | DCC_EMOJI_PARAMS_PRESENT_ROLES;
  dcc_status_t status = dcc_task9_record(params, DCC_EMOJI_PARAMS_VERSION, known, &view);
  if (status != DCC_OK || (view.present & ~allowed) != 0U ||
      (view.present & required) != required ||
      (params->nulls & ~DCC_EMOJI_PARAMS_PRESENT_ROLES) != 0U ||
      (params->nulls & ~view.present) != 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_EMOJI_PARAMS_PRESENT_NAME) != 0U &&
      (params->name == NULL || params->name[0] == '\0')) return DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_EMOJI_PARAMS_PRESENT_IMAGE) != 0U &&
      (params->image == NULL || strncmp(params->image, "data:image/", 11U) != 0 ||
       strlen(params->image) > 349600U)) return DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_EMOJI_PARAMS_PRESENT_ROLES) != 0U &&
      (params->nulls & DCC_EMOJI_PARAMS_PRESENT_ROLES) == 0U &&
      params->role_count != 0U && params->roles == NULL) return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0}; int first = 1;
  status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK && (view.present & DCC_EMOJI_PARAMS_PRESENT_NAME) != 0U)
    status = dcc_rest_json_append_string_member(&body, &first, "name", params->name);
  if (status == DCC_OK && (view.present & DCC_EMOJI_PARAMS_PRESENT_IMAGE) != 0U)
    status = dcc_rest_json_append_string_member(&body, &first, "image", params->image);
  if (status == DCC_OK && (view.present & DCC_EMOJI_PARAMS_PRESENT_ROLES) != 0U) {
    if ((params->nulls & DCC_EMOJI_PARAMS_PRESENT_ROLES) != 0U)
      status = dcc_rest_json_append_raw_member(&body, &first, "roles", "null");
    else status = dcc_rest_json_append_snowflake_string_array_member(
        &body, &first, "roles", params->roles, params->role_count);
  }
  return dcc_task9_json_finish(&body, out, status);
}

dcc_status_t dcc_task9_template_body(
    const dcc_template_params_t *params, int create, char **out) {
  if (out == NULL) return DCC_ERR_INVALID_ARG; *out = NULL;
  const uint64_t known = DCC_TEMPLATE_PARAMS_PRESENT_NAME |
      DCC_TEMPLATE_PARAMS_PRESENT_DESCRIPTION;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = dcc_task9_record(params, DCC_TEMPLATE_PARAMS_VERSION, known, &view);
  if (status != DCC_OK || (create && (view.present & DCC_TEMPLATE_PARAMS_PRESENT_NAME) == 0U) ||
      (!create && view.present == 0U) || (params->nulls & ~DCC_TEMPLATE_PARAMS_PRESENT_DESCRIPTION) != 0U ||
      (params->nulls & ~view.present) != 0U) return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_TEMPLATE_PARAMS_PRESENT_NAME) != 0U &&
      (params->name == NULL || params->name[0] == '\0' || strlen(params->name) > 100U)) return DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_TEMPLATE_PARAMS_PRESENT_DESCRIPTION) != 0U &&
      (params->nulls & DCC_TEMPLATE_PARAMS_PRESENT_DESCRIPTION) == 0U &&
      (params->description == NULL || strlen(params->description) > 120U)) return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0}; int first = 1; status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK && (view.present & DCC_TEMPLATE_PARAMS_PRESENT_NAME) != 0U)
    status = dcc_rest_json_append_string_member(&body, &first, "name", params->name);
  if (status == DCC_OK && (view.present & DCC_TEMPLATE_PARAMS_PRESENT_DESCRIPTION) != 0U)
    status = (params->nulls & DCC_TEMPLATE_PARAMS_PRESENT_DESCRIPTION) != 0U
        ? dcc_rest_json_append_raw_member(&body, &first, "description", "null")
        : dcc_rest_json_append_string_member(&body, &first, "description", params->description);
  return dcc_task9_json_finish(&body, out, status);
}

dcc_status_t dcc_task9_stage_body(
    const dcc_stage_instance_params_t *params, int create, char **out) {
  if (out == NULL) return DCC_ERR_INVALID_ARG; *out = NULL;
  const uint64_t known = DCC_STAGE_INSTANCE_PARAMS_PRESENT_CHANNEL_ID |
      DCC_STAGE_INSTANCE_PARAMS_PRESENT_TOPIC |
      DCC_STAGE_INSTANCE_PARAMS_PRESENT_PRIVACY_LEVEL |
      DCC_STAGE_INSTANCE_PARAMS_PRESENT_SEND_START_NOTIFICATION |
      DCC_STAGE_INSTANCE_PARAMS_PRESENT_GUILD_SCHEDULED_EVENT_ID;
  const uint64_t allowed = create ? known : DCC_STAGE_INSTANCE_PARAMS_PRESENT_PRIVACY_LEVEL;
  const uint64_t required = create ? (DCC_STAGE_INSTANCE_PARAMS_PRESENT_CHANNEL_ID |
      DCC_STAGE_INSTANCE_PARAMS_PRESENT_TOPIC) : 0U;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = dcc_task9_record(params, DCC_STAGE_INSTANCE_PARAMS_VERSION, known, &view);
  if (status != DCC_OK || (view.present & ~allowed) != 0U ||
      (view.present & required) != required || (!create && view.present == 0U))
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_STAGE_INSTANCE_PARAMS_PRESENT_TOPIC) != 0U &&
      (params->topic == NULL || params->topic[0] == '\0' || strlen(params->topic) > 120U)) return DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_STAGE_INSTANCE_PARAMS_PRESENT_PRIVACY_LEVEL) != 0U &&
      params->privacy_level != DCC_STAGE_PRIVACY_GUILD_ONLY) return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0}; int first = 1; status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK && (view.present & DCC_STAGE_INSTANCE_PARAMS_PRESENT_CHANNEL_ID) != 0U)
    status = dcc_rest_json_append_u64_string_member(&body, &first, "channel_id", params->channel_id);
  if (status == DCC_OK && (view.present & DCC_STAGE_INSTANCE_PARAMS_PRESENT_TOPIC) != 0U)
    status = dcc_rest_json_append_string_member(&body, &first, "topic", params->topic);
  if (status == DCC_OK && (view.present & DCC_STAGE_INSTANCE_PARAMS_PRESENT_PRIVACY_LEVEL) != 0U)
    status = dcc_rest_json_append_u64_member(&body, &first, "privacy_level", params->privacy_level);
  if (status == DCC_OK && (view.present & DCC_STAGE_INSTANCE_PARAMS_PRESENT_SEND_START_NOTIFICATION) != 0U)
    status = dcc_rest_json_append_bool_member(&body, &first, "send_start_notification", params->send_start_notification);
  if (status == DCC_OK && (view.present & DCC_STAGE_INSTANCE_PARAMS_PRESENT_GUILD_SCHEDULED_EVENT_ID) != 0U)
    status = dcc_rest_json_append_u64_string_member(&body, &first, "guild_scheduled_event_id", params->guild_scheduled_event_id);
  return dcc_task9_json_finish(&body, out, status);
}

dcc_status_t dcc_task9_scheduled_body(
    const dcc_scheduled_event_params_t *params, int create, char **out) {
  if (out == NULL) return DCC_ERR_INVALID_ARG; *out = NULL;
  const uint64_t known = DCC_SCHEDULED_EVENT_PARAMS_PRESENT_CHANNEL_ID |
      DCC_SCHEDULED_EVENT_PARAMS_PRESENT_NAME |
      DCC_SCHEDULED_EVENT_PARAMS_PRESENT_DESCRIPTION |
      DCC_SCHEDULED_EVENT_PARAMS_PRESENT_IMAGE |
      DCC_SCHEDULED_EVENT_PARAMS_PRESENT_SCHEDULED_START_TIME |
      DCC_SCHEDULED_EVENT_PARAMS_PRESENT_SCHEDULED_END_TIME |
      DCC_SCHEDULED_EVENT_PARAMS_PRESENT_LOCATION |
      DCC_SCHEDULED_EVENT_PARAMS_PRESENT_PRIVACY_LEVEL |
      DCC_SCHEDULED_EVENT_PARAMS_PRESENT_STATUS |
      DCC_SCHEDULED_EVENT_PARAMS_PRESENT_ENTITY_TYPE;
  const uint64_t nullable = DCC_SCHEDULED_EVENT_PARAMS_PRESENT_CHANNEL_ID |
      DCC_SCHEDULED_EVENT_PARAMS_PRESENT_DESCRIPTION |
      DCC_SCHEDULED_EVENT_PARAMS_PRESENT_IMAGE;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = dcc_task9_record(
      params, DCC_SCHEDULED_EVENT_PARAMS_VERSION, known, &view);
  if (status != DCC_OK || (params->nulls & ~nullable) != 0U ||
      (params->nulls & ~view.present) != 0U ||
      (create && (view.present &
          (DCC_SCHEDULED_EVENT_PARAMS_PRESENT_NAME |
           DCC_SCHEDULED_EVENT_PARAMS_PRESENT_SCHEDULED_START_TIME |
           DCC_SCHEDULED_EVENT_PARAMS_PRESENT_PRIVACY_LEVEL |
           DCC_SCHEDULED_EVENT_PARAMS_PRESENT_ENTITY_TYPE)) !=
          (DCC_SCHEDULED_EVENT_PARAMS_PRESENT_NAME |
           DCC_SCHEDULED_EVENT_PARAMS_PRESENT_SCHEDULED_START_TIME |
           DCC_SCHEDULED_EVENT_PARAMS_PRESENT_PRIVACY_LEVEL |
           DCC_SCHEDULED_EVENT_PARAMS_PRESENT_ENTITY_TYPE)) ||
      (!create && view.present == 0U))
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_SCHEDULED_EVENT_PARAMS_PRESENT_NAME) != 0U &&
      (params->name == NULL || params->name[0] == '\0' || strlen(params->name) > 100U))
    return DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_SCHEDULED_EVENT_PARAMS_PRESENT_PRIVACY_LEVEL) != 0U &&
      params->privacy_level != DCC_SCHEDULED_EVENT_PRIVACY_GUILD_ONLY)
    return DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_SCHEDULED_EVENT_PARAMS_PRESENT_ENTITY_TYPE) != 0U &&
      (params->entity_type < DCC_SCHEDULED_EVENT_ENTITY_STAGE_INSTANCE ||
       params->entity_type > DCC_SCHEDULED_EVENT_ENTITY_EXTERNAL))
    return DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_SCHEDULED_EVENT_PARAMS_PRESENT_STATUS) != 0U &&
      (params->status < DCC_SCHEDULED_EVENT_STATUS_SCHEDULED ||
       params->status > DCC_SCHEDULED_EVENT_STATUS_CANCELLED))
    return DCC_ERR_INVALID_ARG;
  if (create && params->entity_type == DCC_SCHEDULED_EVENT_ENTITY_EXTERNAL) {
    if ((view.present & DCC_SCHEDULED_EVENT_PARAMS_PRESENT_CHANNEL_ID) != 0U &&
        (params->nulls & DCC_SCHEDULED_EVENT_PARAMS_PRESENT_CHANNEL_ID) == 0U)
      return DCC_ERR_INVALID_ARG;
    if ((view.present & DCC_SCHEDULED_EVENT_PARAMS_PRESENT_LOCATION) == 0U ||
        params->location == NULL || params->location[0] == '\0' ||
        (view.present & DCC_SCHEDULED_EVENT_PARAMS_PRESENT_SCHEDULED_END_TIME) == 0U)
      return DCC_ERR_INVALID_ARG;
  } else if (create &&
      ((view.present & DCC_SCHEDULED_EVENT_PARAMS_PRESENT_CHANNEL_ID) == 0U ||
       (params->nulls & DCC_SCHEDULED_EVENT_PARAMS_PRESENT_CHANNEL_ID) != 0U ||
       params->channel_id == 0U ||
       (view.present & DCC_SCHEDULED_EVENT_PARAMS_PRESENT_LOCATION) != 0U)) {
    return DCC_ERR_INVALID_ARG;
  }
  dcc_rest_buffer_t body = {0}; int first = 1;
  status = dcc_rest_buffer_append_cstr(&body, "{");
#define SCHEDULED_HAS(bit_) ((view.present & (bit_)) != 0U)
  if (status == DCC_OK && SCHEDULED_HAS(DCC_SCHEDULED_EVENT_PARAMS_PRESENT_NAME))
    status = dcc_rest_json_append_string_member(&body, &first, "name", params->name);
  if (status == DCC_OK && SCHEDULED_HAS(DCC_SCHEDULED_EVENT_PARAMS_PRESENT_DESCRIPTION))
    status = (params->nulls & DCC_SCHEDULED_EVENT_PARAMS_PRESENT_DESCRIPTION) != 0U
        ? dcc_rest_json_append_raw_member(&body, &first, "description", "null")
        : dcc_rest_json_append_string_member(&body, &first, "description", params->description);
  if (status == DCC_OK && SCHEDULED_HAS(DCC_SCHEDULED_EVENT_PARAMS_PRESENT_IMAGE))
    status = (params->nulls & DCC_SCHEDULED_EVENT_PARAMS_PRESENT_IMAGE) != 0U
        ? dcc_rest_json_append_raw_member(&body, &first, "image", "null")
        : dcc_rest_json_append_string_member(&body, &first, "image", params->image);
  if (status == DCC_OK && SCHEDULED_HAS(DCC_SCHEDULED_EVENT_PARAMS_PRESENT_SCHEDULED_START_TIME))
    status = dcc_rest_json_append_string_member(&body, &first, "scheduled_start_time", params->scheduled_start_time);
  if (status == DCC_OK && SCHEDULED_HAS(DCC_SCHEDULED_EVENT_PARAMS_PRESENT_SCHEDULED_END_TIME))
    status = dcc_rest_json_append_string_member(&body, &first, "scheduled_end_time", params->scheduled_end_time);
  if (status == DCC_OK && SCHEDULED_HAS(DCC_SCHEDULED_EVENT_PARAMS_PRESENT_CHANNEL_ID))
    status = (params->nulls & DCC_SCHEDULED_EVENT_PARAMS_PRESENT_CHANNEL_ID) != 0U
        ? dcc_rest_json_append_raw_member(&body, &first, "channel_id", "null")
        : dcc_rest_json_append_u64_string_member(&body, &first, "channel_id", params->channel_id);
  if (status == DCC_OK && SCHEDULED_HAS(DCC_SCHEDULED_EVENT_PARAMS_PRESENT_PRIVACY_LEVEL))
    status = dcc_rest_json_append_u64_member(&body, &first, "privacy_level", params->privacy_level);
  if (status == DCC_OK && SCHEDULED_HAS(DCC_SCHEDULED_EVENT_PARAMS_PRESENT_ENTITY_TYPE))
    status = dcc_rest_json_append_u64_member(&body, &first, "entity_type", params->entity_type);
  if (status == DCC_OK && SCHEDULED_HAS(DCC_SCHEDULED_EVENT_PARAMS_PRESENT_STATUS))
    status = dcc_rest_json_append_u64_member(&body, &first, "status", params->status);
  if (status == DCC_OK && SCHEDULED_HAS(DCC_SCHEDULED_EVENT_PARAMS_PRESENT_LOCATION)) {
    dcc_rest_buffer_t metadata = {0}; int metadata_first = 1;
    status = dcc_rest_buffer_append_cstr(&metadata, "{");
    if (status == DCC_OK)
      status = dcc_rest_json_append_string_member(
          &metadata, &metadata_first, "location", params->location);
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&metadata, "}");
    if (status == DCC_OK)
      status = dcc_rest_json_append_raw_member(
          &body, &first, "entity_metadata", metadata.data);
    dcc_rest_buffer_deinit(&metadata);
  }
#undef SCHEDULED_HAS
  return dcc_task9_json_finish(&body, out, status);
}

dcc_status_t dcc_task9_sticker_modify_body(
    const dcc_guild_sticker_params_t *params, char **out) {
  if (out == NULL) return DCC_ERR_INVALID_ARG; *out = NULL;
  if (params == NULL || params->size < sizeof(dcc_endpoint_record_view_t) ||
      params->version != DCC_GUILD_STICKER_PARAMS_VERSION ||
      (params->present & DCC_GUILD_STICKER_PARAMS_PRESENT_FILE) != 0U ||
      params->present == 0U) return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0}; int first = 1;
  dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK && (params->present & DCC_GUILD_STICKER_PARAMS_PRESENT_NAME) != 0U)
    status = dcc_rest_json_append_string_member(&body, &first, "name", params->name);
  if (status == DCC_OK && (params->present & DCC_GUILD_STICKER_PARAMS_PRESENT_DESCRIPTION) != 0U)
    status = (params->nulls & DCC_GUILD_STICKER_PARAMS_PRESENT_DESCRIPTION) != 0U
      ? dcc_rest_json_append_raw_member(&body, &first, "description", "null")
      : dcc_rest_json_append_string_member(&body, &first, "description", params->description);
  if (status == DCC_OK && (params->present & DCC_GUILD_STICKER_PARAMS_PRESENT_TAGS) != 0U)
    status = dcc_rest_json_append_string_member(&body, &first, "tags", params->tags);
  return dcc_task9_json_finish(&body, out, status);
}

dcc_status_t dcc_task9_sticker_multipart(
    const dcc_guild_sticker_params_t *params, char **out, size_t *out_len,
    const char **out_content_type) {
  if (out == NULL || out_len == NULL || out_content_type == NULL) return DCC_ERR_INVALID_ARG;
  *out = NULL; *out_len = 0U; *out_content_type = NULL;
  const uint64_t required = DCC_GUILD_STICKER_PARAMS_PRESENT_NAME |
      DCC_GUILD_STICKER_PARAMS_PRESENT_TAGS | DCC_GUILD_STICKER_PARAMS_PRESENT_FILE;
  if (params == NULL || params->size < sizeof(*params) ||
      params->version != DCC_GUILD_STICKER_PARAMS_VERSION ||
      (params->present & required) != required || params->file.data == NULL ||
      params->file.data_len > 512U * 1024U || params->file.filename == NULL ||
      params->file.content_type == NULL) return DCC_ERR_INVALID_ARG;
  dcc_rest_multipart_field_t fields[3]; size_t count = 0U;
  fields[count++] = (dcc_rest_multipart_field_t){"name", params->name};
  if ((params->present & DCC_GUILD_STICKER_PARAMS_PRESENT_DESCRIPTION) != 0U)
    fields[count++] = (dcc_rest_multipart_field_t){"description", params->description != NULL ? params->description : ""};
  fields[count++] = (dcc_rest_multipart_field_t){"tags", params->tags};
  dcc_status_t status = dcc_rest_build_multipart_body(fields, count, &params->file, 1U, out, out_len);
  if (status == DCC_OK) *out_content_type = DCC_REST_MULTIPART_CONTENT_TYPE;
  return status;
}

dcc_status_t dcc_task9_permissions_body(
    const dcc_guild_command_permissions_params_t *params, char **out) {
  if (out == NULL) return DCC_ERR_INVALID_ARG; *out = NULL;
  if (params == NULL || params->size < sizeof(*params) ||
      params->version != DCC_GUILD_COMMAND_PERMISSIONS_PARAMS_VERSION ||
      params->present != DCC_GUILD_COMMAND_PERMISSIONS_PARAMS_PRESENT_PERMISSIONS ||
      params->permission_count > 100U ||
      (params->permission_count != 0U && params->permissions == NULL)) return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0}; dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{\"permissions\":[");
  for (size_t i = 0U; i < params->permission_count && status == DCC_OK; ++i) {
    const dcc_application_command_permission_t *p = &params->permissions[i];
    if (p->id == 0U || p->type < 1U || p->type > 3U || p->permission > 1U) { status = DCC_ERR_INVALID_ARG; break; }
    if (i != 0U) status = dcc_rest_buffer_append_cstr(&body, ",");
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&body, "{\"id\":\"");
    if (status == DCC_OK) status = dcc_rest_buffer_append_u64_text(&body, p->id);
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&body, "\",\"type\":");
    if (status == DCC_OK) status = dcc_rest_buffer_append_u64_text(&body, p->type);
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&body, p->permission ? ",\"permission\":true}" : ",\"permission\":false}");
  }
  if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&body, "]}");
  if (status == DCC_OK) { *out = body.data; return DCC_OK; }
  dcc_rest_buffer_deinit(&body); return status;
}
