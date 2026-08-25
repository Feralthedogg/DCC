#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_task10_internal.h"

#include <dcc/rest/official_surface.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static dcc_status_t record_read(const void *value, size_t mandatory,
                                uint64_t known,
                                dcc_endpoint_record_view_t *view) {
  return dcc_endpoint_record_read(value,
                                  offsetof(dcc_endpoint_record_view_t, version),
                                  offsetof(dcc_endpoint_record_view_t, present),
                                  mandatory, 1U, known, view);
}

static dcc_status_t finish_object(dcc_rest_buffer_t *body, char **out) {
  dcc_status_t status = dcc_rest_buffer_append_cstr(body, "}");
  if (status == DCC_OK) {
    *out = body->data;
    return DCC_OK;
  }
  dcc_rest_buffer_deinit(body);
  return status;
}

static dcc_status_t map_json(const dcc_rest_string_map_t *map, char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (map == NULL)
    return DCC_ERR_INVALID_ARG;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = record_read(map, sizeof(*map), 0U, &view);
  if (status != DCC_OK || (map->entry_count != 0U && map->entries == NULL))
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  status = dcc_rest_buffer_append_cstr(&body, "{");
  for (size_t i = 0U; status == DCC_OK && i < map->entry_count; ++i) {
    const dcc_rest_string_map_entry_t *entry = &map->entries[i];
    if (entry->key == NULL || entry->value == NULL) {
      status = DCC_ERR_INVALID_ARG;
      break;
    }
    for (size_t j = 0U; j < i; ++j) {
      if (strcmp(entry->key, map->entries[j].key) == 0) {
        status = DCC_ERR_INVALID_ARG;
        break;
      }
    }
    if (status == DCC_OK)
      status = dcc_rest_json_append_string_member(&body, &first, entry->key,
                                                  entry->value);
  }
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return finish_object(&body, out);
}

static dcc_status_t append_map_member(dcc_rest_buffer_t *body, int *first,
                                      const char *name,
                                      const dcc_rest_string_map_t *map) {
  if (map == NULL)
    return dcc_rest_json_append_raw_member(body, first, name, "null");
  char *json = NULL;
  dcc_status_t status = map_json(map, &json);
  if (status == DCC_OK)
    status = dcc_rest_json_append_raw_member(body, first, name, json);
  free(json);
  return status;
}

static dcc_status_t append_install(dcc_rest_buffer_t *body, int *first,
                                   const char *name,
                                   const dcc_application_install_params_t *p) {
  dcc_endpoint_record_view_t view;
  dcc_status_t status = record_read(p, sizeof(*p), UINT64_C(3), &view);
  if (status != DCC_OK || view.present != UINT64_C(3) ||
      p->permissions == NULL || p->permissions[0] == '\0' ||
      (p->scope_count != 0U && p->scopes == NULL))
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t nested = {0};
  int nested_first = 1;
  status = dcc_rest_buffer_append_cstr(&nested, "{");
  if (status == DCC_OK)
    status = dcc_rest_json_append_string_array_member(
        &nested, &nested_first, "scopes", p->scopes, p->scope_count);
  if (status == DCC_OK)
    status = dcc_rest_json_append_string_member(&nested, &nested_first,
                                                "permissions", p->permissions);
  if (status == DCC_OK)
    status = dcc_rest_buffer_append_cstr(&nested, "}");
  if (status == DCC_OK)
    status = dcc_rest_json_append_raw_member(body, first, name, nested.data);
  dcc_rest_buffer_deinit(&nested);
  return status;
}

