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

static uint64_t guild_nulls(const void *value, size_t offset, size_t size,
                            int *partial) {
  uint64_t nulls = 0U;
  *partial = dcc_endpoint_field_partially_covered(size, offset, sizeof(nulls));
  if (dcc_endpoint_field_covered(size, offset, sizeof(nulls)))
    memcpy(&nulls, (const unsigned char *)value + offset, sizeof(nulls));
  return nulls;
}
static int text_valid(const char *text, size_t min, size_t max) {
  size_t n = 0U;
  return text != NULL && dcc_endpoint_utf8_scalar_count(text, &n) && n >= min &&
         n <= max;
}
static int string_array_valid(const char *const *values, size_t count) {
  if (count == 0U)
    return 1;
  if (values == NULL)
    return 0;
  uintptr_t address = (uintptr_t)values;
  if (address % _Alignof(const char *) != 0U ||
      count > SIZE_MAX / sizeof(*values))
    return 0;
  size_t span = count * sizeof(*values);
  if (address > UINTPTR_MAX - (span - 1U))
    return 0;
  for (size_t i = 0U; i < count; ++i)
    if (!text_valid(values[i], 1U, SIZE_MAX))
      return 0;
  return 1;
}
static dcc_status_t close_object(dcc_rest_buffer_t *body, char **out) {
  dcc_status_t status = dcc_rest_buffer_append_cstr(body, "}");
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(body);
    return status;
  }
  *out = body->data;
  return DCC_OK;
}
static dcc_status_t nullable_string(dcc_rest_buffer_t *body, int *first,
                                    const char *key, const char *value,
                                    int null_value) {
  return null_value
             ? dcc_rest_json_append_raw_member(body, first, key, "null")
             : dcc_rest_json_append_string_member(body, first, key, value);
}
static dcc_status_t nullable_u64(dcc_rest_buffer_t *body, int *first,
                                 const char *key, uint64_t value,
                                 int null_value, int as_string) {
  if (null_value)
    return dcc_rest_json_append_raw_member(body, first, key, "null");
  return as_string
             ? dcc_rest_json_append_u64_string_member(body, first, key, value)
             : dcc_rest_json_append_u64_member(body, first, key, value);
}

