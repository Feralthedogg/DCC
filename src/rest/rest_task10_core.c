#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_query_internal.h"
#include "internal/rest/dcc_rest_sensitive_internal.h"
#include "internal/rest/dcc_rest_task10_internal.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static dcc_status_t dcc_task10_record(const void *record, size_t mandatory,
                                      uint64_t known,
                                      dcc_endpoint_record_view_t *view) {
  return dcc_endpoint_record_read(record,
                                  offsetof(dcc_endpoint_record_view_t, version),
                                  offsetof(dcc_endpoint_record_view_t, present),
                                  mandatory, 1U, known, view);
}

static dcc_status_t dcc_task10_finish(dcc_rest_buffer_t *body, char **out) {
  dcc_status_t status = dcc_rest_buffer_append_cstr(body, "}");
  if (status == DCC_OK) {
    *out = body->data;
    return DCC_OK;
  }
  dcc_rest_buffer_deinit(body);
  return status;
}

dcc_status_t dcc_task10_prepare(dcc_client_t *client,
                                const dcc_rest_call_options_t *options,
                                dcc_endpoint_auth_policy_t auth,
                                uint64_t forced_flags,
                                dcc_rest_request_t **out_request,
                                dcc_rest_call_options_t *resolved) {
  dcc_status_t status = dcc_endpoint_prepare_policy(
      options, out_request, resolved, auth, DCC_ENDPOINT_AUDIT_REASON_DENIED);
  if (status != DCC_OK || client == NULL)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  resolved->flags |= forced_flags;
  return DCC_OK;
}

dcc_status_t dcc_task10_submit(dcc_client_t *client, const char *operation,
                               dcc_rest_method_t method, const char *path,
                               char *json,
                               const dcc_rest_call_options_t *resolved,
                               dcc_rest_request_t **out_request) {
  dcc_endpoint_body_t body = {json, json != NULL ? strlen(json) : 0U,
                              "application/json"};
  dcc_status_t status = dcc_endpoint_submit_named(
      client, operation, method, path, json != NULL ? &body : NULL, resolved,
      DCC_ENDPOINT_PATH_PUBLIC, out_request);
  if (json != NULL &&
      (resolved->flags & DCC_REST_CALL_FLAG_SENSITIVE_REQUEST_BODY) != 0U)
    dcc_rest_sensitive_free(json, strlen(json) + 1U);
  else
    free(json);
  return status;
}

dcc_status_t dcc_task10_submit_empty(dcc_client_t *client,
                                     const char *operation,
                                     dcc_rest_method_t method, const char *path,
                                     const dcc_rest_call_options_t *resolved,
                                     dcc_rest_request_t **out_request) {
  return dcc_endpoint_submit_named(client, operation, method, path, NULL,
                                   resolved, DCC_ENDPOINT_PATH_PUBLIC,
                                   out_request);
}

dcc_status_t
dcc_task10_build_current_user(const dcc_current_user_params_t *params,
                              char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  dcc_endpoint_record_view_t view;
  const uint64_t known = DCC_CURRENT_USER_PRESENT_USERNAME |
                         DCC_CURRENT_USER_PRESENT_AVATAR |
                         DCC_CURRENT_USER_PRESENT_BANNER;
  dcc_status_t status = dcc_task10_record(
      params, offsetof(dcc_current_user_params_t, username), known, &view);
  if (status != DCC_OK)
    return status;
  if ((view.present & DCC_CURRENT_USER_PRESENT_USERNAME) &&
      !dcc_endpoint_present_field_covered(
          &view, DCC_CURRENT_USER_PRESENT_USERNAME,
          offsetof(dcc_current_user_params_t, username),
          sizeof(params->username)))
    return DCC_ERR_INVALID_ARG;
  size_t scalars = 0U;
  if ((view.present & DCC_CURRENT_USER_PRESENT_USERNAME) &&
      (params->username == NULL ||
       !dcc_endpoint_utf8_scalar_count(params->username, &scalars) ||
       scalars < 2U || scalars > 32U))
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK && (view.present & DCC_CURRENT_USER_PRESENT_USERNAME))
    status = dcc_rest_json_append_string_member(&body, &first, "username",
                                                params->username);
  if (status == DCC_OK && (view.present & DCC_CURRENT_USER_PRESENT_AVATAR))
    status = dcc_rest_json_append_nullable_string_member(
        &body, &first, "avatar", params->avatar);
  if (status == DCC_OK && (view.present & DCC_CURRENT_USER_PRESENT_BANNER))
    status = dcc_rest_json_append_nullable_string_member(
        &body, &first, "banner", params->banner);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return dcc_task10_finish(&body, out);
}