static dcc_status_t application_body(const dcc_application_modify_params_t *p,
                                     char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  const uint64_t known = (UINT64_C(1) << 13U) - 1U;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = record_read(
      p, offsetof(dcc_application_modify_params_t, custom_install_url), known,
      &view);
  if (status != DCC_OK)
    return status;
  if ((view.present & DCC_APPLICATION_MODIFY_PRESENT_TAGS) &&
      (p->tag_count > 5U || (p->tag_count != 0U && p->tags == NULL)))
    return DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_APPLICATION_MODIFY_PRESENT_EVENT_WEBHOOKS_STATUS) &&
      p->event_webhooks_status != DCC_APPLICATION_EVENT_WEBHOOK_DISABLED &&
      p->event_webhooks_status != DCC_APPLICATION_EVENT_WEBHOOK_ENABLED)
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  status = dcc_rest_buffer_append_cstr(&body, "{");
#define APP_STRING(bit_, key_, field_)                                         \
  do {                                                                         \
    if (status == DCC_OK && (view.present & (bit_))) {                         \
      if (p->field_ == NULL)                                                   \
        status = DCC_ERR_INVALID_ARG;                                          \
      else                                                                     \
        status = dcc_rest_json_append_string_member(&body, &first, (key_),     \
                                                    p->field_);                \
    }                                                                          \
  } while (0)
  APP_STRING(DCC_APPLICATION_MODIFY_PRESENT_CUSTOM_INSTALL_URL,
             "custom_install_url", custom_install_url);
  APP_STRING(DCC_APPLICATION_MODIFY_PRESENT_DESCRIPTION, "description",
             description);
  APP_STRING(DCC_APPLICATION_MODIFY_PRESENT_ROLE_CONNECTIONS_VERIFICATION_URL,
             "role_connections_verification_url",
             role_connections_verification_url);
  if (status == DCC_OK &&
      (view.present & DCC_APPLICATION_MODIFY_PRESENT_INSTALL_PARAMS))
    status = append_install(&body, &first, "install_params", p->install_params);
  if (status == DCC_OK && (view.present & DCC_APPLICATION_MODIFY_PRESENT_FLAGS))
    status = dcc_rest_json_append_u64_member(&body, &first, "flags", p->flags);
  if (status == DCC_OK && (view.present & DCC_APPLICATION_MODIFY_PRESENT_ICON))
    status = dcc_rest_json_append_nullable_string_member(&body, &first, "icon",
                                                         p->icon);
  if (status == DCC_OK &&
      (view.present & DCC_APPLICATION_MODIFY_PRESENT_COVER_IMAGE))
    status = dcc_rest_json_append_nullable_string_member(
        &body, &first, "cover_image", p->cover_image);
  APP_STRING(DCC_APPLICATION_MODIFY_PRESENT_INTERACTIONS_ENDPOINT_URL,
             "interactions_endpoint_url", interactions_endpoint_url);
  if (status == DCC_OK && (view.present & DCC_APPLICATION_MODIFY_PRESENT_TAGS))
    status = dcc_rest_json_append_string_array_member(&body, &first, "tags",
                                                      p->tags, p->tag_count);
  APP_STRING(DCC_APPLICATION_MODIFY_PRESENT_EVENT_WEBHOOKS_URL,
             "event_webhooks_url", event_webhooks_url);
  if (status == DCC_OK &&
      (view.present & DCC_APPLICATION_MODIFY_PRESENT_EVENT_WEBHOOKS_STATUS))
    status = dcc_rest_json_append_u64_member(
        &body, &first, "event_webhooks_status", p->event_webhooks_status);
  if (status == DCC_OK &&
      (view.present & DCC_APPLICATION_MODIFY_PRESENT_EVENT_WEBHOOKS_TYPES))
    status = dcc_rest_json_append_string_array_member(
        &body, &first, "event_webhooks_types", p->event_webhooks_types,
        p->event_webhooks_type_count);
#undef APP_STRING
  if (view.present & DCC_APPLICATION_MODIFY_PRESENT_INTEGRATION_TYPES_CONFIG) {
    /* Full nested validation is intentionally performed before admission. */
    if (p->integration_type_config_count > 2U ||
        (p->integration_type_config_count != 0U &&
         p->integration_types_config == NULL))
      status = DCC_ERR_INVALID_ARG;
    if (status == DCC_OK) {
      dcc_rest_buffer_t nested = {0};
      status = dcc_rest_buffer_append_cstr(&nested, "{");
      for (size_t i = 0U;
           status == DCC_OK && i < p->integration_type_config_count; ++i) {
        const dcc_application_integration_type_config_t *config =
            (const void *)((const unsigned char *)p->integration_types_config +
                           i * p->integration_types_config[0].size);
        dcc_endpoint_record_view_t config_view;
        status =
            record_read(config, sizeof(*config), UINT64_C(1), &config_view);
        if (status != DCC_OK || config->integration_type > 1) {
          status = DCC_ERR_INVALID_ARG;
          break;
        }
        if (i != 0U)
          status = dcc_rest_buffer_append_cstr(&nested, ",");
        if (status == DCC_OK)
          status = dcc_rest_buffer_append_cstr(
              &nested, config->integration_type == 0 ? "\"0\":{" : "\"1\":{");
        int nf = 1;
        if (status == DCC_OK && (config_view.present & 1U))
          status = append_install(&nested, &nf, "oauth2_install_params",
                                  config->oauth2_install_params);
        if (status == DCC_OK)
          status = dcc_rest_buffer_append_cstr(&nested, "}");
      }
      if (status == DCC_OK)
        status = dcc_rest_buffer_append_cstr(&nested, "}");
      if (status == DCC_OK)
        status = dcc_rest_json_append_raw_member(
            &body, &first, "integration_types_config", nested.data);
      dcc_rest_buffer_deinit(&nested);
    }
  }
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return finish_object(&body, out);
}

