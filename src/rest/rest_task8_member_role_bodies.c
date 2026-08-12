#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_task8_internal.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define PARTIAL(type_, view_, field_)                                          \
  dcc_endpoint_field_partially_covered((view_).size, offsetof(type_, field_),  \
                                       sizeof(((type_ *)0)->field_))
#define COVERED(type_, view_, bit_, field_)                                    \
  dcc_endpoint_present_field_covered(                                          \
      &(view_), (bit_), offsetof(type_, field_), sizeof(((type_ *)0)->field_))

static dcc_status_t finish_body(dcc_rest_buffer_t *body, char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  dcc_status_t status = dcc_rest_buffer_append_cstr(body, "}");
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(body);
    *out = NULL;
    return status;
  }
  *out = body->data;
  return DCC_OK;
}

static uint64_t read_nulls(const void *value, size_t offset, size_t size,
                           int *partial) {
  uint64_t nulls = 0U;
  *partial = dcc_endpoint_field_partially_covered(size, offset, sizeof(nulls));
  if (dcc_endpoint_field_covered(size, offset, sizeof(nulls)))
    memcpy(&nulls, (const unsigned char *)value + offset, sizeof(nulls));
  return nulls;
}

static int text_scalars_between(const char *text, size_t min, size_t max) {
  size_t count = 0U;
  return text != NULL && dcc_endpoint_utf8_scalar_count(text, &count) &&
         count >= min && count <= max;
}

static int snowflakes_valid(const dcc_snowflake_t *values, size_t count) {
  if (count == 0U)
    return 1;
  if (values == NULL)
    return 0;
  uintptr_t address = (uintptr_t)values;
  if (address % _Alignof(dcc_snowflake_t) != 0U ||
      count > SIZE_MAX / sizeof(*values))
    return 0;
  size_t span = count * sizeof(*values);
  if (address > UINTPTR_MAX - (span - 1U))
    return 0;
  for (size_t i = 0U; i < count; ++i)
    if (values[i] == 0U)
      return 0;
  return 1;
}

static dcc_status_t append_nullable_bool(dcc_rest_buffer_t *body, int *first,
                                         const char *key, uint8_t value,
                                         int is_null) {
  return is_null ? dcc_rest_json_append_raw_member(body, first, key, "null")
                 : dcc_rest_json_append_bool_member(body, first, key, value);
}

static dcc_status_t append_nullable_u64(dcc_rest_buffer_t *body, int *first,
                                        const char *key, uint64_t value,
                                        int is_null, int string_value) {
  if (is_null)
    return dcc_rest_json_append_raw_member(body, first, key, "null");
  return string_value
             ? dcc_rest_json_append_u64_string_member(body, first, key, value)
             : dcc_rest_json_append_u64_member(body, first, key, value);
}

dcc_status_t
dcc_task8_build_member_add(const dcc_rest_guild_member_add_t *value,
                           char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  const uint64_t nick = DCC_REST_GUILD_MEMBER_ADD_PRESENT_NICK;
  const uint64_t roles = DCC_REST_GUILD_MEMBER_ADD_PRESENT_ROLES;
  const uint64_t mute = DCC_REST_GUILD_MEMBER_ADD_PRESENT_MUTE;
  const uint64_t deaf = DCC_REST_GUILD_MEMBER_ADD_PRESENT_DEAF;
  dcc_endpoint_record_view_t view;
  if (dcc_endpoint_record_read(
          value, offsetof(dcc_rest_guild_member_add_t, version),
          offsetof(dcc_rest_guild_member_add_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_member_add_t, access_token),
          DCC_REST_GUILD_MEMBER_ADD_VERSION, nick | roles | mute | deaf,
          &view) != DCC_OK ||
      PARTIAL(dcc_rest_guild_member_add_t, view, nick) ||
      PARTIAL(dcc_rest_guild_member_add_t, view, roles) ||
      PARTIAL(dcc_rest_guild_member_add_t, view, role_count) ||
      PARTIAL(dcc_rest_guild_member_add_t, view, mute) ||
      PARTIAL(dcc_rest_guild_member_add_t, view, deaf) ||
      !COVERED(dcc_rest_guild_member_add_t, view, nick, nick) ||
      !COVERED(dcc_rest_guild_member_add_t, view, roles, roles) ||
      !COVERED(dcc_rest_guild_member_add_t, view, roles, role_count) ||
      !COVERED(dcc_rest_guild_member_add_t, view, mute, mute) ||
      !COVERED(dcc_rest_guild_member_add_t, view, deaf, deaf) ||
      value->access_token == NULL || value->access_token[0] == '\0' ||
      ((view.present & nick) != 0U &&
       !text_scalars_between(value->nick, 1U, 32U)) ||
      ((view.present & roles) != 0U &&
       !snowflakes_valid(value->roles, value->role_count)) ||
      ((view.present & mute) != 0U && value->mute > 1U) ||
      ((view.present & deaf) != 0U && value->deaf > 1U))
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK)
    status = dcc_rest_json_append_string_member(&body, &first, "access_token",
                                                value->access_token);
  if (status == DCC_OK && (view.present & nick))
    status =
        dcc_rest_json_append_string_member(&body, &first, "nick", value->nick);
  if (status == DCC_OK && (view.present & roles))
    status = dcc_rest_json_append_snowflake_string_array_member(
        &body, &first, "roles", value->roles, value->role_count);
  if (status == DCC_OK && (view.present & mute))
    status =
        dcc_rest_json_append_bool_member(&body, &first, "mute", value->mute);
  if (status == DCC_OK && (view.present & deaf))
    status =
        dcc_rest_json_append_bool_member(&body, &first, "deaf", value->deaf);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return finish_body(&body, out);
}