static dcc_status_t
dcc_task10_build_voice_common(const void *record, size_t mandatory,
                              uint64_t known, dcc_snowflake_t channel_id,
                              uint8_t suppress, const char *timestamp,
                              int current, char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = dcc_task10_record(record, mandatory, known, &view);
  if (status != DCC_OK)
    return status;
  if ((view.present & DCC_VOICE_STATE_PRESENT_CHANNEL_ID) && channel_id == 0U)
    return DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_VOICE_STATE_PRESENT_SUPPRESS) && suppress > 1U)
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK && (view.present & DCC_VOICE_STATE_PRESENT_CHANNEL_ID))
    status = dcc_rest_json_append_u64_string_member(&body, &first, "channel_id",
                                                    channel_id);
  if (status == DCC_OK && (view.present & DCC_VOICE_STATE_PRESENT_SUPPRESS))
    status =
        dcc_rest_json_append_bool_member(&body, &first, "suppress", suppress);
  if (status == DCC_OK && current &&
      (view.present & DCC_VOICE_STATE_PRESENT_REQUEST_TO_SPEAK_TIMESTAMP))
    status = dcc_rest_json_append_nullable_string_member(
        &body, &first, "request_to_speak_timestamp", timestamp);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return dcc_task10_finish(&body, out);
}
dcc_status_t
dcc_task10_build_current_voice(const dcc_current_user_voice_state_params_t *p,
                               char **out) {
  return p == NULL
             ? DCC_ERR_INVALID_ARG
             : dcc_task10_build_voice_common(
                   p,
                   offsetof(dcc_current_user_voice_state_params_t, channel_id),
                   UINT64_C(7), p->channel_id, p->suppress,
                   p->request_to_speak_timestamp, 1, out);
}
dcc_status_t dcc_task10_build_user_voice(const dcc_user_voice_state_params_t *p,
                                         char **out) {
  return p == NULL ? DCC_ERR_INVALID_ARG
                   : dcc_task10_build_voice_common(
                         p, offsetof(dcc_user_voice_state_params_t, channel_id),
                         UINT64_C(3), p->channel_id, p->suppress, NULL, 0, out);
}

dcc_status_t dcc_task10_build_group_dm(const dcc_group_dm_recipient_params_t *p,
                                       char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = dcc_task10_record(p, sizeof(*p), 0U, &view);
  if (status != DCC_OK || p->access_token == NULL ||
      p->access_token[0] == '\0' || p->nick == NULL)
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK)
    status = dcc_rest_json_append_string_member(&body, &first, "access_token",
                                                p->access_token);
  if (status == DCC_OK)
    status = dcc_rest_json_append_string_member(&body, &first, "nick", p->nick);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return dcc_task10_finish(&body, out);
}

dcc_status_t
dcc_task10_build_test_entitlement(const dcc_test_entitlement_params_t *p,
                                  char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  dcc_endpoint_record_view_t view;
  dcc_status_t status = dcc_task10_record(p, sizeof(*p), 0U, &view);
  if (status != DCC_OK || p->sku_id == 0U || p->owner_id == 0U ||
      (p->owner_type != 1U && p->owner_type != 2U))
    return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK)
    status = dcc_rest_json_append_u64_string_member(&body, &first, "sku_id",
                                                    p->sku_id);
  if (status == DCC_OK)
    status = dcc_rest_json_append_u64_string_member(&body, &first, "owner_id",
                                                    p->owner_id);
  if (status == DCC_OK)
    status = dcc_rest_json_append_u64_member(&body, &first, "owner_type",
                                             p->owner_type);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return dcc_task10_finish(&body, out);
}