static dcc_status_t lobby_member_spec_json(const dcc_lobby_member_spec_t *p,
                                           dcc_rest_buffer_t *body) {
  dcc_endpoint_record_view_t view;
  dcc_status_t status = record_read(
      p, offsetof(dcc_lobby_member_spec_t, user_id), UINT64_C(3), &view);
  if (status != DCC_OK || p->user_id == 0U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  int first = 1;
  status = dcc_rest_buffer_append_cstr(body, "{");
  if (status == DCC_OK)
    status =
        dcc_rest_json_append_u64_string_member(body, &first, "id", p->user_id);
  if (status == DCC_OK && (view.present & 1U))
    status = append_map_member(body, &first, "metadata", p->metadata);
  if (status == DCC_OK && (view.present & 2U))
    status = dcc_rest_json_append_u64_member(body, &first, "flags", p->flags);
  return status == DCC_OK ? dcc_rest_buffer_append_cstr(body, "}") : status;
}

static dcc_status_t lobby_body(const dcc_lobby_params_t *p, char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = record_read(p, offsetof(dcc_lobby_params_t, metadata),
                                    UINT64_C(7), &view);
  if (status != DCC_OK)
    return status;
  if ((view.present & DCC_LOBBY_PRESENT_MEMBERS) &&
      (p->member_count > 25U || (p->member_count != 0U && p->members == NULL)))
    return DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_LOBBY_PRESENT_IDLE_TIMEOUT_SECONDS) &&
      (p->idle_timeout_seconds < 5U || p->idle_timeout_seconds > 604800U))
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK && (view.present & DCC_LOBBY_PRESENT_METADATA))
    status = append_map_member(&body, &first, "metadata", p->metadata);
  if (status == DCC_OK && (view.present & DCC_LOBBY_PRESENT_MEMBERS)) {
    status = dcc_rest_json_append_member_key(&body, &first, "members");
    if (status == DCC_OK)
      status = dcc_rest_buffer_append_cstr(&body, "[");
    for (size_t i = 0U; status == DCC_OK && i < p->member_count; ++i) {
      const dcc_lobby_member_spec_t *member =
          (const void *)((const unsigned char *)p->members +
                         i * p->members[0].size);
      if (i != 0U)
        status = dcc_rest_buffer_append_cstr(&body, ",");
      if (status == DCC_OK)
        status = lobby_member_spec_json(member, &body);
    }
    if (status == DCC_OK)
      status = dcc_rest_buffer_append_cstr(&body, "]");
  }
  if (status == DCC_OK &&
      (view.present & DCC_LOBBY_PRESENT_IDLE_TIMEOUT_SECONDS))
    status = dcc_rest_json_append_u64_member(
        &body, &first, "idle_timeout_seconds", p->idle_timeout_seconds);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return finish_object(&body, out);
}

static dcc_status_t lobby_join_body(const dcc_lobby_create_or_join_params_t *p,
                                    char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = record_read(p, sizeof(*p), UINT64_C(7), &view);
  size_t scalars = 0U;
  if (status != DCC_OK || p->secret == NULL || p->secret[0] == '\0' ||
      !dcc_endpoint_utf8_scalar_count(p->secret, &scalars) || scalars > 250U)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  if ((view.present & 1U) &&
      (p->idle_timeout_seconds < 5U || p->idle_timeout_seconds > 604800U))
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK)
    status =
        dcc_rest_json_append_string_member(&body, &first, "secret", p->secret);
  if (status == DCC_OK && (view.present & 1U))
    status = dcc_rest_json_append_u64_member(
        &body, &first, "idle_timeout_seconds", p->idle_timeout_seconds);
  if (status == DCC_OK && (view.present & 2U))
    status =
        append_map_member(&body, &first, "lobby_metadata", p->lobby_metadata);
  if (status == DCC_OK && (view.present & 4U))
    status =
        append_map_member(&body, &first, "member_metadata", p->member_metadata);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return finish_object(&body, out);
}

static dcc_status_t lobby_member_body(const dcc_lobby_member_params_t *p,
                                      char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = record_read(
      p, offsetof(dcc_lobby_member_params_t, metadata), UINT64_C(7), &view);
  if (status != DCC_OK)
    return status;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK && (view.present & 1U))
    status = append_map_member(&body, &first, "metadata", p->metadata);
  if (status == DCC_OK && (view.present & 2U))
    status = dcc_rest_json_append_u64_member(&body, &first, "flags", p->flags);
  if (status == DCC_OK && (view.present & 4U))
    status = dcc_rest_json_append_nullable_string_member(
        &body, &first, "additional_name", p->additional_name);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return finish_object(&body, out);
}

static dcc_status_t lobby_bulk_body(const dcc_lobby_member_bulk_params_t *p,
                                    char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = record_read(p, sizeof(*p), 0U, &view);
  if (status != DCC_OK || p->item_count == 0U || p->item_count > 25U ||
      p->items == NULL)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  status = dcc_rest_buffer_append_cstr(&body, "{");
  int first = 1;
  if (status == DCC_OK)
    status = dcc_rest_json_append_member_key(&body, &first, "members");
  if (status == DCC_OK)
    status = dcc_rest_buffer_append_cstr(&body, "[");
  for (size_t i = 0U; status == DCC_OK && i < p->item_count; ++i) {
    const dcc_lobby_member_bulk_item_t *item =
        (const void *)((const unsigned char *)p->items + i * p->items[0].size);
    dcc_endpoint_record_view_t iv;
    status = record_read(item, offsetof(dcc_lobby_member_bulk_item_t, user_id),
                         UINT64_C(15), &iv);
    if (status != DCC_OK || item->user_id == 0U ||
        ((iv.present & 8U) && item->remove_member > 1U)) {
      status = DCC_ERR_INVALID_ARG;
      break;
    }
    if (i != 0U)
      status = dcc_rest_buffer_append_cstr(&body, ",");
    if (status == DCC_OK)
      status = dcc_rest_buffer_append_cstr(&body, "{");
    int item_first = 1;
    if (status == DCC_OK)
      status = dcc_rest_json_append_u64_string_member(&body, &item_first, "id",
                                                      item->user_id);
    if (status == DCC_OK && (iv.present & 1U))
      status =
          append_map_member(&body, &item_first, "metadata", item->metadata);
    if (status == DCC_OK && (iv.present & 2U))
      status = dcc_rest_json_append_u64_member(&body, &item_first, "flags",
                                               item->flags);
    if (status == DCC_OK && (iv.present & 4U))
      status = dcc_rest_json_append_nullable_string_member(
          &body, &item_first, "additional_name", item->additional_name);
    if (status == DCC_OK && (iv.present & 8U))
      status = dcc_rest_json_append_bool_member(
          &body, &item_first, "remove_member", item->remove_member);
    if (status == DCC_OK)
      status = dcc_rest_buffer_append_cstr(&body, "}");
  }
  if (status == DCC_OK)
    status = dcc_rest_buffer_append_cstr(&body, "]}");
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  *out = body.data;
  return DCC_OK;
}