dcc_status_t
dcc_task8_build_member_update(const dcc_rest_guild_member_update_t *value,
                              char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (value == NULL)
    return dcc_task8_empty_object(out);
  const uint64_t known = (UINT64_C(1) << 7U) - 1U;
  dcc_endpoint_record_view_t view;
  int partial = 0;
  if (dcc_endpoint_record_read(
          value, offsetof(dcc_rest_guild_member_update_t, version),
          offsetof(dcc_rest_guild_member_update_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_member_update_t, present),
          DCC_REST_GUILD_MEMBER_UPDATE_VERSION, known, &view) != DCC_OK)
    return DCC_ERR_INVALID_ARG;
  uint64_t nulls =
      read_nulls(value, offsetof(dcc_rest_guild_member_update_t, nulls),
                 view.size, &partial);
  const size_t offsets[] = {
      offsetof(dcc_rest_guild_member_update_t, nick),
      offsetof(dcc_rest_guild_member_update_t, roles),
      offsetof(dcc_rest_guild_member_update_t, role_count),
      offsetof(dcc_rest_guild_member_update_t, mute),
      offsetof(dcc_rest_guild_member_update_t, deaf),
      offsetof(dcc_rest_guild_member_update_t, channel_id),
      offsetof(dcc_rest_guild_member_update_t, communication_disabled_until),
      offsetof(dcc_rest_guild_member_update_t, flags)};
  const size_t widths[] = {sizeof(value->nick),
                           sizeof(value->roles),
                           sizeof(value->role_count),
                           sizeof(value->mute),
                           sizeof(value->deaf),
                           sizeof(value->channel_id),
                           sizeof(value->communication_disabled_until),
                           sizeof(value->flags)};
  if (partial || (nulls & ~known) != 0U || (nulls & ~view.present) != 0U)
    return DCC_ERR_INVALID_ARG;
  for (size_t i = 0; i < 8U; ++i)
    if (dcc_endpoint_field_partially_covered(view.size, offsets[i], widths[i]))
      return DCC_ERR_INVALID_ARG;
  const uint64_t bits[] = {UINT64_C(1),  UINT64_C(2), UINT64_C(2),
                           UINT64_C(4),  UINT64_C(8), UINT64_C(16),
                           UINT64_C(32), UINT64_C(64)};
  for (size_t i = 0; i < 8U; ++i)
    if (!dcc_endpoint_present_field_covered(&view, bits[i], offsets[i],
                                            widths[i]))
      return DCC_ERR_INVALID_ARG;
  if ((view.present & UINT64_C(1)) && !(nulls & UINT64_C(1)) &&
      !text_scalars_between(value->nick, 1U, 32U))
    return DCC_ERR_INVALID_ARG;
  if ((view.present & UINT64_C(2)) && !(nulls & UINT64_C(2)) &&
      !snowflakes_valid(value->roles, value->role_count))
    return DCC_ERR_INVALID_ARG;
  if ((view.present & UINT64_C(4)) && !(nulls & UINT64_C(4)) &&
      value->mute > 1U)
    return DCC_ERR_INVALID_ARG;
  if ((view.present & UINT64_C(8)) && !(nulls & UINT64_C(8)) &&
      value->deaf > 1U)
    return DCC_ERR_INVALID_ARG;
  if ((view.present & UINT64_C(16)) && !(nulls & UINT64_C(16)) &&
      value->channel_id == 0U)
    return DCC_ERR_INVALID_ARG;
  if ((view.present & UINT64_C(32)) && !(nulls & UINT64_C(32)) &&
      !dcc_task8_iso8601_valid(value->communication_disabled_until))
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK && (view.present & UINT64_C(1)))
    status =
        (nulls & UINT64_C(1))
            ? dcc_rest_json_append_raw_member(&body, &first, "nick", "null")
            : dcc_rest_json_append_string_member(&body, &first, "nick",
                                                 value->nick);
  if (status == DCC_OK && (view.present & UINT64_C(2)))
    status =
        (nulls & UINT64_C(2))
            ? dcc_rest_json_append_raw_member(&body, &first, "roles", "null")
            : dcc_rest_json_append_snowflake_string_array_member(
                  &body, &first, "roles", value->roles, value->role_count);
  if (status == DCC_OK && (view.present & UINT64_C(4)))
    status = append_nullable_bool(&body, &first, "mute", value->mute,
                                  (nulls & UINT64_C(4)) != 0U);
  if (status == DCC_OK && (view.present & UINT64_C(8)))
    status = append_nullable_bool(&body, &first, "deaf", value->deaf,
                                  (nulls & UINT64_C(8)) != 0U);
  if (status == DCC_OK && (view.present & UINT64_C(16)))
    status = append_nullable_u64(&body, &first, "channel_id", value->channel_id,
                                 (nulls & UINT64_C(16)) != 0U, 1);
  if (status == DCC_OK && (view.present & UINT64_C(32)))
    status = (nulls & UINT64_C(32))
                 ? dcc_rest_json_append_raw_member(
                       &body, &first, "communication_disabled_until", "null")
                 : dcc_rest_json_append_string_member(
                       &body, &first, "communication_disabled_until",
                       value->communication_disabled_until);
  if (status == DCC_OK && (view.present & UINT64_C(64)))
    status = append_nullable_u64(&body, &first, "flags", value->flags,
                                 (nulls & UINT64_C(64)) != 0U, 0);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return finish_body(&body, out);
}