static int afk_timeout_valid(dcc_guild_afk_timeout_t value) {
  return value == DCC_GUILD_AFK_OFF || value == DCC_GUILD_AFK_60 ||
         value == DCC_GUILD_AFK_300 || value == DCC_GUILD_AFK_900 ||
         value == DCC_GUILD_AFK_1800 || value == DCC_GUILD_AFK_3600;
}
dcc_status_t dcc_task8_build_guild_update(const dcc_rest_guild_update_t *value,
                                          char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (value == NULL)
    return dcc_task8_empty_object(out);
  const uint64_t known = (UINT64_C(1) << 20U) - 1U;
  const uint64_t nonnullable =
      DCC_REST_GUILD_UPDATE_PRESENT_NAME |
      DCC_REST_GUILD_UPDATE_PRESENT_AFK_TIMEOUT |
      DCC_REST_GUILD_UPDATE_PRESENT_SYSTEM_CHANNEL_FLAGS |
      DCC_REST_GUILD_UPDATE_PRESENT_FEATURES |
      DCC_REST_GUILD_UPDATE_PRESENT_PREMIUM_PROGRESS_BAR_ENABLED;
  dcc_endpoint_record_view_t view;
  int partial = 0;
  if (dcc_endpoint_record_read(
          value, offsetof(dcc_rest_guild_update_t, version),
          offsetof(dcc_rest_guild_update_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_update_t, present),
          DCC_REST_GUILD_UPDATE_VERSION, known, &view) != DCC_OK)
    return DCC_ERR_INVALID_ARG;
  uint64_t nulls = guild_nulls(value, offsetof(dcc_rest_guild_update_t, nulls),
                               view.size, &partial);
  if (partial || (nulls & nonnullable) || (nulls & ~view.present))
    return DCC_ERR_INVALID_ARG;
  const size_t offs[] = {
      offsetof(dcc_rest_guild_update_t, name),
      offsetof(dcc_rest_guild_update_t, region),
      offsetof(dcc_rest_guild_update_t, verification_level),
      offsetof(dcc_rest_guild_update_t, default_message_notifications),
      offsetof(dcc_rest_guild_update_t, explicit_content_filter),
      offsetof(dcc_rest_guild_update_t, afk_channel_id),
      offsetof(dcc_rest_guild_update_t, afk_timeout),
      offsetof(dcc_rest_guild_update_t, icon),
      offsetof(dcc_rest_guild_update_t, splash),
      offsetof(dcc_rest_guild_update_t, discovery_splash),
      offsetof(dcc_rest_guild_update_t, banner),
      offsetof(dcc_rest_guild_update_t, system_channel_id),
      offsetof(dcc_rest_guild_update_t, system_channel_flags),
      offsetof(dcc_rest_guild_update_t, rules_channel_id),
      offsetof(dcc_rest_guild_update_t, public_updates_channel_id),
      offsetof(dcc_rest_guild_update_t, preferred_locale),
      offsetof(dcc_rest_guild_update_t, features),
      offsetof(dcc_rest_guild_update_t, feature_count),
      offsetof(dcc_rest_guild_update_t, description),
      offsetof(dcc_rest_guild_update_t, premium_progress_bar_enabled),
      offsetof(dcc_rest_guild_update_t, safety_alerts_channel_id)};
  const size_t widths[] = {sizeof(value->name),
                           sizeof(value->region),
                           sizeof(value->verification_level),
                           sizeof(value->default_message_notifications),
                           sizeof(value->explicit_content_filter),
                           sizeof(value->afk_channel_id),
                           sizeof(value->afk_timeout),
                           sizeof(value->icon),
                           sizeof(value->splash),
                           sizeof(value->discovery_splash),
                           sizeof(value->banner),
                           sizeof(value->system_channel_id),
                           sizeof(value->system_channel_flags),
                           sizeof(value->rules_channel_id),
                           sizeof(value->public_updates_channel_id),
                           sizeof(value->preferred_locale),
                           sizeof(value->features),
                           sizeof(value->feature_count),
                           sizeof(value->description),
                           sizeof(value->premium_progress_bar_enabled),
                           sizeof(value->safety_alerts_channel_id)};
  const uint64_t bits[] = {
      UINT64_C(1),     UINT64_C(2),     UINT64_C(4),      UINT64_C(8),
      UINT64_C(16),    UINT64_C(32),    UINT64_C(64),     UINT64_C(128),
      UINT64_C(256),   UINT64_C(512),   UINT64_C(1024),   UINT64_C(2048),
      UINT64_C(4096),  UINT64_C(8192),  UINT64_C(16384),  UINT64_C(32768),
      UINT64_C(65536), UINT64_C(65536), UINT64_C(131072), UINT64_C(262144),
      UINT64_C(524288)};
  for (size_t i = 0; i < 21U; ++i)
    if (dcc_endpoint_field_partially_covered(view.size, offs[i], widths[i]) ||
        !dcc_endpoint_present_field_covered(&view, bits[i], offs[i], widths[i]))
      return DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_REST_GUILD_UPDATE_PRESENT_NAME) != 0U) {
    if (!text_valid(value->name, 2U, 100U))
      return DCC_ERR_INVALID_ARG;
    if (dcc_task8_text_has_edge_whitespace(value->name))
      return DCC_ERR_INVALID_ARG;
  }
  if ((view.present & DCC_REST_GUILD_UPDATE_PRESENT_REGION) &&
      !(nulls & DCC_REST_GUILD_UPDATE_PRESENT_REGION) &&
      !text_valid(value->region, 1U, SIZE_MAX))
    return DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_REST_GUILD_UPDATE_PRESENT_VERIFICATION_LEVEL) &&
      !(nulls & DCC_REST_GUILD_UPDATE_PRESENT_VERIFICATION_LEVEL) &&
      (value->verification_level < DCC_GUILD_VERIFY_NONE ||
       value->verification_level > DCC_GUILD_VERIFY_VERY_HIGH))
    return DCC_ERR_INVALID_ARG;
  if ((view.present &
       DCC_REST_GUILD_UPDATE_PRESENT_DEFAULT_MESSAGE_NOTIFICATIONS) &&
      !(nulls & DCC_REST_GUILD_UPDATE_PRESENT_DEFAULT_MESSAGE_NOTIFICATIONS) &&
      (value->default_message_notifications < DCC_GUILD_NOTIFY_ALL ||
       value->default_message_notifications > DCC_GUILD_NOTIFY_ONLY_MENTIONS))
    return DCC_ERR_INVALID_ARG;
  if ((view.present & DCC_REST_GUILD_UPDATE_PRESENT_EXPLICIT_CONTENT_FILTER) &&
      !(nulls & DCC_REST_GUILD_UPDATE_PRESENT_EXPLICIT_CONTENT_FILTER) &&
      (value->explicit_content_filter < DCC_GUILD_EXPLICIT_CONTENT_DISABLED ||
       value->explicit_content_filter > DCC_GUILD_EXPLICIT_CONTENT_ALL_MEMBERS))
    return DCC_ERR_INVALID_ARG;
