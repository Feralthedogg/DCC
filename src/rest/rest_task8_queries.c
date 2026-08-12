#include "internal/rest/dcc_rest_query_collections_internal.h"
#include "internal/rest/dcc_rest_task8_internal.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define FIELD_PARTIAL(type_, view_, field_)                                    \
  dcc_endpoint_field_partially_covered((view_).size, offsetof(type_, field_),  \
                                       sizeof(((type_ *)0)->field_))
#define FIELD_PRESENT(type_, view_, bit_, field_)                              \
  dcc_endpoint_present_field_covered(                                          \
      &(view_), (bit_), offsetof(type_, field_), sizeof(((type_ *)0)->field_))

static dcc_status_t query_record(const void *value, size_t version_offset,
                                 size_t present_offset, size_t mandatory_size,
                                 uint32_t version, uint64_t known,
                                 dcc_endpoint_record_view_t *view) {
  return dcc_endpoint_record_read(value, version_offset, present_offset,
                                  mandatory_size, version, known, view);
}

dcc_status_t dcc_task8_query_guild(const dcc_rest_guild_query_t *query,
                                   dcc_rest_buffer_t *out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  if (query == NULL)
    return DCC_OK;
  dcc_endpoint_record_view_t view;
  const uint64_t bit = DCC_REST_GUILD_QUERY_PRESENT_WITH_COUNTS;
  if (query_record(query, offsetof(dcc_rest_guild_query_t, version),
                   offsetof(dcc_rest_guild_query_t, present),
                   DCC_ENDPOINT_FIELD_END(dcc_rest_guild_query_t, present),
                   DCC_REST_GUILD_QUERY_VERSION, bit, &view) != DCC_OK ||
      FIELD_PARTIAL(dcc_rest_guild_query_t, view, with_counts) ||
      !FIELD_PRESENT(dcc_rest_guild_query_t, view, bit, with_counts) ||
      ((view.present & bit) != 0U && query->with_counts > 1U))
    return DCC_ERR_INVALID_ARG;
  return (view.present & bit) != 0U
             ? dcc_rest_query_append_bool(out, "with_counts",
                                          query->with_counts)
             : DCC_OK;
}

dcc_status_t dcc_task8_query_current_user_guilds(
    const dcc_rest_current_user_guilds_query_t *query, dcc_rest_buffer_t *out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  if (query == NULL)
    return DCC_OK;
  const uint64_t before = DCC_REST_CURRENT_USER_GUILDS_QUERY_PRESENT_BEFORE;
  const uint64_t after = DCC_REST_CURRENT_USER_GUILDS_QUERY_PRESENT_AFTER;
  const uint64_t limit = DCC_REST_CURRENT_USER_GUILDS_QUERY_PRESENT_LIMIT;
  const uint64_t counts =
      DCC_REST_CURRENT_USER_GUILDS_QUERY_PRESENT_WITH_COUNTS;
  dcc_endpoint_record_view_t view;
  if (query_record(
          query, offsetof(dcc_rest_current_user_guilds_query_t, version),
          offsetof(dcc_rest_current_user_guilds_query_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_current_user_guilds_query_t, present),
          DCC_REST_CURRENT_USER_GUILDS_QUERY_VERSION,
          before | after | limit | counts, &view) != DCC_OK ||
      FIELD_PARTIAL(dcc_rest_current_user_guilds_query_t, view, before) ||
      FIELD_PARTIAL(dcc_rest_current_user_guilds_query_t, view, after) ||
      FIELD_PARTIAL(dcc_rest_current_user_guilds_query_t, view, limit) ||
      FIELD_PARTIAL(dcc_rest_current_user_guilds_query_t, view, with_counts) ||
      !FIELD_PRESENT(dcc_rest_current_user_guilds_query_t, view, before,
                     before) ||
      !FIELD_PRESENT(dcc_rest_current_user_guilds_query_t, view, after,
                     after) ||
      !FIELD_PRESENT(dcc_rest_current_user_guilds_query_t, view, limit,
                     limit) ||
      !FIELD_PRESENT(dcc_rest_current_user_guilds_query_t, view, counts,
                     with_counts) ||
      ((view.present & before) != 0U && query->before == 0U) ||
      ((view.present & after) != 0U && query->after == 0U) ||
      ((view.present & limit) != 0U &&
       (query->limit < 1U || query->limit > 200U)) ||
      ((view.present & counts) != 0U && query->with_counts > 1U))
    return DCC_ERR_INVALID_ARG;
  dcc_status_t status = DCC_OK;
  if ((view.present & before) != 0U)
    status = dcc_rest_query_append_u64_value(out, "before", query->before);
  if (status == DCC_OK && (view.present & after) != 0U)
    status = dcc_rest_query_append_u64_value(out, "after", query->after);
  if (status == DCC_OK && (view.present & limit) != 0U)
    status = dcc_rest_query_append_u64_value(out, "limit", query->limit);
  if (status == DCC_OK && (view.present & counts) != 0U)
    status = dcc_rest_query_append_bool(out, "with_counts", query->with_counts);
  return status;
}