static dcc_status_t lobby_message_body(const dcc_lobby_message_params_t *p,
                                       char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = record_read(
      p, offsetof(dcc_lobby_message_params_t, content), UINT64_C(3), &view);
  if (status != DCC_OK || p->content == NULL || p->content[0] == '\0')
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK)
    status = dcc_rest_json_append_string_member(&body, &first, "content",
                                                p->content);
  if (status == DCC_OK && (view.present & 1U))
    status = append_map_member(&body, &first, "metadata", p->metadata);
  if (status == DCC_OK && (view.present & 2U))
    status = dcc_rest_json_append_u64_member(&body, &first, "flags", p->flags);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return finish_object(&body, out);
}

static dcc_status_t
moderation_body(const dcc_lobby_message_moderation_metadata_params_t *p,
                char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = record_read(p, sizeof(*p), 0U, &view);
  if (status != DCC_OK || p->item_count > 5U ||
      (p->item_count != 0U && p->items == NULL))
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  status = dcc_rest_buffer_append_cstr(&body, "{");
  for (size_t i = 0U; status == DCC_OK && i < p->item_count; ++i) {
    if (p->items[i].key == NULL || p->items[i].value == NULL) {
      status = DCC_ERR_INVALID_ARG;
      break;
    }
    for (size_t j = 0U; j < i; ++j)
      if (strcmp(p->items[i].key, p->items[j].key) == 0)
        status = DCC_ERR_INVALID_ARG;
    if (status == DCC_OK)
      status = dcc_rest_json_append_string_member(
          &body, &first, p->items[i].key, p->items[i].value);
  }
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return finish_object(&body, out);
}

static dcc_status_t id_path_request(dcc_client_t *client, const char *operation,
                                    const char *route, dcc_rest_method_t method,
                                    dcc_endpoint_auth_policy_t auth,
                                    dcc_snowflake_t first_id,
                                    dcc_snowflake_t second_id,
                                    const dcc_rest_call_options_t *options,
                                    dcc_rest_request_t **out_request) {
  dcc_rest_call_options_t resolved;
  dcc_status_t status =
      dcc_task10_prepare(client, options, auth, 0U, out_request, &resolved);
  if (status != DCC_OK || first_id == 0U ||
      (strstr(route, "%llu") != strrchr(route, '%') && second_id == 0U))
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  char path[176];
  status = second_id != 0U ? dcc_rest_format_path(path, sizeof(path), route,
                                                  (unsigned long long)first_id,
                                                  (unsigned long long)second_id)
                           : dcc_rest_format_path(path, sizeof(path), route,
                                                  (unsigned long long)first_id);
  return status == DCC_OK
             ? dcc_task10_submit_empty(client, operation, method, path,
                                       &resolved, out_request)
             : status;
}