#define CHECK_NONZERO_ID(bit_, field_)                                         \
  if ((view.present & (bit_)) && !(nulls & (bit_)) && value->field_ == 0U)     \
  return DCC_ERR_INVALID_ARG
  CHECK_NONZERO_ID(DCC_REST_GUILD_UPDATE_PRESENT_AFK_CHANNEL_ID,
                   afk_channel_id);
  CHECK_NONZERO_ID(DCC_REST_GUILD_UPDATE_PRESENT_SYSTEM_CHANNEL_ID,
                   system_channel_id);
  CHECK_NONZERO_ID(DCC_REST_GUILD_UPDATE_PRESENT_RULES_CHANNEL_ID,
                   rules_channel_id);
  CHECK_NONZERO_ID(DCC_REST_GUILD_UPDATE_PRESENT_PUBLIC_UPDATES_CHANNEL_ID,
                   public_updates_channel_id);
  CHECK_NONZERO_ID(DCC_REST_GUILD_UPDATE_PRESENT_SAFETY_ALERTS_CHANNEL_ID,
                   safety_alerts_channel_id);
#undef CHECK_NONZERO_ID
  if ((view.present & DCC_REST_GUILD_UPDATE_PRESENT_AFK_TIMEOUT) &&
      !afk_timeout_valid(value->afk_timeout))
    return DCC_ERR_INVALID_ARG;
#define CHECK_STRING(bit_, field_)                                             \
  if ((view.present & (bit_)) && !(nulls & (bit_)) &&                          \
      !text_valid(value->field_, 1U, SIZE_MAX))                                \
  return DCC_ERR_INVALID_ARG
  CHECK_STRING(DCC_REST_GUILD_UPDATE_PRESENT_ICON, icon);
  CHECK_STRING(DCC_REST_GUILD_UPDATE_PRESENT_SPLASH, splash);
  CHECK_STRING(DCC_REST_GUILD_UPDATE_PRESENT_DISCOVERY_SPLASH,
               discovery_splash);
  CHECK_STRING(DCC_REST_GUILD_UPDATE_PRESENT_BANNER, banner);
  CHECK_STRING(DCC_REST_GUILD_UPDATE_PRESENT_PREFERRED_LOCALE,
               preferred_locale);
  CHECK_STRING(DCC_REST_GUILD_UPDATE_PRESENT_DESCRIPTION, description);
#undef CHECK_STRING
  if ((view.present & DCC_REST_GUILD_UPDATE_PRESENT_FEATURES)) {
    if (!string_array_valid(value->features, value->feature_count))
      return DCC_ERR_INVALID_ARG;
  }
  if ((view.present &
       DCC_REST_GUILD_UPDATE_PRESENT_PREMIUM_PROGRESS_BAR_ENABLED) &&
      value->premium_progress_bar_enabled > 1U)
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
#define APPEND_STRING(bit_, key_, field_)                                      \
  if (status == DCC_OK && (view.present & (bit_)))                             \
  status = nullable_string(&body, &first, (key_), value->field_,               \
                           (nulls & (bit_)) != 0U)
#define APPEND_NUMBER(bit_, key_, field_)                                      \
  if (status == DCC_OK && (view.present & (bit_)))                             \
  status = nullable_u64(&body, &first, (key_), (uint64_t)value->field_,        \
                        (nulls & (bit_)) != 0U, 0)