dcc_status_t
dcc_task8_query_audit_log(const dcc_rest_guild_audit_log_query_t *query,
                          dcc_rest_buffer_t *out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  if (query == NULL)
    return DCC_OK;
  const uint64_t user = DCC_REST_GUILD_AUDIT_LOG_QUERY_PRESENT_USER_ID;
  const uint64_t action = DCC_REST_GUILD_AUDIT_LOG_QUERY_PRESENT_ACTION_TYPE;
  const uint64_t before = DCC_REST_GUILD_AUDIT_LOG_QUERY_PRESENT_BEFORE;
  const uint64_t after = DCC_REST_GUILD_AUDIT_LOG_QUERY_PRESENT_AFTER;
  const uint64_t limit = DCC_REST_GUILD_AUDIT_LOG_QUERY_PRESENT_LIMIT;
  dcc_endpoint_record_view_t view;
  if (query_record(
          query, offsetof(dcc_rest_guild_audit_log_query_t, version),
          offsetof(dcc_rest_guild_audit_log_query_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_audit_log_query_t, present),
          DCC_REST_GUILD_AUDIT_LOG_QUERY_VERSION,
          user | action | before | after | limit, &view) != DCC_OK ||
      FIELD_PARTIAL(dcc_rest_guild_audit_log_query_t, view, user_id) ||
      FIELD_PARTIAL(dcc_rest_guild_audit_log_query_t, view, action_type) ||
      FIELD_PARTIAL(dcc_rest_guild_audit_log_query_t, view, before) ||
      FIELD_PARTIAL(dcc_rest_guild_audit_log_query_t, view, after) ||
      FIELD_PARTIAL(dcc_rest_guild_audit_log_query_t, view, limit) ||
      !FIELD_PRESENT(dcc_rest_guild_audit_log_query_t, view, user, user_id) ||
      !FIELD_PRESENT(dcc_rest_guild_audit_log_query_t, view, action,
                     action_type) ||
      !FIELD_PRESENT(dcc_rest_guild_audit_log_query_t, view, before, before) ||
      !FIELD_PRESENT(dcc_rest_guild_audit_log_query_t, view, after, after) ||
      !FIELD_PRESENT(dcc_rest_guild_audit_log_query_t, view, limit, limit) ||
      ((view.present & user) != 0U && query->user_id == 0U) ||
      ((view.present & before) != 0U && query->before == 0U) ||
      ((view.present & limit) != 0U &&
       (query->limit < 1U || query->limit > 100U)))
    return DCC_ERR_INVALID_ARG;
  dcc_status_t status = DCC_OK;
  if ((view.present & user) != 0U)
    status = dcc_rest_query_append_u64_value(out, "user_id", query->user_id);
  if (status == DCC_OK && (view.present & action) != 0U)
    status =
        dcc_rest_query_append_u64_value(out, "action_type", query->action_type);
  if (status == DCC_OK && (view.present & before) != 0U)
    status = dcc_rest_query_append_u64_value(out, "before", query->before);
  if (status == DCC_OK && (view.present & after) != 0U)
    status = dcc_rest_query_append_u64_value(out, "after", query->after);
  if (status == DCC_OK && (view.present & limit) != 0U)
    status = dcc_rest_query_append_u64_value(out, "limit", query->limit);
  return status;
}