dcc_status_t dcc_rest_get_gateway(dcc_client_t *client,
                                  const dcc_rest_call_options_t *options,
                                  dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_NONE,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DCC_DCC_REST_GET_GATEWAY, DCC_REST_GET);
  dcc_rest_call_options_t r;
  dcc_status_t s = dcc_task10_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_NONE, 0U, out_request, &r);
  return s == DCC_OK
             ? dcc_task10_submit_empty(
                   client, "dcc_rest_get_gateway", DCC_REST_GET,
                   DCC_REST_ROUTE_DCC_DCC_REST_GET_GATEWAY, &r, out_request)
             : s;
}
dcc_status_t dcc_rest_modify_current_application(
    dcc_client_t *client, const dcc_application_modify_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_MODIFY_CURRENT_APPLICATION, DCC_REST_PATCH);
  dcc_rest_call_options_t r;
  char *b = NULL;
  dcc_status_t s = dcc_task10_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U, out_request, &r);
  if (s == DCC_OK)
    s = application_body(params, &b);
  return s == DCC_OK
             ? dcc_task10_submit(
                   client, "dcc_rest_modify_current_application",
                   DCC_REST_PATCH,
                   DCC_REST_ROUTE_DCC_DCC_REST_MODIFY_CURRENT_APPLICATION, b,
                   &r, out_request)
             : s;
}
dcc_status_t dcc_rest_get_activity_instance(
    dcc_client_t *client, dcc_snowflake_t application_id,
    const char *instance_id, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_GET_ACTIVITY_INSTANCE, DCC_REST_GET);
  dcc_rest_call_options_t r;
  dcc_status_t s = dcc_task10_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U, out_request, &r);
  char *escaped = NULL;
  char path[176];
  if (s == DCC_OK &&
      (application_id == 0U || instance_id == NULL || instance_id[0] == '\0'))
    s = DCC_ERR_INVALID_ARG;
  if (s == DCC_OK)
    s = dcc_rest_escape_path_segment(instance_id, &escaped);
  if (s == DCC_OK)
    s = dcc_rest_format_path(path, sizeof(path),
                             DCC_REST_ROUTE_DCC_DCC_REST_GET_ACTIVITY_INSTANCE,
                             (unsigned long long)application_id, escaped);
  free(escaped);
  return s == DCC_OK
             ? dcc_task10_submit_empty(client, "dcc_rest_get_activity_instance",
                                       DCC_REST_GET, path, &r, out_request)
             : s;
}
dcc_status_t
dcc_rest_get_default_soundboard_sounds(dcc_client_t *client,
                                       const dcc_rest_call_options_t *options,
                                       dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_GET_DEFAULT_SOUNDBOARD_SOUNDS, DCC_REST_GET);
  dcc_rest_call_options_t r;
  dcc_status_t s = dcc_task10_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U, out_request, &r);
  return s == DCC_OK
             ? dcc_task10_submit_empty(
                   client, "dcc_rest_get_default_soundboard_sounds",
                   DCC_REST_GET,
                   DCC_REST_ROUTE_DCC_DCC_REST_GET_DEFAULT_SOUNDBOARD_SOUNDS,
                   &r, out_request)
             : s;
}
dcc_status_t
dcc_rest_send_soundboard_sound(dcc_client_t *client, dcc_snowflake_t channel_id,
                               const dcc_soundboard_send_params_t *params,
                               const dcc_rest_call_options_t *options,
                               dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_SEND_SOUNDBOARD_SOUND, DCC_REST_POST);
  dcc_rest_call_options_t r;
  dcc_status_t s = dcc_task10_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U, out_request, &r);
  dcc_endpoint_record_view_t v;
  char path[96];
  char *b = NULL;
  if (s == DCC_OK)
    s = record_read(params, offsetof(dcc_soundboard_send_params_t, sound_id),
                    1U, &v);
  if (s == DCC_OK && (channel_id == 0U || params->sound_id == 0U ||
                      ((v.present & 1U) && params->source_guild_id == 0U)))
    s = DCC_ERR_INVALID_ARG;
  if (s == DCC_OK)
    s = dcc_rest_format_path(path, sizeof(path),
                             DCC_REST_ROUTE_DCC_DCC_REST_SEND_SOUNDBOARD_SOUND,
                             (unsigned long long)channel_id);
  if (s == DCC_OK) {
    dcc_rest_buffer_t body = {0};
    int first = 1;
    s = dcc_rest_buffer_append_cstr(&body, "{");
    if (s == DCC_OK)
      s = dcc_rest_json_append_u64_string_member(&body, &first, "sound_id",
                                                 params->sound_id);
    if (s == DCC_OK && (v.present & 1U))
      s = dcc_rest_json_append_u64_string_member(
          &body, &first, "source_guild_id", params->source_guild_id);
    if (s == DCC_OK)
      s = finish_object(&body, &b);
    else
      dcc_rest_buffer_deinit(&body);
  }
  return s == DCC_OK
             ? dcc_task10_submit(client, "dcc_rest_send_soundboard_sound",
                                 DCC_REST_POST, path, b, &r, out_request)
             : s;
}

dcc_status_t dcc_rest_get_entitlement(dcc_client_t *client,
                                      dcc_snowflake_t application_id,
                                      dcc_snowflake_t entitlement_id,
                                      const dcc_rest_call_options_t *options,
                                      dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_GET_ENTITLEMENT, DCC_REST_GET);
  return id_path_request(client, "dcc_rest_get_entitlement",
                         DCC_REST_ROUTE_DCC_DCC_REST_GET_ENTITLEMENT,
                         DCC_REST_GET, DCC_ENDPOINT_AUTH_POLICY_BOT,
                         application_id, entitlement_id, options, out_request);
}
dcc_status_t dcc_rest_delete_current_user_application_role_connection(
    dcc_client_t *client, dcc_snowflake_t application_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BEARER, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_DELETE_CURRENT_USER_APPLICATION_ROLE_CONNECTION,
      DCC_REST_DELETE);
  return id_path_request(
      client, "dcc_rest_delete_current_user_application_role_connection",
      DCC_REST_ROUTE_DCC_DCC_REST_DELETE_CURRENT_USER_APPLICATION_ROLE_CONNECTION,
      DCC_REST_DELETE, DCC_ENDPOINT_AUTH_POLICY_BEARER, application_id, 0U,
      options, out_request);
}
dcc_status_t
dcc_rest_get_sku_subscription(dcc_client_t *client, dcc_snowflake_t sku_id,
                              dcc_snowflake_t subscription_id,
                              const dcc_rest_call_options_t *options,
                              dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT_OR_BEARER, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_GET_SKU_SUBSCRIPTION, DCC_REST_GET);
  return id_path_request(client, "dcc_rest_get_sku_subscription",
                         DCC_REST_ROUTE_DCC_DCC_REST_GET_SKU_SUBSCRIPTION,
                         DCC_REST_GET, DCC_ENDPOINT_AUTH_POLICY_BOT_OR_BEARER,
                         sku_id, subscription_id, options, out_request);
}
dcc_status_t
dcc_rest_get_sku_subscriptions(dcc_client_t *client, dcc_snowflake_t sku_id,
                               const dcc_rest_sku_subscription_query_t *query,
                               const dcc_rest_call_options_t *options,
                               dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT_OR_BEARER, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_GET_SKU_SUBSCRIPTIONS, DCC_REST_GET);
  dcc_rest_call_options_t r;
  dcc_status_t s = dcc_task10_prepare(client, options,
                                      DCC_ENDPOINT_AUTH_POLICY_BOT_OR_BEARER,
                                      0U, out_request, &r);
  char base[96];
  char *query_text = NULL;
  char *path = NULL;
  if (s == DCC_OK && sku_id == 0U)
    s = DCC_ERR_INVALID_ARG;
  if (s == DCC_OK)
    s = dcc_rest_format_path(base, sizeof(base),
                             DCC_REST_ROUTE_DCC_DCC_REST_GET_SKU_SUBSCRIPTIONS,
                             (unsigned long long)sku_id);
  if (s == DCC_OK)
    s = dcc_task10_subscription_query(query, r.auth_mode, &query_text);
  if (s == DCC_OK) {
    size_t a = strlen(base), b = query_text ? strlen(query_text) : 0U;
    path = malloc(a + b + 1U);
    if (!path)
      s = DCC_ERR_NOMEM;
    else {
      memcpy(path, base, a);
      if (b)
        memcpy(path + a, query_text, b);
      path[a + b] = '\0';
    }
  }
  free(query_text);
  if (s == DCC_OK)
    s = dcc_task10_submit_empty(client, "dcc_rest_get_sku_subscriptions",
                                DCC_REST_GET, path, &r, out_request);
  free(path);
  return s;
}