static dcc_status_t dcc_task10_query_sep(dcc_rest_buffer_t *q, int *first,
                                         const char *key) {
  dcc_status_t s = dcc_rest_buffer_append_cstr(q, *first ? "?" : "&");
  *first = 0;
  if (s == DCC_OK)
    s = dcc_rest_buffer_append_cstr(q, key);
  return s == DCC_OK ? dcc_rest_buffer_append_cstr(q, "=") : s;
}
static dcc_status_t dcc_task10_query_u64(dcc_rest_buffer_t *q, int *first,
                                         const char *key, uint64_t value) {
  dcc_status_t s = dcc_task10_query_sep(q, first, key);
  return s == DCC_OK ? dcc_rest_buffer_append_u64_text(q, value) : s;
}
dcc_status_t dcc_task10_entitlement_query(const dcc_rest_entitlement_query_t *p,
                                          char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (p == NULL)
    return DCC_OK;
  dcc_endpoint_record_view_t v;
  dcc_status_t s = dcc_task10_record(
      p, offsetof(dcc_rest_entitlement_query_t, user_id), UINT64_C(255), &v);
  if (s != DCC_OK)
    return s;
  if ((v.present & DCC_REST_ENTITLEMENT_QUERY_PRESENT_SKU_IDS) &&
      ((p->sku_ids == NULL) != (p->sku_id_count == 0U)))
    return DCC_ERR_INVALID_ARG;
  if ((v.present & DCC_REST_ENTITLEMENT_QUERY_PRESENT_LIMIT) &&
      (p->limit == 0U || p->limit > 100U))
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t q = {0};
  int first = 1;
  if (v.present & 1U)
    s = dcc_task10_query_u64(&q, &first, "user_id", p->user_id);
  if (s == DCC_OK && (v.present & 2U)) {
    s = dcc_task10_query_sep(&q, &first, "sku_ids");
    for (size_t i = 0; s == DCC_OK && i < p->sku_id_count; i++) {
      if (p->sku_ids[i] == 0U) {
        s = DCC_ERR_INVALID_ARG;
        break;
      }
      if (i)
        s = dcc_rest_buffer_append_cstr(&q, ",");
      if (s == DCC_OK)
        s = dcc_rest_buffer_append_u64_text(&q, p->sku_ids[i]);
    }
  }
  if (s == DCC_OK && (v.present & 4U))
    s = dcc_task10_query_u64(&q, &first, "before", p->before);
  if (s == DCC_OK && (v.present & 8U))
    s = dcc_task10_query_u64(&q, &first, "after", p->after);
  if (s == DCC_OK && (v.present & 16U))
    s = dcc_task10_query_u64(&q, &first, "limit", p->limit);
  if (s == DCC_OK && (v.present & 32U))
    s = dcc_task10_query_u64(&q, &first, "guild_id", p->guild_id);
  if (s == DCC_OK && (v.present & 64U)) {
    s = dcc_task10_query_sep(&q, &first, "exclude_ended");
    if (s == DCC_OK)
      s = dcc_rest_buffer_append_cstr(&q, p->exclude_ended ? "true" : "false");
  }
  if (s == DCC_OK && (v.present & 128U)) {
    s = dcc_task10_query_sep(&q, &first, "exclude_deleted");
    if (s == DCC_OK)
      s = dcc_rest_buffer_append_cstr(&q,
                                      p->exclude_deleted ? "true" : "false");
  }
  if (s != DCC_OK) {
    dcc_rest_buffer_deinit(&q);
    return s;
  }
  *out = q.data;
  return DCC_OK;
}