#define APPEND_ID(bit_, key_, field_)                                          \
  if (status == DCC_OK && (view.present & (bit_)))                             \
  status = nullable_u64(&body, &first, (key_), value->field_,                  \
                        (nulls & (bit_)) != 0U, 1)
  APPEND_STRING(DCC_REST_GUILD_UPDATE_PRESENT_NAME, "name", name);
  APPEND_STRING(DCC_REST_GUILD_UPDATE_PRESENT_REGION, "region", region);
  APPEND_NUMBER(DCC_REST_GUILD_UPDATE_PRESENT_VERIFICATION_LEVEL,
                "verification_level", verification_level);
  APPEND_NUMBER(DCC_REST_GUILD_UPDATE_PRESENT_DEFAULT_MESSAGE_NOTIFICATIONS,
                "default_message_notifications", default_message_notifications);
  APPEND_NUMBER(DCC_REST_GUILD_UPDATE_PRESENT_EXPLICIT_CONTENT_FILTER,
                "explicit_content_filter", explicit_content_filter);
  APPEND_ID(DCC_REST_GUILD_UPDATE_PRESENT_AFK_CHANNEL_ID, "afk_channel_id",
            afk_channel_id);
  APPEND_NUMBER(DCC_REST_GUILD_UPDATE_PRESENT_AFK_TIMEOUT, "afk_timeout",
                afk_timeout);
  APPEND_STRING(DCC_REST_GUILD_UPDATE_PRESENT_ICON, "icon", icon);
  APPEND_STRING(DCC_REST_GUILD_UPDATE_PRESENT_SPLASH, "splash", splash);
  APPEND_STRING(DCC_REST_GUILD_UPDATE_PRESENT_DISCOVERY_SPLASH,
                "discovery_splash", discovery_splash);
  APPEND_STRING(DCC_REST_GUILD_UPDATE_PRESENT_BANNER, "banner", banner);
  APPEND_ID(DCC_REST_GUILD_UPDATE_PRESENT_SYSTEM_CHANNEL_ID,
            "system_channel_id", system_channel_id);
  APPEND_NUMBER(DCC_REST_GUILD_UPDATE_PRESENT_SYSTEM_CHANNEL_FLAGS,
                "system_channel_flags", system_channel_flags);
  APPEND_ID(DCC_REST_GUILD_UPDATE_PRESENT_RULES_CHANNEL_ID, "rules_channel_id",
            rules_channel_id);
  APPEND_ID(DCC_REST_GUILD_UPDATE_PRESENT_PUBLIC_UPDATES_CHANNEL_ID,
            "public_updates_channel_id", public_updates_channel_id);
  APPEND_STRING(DCC_REST_GUILD_UPDATE_PRESENT_PREFERRED_LOCALE,
                "preferred_locale", preferred_locale);
  if (status == DCC_OK &&
      (view.present & DCC_REST_GUILD_UPDATE_PRESENT_FEATURES))
    status = dcc_rest_json_append_string_array_member(
        &body, &first, "features", value->features, value->feature_count);
  APPEND_STRING(DCC_REST_GUILD_UPDATE_PRESENT_DESCRIPTION, "description",
                description);
  if (status == DCC_OK &&
      (view.present &
       DCC_REST_GUILD_UPDATE_PRESENT_PREMIUM_PROGRESS_BAR_ENABLED))
    status = dcc_rest_json_append_bool_member(
        &body, &first, "premium_progress_bar_enabled",
        value->premium_progress_bar_enabled);
  APPEND_ID(DCC_REST_GUILD_UPDATE_PRESENT_SAFETY_ALERTS_CHANNEL_ID,
            "safety_alerts_channel_id", safety_alerts_channel_id);
#undef APPEND_STRING
#undef APPEND_NUMBER
#undef APPEND_ID
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return close_object(&body, out);
}

dcc_status_t
dcc_task8_build_incident_actions(const dcc_rest_guild_incident_actions_t *value,
                                 char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (value == NULL)
    return dcc_task8_empty_object(out);
  const uint64_t
      invites = DCC_REST_GUILD_INCIDENT_ACTIONS_PRESENT_INVITES_DISABLED_UNTIL,
      dms = DCC_REST_GUILD_INCIDENT_ACTIONS_PRESENT_DMS_DISABLED_UNTIL;
  dcc_endpoint_record_view_t view;
  int partial = 0;
  if (dcc_endpoint_record_read(
          value, offsetof(dcc_rest_guild_incident_actions_t, version),
          offsetof(dcc_rest_guild_incident_actions_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_incident_actions_t, present),
          DCC_REST_GUILD_INCIDENT_ACTIONS_VERSION, invites | dms,
          &view) != DCC_OK)
    return DCC_ERR_INVALID_ARG;
  uint64_t nulls =
      guild_nulls(value, offsetof(dcc_rest_guild_incident_actions_t, nulls),
                  view.size, &partial);
  if (partial || (nulls & ~(invites | dms)) || (nulls & ~view.present) ||
      PARTIAL(dcc_rest_guild_incident_actions_t, view,
              invites_disabled_until) ||
      PARTIAL(dcc_rest_guild_incident_actions_t, view, dms_disabled_until) ||
      !COVERED(dcc_rest_guild_incident_actions_t, view, invites,
               invites_disabled_until) ||
      !COVERED(dcc_rest_guild_incident_actions_t, view, dms,
               dms_disabled_until) ||
      ((view.present & invites) && !(nulls & invites) &&
       !dcc_task8_iso8601_valid(value->invites_disabled_until)) ||
      ((view.present & dms) && !(nulls & dms) &&
       !dcc_task8_iso8601_valid(value->dms_disabled_until)))
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK && (view.present & invites))
    status =
        nullable_string(&body, &first, "invites_disabled_until",
                        value->invites_disabled_until, (nulls & invites) != 0U);
  if (status == DCC_OK && (view.present & dms))
    status = nullable_string(&body, &first, "dms_disabled_until",
                             value->dms_disabled_until, (nulls & dms) != 0U);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return close_object(&body, out);
}