dcc_status_t dcc_task8_query_bans(const dcc_rest_guild_bans_query_t *query,
                                  dcc_rest_buffer_t *out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  if (query == NULL)
    return DCC_OK;
  const uint64_t before = DCC_REST_GUILD_BANS_QUERY_PRESENT_BEFORE;
  const uint64_t after = DCC_REST_GUILD_BANS_QUERY_PRESENT_AFTER;
  const uint64_t limit = DCC_REST_GUILD_BANS_QUERY_PRESENT_LIMIT;
  dcc_endpoint_record_view_t view;
  if (query_record(query, offsetof(dcc_rest_guild_bans_query_t, version),
                   offsetof(dcc_rest_guild_bans_query_t, present),
                   DCC_ENDPOINT_FIELD_END(dcc_rest_guild_bans_query_t, present),
                   DCC_REST_GUILD_BANS_QUERY_VERSION, before | after | limit,
                   &view) != DCC_OK ||
      FIELD_PARTIAL(dcc_rest_guild_bans_query_t, view, before) ||
      FIELD_PARTIAL(dcc_rest_guild_bans_query_t, view, after) ||
      FIELD_PARTIAL(dcc_rest_guild_bans_query_t, view, limit) ||
      !FIELD_PRESENT(dcc_rest_guild_bans_query_t, view, before, before) ||
      !FIELD_PRESENT(dcc_rest_guild_bans_query_t, view, after, after) ||
      !FIELD_PRESENT(dcc_rest_guild_bans_query_t, view, limit, limit) ||
      ((view.present & before) != 0U && query->before == 0U) ||
      ((view.present & after) != 0U && query->after == 0U) ||
      ((view.present & limit) != 0U &&
       (query->limit < 1U || query->limit > 1000U)))
    return DCC_ERR_INVALID_ARG;
  dcc_status_t status = DCC_OK;
  if ((view.present & limit) != 0U)
    status = dcc_rest_query_append_u64_value(out, "limit", query->limit);
  if (status == DCC_OK && (view.present & before) != 0U)
    status = dcc_rest_query_append_u64_value(out, "before", query->before);
  if (status == DCC_OK && (view.present & after) != 0U)
    status = dcc_rest_query_append_u64_value(out, "after", query->after);
  return status;
}

dcc_status_t
dcc_task8_query_members(const dcc_rest_guild_members_query_t *query,
                        dcc_rest_buffer_t *out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  if (query == NULL)
    return DCC_OK;
  const uint64_t after = DCC_REST_GUILD_MEMBERS_QUERY_PRESENT_AFTER;
  const uint64_t limit = DCC_REST_GUILD_MEMBERS_QUERY_PRESENT_LIMIT;
  dcc_endpoint_record_view_t view;
  if (query_record(
          query, offsetof(dcc_rest_guild_members_query_t, version),
          offsetof(dcc_rest_guild_members_query_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_members_query_t, present),
          DCC_REST_GUILD_MEMBERS_QUERY_VERSION, after | limit,
          &view) != DCC_OK ||
      FIELD_PARTIAL(dcc_rest_guild_members_query_t, view, after) ||
      FIELD_PARTIAL(dcc_rest_guild_members_query_t, view, limit) ||
      !FIELD_PRESENT(dcc_rest_guild_members_query_t, view, after, after) ||
      !FIELD_PRESENT(dcc_rest_guild_members_query_t, view, limit, limit) ||
      ((view.present & limit) != 0U &&
       (query->limit < 1U || query->limit > 1000U)))
    return DCC_ERR_INVALID_ARG;
  dcc_status_t status = DCC_OK;
  if ((view.present & limit) != 0U)
    status = dcc_rest_query_append_u64_value(out, "limit", query->limit);
  if (status == DCC_OK && (view.present & after) != 0U)
    status = dcc_rest_query_append_u64_value(out, "after", query->after);
  return status;
}

dcc_status_t
dcc_task8_query_member_search(const dcc_rest_guild_member_search_query_t *query,
                              dcc_rest_buffer_t *out) {
  if (out == NULL || query == NULL)
    return DCC_ERR_INVALID_ARG;
  const uint64_t limit = DCC_REST_GUILD_MEMBER_SEARCH_QUERY_PRESENT_LIMIT;
  dcc_endpoint_record_view_t view;
  size_t scalars = 0U;
  if (query_record(
          query, offsetof(dcc_rest_guild_member_search_query_t, version),
          offsetof(dcc_rest_guild_member_search_query_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_member_search_query_t, query),
          DCC_REST_GUILD_MEMBER_SEARCH_QUERY_VERSION, limit, &view) != DCC_OK ||
      FIELD_PARTIAL(dcc_rest_guild_member_search_query_t, view, limit) ||
      !FIELD_PRESENT(dcc_rest_guild_member_search_query_t, view, limit,
                     limit) ||
      query->query == NULL || query->query[0] == '\0' ||
      !dcc_endpoint_utf8_scalar_count(query->query, &scalars) ||
      scalars == 0U ||
      ((view.present & limit) != 0U &&
       (query->limit < 1U || query->limit > 1000U)))
    return DCC_ERR_INVALID_ARG;
  dcc_status_t status =
      dcc_rest_query_append_string(out, "query", query->query);
  if (status == DCC_OK && (view.present & limit) != 0U)
    status = dcc_rest_query_append_u64_value(out, "limit", query->limit);
  return status;
}