dcc_status_t
dcc_task10_subscription_query(const dcc_rest_sku_subscription_query_t *p,
                              dcc_rest_auth_mode_t auth, char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (p == NULL)
    return (auth == DCC_REST_AUTH_BEARER) ? DCC_OK : DCC_ERR_INVALID_ARG;
  dcc_endpoint_record_view_t v;
  dcc_status_t s = dcc_task10_record(
      p, offsetof(dcc_rest_sku_subscription_query_t, before), UINT64_C(15), &v);
  if (s != DCC_OK)
    return s;
  if ((v.present & 4U) && (p->limit == 0U || p->limit > 100U))
    return DCC_ERR_INVALID_ARG;
  if (auth != DCC_REST_AUTH_BEARER && (!(v.present & 8U) || p->user_id == 0U))
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t q = {0};
  int first = 1;
  if (v.present & 1U)
    s = dcc_task10_query_u64(&q, &first, "before", p->before);
  if (s == DCC_OK && (v.present & 2U))
    s = dcc_task10_query_u64(&q, &first, "after", p->after);
  if (s == DCC_OK && (v.present & 4U))
    s = dcc_task10_query_u64(&q, &first, "limit", p->limit);
  if (s == DCC_OK && (v.present & 8U))
    s = dcc_task10_query_u64(&q, &first, "user_id", p->user_id);
  if (s != DCC_OK) {
    dcc_rest_buffer_deinit(&q);
    return s;
  }
  *out = q.data;
  return DCC_OK;
}

/* Role-connection serialization is kept here so both public owners share the
 * same version/presence and ownership validation. */
static dcc_status_t
dcc_task10_append_localizations(dcc_rest_buffer_t *body, int *first,
                                const char *key,
                                const dcc_localization_t *items, size_t count) {
  if (count != 0U && items == NULL)
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t nested = {0};
  dcc_status_t status = dcc_rest_buffer_append_cstr(&nested, "{");
  int nested_first = 1;
  for (size_t i = 0U; status == DCC_OK && i < count; ++i) {
    if (items[i].locale == NULL || items[i].locale[0] == '\0' ||
        items[i].value == NULL) {
      status = DCC_ERR_INVALID_ARG;
      break;
    }
    for (size_t j = 0U; j < i; ++j) {
      if (strcmp(items[i].locale, items[j].locale) == 0) {
        status = DCC_ERR_INVALID_ARG;
        break;
      }
    }
    if (status == DCC_OK)
      status = dcc_rest_json_append_string_member(
          &nested, &nested_first, items[i].locale, items[i].value);
  }
  if (status == DCC_OK)
    status = dcc_rest_buffer_append_cstr(&nested, "}");
  if (status == DCC_OK)
    status = dcc_rest_json_append_raw_member(body, first, key, nested.data);
  dcc_rest_buffer_deinit(&nested);
  return status;
}

dcc_status_t dcc_task10_build_role_metadata(
    const dcc_application_role_connection_metadata_params_t *m, size_t count,
    char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (count > 5U || (count != 0U && m == NULL))
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t b = {0};
  dcc_status_t s = dcc_rest_buffer_append_cstr(&b, "[");
  for (size_t i = 0; s == DCC_OK && i < count; i++) {
    const dcc_application_role_connection_metadata_params_t *x =
        (const void *)((const unsigned char *)m + i * m[0].size);
    dcc_endpoint_record_view_t v;
    s = dcc_task10_record(
        x, offsetof(dcc_application_role_connection_metadata_params_t, type),
        UINT64_C(3), &v);
    if (s != DCC_OK || x->type < 1 || x->type > 8 || x->key == NULL ||
        x->name == NULL || x->description == NULL) {
      s = s != DCC_OK ? s : DCC_ERR_INVALID_ARG;
      break;
    }
    if (i)
      s = dcc_rest_buffer_append_cstr(&b, ",");
    if (s == DCC_OK)
      s = dcc_rest_buffer_append_cstr(&b, "{");
    int first = 1;
    if (s == DCC_OK)
      s = dcc_rest_json_append_u64_member(&b, &first, "type", x->type);
    if (s == DCC_OK)
      s = dcc_rest_json_append_string_member(&b, &first, "key", x->key);
    if (s == DCC_OK)
      s = dcc_rest_json_append_string_member(&b, &first, "name", x->name);
    if (s == DCC_OK &&
        (v.present & DCC_ROLE_CONNECTION_METADATA_PRESENT_NAME_LOCALIZATIONS))
      s = dcc_task10_append_localizations(&b, &first, "name_localizations",
                                          x->name_localizations,
                                          x->name_localization_count);
    if (s == DCC_OK)
      s = dcc_rest_json_append_string_member(&b, &first, "description",
                                             x->description);
    if (s == DCC_OK &&
        (v.present &
         DCC_ROLE_CONNECTION_METADATA_PRESENT_DESCRIPTION_LOCALIZATIONS))
      s = dcc_task10_append_localizations(
          &b, &first, "description_localizations", x->description_localizations,
          x->description_localization_count);
    if (s == DCC_OK)
      s = dcc_rest_buffer_append_cstr(&b, "}");
  }
  if (s == DCC_OK)
    s = dcc_rest_buffer_append_cstr(&b, "]");
  if (s != DCC_OK) {
    dcc_rest_buffer_deinit(&b);
    return s;
  }
  *out = b.data;
  return DCC_OK;
}