dcc_status_t
dcc_task8_build_widget_update(const dcc_rest_guild_widget_update_t *value,
                              char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (value == NULL)
    return dcc_task8_empty_object(out);
  const uint64_t enabled = DCC_REST_GUILD_WIDGET_UPDATE_PRESENT_ENABLED,
                 channel = DCC_REST_GUILD_WIDGET_UPDATE_PRESENT_CHANNEL_ID;
  dcc_endpoint_record_view_t view;
  int partial = 0;
  if (dcc_endpoint_record_read(
          value, offsetof(dcc_rest_guild_widget_update_t, version),
          offsetof(dcc_rest_guild_widget_update_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_widget_update_t, present),
          DCC_REST_GUILD_WIDGET_UPDATE_VERSION, enabled | channel,
          &view) != DCC_OK)
    return DCC_ERR_INVALID_ARG;
  uint64_t nulls =
      guild_nulls(value, offsetof(dcc_rest_guild_widget_update_t, nulls),
                  view.size, &partial);
  if (partial || (nulls & ~channel) || (nulls & ~view.present) ||
      PARTIAL(dcc_rest_guild_widget_update_t, view, enabled) ||
      PARTIAL(dcc_rest_guild_widget_update_t, view, channel_id) ||
      !COVERED(dcc_rest_guild_widget_update_t, view, enabled, enabled) ||
      !COVERED(dcc_rest_guild_widget_update_t, view, channel, channel_id) ||
      ((view.present & enabled) && value->enabled > 1U) ||
      ((view.present & channel) && !(nulls & channel) &&
       value->channel_id == 0U))
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK && (view.present & enabled))
    status = dcc_rest_json_append_bool_member(&body, &first, "enabled",
                                              value->enabled);
  if (status == DCC_OK && (view.present & channel))
    status = nullable_u64(&body, &first, "channel_id", value->channel_id,
                          (nulls & channel) != 0U, 1);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return close_object(&body, out);
}

static dcc_status_t
welcome_channel_validate(const dcc_rest_welcome_channel_t *v, size_t expected,
                         dcc_endpoint_record_view_t *view,
                         uint64_t *out_nulls) {
  const uint64_t emoji_id = DCC_REST_WELCOME_CHANNEL_PRESENT_EMOJI_ID,
                 emoji_name = DCC_REST_WELCOME_CHANNEL_PRESENT_EMOJI_NAME;
  int partial = 0;
  if (dcc_endpoint_record_read(
          v, offsetof(dcc_rest_welcome_channel_t, version),
          offsetof(dcc_rest_welcome_channel_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_welcome_channel_t, description),
          DCC_REST_WELCOME_CHANNEL_VERSION, emoji_id | emoji_name,
          view) != DCC_OK ||
      (expected != 0U && view->size != expected))
    return DCC_ERR_INVALID_ARG;
  uint64_t nulls = guild_nulls(v, offsetof(dcc_rest_welcome_channel_t, nulls),
                               view->size, &partial);
  if (partial || (nulls & ~(emoji_id | emoji_name)) ||
      (nulls & ~view->present) ||
      PARTIAL(dcc_rest_welcome_channel_t, *view, emoji_id) ||
      PARTIAL(dcc_rest_welcome_channel_t, *view, emoji_name) ||
      !COVERED(dcc_rest_welcome_channel_t, *view, emoji_id, emoji_id) ||
      !COVERED(dcc_rest_welcome_channel_t, *view, emoji_name, emoji_name) ||
      v->channel_id == 0U || !text_valid(v->description, 1U, SIZE_MAX) ||
      ((view->present & emoji_id) && !(nulls & emoji_id) &&
       v->emoji_id == 0U) ||
      ((view->present & emoji_name) && !(nulls & emoji_name) &&
       !text_valid(v->emoji_name, 1U, SIZE_MAX)) ||
      ((view->present & emoji_id) && !(nulls & emoji_id) &&
       (view->present & emoji_name) && !(nulls & emoji_name)))
    return DCC_ERR_INVALID_ARG;
  *out_nulls = nulls;
  return DCC_OK;
}