static dcc_status_t build_nullable_strings(
    const void *value, size_t version_offset, size_t present_offset,
    size_t mandatory, uint32_t version, uint64_t known, size_t nulls_offset,
    const size_t *offsets, const char *const *keys, size_t count, char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (value == NULL)
    return dcc_task8_empty_object(out);
  dcc_endpoint_record_view_t view;
  int partial = 0;
  if (dcc_endpoint_record_read(value, version_offset, present_offset, mandatory,
                               version, known, &view) != DCC_OK)
    return DCC_ERR_INVALID_ARG;
  uint64_t nulls = read_nulls(value, nulls_offset, view.size, &partial);
  if (partial || (nulls & ~known) != 0U || (nulls & ~view.present) != 0U)
    return DCC_ERR_INVALID_ARG;
  for (size_t i = 0; i < count; ++i) {
    if (dcc_endpoint_field_partially_covered(view.size, offsets[i],
                                             sizeof(const char *)) ||
        !dcc_endpoint_present_field_covered(&view, UINT64_C(1) << i, offsets[i],
                                            sizeof(const char *)))
      return DCC_ERR_INVALID_ARG;
    if ((view.present & (UINT64_C(1) << i)) && !(nulls & (UINT64_C(1) << i))) {
      const char *text = NULL;
      memcpy(&text, (const unsigned char *)value + offsets[i], sizeof(text));
      size_t scalars = 0U;
      if (text == NULL || !dcc_endpoint_utf8_scalar_count(text, &scalars) ||
          (i == 0U && (scalars < 1U || scalars > 32U)))
        return DCC_ERR_INVALID_ARG;
    }
  }
  dcc_rest_buffer_t body = {0};
  int first = 1;
  dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
  for (size_t i = 0; i < count && status == DCC_OK; ++i)
    if (view.present & (UINT64_C(1) << i)) {
      const char *text = NULL;
      memcpy(&text, (const unsigned char *)value + offsets[i], sizeof(text));
      status =
          (nulls & (UINT64_C(1) << i))
              ? dcc_rest_json_append_raw_member(&body, &first, keys[i], "null")
              : dcc_rest_json_append_string_member(&body, &first, keys[i],
                                                   text);
    }
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return finish_body(&body, out);
}