#define LOBBY_SIMPLE(fn_, route_, method_, auth_, ids_)                        \
  dcc_status_t fn_ ids_ {                                                      \
    DCC_ENDPOINT_CONTRACT(auth_, DCC_ENDPOINT_AUDIT_REASON_DENIED, route_,     \
                          method_);                                            \
    return id_path_request(client, #fn_, route_, method_, auth_, lobby_id,     \
                           user_id, options, out_request);                     \
  }

dcc_status_t dcc_rest_create_lobby(dcc_client_t *client,
                                   const dcc_lobby_params_t *params,
                                   const dcc_rest_call_options_t *options,
                                   dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_CREATE_LOBBY, DCC_REST_POST);
  dcc_rest_call_options_t r;
  char *b = NULL;
  dcc_status_t s = dcc_task10_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U, out_request, &r);
  if (s == DCC_OK)
    s = lobby_body(params, &b);
  return s == DCC_OK
             ? dcc_task10_submit(client, "dcc_rest_create_lobby", DCC_REST_POST,
                                 DCC_REST_ROUTE_DCC_DCC_REST_CREATE_LOBBY, b,
                                 &r, out_request)
             : s;
}
dcc_status_t dcc_rest_create_or_join_lobby(
    dcc_client_t *client, const dcc_lobby_create_or_join_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BEARER, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_CREATE_OR_JOIN_LOBBY, DCC_REST_PUT);
  dcc_rest_call_options_t r;
  char *b = NULL;
  dcc_status_t s = dcc_task10_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BEARER,
      DCC_REST_CALL_FLAG_SENSITIVE_REQUEST_BODY, out_request, &r);
  if (s == DCC_OK)
    s = lobby_join_body(params, &b);
  return s == DCC_OK
             ? dcc_task10_submit(
                   client, "dcc_rest_create_or_join_lobby", DCC_REST_PUT,
                   DCC_REST_ROUTE_DCC_DCC_REST_CREATE_OR_JOIN_LOBBY, b, &r,
                   out_request)
             : s;
}
dcc_status_t dcc_rest_get_lobby(dcc_client_t *client, dcc_snowflake_t lobby_id,
                                const dcc_rest_call_options_t *options,
                                dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  dcc_snowflake_t user_id = 0U;
  DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT,
                        DCC_ENDPOINT_AUDIT_REASON_DENIED,
                        DCC_REST_ROUTE_DCC_DCC_REST_GET_LOBBY, DCC_REST_GET);
  return id_path_request(client, "dcc_rest_get_lobby",
                         DCC_REST_ROUTE_DCC_DCC_REST_GET_LOBBY, DCC_REST_GET,
                         DCC_ENDPOINT_AUTH_POLICY_BOT, lobby_id, user_id,
                         options, out_request);
}
dcc_status_t dcc_rest_modify_lobby(dcc_client_t *client,
                                   dcc_snowflake_t lobby_id,
                                   const dcc_lobby_params_t *params,
                                   const dcc_rest_call_options_t *options,
                                   dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_MODIFY_LOBBY, DCC_REST_PATCH);
  dcc_rest_call_options_t r;
  dcc_status_t s = dcc_task10_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U, out_request, &r);
  char path[96];
  char *b = NULL;
  if (s == DCC_OK && lobby_id == 0U)
    s = DCC_ERR_INVALID_ARG;
  if (s == DCC_OK)
    s = dcc_rest_format_path(path, sizeof(path),
                             DCC_REST_ROUTE_DCC_DCC_REST_MODIFY_LOBBY,
                             (unsigned long long)lobby_id);
  if (s == DCC_OK)
    s = lobby_body(params, &b);
  return s == DCC_OK
             ? dcc_task10_submit(client, "dcc_rest_modify_lobby",
                                 DCC_REST_PATCH, path, b, &r, out_request)
             : s;
}
dcc_status_t dcc_rest_delete_lobby(dcc_client_t *client,
                                   dcc_snowflake_t lobby_id,
                                   const dcc_rest_call_options_t *options,
                                   dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  dcc_snowflake_t user_id = 0U;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_DELETE_LOBBY, DCC_REST_DELETE);
  return id_path_request(client, "dcc_rest_delete_lobby",
                         DCC_REST_ROUTE_DCC_DCC_REST_DELETE_LOBBY,
                         DCC_REST_DELETE, DCC_ENDPOINT_AUTH_POLICY_BOT,
                         lobby_id, user_id, options, out_request);
}
dcc_status_t dcc_rest_remove_lobby_member(
    dcc_client_t *client, dcc_snowflake_t lobby_id, dcc_snowflake_t user_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_REMOVE_LOBBY_MEMBER, DCC_REST_DELETE);
  return id_path_request(client, "dcc_rest_remove_lobby_member",
                         DCC_REST_ROUTE_DCC_DCC_REST_REMOVE_LOBBY_MEMBER,
                         DCC_REST_DELETE, DCC_ENDPOINT_AUTH_POLICY_BOT,
                         lobby_id, user_id, options, out_request);
}
dcc_status_t dcc_rest_leave_lobby(dcc_client_t *client,
                                  dcc_snowflake_t lobby_id,
                                  const dcc_rest_call_options_t *options,
                                  dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  dcc_snowflake_t user_id = 0U;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BEARER, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_LEAVE_LOBBY, DCC_REST_DELETE);
  return id_path_request(client, "dcc_rest_leave_lobby",
                         DCC_REST_ROUTE_DCC_DCC_REST_LEAVE_LOBBY,
                         DCC_REST_DELETE, DCC_ENDPOINT_AUTH_POLICY_BEARER,
                         lobby_id, user_id, options, out_request);
}