dcc_status_t dcc_task8_build_welcome_screen(
    const dcc_rest_guild_welcome_screen_update_t *value, char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (value == NULL)
    return dcc_task8_empty_object(out);
  const uint64_t
      enabled = DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_ENABLED,
      channels = DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_WELCOME_CHANNELS,
      description = DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_DESCRIPTION;
  dcc_endpoint_record_view_t view;
  int partial = 0;
  if (dcc_endpoint_record_read(
          value, offsetof(dcc_rest_guild_welcome_screen_update_t, version),
          offsetof(dcc_rest_guild_welcome_screen_update_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_welcome_screen_update_t,
                                 present),
          DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_VERSION,
          enabled | channels | description, &view) != DCC_OK)
    return DCC_ERR_INVALID_ARG;
  uint64_t nulls = guild_nulls(
      value, offsetof(dcc_rest_guild_welcome_screen_update_t, nulls), view.size,
      &partial);
  if (partial || (nulls & ~(enabled | channels | description)) ||
      (nulls & ~view.present) ||
      PARTIAL(dcc_rest_guild_welcome_screen_update_t, view, enabled) ||
      PARTIAL(dcc_rest_guild_welcome_screen_update_t, view, welcome_channels) ||
      PARTIAL(dcc_rest_guild_welcome_screen_update_t, view,
              welcome_channel_count) ||
      PARTIAL(dcc_rest_guild_welcome_screen_update_t, view, description) ||
      !COVERED(dcc_rest_guild_welcome_screen_update_t, view, enabled,
               enabled) ||
      !COVERED(dcc_rest_guild_welcome_screen_update_t, view, channels,
               welcome_channels) ||
      !COVERED(dcc_rest_guild_welcome_screen_update_t, view, channels,
               welcome_channel_count) ||
      !COVERED(dcc_rest_guild_welcome_screen_update_t, view, description,
               description) ||
      ((view.present & enabled) && !(nulls & enabled) && value->enabled > 1U) ||
      ((view.present & description) && !(nulls & description) &&
       !text_valid(value->description, 1U, SIZE_MAX)) ||
      ((view.present & channels) && !(nulls & channels) &&
       value->welcome_channel_count > 5U) ||
      ((view.present & channels) && !(nulls & channels) &&
       value->welcome_channel_count != 0U && value->welcome_channels == NULL))
    return DCC_ERR_INVALID_ARG;
  size_t stride = 0U;
  if ((view.present & channels) && !(nulls & channels) &&
      value->welcome_channel_count != 0U) {
    uintptr_t address = (uintptr_t)value->welcome_channels;
    dcc_endpoint_record_view_t first_view;
    uint64_t item_nulls;
    if (address % _Alignof(dcc_rest_welcome_channel_t) != 0U ||
        welcome_channel_validate(value->welcome_channels, 0U, &first_view,
                                 &item_nulls) != DCC_OK ||
        first_view.size % _Alignof(dcc_rest_welcome_channel_t) != 0U ||
        value->welcome_channel_count > SIZE_MAX / first_view.size ||
        address >
            UINTPTR_MAX - (value->welcome_channel_count * first_view.size - 1U))
      return DCC_ERR_INVALID_ARG;
    stride = first_view.size;
    for (size_t i = 0; i < value->welcome_channel_count; ++i) {
      const dcc_rest_welcome_channel_t *item =
          (const dcc_rest_welcome_channel_t *)((const unsigned char *)
                                                   value->welcome_channels +
                                               i * stride);
      dcc_endpoint_record_view_t item_view;
      if (welcome_channel_validate(item, stride, &item_view, &item_nulls) !=
          DCC_OK)
        return DCC_ERR_INVALID_ARG;
    }
  }
  dcc_rest_buffer_t body = {0};
  int first = 1;
  dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK && (view.present & enabled))
    status =
        nulls & enabled
            ? dcc_rest_json_append_raw_member(&body, &first, "enabled", "null")
            : dcc_rest_json_append_bool_member(&body, &first, "enabled",
                                               value->enabled);
  if (status == DCC_OK && (view.present & channels)) {
    if (nulls & channels)
      status = dcc_rest_json_append_raw_member(&body, &first,
                                               "welcome_channels", "null");
    else {
      status =
          dcc_rest_json_append_member_key(&body, &first, "welcome_channels");
      if (status == DCC_OK)
        status = dcc_rest_buffer_append_cstr(&body, "[");
      for (size_t i = 0; i < value->welcome_channel_count && status == DCC_OK;
           ++i) {
        const dcc_rest_welcome_channel_t *item =
            (const dcc_rest_welcome_channel_t *)((const unsigned char *)
                                                     value->welcome_channels +
                                                 i * stride);
        dcc_endpoint_record_view_t item_view;
        uint64_t item_nulls;
        welcome_channel_validate(item, stride, &item_view, &item_nulls);
        if (i)
          status = dcc_rest_buffer_append_cstr(&body, ",");
        int nested = 1;
        if (status == DCC_OK)
          status = dcc_rest_buffer_append_cstr(&body, "{");
        if (status == DCC_OK)
          status = dcc_rest_json_append_u64_string_member(
              &body, &nested, "channel_id", item->channel_id);
        if (status == DCC_OK)
          status = dcc_rest_json_append_string_member(
              &body, &nested, "description", item->description);
        if (status == DCC_OK &&
            (item_view.present & DCC_REST_WELCOME_CHANNEL_PRESENT_EMOJI_ID))
          status = nullable_u64(
              &body, &nested, "emoji_id", item->emoji_id,
              (item_nulls & DCC_REST_WELCOME_CHANNEL_PRESENT_EMOJI_ID) != 0U,
              1);
        if (status == DCC_OK &&
            (item_view.present & DCC_REST_WELCOME_CHANNEL_PRESENT_EMOJI_NAME))
          status = nullable_string(
              &body, &nested, "emoji_name", item->emoji_name,
              (item_nulls & DCC_REST_WELCOME_CHANNEL_PRESENT_EMOJI_NAME) != 0U);
        if (status == DCC_OK)
          status = dcc_rest_buffer_append_cstr(&body, "}");
      }
      if (status == DCC_OK)
        status = dcc_rest_buffer_append_cstr(&body, "]");
    }
  }
  if (status == DCC_OK && (view.present & description))
    status = nullable_string(&body, &first, "description", value->description,
                             (nulls & description) != 0U);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return close_object(&body, out);
}