static int nonzero_ids(const dcc_snowflake_t *ids, size_t count) {
  if (ids == NULL || count == 0U)
    return 0;
  uintptr_t address = (uintptr_t)ids;
  if (address % _Alignof(dcc_snowflake_t) != 0U ||
      count > SIZE_MAX / sizeof(*ids))
    return 0;
  size_t span = count * sizeof(*ids);
  if (address > UINTPTR_MAX - (span - 1U))
    return 0;
  for (size_t i = 0U; i < count; ++i)
    if (ids[i] == 0U)
      return 0;
  return 1;
}

dcc_status_t dcc_task8_query_prune(const dcc_rest_guild_prune_query_t *query,
                                   dcc_rest_buffer_t *out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  if (query == NULL)
    return DCC_OK;
  const uint64_t days = DCC_REST_GUILD_PRUNE_QUERY_PRESENT_DAYS;
  const uint64_t roles = DCC_REST_GUILD_PRUNE_QUERY_PRESENT_INCLUDE_ROLES;
  dcc_endpoint_record_view_t view;
  if (query_record(
          query, offsetof(dcc_rest_guild_prune_query_t, version),
          offsetof(dcc_rest_guild_prune_query_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_prune_query_t, present),
          DCC_REST_GUILD_PRUNE_QUERY_VERSION, days | roles, &view) != DCC_OK ||
      FIELD_PARTIAL(dcc_rest_guild_prune_query_t, view, days) ||
      FIELD_PARTIAL(dcc_rest_guild_prune_query_t, view, include_roles) ||
      FIELD_PARTIAL(dcc_rest_guild_prune_query_t, view, include_role_count) ||
      !FIELD_PRESENT(dcc_rest_guild_prune_query_t, view, days, days) ||
      !FIELD_PRESENT(dcc_rest_guild_prune_query_t, view, roles,
                     include_roles) ||
      !FIELD_PRESENT(dcc_rest_guild_prune_query_t, view, roles,
                     include_role_count) ||
      ((view.present & days) != 0U &&
       (query->days < 1U || query->days > 30U)) ||
      ((view.present & roles) != 0U &&
       !nonzero_ids(query->include_roles, query->include_role_count)))
    return DCC_ERR_INVALID_ARG;
  dcc_status_t status = DCC_OK;
  if ((view.present & days) != 0U)
    status = dcc_rest_query_append_u64_value(out, "days", query->days);
  if (status == DCC_OK && (view.present & roles) != 0U)
    status = dcc_rest_query_append_snowflake_csv(
        out, "include_roles", query->include_roles, query->include_role_count);
  return status;
}

dcc_status_t
dcc_task8_query_widget_image(const dcc_rest_guild_widget_image_query_t *query,
                             dcc_rest_buffer_t *out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  if (query == NULL)
    return DCC_OK;
  const uint64_t style = DCC_REST_GUILD_WIDGET_IMAGE_QUERY_PRESENT_STYLE;
  dcc_endpoint_record_view_t view;
  static const char *const names[] = {"shield", "banner1", "banner2", "banner3",
                                      "banner4"};
  if (query_record(
          query, offsetof(dcc_rest_guild_widget_image_query_t, version),
          offsetof(dcc_rest_guild_widget_image_query_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_widget_image_query_t, present),
          DCC_REST_GUILD_WIDGET_IMAGE_QUERY_VERSION, style, &view) != DCC_OK ||
      FIELD_PARTIAL(dcc_rest_guild_widget_image_query_t, view, style) ||
      !FIELD_PRESENT(dcc_rest_guild_widget_image_query_t, view, style, style) ||
      ((view.present & style) != 0U &&
       (query->style < DCC_REST_GUILD_WIDGET_STYLE_SHIELD ||
        query->style > DCC_REST_GUILD_WIDGET_STYLE_BANNER4)))
    return DCC_ERR_INVALID_ARG;
  return (view.present & style) != 0U
             ? dcc_rest_query_append_string(out, "style",
                                            names[(unsigned)query->style])
             : DCC_OK;
}

#undef FIELD_PARTIAL
#undef FIELD_PRESENT