dcc_status_t dcc_rest_add_lobby_member(dcc_client_t *client,
                                       dcc_snowflake_t lobby_id,
                                       dcc_snowflake_t user_id,
                                       const dcc_lobby_member_params_t *params,
                                       const dcc_rest_call_options_t *options,
                                       dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_ADD_LOBBY_MEMBER, DCC_REST_PUT);
  dcc_rest_call_options_t r;
  dcc_status_t s = dcc_task10_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U, out_request, &r);
  char path[128];
  char *b = NULL;
  if (s == DCC_OK && (lobby_id == 0U || user_id == 0U))
    s = DCC_ERR_INVALID_ARG;
  if (s == DCC_OK)
    s = dcc_rest_format_path(
        path, sizeof(path), DCC_REST_ROUTE_DCC_DCC_REST_ADD_LOBBY_MEMBER,
        (unsigned long long)lobby_id, (unsigned long long)user_id);
  if (s == DCC_OK)
    s = lobby_member_body(params, &b);
  return s == DCC_OK ? dcc_task10_submit(client, "dcc_rest_add_lobby_member",
                                         DCC_REST_PUT, path, b, &r, out_request)
                     : s;
}
dcc_status_t dcc_rest_bulk_update_lobby_members(
    dcc_client_t *client, dcc_snowflake_t lobby_id,
    const dcc_lobby_member_bulk_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_BULK_UPDATE_LOBBY_MEMBERS, DCC_REST_POST);
  dcc_rest_call_options_t r;
  dcc_status_t s = dcc_task10_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U, out_request, &r);
  char path[112];
  char *b = NULL;
  if (s == DCC_OK && lobby_id == 0U)
    s = DCC_ERR_INVALID_ARG;
  if (s == DCC_OK)
    s = dcc_rest_format_path(
        path, sizeof(path),
        DCC_REST_ROUTE_DCC_DCC_REST_BULK_UPDATE_LOBBY_MEMBERS,
        (unsigned long long)lobby_id);
  if (s == DCC_OK)
    s = lobby_bulk_body(params, &b);
  return s == DCC_OK
             ? dcc_task10_submit(client, "dcc_rest_bulk_update_lobby_members",
                                 DCC_REST_POST, path, b, &r, out_request)
             : s;
}
dcc_status_t
dcc_rest_link_lobby_channel(dcc_client_t *client, dcc_snowflake_t lobby_id,
                            const dcc_lobby_channel_link_params_t *params,
                            const dcc_rest_call_options_t *options,
                            dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BEARER, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_LINK_LOBBY_CHANNEL, DCC_REST_PATCH);
  dcc_rest_call_options_t r;
  dcc_status_t s = dcc_task10_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BEARER, 0U, out_request, &r);
  dcc_endpoint_record_view_t v;
  char path[112];
  char *b = NULL;
  if (s == DCC_OK && lobby_id == 0U)
    s = DCC_ERR_INVALID_ARG;
  if (s == DCC_OK)
    s = record_read(
        params, offsetof(dcc_lobby_channel_link_params_t, channel_id), 1U, &v);
  if (s == DCC_OK && (v.present & 1U) && params->channel_id == 0U)
    s = DCC_ERR_INVALID_ARG;
  if (s == DCC_OK)
    s = dcc_rest_format_path(path, sizeof(path),
                             DCC_REST_ROUTE_DCC_DCC_REST_LINK_LOBBY_CHANNEL,
                             (unsigned long long)lobby_id);
  if (s == DCC_OK && (v.present & 1U)) {
    dcc_rest_buffer_t body = {0};
    int first = 1;
    s = dcc_rest_buffer_append_cstr(&body, "{");
    if (s == DCC_OK)
      s = dcc_rest_json_append_u64_string_member(&body, &first, "channel_id",
                                                 params->channel_id);
    if (s == DCC_OK)
      s = finish_object(&body, &b);
    else
      dcc_rest_buffer_deinit(&body);
  }
  return s == DCC_OK
             ? (b ? dcc_task10_submit(client, "dcc_rest_link_lobby_channel",
                                      DCC_REST_PATCH, path, b, &r, out_request)
                  : dcc_task10_submit_empty(
                        client, "dcc_rest_link_lobby_channel", DCC_REST_PATCH,
                        path, &r, out_request))
             : s;
}
dcc_status_t
dcc_rest_send_lobby_message(dcc_client_t *client, dcc_snowflake_t lobby_id,
                            const dcc_lobby_message_params_t *params,
                            const dcc_rest_call_options_t *options,
                            dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BEARER, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_SEND_LOBBY_MESSAGE, DCC_REST_POST);
  dcc_rest_call_options_t r;
  dcc_status_t s = dcc_task10_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BEARER, 0U, out_request, &r);
  char path[112];
  char *b = NULL;
  if (s == DCC_OK && lobby_id == 0U)
    s = DCC_ERR_INVALID_ARG;
  if (s == DCC_OK)
    s = dcc_rest_format_path(path, sizeof(path),
                             DCC_REST_ROUTE_DCC_DCC_REST_SEND_LOBBY_MESSAGE,
                             (unsigned long long)lobby_id);
  if (s == DCC_OK)
    s = lobby_message_body(params, &b);
  return s == DCC_OK
             ? dcc_task10_submit(client, "dcc_rest_send_lobby_message",
                                 DCC_REST_POST, path, b, &r, out_request)
             : s;
}
dcc_status_t
dcc_rest_get_lobby_messages(dcc_client_t *client, dcc_snowflake_t lobby_id,
                            const dcc_rest_lobby_message_query_t *query,
                            const dcc_rest_call_options_t *options,
                            dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BEARER, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_GET_LOBBY_MESSAGES, DCC_REST_GET);
  dcc_rest_call_options_t r;
  dcc_status_t s = dcc_task10_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BEARER, 0U, out_request, &r);
  char base[112], path[144];
  dcc_endpoint_record_view_t v;
  if (s == DCC_OK && lobby_id == 0U)
    s = DCC_ERR_INVALID_ARG;
  if (s == DCC_OK)
    s = dcc_rest_format_path(base, sizeof(base),
                             DCC_REST_ROUTE_DCC_DCC_REST_GET_LOBBY_MESSAGES,
                             (unsigned long long)lobby_id);
  if (s == DCC_OK && query != NULL) {
    s = record_read(query, offsetof(dcc_rest_lobby_message_query_t, limit), 1U,
                    &v);
    if (s == DCC_OK && (v.present & 1U) &&
        (query->limit == 0U || query->limit > 200U))
      s = DCC_ERR_INVALID_ARG;
    if (s == DCC_OK && (v.present & 1U))
      s = dcc_rest_format_path(path, sizeof(path), "%s?limit=%u", base,
                               (unsigned)query->limit);
    else if (s == DCC_OK)
      memcpy(path, base, strlen(base) + 1U);
  } else if (s == DCC_OK)
    memcpy(path, base, strlen(base) + 1U);
  return s == DCC_OK
             ? dcc_task10_submit_empty(client, "dcc_rest_get_lobby_messages",
                                       DCC_REST_GET, path, &r, out_request)
             : s;
}
dcc_status_t dcc_rest_update_lobby_message_moderation_metadata(
    dcc_client_t *client, dcc_snowflake_t lobby_id, dcc_snowflake_t message_id,
    const dcc_lobby_message_moderation_metadata_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request) {
  (void)DCC_ENDPOINT_PATH_PUBLIC;
  DCC_ENDPOINT_CONTRACT(
      DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
      DCC_REST_ROUTE_DCC_DCC_REST_UPDATE_LOBBY_MESSAGE_MODERATION_METADATA,
      DCC_REST_PUT);
  dcc_rest_call_options_t r;
  dcc_status_t s = dcc_task10_prepare(
      client, options, DCC_ENDPOINT_AUTH_POLICY_BOT, 0U, out_request, &r);
  char path[144];
  char *b = NULL;
  if (s == DCC_OK && (lobby_id == 0U || message_id == 0U))
    s = DCC_ERR_INVALID_ARG;
  if (s == DCC_OK)
    s = dcc_rest_format_path(
        path, sizeof(path),
        DCC_REST_ROUTE_DCC_DCC_REST_UPDATE_LOBBY_MESSAGE_MODERATION_METADATA,
        (unsigned long long)lobby_id, (unsigned long long)message_id);
  if (s == DCC_OK)
    s = moderation_body(params, &b);
  return s == DCC_OK
             ? dcc_task10_submit(
                   client, "dcc_rest_update_lobby_message_moderation_metadata",
                   DCC_REST_PUT, path, b, &r, out_request)
             : s;
}