dcc_status_t dcc_task8_build_current_member_update(
    const dcc_rest_current_guild_member_update_t *value, char **out) {
  const size_t offsets[] = {
      offsetof(dcc_rest_current_guild_member_update_t, nick),
      offsetof(dcc_rest_current_guild_member_update_t, banner),
      offsetof(dcc_rest_current_guild_member_update_t, avatar),
      offsetof(dcc_rest_current_guild_member_update_t, bio)};
  static const char *const keys[] = {"nick", "banner", "avatar", "bio"};
  return build_nullable_strings(
      value, offsetof(dcc_rest_current_guild_member_update_t, version),
      offsetof(dcc_rest_current_guild_member_update_t, present),
      DCC_ENDPOINT_FIELD_END(dcc_rest_current_guild_member_update_t, present),
      DCC_REST_CURRENT_GUILD_MEMBER_UPDATE_VERSION, UINT64_C(15),
      offsetof(dcc_rest_current_guild_member_update_t, nulls), offsets, keys,
      4U, out);
}

dcc_status_t dcc_task8_build_current_member_nickname(
    const dcc_rest_current_guild_member_nickname_t *value, char **out) {
  const size_t offsets[] = {
      offsetof(dcc_rest_current_guild_member_nickname_t, nick)};
  static const char *const keys[] = {"nick"};
  return build_nullable_strings(
      value, offsetof(dcc_rest_current_guild_member_nickname_t, version),
      offsetof(dcc_rest_current_guild_member_nickname_t, present),
      DCC_ENDPOINT_FIELD_END(dcc_rest_current_guild_member_nickname_t, present),
      DCC_REST_CURRENT_GUILD_MEMBER_NICKNAME_VERSION, UINT64_C(1),
      offsetof(dcc_rest_current_guild_member_nickname_t, nulls), offsets, keys,
      1U, out);
}

dcc_status_t
dcc_task8_build_ban_create(const dcc_rest_guild_ban_create_t *value,
                           char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (value == NULL)
    return dcc_task8_empty_object(out);
  dcc_endpoint_record_view_t view;
  const uint64_t bit = DCC_REST_GUILD_BAN_CREATE_PRESENT_DELETE_MESSAGE_SECONDS;
  if (dcc_endpoint_record_read(
          value, offsetof(dcc_rest_guild_ban_create_t, version),
          offsetof(dcc_rest_guild_ban_create_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_ban_create_t, present),
          DCC_REST_GUILD_BAN_CREATE_VERSION, bit, &view) != DCC_OK ||
      PARTIAL(dcc_rest_guild_ban_create_t, view, delete_message_seconds) ||
      !COVERED(dcc_rest_guild_ban_create_t, view, bit,
               delete_message_seconds) ||
      ((view.present & bit) && value->delete_message_seconds > 604800U))
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK && (view.present & bit))
    status = dcc_rest_json_append_u64_member(
        &body, &first, "delete_message_seconds", value->delete_message_seconds);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return finish_body(&body, out);
}

dcc_status_t dcc_task8_build_bulk_ban(const dcc_rest_guild_bulk_ban_t *value,
                                      char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  dcc_endpoint_record_view_t view;
  const uint64_t bit = DCC_REST_GUILD_BULK_BAN_PRESENT_DELETE_MESSAGE_SECONDS;
  if (dcc_endpoint_record_read(
          value, offsetof(dcc_rest_guild_bulk_ban_t, version),
          offsetof(dcc_rest_guild_bulk_ban_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_bulk_ban_t, user_id_count),
          DCC_REST_GUILD_BULK_BAN_VERSION, bit, &view) != DCC_OK ||
      PARTIAL(dcc_rest_guild_bulk_ban_t, view, delete_message_seconds) ||
      !COVERED(dcc_rest_guild_bulk_ban_t, view, bit, delete_message_seconds) ||
      value->user_id_count < 1U || value->user_id_count > 200U ||
      !snowflakes_valid(value->user_ids, value->user_id_count) ||
      ((view.present & bit) && value->delete_message_seconds > 604800U))
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK)
    status = dcc_rest_json_append_snowflake_string_array_member(
        &body, &first, "user_ids", value->user_ids, value->user_id_count);
  if (status == DCC_OK && (view.present & bit))
    status = dcc_rest_json_append_u64_member(
        &body, &first, "delete_message_seconds", value->delete_message_seconds);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return finish_body(&body, out);
}