static dcc_status_t dcc_task10_map_json(const dcc_rest_string_map_t *map,
                                        char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (map == NULL)
    return DCC_ERR_INVALID_ARG;
  dcc_endpoint_record_view_t v;
  dcc_status_t s = dcc_task10_record(map, sizeof(*map), 0U, &v);
  if (s != DCC_OK || (map->entry_count != 0U && map->entries == NULL))
    return s != DCC_OK ? s : DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t b = {0};
  s = dcc_rest_buffer_append_cstr(&b, "{");
  int first = 1;
  for (size_t i = 0; s == DCC_OK && i < map->entry_count; i++) {
    if (map->entries[i].key == NULL || map->entries[i].value == NULL) {
      s = DCC_ERR_INVALID_ARG;
      break;
    }
    for (size_t j = 0; j < i; j++)
      if (strcmp(map->entries[i].key, map->entries[j].key) == 0) {
        s = DCC_ERR_INVALID_ARG;
        break;
      }
    if (s == DCC_OK)
      s = dcc_rest_json_append_string_member(&b, &first, map->entries[i].key,
                                             map->entries[i].value);
  }
  if (s == DCC_OK)
    s = dcc_rest_buffer_append_cstr(&b, "}");
  if (s != DCC_OK) {
    dcc_rest_buffer_deinit(&b);
    return s;
  }
  *out = b.data;
  return DCC_OK;
}
dcc_status_t dcc_task10_build_role_connection(
    const dcc_application_role_connection_params_t *p, char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  dcc_endpoint_record_view_t v;
  dcc_status_t s = dcc_task10_record(
      p, offsetof(dcc_application_role_connection_params_t, platform_name),
      UINT64_C(7), &v);
  if (s != DCC_OK)
    return s;
  dcc_rest_buffer_t b = {0};
  s = dcc_rest_buffer_append_cstr(&b, "{");
  int first = 1;
  if (s == DCC_OK && (v.present & 1U)) {
    if (p->platform_name == NULL)
      s = DCC_ERR_INVALID_ARG;
    else
      s = dcc_rest_json_append_string_member(&b, &first, "platform_name",
                                             p->platform_name);
  }
  if (s == DCC_OK && (v.present & 2U)) {
    if (p->platform_username == NULL)
      s = DCC_ERR_INVALID_ARG;
    else
      s = dcc_rest_json_append_string_member(&b, &first, "platform_username",
                                             p->platform_username);
  }
  if (s == DCC_OK && (v.present & 4U)) {
    char *map = NULL;
    s = dcc_task10_map_json(p->metadata, &map);
    if (s == DCC_OK)
      s = dcc_rest_json_append_raw_member(&b, &first, "metadata", map);
    free(map);
  }
  if (s != DCC_OK) {
    dcc_rest_buffer_deinit(&b);
    return s;
  }
  return dcc_task10_finish(&b, out);
}