static int base64_sound_valid(const char *sound) {
  static const char mp3[] = "data:audio/mpeg;base64,";
  static const char ogg[] = "data:audio/ogg;base64,";
  const char *p = NULL;
  if (sound != NULL && strncmp(sound, mp3, sizeof(mp3) - 1U) == 0)
    p = sound + sizeof(mp3) - 1U;
  else if (sound != NULL && strncmp(sound, ogg, sizeof(ogg) - 1U) == 0)
    p = sound + sizeof(ogg) - 1U;
  else
    return 0;
  size_t n = strlen(p);
  if (n == 0U || n % 4U != 0U)
    return 0;
  size_t padding = 0U;
  if (n && p[n - 1U] == '=')
    ++padding;
  if (n > 1U && p[n - 2U] == '=')
    ++padding;
  if ((n / 4U) * 3U - padding > 512U * 1024U)
    return 0;
  for (size_t i = 0; i < n; ++i) {
    char c = p[i];
    int ok = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
             (c >= '0' && c <= '9') || c == '+' || c == '/';
    if (!ok) {
      if (c != '=' || i < n - padding)
        return 0;
    }
  }
  return 1;
}
static dcc_status_t append_double(dcc_rest_buffer_t *body, int *first,
                                  const char *key, double value) {
  char text[64];
  int n = snprintf(text, sizeof(text), "%.17g", value);
  if (n < 0 || (size_t)n >= sizeof(text))
    return DCC_ERR_INVALID_ARG;
  return dcc_rest_json_append_raw_member(body, first, key, text);
}

dcc_status_t dcc_task8_build_sound_create(
    const dcc_rest_guild_soundboard_sound_create_t *value, char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  const uint64_t volume = DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_PRESENT_VOLUME,
                 emoji_id =
                     DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_PRESENT_EMOJI_ID,
                 emoji_name =
                     DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_PRESENT_EMOJI_NAME;
  dcc_endpoint_record_view_t view;
  int partial = 0;
  if (dcc_endpoint_record_read(
          value, offsetof(dcc_rest_guild_soundboard_sound_create_t, version),
          offsetof(dcc_rest_guild_soundboard_sound_create_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_soundboard_sound_create_t,
                                 sound),
          DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_VERSION,
          volume | emoji_id | emoji_name, &view) != DCC_OK)
    return DCC_ERR_INVALID_ARG;
  uint64_t nulls = guild_nulls(
      value, offsetof(dcc_rest_guild_soundboard_sound_create_t, nulls),
      view.size, &partial);
  if (partial || (nulls & ~(volume | emoji_id | emoji_name)) ||
      (nulls & ~view.present) ||
      PARTIAL(dcc_rest_guild_soundboard_sound_create_t, view, volume) ||
      PARTIAL(dcc_rest_guild_soundboard_sound_create_t, view, emoji_id) ||
      PARTIAL(dcc_rest_guild_soundboard_sound_create_t, view, emoji_name) ||
      !COVERED(dcc_rest_guild_soundboard_sound_create_t, view, volume,
               volume) ||
      !COVERED(dcc_rest_guild_soundboard_sound_create_t, view, emoji_id,
               emoji_id) ||
      !COVERED(dcc_rest_guild_soundboard_sound_create_t, view, emoji_name,
               emoji_name) ||
      !text_valid(value->name, 2U, 32U) || !base64_sound_valid(value->sound) ||
      ((view.present & volume) && !(nulls & volume) &&
       (!isfinite(value->volume) || value->volume < 0.0 ||
        value->volume > 1.0)) ||
      ((view.present & emoji_id) && !(nulls & emoji_id) &&
       value->emoji_id == 0U) ||
      ((view.present & emoji_name) && !(nulls & emoji_name) &&
       !text_valid(value->emoji_name, 1U, SIZE_MAX)) ||
      ((view.present & emoji_id) && !(nulls & emoji_id) &&
       (view.present & emoji_name) && !(nulls & emoji_name)))
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK)
    status =
        dcc_rest_json_append_string_member(&body, &first, "name", value->name);
  if (status == DCC_OK)
    status = dcc_rest_json_append_string_member(&body, &first, "sound",
                                                value->sound);
  if (status == DCC_OK && (view.present & volume))
    status =
        nulls & volume
            ? dcc_rest_json_append_raw_member(&body, &first, "volume", "null")
            : append_double(&body, &first, "volume", value->volume);
  if (status == DCC_OK && (view.present & emoji_id))
    status = nullable_u64(&body, &first, "emoji_id", value->emoji_id,
                          (nulls & emoji_id) != 0U, 1);
  if (status == DCC_OK && (view.present & emoji_name))
    status = nullable_string(&body, &first, "emoji_name", value->emoji_name,
                             (nulls & emoji_name) != 0U);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return close_object(&body, out);
}