dcc_status_t dcc_task8_build_prune(const dcc_rest_guild_prune_t *value,
                                   char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (value == NULL)
    return dcc_task8_empty_object(out);
  const uint64_t days = DCC_REST_GUILD_PRUNE_PRESENT_DAYS,
                 compute = DCC_REST_GUILD_PRUNE_PRESENT_COMPUTE_PRUNE_COUNT,
                 roles = DCC_REST_GUILD_PRUNE_PRESENT_INCLUDE_ROLES;
  dcc_endpoint_record_view_t view;
  if (dcc_endpoint_record_read(
          value, offsetof(dcc_rest_guild_prune_t, version),
          offsetof(dcc_rest_guild_prune_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_prune_t, present),
          DCC_REST_GUILD_PRUNE_VERSION, days | compute | roles,
          &view) != DCC_OK ||
      PARTIAL(dcc_rest_guild_prune_t, view, days) ||
      PARTIAL(dcc_rest_guild_prune_t, view, compute_prune_count) ||
      PARTIAL(dcc_rest_guild_prune_t, view, include_roles) ||
      PARTIAL(dcc_rest_guild_prune_t, view, include_role_count) ||
      !COVERED(dcc_rest_guild_prune_t, view, days, days) ||
      !COVERED(dcc_rest_guild_prune_t, view, compute, compute_prune_count) ||
      !COVERED(dcc_rest_guild_prune_t, view, roles, include_roles) ||
      !COVERED(dcc_rest_guild_prune_t, view, roles, include_role_count) ||
      ((view.present & days) && (value->days < 1U || value->days > 30U)) ||
      ((view.present & compute) && value->compute_prune_count > 1U) ||
      ((view.present & roles) &&
       !snowflakes_valid(value->include_roles, value->include_role_count)))
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK && (view.present & days))
    status =
        dcc_rest_json_append_u64_member(&body, &first, "days", value->days);
  if (status == DCC_OK && (view.present & compute))
    status = dcc_rest_json_append_bool_member(
        &body, &first, "compute_prune_count", value->compute_prune_count);
  if (status == DCC_OK && (view.present & roles))
    status = dcc_rest_json_append_snowflake_string_array_member(
        &body, &first, "include_roles", value->include_roles,
        value->include_role_count);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return finish_body(&body, out);
}

static dcc_status_t validate_colors(const dcc_rest_role_colors_t *colors,
                                    dcc_endpoint_record_view_t *view,
                                    uint64_t *out_nulls) {
  const uint64_t secondary = DCC_REST_ROLE_COLORS_PRESENT_SECONDARY_COLOR,
                 tertiary = DCC_REST_ROLE_COLORS_PRESENT_TERTIARY_COLOR;
  int partial = 0;
  if (dcc_endpoint_record_read(
          colors, offsetof(dcc_rest_role_colors_t, version),
          offsetof(dcc_rest_role_colors_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_role_colors_t, primary_color),
          DCC_REST_ROLE_COLORS_VERSION, secondary | tertiary, view) != DCC_OK)
    return DCC_ERR_INVALID_ARG;
  uint64_t nulls = read_nulls(colors, offsetof(dcc_rest_role_colors_t, nulls),
                              view->size, &partial);
  if (partial || (nulls & ~(secondary | tertiary)) ||
      (nulls & ~view->present) ||
      PARTIAL(dcc_rest_role_colors_t, *view, secondary_color) ||
      PARTIAL(dcc_rest_role_colors_t, *view, tertiary_color) ||
      !COVERED(dcc_rest_role_colors_t, *view, secondary, secondary_color) ||
      !COVERED(dcc_rest_role_colors_t, *view, tertiary, tertiary_color) ||
      colors->primary_color > 0xFFFFFFU ||
      ((view->present & secondary) && !(nulls & secondary) &&
       colors->secondary_color > 0xFFFFFFU) ||
      ((view->present & tertiary) && !(nulls & tertiary) &&
       colors->tertiary_color > 0xFFFFFFU))
    return DCC_ERR_INVALID_ARG;
  if ((view->present & tertiary) && !(nulls & tertiary) &&
      colors->tertiary_color != 0U) {
    if (colors->primary_color != 11127295U || !(view->present & secondary) ||
        (nulls & secondary) || colors->secondary_color != 16759788U ||
        colors->tertiary_color != 16761760U)
      return DCC_ERR_INVALID_ARG;
  }
  *out_nulls = nulls;
  return DCC_OK;
}

static dcc_status_t append_colors(dcc_rest_buffer_t *body, int *first,
                                  const dcc_rest_role_colors_t *colors,
                                  const dcc_endpoint_record_view_t *view,
                                  uint64_t nulls) {
  const uint64_t secondary = DCC_REST_ROLE_COLORS_PRESENT_SECONDARY_COLOR,
                 tertiary = DCC_REST_ROLE_COLORS_PRESENT_TERTIARY_COLOR;
  dcc_status_t status = dcc_rest_json_append_member_key(body, first, "colors");
  if (status == DCC_OK)
    status = dcc_rest_buffer_append_cstr(body, "{");
  int nested = 1;
  if (status == DCC_OK)
    status = dcc_rest_json_append_u64_member(body, &nested, "primary_color",
                                             colors->primary_color);
  if (status == DCC_OK && (view->present & secondary))
    status = append_nullable_u64(body, &nested, "secondary_color",
                                 colors->secondary_color,
                                 (nulls & secondary) != 0U, 0);
  if (status == DCC_OK && (view->present & tertiary))
    status = append_nullable_u64(body, &nested, "tertiary_color",
                                 colors->tertiary_color,
                                 (nulls & tertiary) != 0U, 0);
  if (status == DCC_OK)
    status = dcc_rest_buffer_append_cstr(body, "}");
  return status;
}