dcc_status_t dcc_task8_build_sound_update(
    const dcc_rest_guild_soundboard_sound_update_t *value, char **out) {
  if (out == NULL)
    return DCC_ERR_INVALID_ARG;
  *out = NULL;
  if (value == NULL)
    return dcc_task8_empty_object(out);
  const uint64_t name = DCC_REST_GUILD_SOUNDBOARD_SOUND_UPDATE_PRESENT_NAME,
                 volume = DCC_REST_GUILD_SOUNDBOARD_SOUND_UPDATE_PRESENT_VOLUME,
                 emoji_id =
                     DCC_REST_GUILD_SOUNDBOARD_SOUND_UPDATE_PRESENT_EMOJI_ID,
                 emoji_name =
                     DCC_REST_GUILD_SOUNDBOARD_SOUND_UPDATE_PRESENT_EMOJI_NAME;
  dcc_endpoint_record_view_t view;
  int partial = 0;
  if (dcc_endpoint_record_read(
          value, offsetof(dcc_rest_guild_soundboard_sound_update_t, version),
          offsetof(dcc_rest_guild_soundboard_sound_update_t, present),
          DCC_ENDPOINT_FIELD_END(dcc_rest_guild_soundboard_sound_update_t,
                                 present),
          DCC_REST_GUILD_SOUNDBOARD_SOUND_UPDATE_VERSION,
          name | volume | emoji_id | emoji_name, &view) != DCC_OK)
    return DCC_ERR_INVALID_ARG;
  uint64_t nulls = guild_nulls(
      value, offsetof(dcc_rest_guild_soundboard_sound_update_t, nulls),
      view.size, &partial);
  if (partial || (nulls & ~(volume | emoji_id | emoji_name)) ||
      (nulls & ~view.present) ||
      PARTIAL(dcc_rest_guild_soundboard_sound_update_t, view, name) ||
      PARTIAL(dcc_rest_guild_soundboard_sound_update_t, view, volume) ||
      PARTIAL(dcc_rest_guild_soundboard_sound_update_t, view, emoji_id) ||
      PARTIAL(dcc_rest_guild_soundboard_sound_update_t, view, emoji_name) ||
      !COVERED(dcc_rest_guild_soundboard_sound_update_t, view, name, name) ||
      !COVERED(dcc_rest_guild_soundboard_sound_update_t, view, volume,
               volume) ||
      !COVERED(dcc_rest_guild_soundboard_sound_update_t, view, emoji_id,
               emoji_id) ||
      !COVERED(dcc_rest_guild_soundboard_sound_update_t, view, emoji_name,
               emoji_name) ||
      ((view.present & name) && !text_valid(value->name, 2U, 32U)) ||
      ((view.present & volume) && !(nulls & volume) &&
       (!isfinite(value->volume) || value->volume < 0.0 ||
        value->volume > 1.0)) ||
      ((view.present & emoji_id) && !(nulls & emoji_id) &&
       value->emoji_id == 0U) ||
      ((view.present & emoji_name) && !(nulls & emoji_name) &&
       !text_valid(value->emoji_name, 1U, SIZE_MAX)) ||
      ((view.present & emoji_id) && !(nulls & emoji_id) &&
       (view.present & emoji_name) && !(nulls & emoji_name)))
    return DCC_ERR_INVALID_ARG;
  dcc_rest_buffer_t body = {0};
  int first = 1;
  dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
  if (status == DCC_OK && (view.present & name))
    status =
        dcc_rest_json_append_string_member(&body, &first, "name", value->name);
  if (status == DCC_OK && (view.present & volume))
    status =
        nulls & volume
            ? dcc_rest_json_append_raw_member(&body, &first, "volume", "null")
            : append_double(&body, &first, "volume", value->volume);
  if (status == DCC_OK && (view.present & emoji_id))
    status = nullable_u64(&body, &first, "emoji_id", value->emoji_id,
                          (nulls & emoji_id) != 0U, 1);
  if (status == DCC_OK && (view.present & emoji_name))
    status = nullable_string(&body, &first, "emoji_name", value->emoji_name,
                             (nulls & emoji_name) != 0U);
  if (status != DCC_OK) {
    dcc_rest_buffer_deinit(&body);
    return status;
  }
  return close_object(&body, out);
}

#undef PARTIAL
#undef COVERED