static dcc_status_t build_role(const void *raw, int create, char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (raw == NULL)
    return dcc_task8_empty_object(out);
  const dcc_rest_guild_role_create_t *value =
      (const dcc_rest_guild_role_create_t *)raw;
  const uint64_t known = UINT64_C(255);
  const uint64_t name = UINT64_C(1), permissions = UINT64_C(2),
                 color = UINT64_C(4), colors = UINT64_C(8),
                 hoist = UINT64_C(16), icon = UINT64_C(32),
                 unicode_emoji = UINT64_C(64), mentionable = UINT64_C(128);
  dcc_endpoint_record_view_t view;
  int partial = 0;
  size_t vo = create ? offsetof(dcc_rest_guild_role_create_t, version)
                     : offsetof(dcc_rest_guild_role_update_t, version);
  size_t po = create ? offsetof(dcc_rest_guild_role_create_t, present)
                     : offsetof(dcc_rest_guild_role_update_t, present);
  uint32_t version = create ? DCC_REST_GUILD_ROLE_CREATE_VERSION
                            : DCC_REST_GUILD_ROLE_UPDATE_VERSION;
  if (dcc_endpoint_record_read(
          raw, vo, po,
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_role_create_t, present),
          version, known, &view) != DCC_OK)
    return DCC_ERR_INVALID_ARG;
  uint64_t nulls = read_nulls(
      raw, offsetof(dcc_rest_guild_role_create_t, nulls), view.size, &partial);
  const uint64_t nullable = create ? icon | unicode_emoji : known;
  const size_t offs[] = {offsetof(dcc_rest_guild_role_create_t, name),
                         offsetof(dcc_rest_guild_role_create_t, permissions),
                         offsetof(dcc_rest_guild_role_create_t, color),
                         offsetof(dcc_rest_guild_role_create_t, colors),
                         offsetof(dcc_rest_guild_role_create_t, hoist),
                         offsetof(dcc_rest_guild_role_create_t, icon),
                         offsetof(dcc_rest_guild_role_create_t, unicode_emoji),
                         offsetof(dcc_rest_guild_role_create_t, mentionable)};
  const size_t widths[] = {
      sizeof(value->name),          sizeof(value->permissions),
      sizeof(value->color),         sizeof(value->colors),
      sizeof(value->hoist),         sizeof(value->icon),
      sizeof(value->unicode_emoji), sizeof(value->mentionable)};
  if (partial || (nulls & ~nullable) || (nulls & ~view.present))
    return DCC_ERR_INVALID_ARG;
  for (size_t i = 0; i < 8U; ++i)
    if (dcc_endpoint_field_partially_covered(view.size, offs[i], widths[i]) ||
        !dcc_endpoint_present_field_covered(&view, UINT64_C(1) << i, offs[i],
                                            widths[i]))
      return DCC_ERR_INVALID_ARG;
  if ((view.present & name) && !(nulls & name) &&
      !text_scalars_between(value->name, 0U, 100U))
    return DCC_ERR_INVALID_ARG;
  if ((view.present & color) && !(nulls & color) && value->color > 0xFFFFFFU)
    return DCC_ERR_INVALID_ARG;
  if ((view.present & color) && (view.present & colors))
    return DCC_ERR_INVALID_ARG;
  dcc_endpoint_record_view_t colors_view = {0};
  uint64_t colors_nulls = 0U;
  if ((view.present & colors) && !(nulls & colors) && value->colors == NULL)
    return DCC_ERR_INVALID_ARG;
  if ((view.present & colors) && !(nulls & colors) &&
      validate_colors(value->colors, &colors_view, &colors_nulls) != DCC_OK)
    return DCC_ERR_INVALID_ARG;
  if ((view.present & hoist) && !(nulls & hoist) && value->hoist > 1U)
    return DCC_ERR_INVALID_ARG;
  if ((view.present & mentionable) && !(nulls & mentionable) &&
      value->mentionable > 1U)
    return DCC_ERR_INVALID_ARG;
  if ((view.present & icon) && !(nulls & icon) &&
      !text_scalars_between(value->icon, 1U, SIZE_MAX))
    return DCC_ERR_INVALID_ARG;
  if ((view.present & unicode_emoji) && !(nulls & unicode_emoji) &&
      !text_scalars_between(value->unicode_emoji, 1U, SIZE_MAX))
    return DCC_ERR_INVALID_ARG;
  if ((view.present & icon) && !(nulls & icon) &&
      (view.present & unicode_emoji) && !(nulls & unicode_emoji))
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK && (view.present & name))
    status =
        (nulls & name)
            ? dcc_rest_json_append_raw_member(&body, &first, "name", "null")
            : dcc_rest_json_append_string_member(&body, &first, "name",
                                                 value->name);
  if (status == DCC_OK && (view.present & permissions))
    status = (nulls & permissions)
                 ? dcc_rest_json_append_raw_member(&body, &first, "permissions",
                                                   "null")
                 : dcc_rest_json_append_u64_string_member(
                       &body, &first, "permissions", value->permissions);
  if (status == DCC_OK && (view.present & color))
    status = append_nullable_u64(&body, &first, "color", value->color,
                                 (nulls & color) != 0U, 0);
  if (status == DCC_OK && (view.present & colors))
    status =
        (nulls & colors)
            ? dcc_rest_json_append_raw_member(&body, &first, "colors", "null")
            : append_colors(&body, &first, value->colors, &colors_view,
                            colors_nulls);
  if (status == DCC_OK && (view.present & hoist))
    status = append_nullable_bool(&body, &first, "hoist", value->hoist,
                                  (nulls & hoist) != 0U);
  if (status == DCC_OK && (view.present & icon))
    status =
        (nulls & icon)
            ? dcc_rest_json_append_raw_member(&body, &first, "icon", "null")
            : dcc_rest_json_append_string_member(&body, &first, "icon",
                                                 value->icon);
  if (status == DCC_OK && (view.present & unicode_emoji))
    status = (nulls & unicode_emoji)
                 ? dcc_rest_json_append_raw_member(&body, &first,
                                                   "unicode_emoji", "null")
                 : dcc_rest_json_append_string_member(
                       &body, &first, "unicode_emoji", value->unicode_emoji);
  if (status == DCC_OK && (view.present & mentionable))
    status =
        append_nullable_bool(&body, &first, "mentionable", value->mentionable,
                             (nulls & mentionable) != 0U);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return finish_body(&body, out);
}
dcc_status_t
dcc_task8_build_role_create(const dcc_rest_guild_role_create_t *value,
                            char **out) {
  return build_role(value, 1, out);
}
dcc_status_t
dcc_task8_build_role_update(const dcc_rest_guild_role_update_t *value,
                            char **out) {
  return build_role(value, 0, out);
}

static dcc_status_t append_i32_member(dcc_rest_buffer_t *body, int *first,
                                      const char *key, int32_t value) {
  char text[32];
  int n = snprintf(text, sizeof(text), "%d", value);
  if (n < 0 || (size_t)n >= sizeof(text))
    return DCC_ERR_INVALID_ARG;
  return dcc_rest_json_append_raw_member(body, first, key, text);
}
dcc_status_t
dcc_task8_build_role_positions(const dcc_rest_guild_role_position_t *values,
                               size_t count, char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (values == NULL || count == 0U)
    return DCC_ERR_INVALID_ARG;
  uintptr_t address = (uintptr_t)values;
  if (address % _Alignof(dcc_rest_guild_role_position_t) != 0U)
    return DCC_ERR_INVALID_ARG;
  dcc_endpoint_record_view_t first_view;
  if (dcc_endpoint_record_read(
          values, offsetof(dcc_rest_guild_role_position_t, version),
          offsetof(dcc_rest_guild_role_position_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_role_position_t, role_id),
          DCC_REST_GUILD_ROLE_POSITION_VERSION,
          DCC_REST_GUILD_ROLE_POSITION_PRESENT_POSITION,
          &first_view) != DCC_OK ||
      first_view.size % _Alignof(dcc_rest_guild_role_position_t) != 0U ||
      count > SIZE_MAX / first_view.size ||
      address > UINTPTR_MAX - (count * first_view.size - 1U))
    return DCC_ERR_INVALID_ARG;
  size_t stride = first_view.size;
  for (size_t i = 0; i < count; ++i) {
    const dcc_rest_guild_role_position_t *v =
        (const dcc_rest_guild_role_position_t *)((const unsigned char *)values +
                                                 i * stride);
    dcc_endpoint_record_view_t view;
    int partial = 0;
    if (dcc_endpoint_record_read(
            v, offsetof(dcc_rest_guild_role_position_t, version),
            offsetof(dcc_rest_guild_role_position_t, present),
            DCC_ENDPOINT_FIELD_END(dcc_rest_guild_role_position_t, role_id),
            DCC_REST_GUILD_ROLE_POSITION_VERSION,
            DCC_REST_GUILD_ROLE_POSITION_PRESENT_POSITION, &view) != DCC_OK ||
        view.size != stride)
      return DCC_ERR_INVALID_ARG;
    uint64_t nulls =
        read_nulls(v, offsetof(dcc_rest_guild_role_position_t, nulls),
                   view.size, &partial);
    if (partial || (nulls & ~DCC_REST_GUILD_ROLE_POSITION_PRESENT_POSITION) ||
        (nulls & ~view.present) ||
        PARTIAL(dcc_rest_guild_role_position_t, view, position) ||
        !COVERED(dcc_rest_guild_role_position_t, view,
                 DCC_REST_GUILD_ROLE_POSITION_PRESENT_POSITION, position) ||
        v->role_id == 0U ||
        ((view.present & DCC_REST_GUILD_ROLE_POSITION_PRESENT_POSITION) &&
         !(nulls & DCC_REST_GUILD_ROLE_POSITION_PRESENT_POSITION) &&
         v->position < 0))
      return DCC_ERR_INVALID_ARG;
    for (size_t j = 0; j < i; ++j) {
      const dcc_rest_guild_role_position_t *p =
          (const dcc_rest_guild_role_position_t *)((const unsigned char *)
                                                       values +
                                                   j * stride);
      if (p->role_id == v->role_id)
        return DCC_ERR_INVALID_ARG;
    }
  }
  dcc_rest_buffer_t body = {0};
  dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "[");
  for (size_t i = 0; i < count && status == DCC_OK; ++i) {
    const dcc_rest_guild_role_position_t *v =
        (const dcc_rest_guild_role_position_t *)((const unsigned char *)values +
                                                 i * stride);
    dcc_endpoint_record_view_t view;
    dcc_endpoint_record_read(
        v, offsetof(dcc_rest_guild_role_position_t, version),
        offsetof(dcc_rest_guild_role_position_t, present),
        DCC_ENDPOINT_FIELD_END(dcc_rest_guild_role_position_t, role_id),
        DCC_REST_GUILD_ROLE_POSITION_VERSION,
        DCC_REST_GUILD_ROLE_POSITION_PRESENT_POSITION, &view);
    int partial = 0;
    uint64_t nulls =
        read_nulls(v, offsetof(dcc_rest_guild_role_position_t, nulls),
                   view.size, &partial);
    if (i)
      status = dcc_rest_buffer_append_cstr(&body, ",");
    int first = 1;
    if (status == DCC_OK)
      status = dcc_rest_buffer_append_cstr(&body, "{");
    if (status == DCC_OK)
      status = dcc_rest_json_append_u64_string_member(&body, &first, "id",
                                                      v->role_id);
    if (status == DCC_OK &&
        (view.present & DCC_REST_GUILD_ROLE_POSITION_PRESENT_POSITION))
      status = (nulls & DCC_REST_GUILD_ROLE_POSITION_PRESENT_POSITION)
                   ? dcc_rest_json_append_raw_member(&body, &first, "position",
                                                     "null")
                   : append_i32_member(&body, &first, "position", v->position);
    if (status == DCC_OK)
      status = dcc_rest_buffer_append_cstr(&body, "}");
  }
  if (status == DCC_OK)
    status = dcc_rest_buffer_append_cstr(&body, "]");
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  *out = body.data;
  return DCC_OK;
}

#undef PARTIAL
#undef COVERED
