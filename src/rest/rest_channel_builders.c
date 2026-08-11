#include "internal/rest/dcc_rest_channel_body_internal.h"
#include "internal/rest/dcc_rest_channel_permission_builders_internal.h"
#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HAS(bits_, bit_) (((bits_) & (bit_)) != 0U)

static int channel_name_valid(const char *name) {
    size_t length = 0U;
    if (!dcc_endpoint_utf8_scalar_count(name, &length)) return 0;
    return length >= 1U && length <= 100U;
}

static int channel_text_valid(const char *text, size_t maximum) {
    size_t length = 0U;
    return dcc_endpoint_utf8_scalar_count(text, &length) && length <= maximum;
}

static dcc_status_t validate_available_tags(
    const dcc_channel_forum_tag_params_t *tags,
    size_t tag_count,
    size_t *out_stride
) {
    if (out_stride == NULL || tag_count > 20U ||
        (tag_count != 0U && tags == NULL)) return DCC_ERR_INVALID_ARG;
    *out_stride = 0U;
    if (tag_count == 0U) return DCC_OK;
    uintptr_t address = (uintptr_t)tags;
    const uint64_t known = DCC_CHANNEL_FORUM_TAG_PRESENT_NAME |
        DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_ID |
        DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_NAME |
        DCC_CHANNEL_FORUM_TAG_PRESENT_MODERATED;
    dcc_endpoint_record_view_t first;
    if (address % _Alignof(dcc_channel_forum_tag_params_t) != 0U ||
        dcc_endpoint_record_read(
            tags, offsetof(dcc_channel_forum_tag_params_t, version),
            offsetof(dcc_channel_forum_tag_params_t, present),
            DCC_ENDPOINT_FIELD_END(dcc_channel_forum_tag_params_t, present),
            DCC_CHANNEL_FORUM_TAG_PARAMS_VERSION, known, &first
        ) != DCC_OK || first.size % _Alignof(dcc_channel_forum_tag_params_t) != 0U ||
        tag_count > SIZE_MAX / first.size ||
        address > UINTPTR_MAX - (tag_count * first.size - 1U)) {
        return DCC_ERR_INVALID_ARG;
    }
    const size_t stride = first.size;
    for (size_t i = 0U; i < tag_count; ++i) {
        const dcc_channel_forum_tag_params_t *tag =
            (const dcc_channel_forum_tag_params_t *)
                ((const unsigned char *)tags + i * stride);
        dcc_endpoint_record_view_t view;
        if (dcc_endpoint_record_read(
                tag, offsetof(dcc_channel_forum_tag_params_t, version),
                offsetof(dcc_channel_forum_tag_params_t, present),
                DCC_ENDPOINT_FIELD_END(dcc_channel_forum_tag_params_t, present),
                DCC_CHANNEL_FORUM_TAG_PARAMS_VERSION, known, &view
            ) != DCC_OK || view.size != stride ||
            dcc_endpoint_field_partially_covered(
                view.size, offsetof(dcc_channel_forum_tag_params_t, name),
                sizeof(tag->name)) ||
            dcc_endpoint_field_partially_covered(
                view.size, offsetof(dcc_channel_forum_tag_params_t, emoji_id),
                sizeof(tag->emoji_id)) ||
            dcc_endpoint_field_partially_covered(
                view.size, offsetof(dcc_channel_forum_tag_params_t, emoji_name),
                sizeof(tag->emoji_name)) ||
            dcc_endpoint_field_partially_covered(
                view.size, offsetof(dcc_channel_forum_tag_params_t, moderated),
                sizeof(tag->moderated)) ||
            !dcc_endpoint_present_field_covered(&view,
                DCC_CHANNEL_FORUM_TAG_PRESENT_NAME,
                offsetof(dcc_channel_forum_tag_params_t, name), sizeof(tag->name)) ||
            !dcc_endpoint_present_field_covered(&view,
                DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_ID,
                offsetof(dcc_channel_forum_tag_params_t, emoji_id), sizeof(tag->emoji_id)) ||
            !dcc_endpoint_present_field_covered(&view,
                DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_NAME,
                offsetof(dcc_channel_forum_tag_params_t, emoji_name), sizeof(tag->emoji_name)) ||
            !dcc_endpoint_present_field_covered(&view,
                DCC_CHANNEL_FORUM_TAG_PRESENT_MODERATED,
                offsetof(dcc_channel_forum_tag_params_t, moderated), sizeof(tag->moderated)) ||
            (view.present & DCC_CHANNEL_FORUM_TAG_PRESENT_NAME) == 0U ||
            !channel_text_valid(tag->name, 20U) ||
            (((view.present & DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_ID) != 0U) &&
                tag->emoji_id == 0U) ||
            (((view.present & DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_NAME) != 0U) &&
                (tag->emoji_name == NULL || tag->emoji_name[0] == '\0' ||
                 !channel_text_valid(tag->emoji_name, SIZE_MAX))) ||
            ((view.present & (DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_ID |
                DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_NAME)) ==
                (DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_ID |
                 DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_NAME)) ||
            (((view.present & DCC_CHANNEL_FORUM_TAG_PRESENT_MODERATED) != 0U) &&
                tag->moderated > 1U)) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    *out_stride = stride;
    return DCC_OK;
}

static dcc_status_t append_default_reaction(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_channel_default_reaction_t *reaction
) {
    const int has_id = reaction->emoji_id != 0U;
    const int has_name = reaction->emoji_name != NULL && reaction->emoji_name[0] != '\0';
    if (has_id == has_name ||
        (has_name && !channel_text_valid(reaction->emoji_name, SIZE_MAX)))
        return DCC_ERR_INVALID_ARG;
    dcc_status_t status = dcc_rest_json_append_member_key(
        body, first, "default_reaction_emoji"
    );
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(body, "{");
    int reaction_first = 1;
    if (status == DCC_OK && has_id) status = dcc_rest_json_append_u64_string_member(
        body, &reaction_first, "emoji_id", reaction->emoji_id
    );
    if (status == DCC_OK && has_name) status = dcc_rest_json_append_string_member(
        body, &reaction_first, "emoji_name", reaction->emoji_name
    );
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(body, "}");
    return status;
}

static dcc_status_t append_available_tags(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_channel_forum_tag_params_t *tags,
    size_t tag_count
) {
    size_t stride = 0U;
    if (validate_available_tags(tags, tag_count, &stride) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    dcc_status_t status = dcc_rest_json_append_member_key(body, first, "available_tags");
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(body, "[");
    for (size_t i = 0U; i < tag_count && status == DCC_OK; ++i) {
        const dcc_channel_forum_tag_params_t *tag =
            (const dcc_channel_forum_tag_params_t *)
                ((const unsigned char *)tags + i * stride);
        const int has_id = (tag->present & DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_ID) != 0U;
        const int has_name = (tag->present & DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_NAME) != 0U;
        if (i != 0U) status = dcc_rest_buffer_append_cstr(body, ",");
        if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(body, "{");
        int tag_first = 1;
        if (status == DCC_OK) status = dcc_rest_json_append_string_member(
            body, &tag_first, "name", tag->name
        );
        if (status == DCC_OK && (tag->present & DCC_CHANNEL_FORUM_TAG_PRESENT_MODERATED) != 0U)
            status = dcc_rest_json_append_bool_member(body, &tag_first, "moderated", tag->moderated);
        if (status == DCC_OK && has_id) status = dcc_rest_json_append_u64_string_member(
            body, &tag_first, "emoji_id", tag->emoji_id
        );
        if (status == DCC_OK && has_name) status = dcc_rest_json_append_string_member(
            body, &tag_first, "emoji_name", tag->emoji_name
        );
        if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(body, "}");
    }
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(body, "]");
    return status;
}

int dcc_rest_channel_archive_duration_valid(
    dcc_channel_auto_archive_duration_t duration
) {
    return duration == DCC_CHANNEL_AUTO_ARCHIVE_1_HOUR ||
        duration == DCC_CHANNEL_AUTO_ARCHIVE_1_DAY ||
        duration == DCC_CHANNEL_AUTO_ARCHIVE_3_DAYS ||
        duration == DCC_CHANNEL_AUTO_ARCHIVE_1_WEEK;
}

static dcc_status_t append_guild(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_channel_guild_params_t *guild,
    uint8_t modify
) {
    const uint64_t known = (UINT64_C(1) << 19U) - 1U;
    const uint64_t known_flags = DCC_CHANNEL_FLAG_REQUIRE_TAG |
        DCC_CHANNEL_FLAG_HIDE_MEDIA_DOWNLOAD_OPTIONS;
    const int forum_or_media = guild->type == DCC_CHANNEL_FORUM ||
        guild->type == DCC_CHANNEL_MEDIA;
    const int voice_or_stage = guild->type == DCC_CHANNEL_VOICE ||
        guild->type == DCC_CHANNEL_STAGE;
    const int valid_type = guild->type == DCC_CHANNEL_TEXT ||
        voice_or_stage || guild->type == DCC_CHANNEL_CATEGORY ||
        guild->type == DCC_CHANNEL_ANNOUNCEMENT || forum_or_media;
    const uint64_t nulls = guild->nulls;
    const size_t topic_maximum = modify && forum_or_media ? 4096U : 1024U;
    if ((guild->present & ~known) != 0U ||
        !valid_type || (nulls & ~(DCC_CHANNEL_GUILD_NULL_PARENT_ID |
            DCC_CHANNEL_GUILD_NULL_DEFAULT_REACTION_EMOJI)) != 0U ||
        (!modify && !HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_NAME)) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_NAME) &&
            !channel_name_valid(guild->name)) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_TOPIC) &&
            (guild->topic != NULL && !channel_text_valid(guild->topic, topic_maximum))) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_BITRATE) &&
            (!voice_or_stage || guild->bitrate < 8000U)) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_USER_LIMIT) &&
            (!voice_or_stage ||
             (guild->type == DCC_CHANNEL_VOICE && guild->user_limit > 99U) ||
             (guild->type == DCC_CHANNEL_STAGE && guild->user_limit > 10000U))) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_RATE_LIMIT_PER_USER) &&
            guild->rate_limit_per_user > 21600U) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_DEFAULT_THREAD_RATE_LIMIT_PER_USER) &&
            (guild->default_thread_rate_limit_per_user > 21600U ||
             (guild->type != DCC_CHANNEL_TEXT &&
              guild->type != DCC_CHANNEL_ANNOUNCEMENT && !forum_or_media))) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_NSFW) &&
            (guild->nsfw > 1U ||
             (guild->type != DCC_CHANNEL_TEXT && !voice_or_stage &&
              guild->type != DCC_CHANNEL_ANNOUNCEMENT &&
              guild->type != DCC_CHANNEL_FORUM &&
              (!modify || guild->type != DCC_CHANNEL_MEDIA)))) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_RTC_REGION) &&
            (!voice_or_stage || (guild->rtc_region != NULL &&
             !channel_text_valid(guild->rtc_region, SIZE_MAX)))) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_VIDEO_QUALITY_MODE) &&
            (!voice_or_stage || (guild->video_quality_mode != 1U &&
             guild->video_quality_mode != 2U))) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_DEFAULT_AUTO_ARCHIVE_DURATION) &&
            (guild->type != DCC_CHANNEL_TEXT &&
             guild->type != DCC_CHANNEL_ANNOUNCEMENT && !forum_or_media)) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_AVAILABLE_TAGS) &&
            !forum_or_media) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_AVAILABLE_TAGS) &&
            (guild->available_tag_count > 20U ||
             (guild->available_tag_count != 0U && guild->available_tags == NULL))) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_DEFAULT_REACTION_EMOJI) &&
            !forum_or_media) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_DEFAULT_SORT_ORDER) &&
            (!forum_or_media || guild->default_sort_order > 1U)) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_DEFAULT_FORUM_LAYOUT) &&
            (guild->type != DCC_CHANNEL_FORUM || guild->default_forum_layout >
                DCC_CHANNEL_FORUM_LAYOUT_GALLERY_VIEW)) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_FLAGS) &&
            ((guild->flags & ~known_flags) != 0U ||
             ((guild->flags & DCC_CHANNEL_FLAG_REQUIRE_TAG) != 0U &&
              !forum_or_media) ||
             ((guild->flags & DCC_CHANNEL_FLAG_HIDE_MEDIA_DOWNLOAD_OPTIONS) != 0U &&
              guild->type != DCC_CHANNEL_MEDIA))) ||
        ((nulls & DCC_CHANNEL_GUILD_NULL_PARENT_ID) != 0U &&
            (!HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_PARENT_ID) ||
             guild->parent_id != 0U)) ||
        (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_PARENT_ID) &&
            guild->parent_id == 0U &&
            (nulls & DCC_CHANNEL_GUILD_NULL_PARENT_ID) == 0U) ||
        ((nulls & DCC_CHANNEL_GUILD_NULL_DEFAULT_REACTION_EMOJI) != 0U &&
            (!HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_DEFAULT_REACTION_EMOJI) ||
             guild->default_reaction_emoji.emoji_id != 0U ||
             guild->default_reaction_emoji.emoji_name != NULL))) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = DCC_OK;
#define APPEND(bit_, expr_) do { if (status == DCC_OK && HAS(guild->present, bit_)) status = (expr_); } while (0)
    APPEND(DCC_CHANNEL_GUILD_PRESENT_NAME,
        dcc_rest_json_append_string_member(body, first, "name", guild->name));
    APPEND(DCC_CHANNEL_GUILD_PRESENT_TYPE,
        dcc_rest_json_append_u64_member(body, first, "type", guild->type));
    APPEND(DCC_CHANNEL_GUILD_PRESENT_TOPIC,
        dcc_rest_json_append_nullable_string_member(body, first, "topic", guild->topic));
    APPEND(DCC_CHANNEL_GUILD_PRESENT_BITRATE,
        dcc_rest_json_append_u64_member(body, first, "bitrate", guild->bitrate));
    APPEND(DCC_CHANNEL_GUILD_PRESENT_USER_LIMIT,
        dcc_rest_json_append_u64_member(body, first, "user_limit", guild->user_limit));
    APPEND(DCC_CHANNEL_GUILD_PRESENT_RATE_LIMIT_PER_USER,
        dcc_rest_json_append_u64_member(body, first, "rate_limit_per_user", guild->rate_limit_per_user));
    if (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_POSITION)) {
        if (guild->position < 0) return DCC_ERR_INVALID_ARG;
        status = dcc_rest_json_append_u64_member(body, first, "position", (uint64_t)guild->position);
    }
    if (status == DCC_OK && HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_PERMISSION_OVERWRITES)) {
        status = dcc_rest_json_append_member_key(body, first, "permission_overwrites");
        if (status == DCC_OK) status = dcc_rest_build_channel_permission_overwrites_array(
            guild->permission_overwrites, guild->permission_overwrite_count, body
        );
    }
    if (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_PARENT_ID)) {
        status = (nulls & DCC_CHANNEL_GUILD_NULL_PARENT_ID) != 0U
            ? dcc_rest_json_append_raw_member(body, first, "parent_id", "null")
            : dcc_rest_json_append_u64_string_member(body, first, "parent_id", guild->parent_id);
    }
    APPEND(DCC_CHANNEL_GUILD_PRESENT_NSFW,
        dcc_rest_json_append_bool_member(body, first, "nsfw", guild->nsfw));
    APPEND(DCC_CHANNEL_GUILD_PRESENT_RTC_REGION,
        dcc_rest_json_append_nullable_string_member(body, first, "rtc_region", guild->rtc_region));
    APPEND(DCC_CHANNEL_GUILD_PRESENT_VIDEO_QUALITY_MODE,
        dcc_rest_json_append_u64_member(body, first, "video_quality_mode", guild->video_quality_mode));
    if (HAS(guild->present, DCC_CHANNEL_GUILD_PRESENT_DEFAULT_AUTO_ARCHIVE_DURATION) &&
        !dcc_rest_channel_archive_duration_valid(guild->default_auto_archive_duration)) {
        return DCC_ERR_INVALID_ARG;
    }
    APPEND(DCC_CHANNEL_GUILD_PRESENT_DEFAULT_AUTO_ARCHIVE_DURATION,
        dcc_rest_json_append_u64_member(body, first, "default_auto_archive_duration", guild->default_auto_archive_duration));
    if (status == DCC_OK && HAS(guild->present,
            DCC_CHANNEL_GUILD_PRESENT_DEFAULT_REACTION_EMOJI)) {
        status = (nulls & DCC_CHANNEL_GUILD_NULL_DEFAULT_REACTION_EMOJI) != 0U
            ? dcc_rest_json_append_raw_member(
                body, first, "default_reaction_emoji", "null"
            )
            : append_default_reaction(body, first, &guild->default_reaction_emoji);
    }
    APPEND(DCC_CHANNEL_GUILD_PRESENT_AVAILABLE_TAGS,
        append_available_tags(body, first, guild->available_tags, guild->available_tag_count));
    APPEND(DCC_CHANNEL_GUILD_PRESENT_DEFAULT_SORT_ORDER,
        dcc_rest_json_append_u64_member(body, first, "default_sort_order", guild->default_sort_order));
    APPEND(DCC_CHANNEL_GUILD_PRESENT_DEFAULT_FORUM_LAYOUT,
        dcc_rest_json_append_u64_member(body, first, "default_forum_layout", guild->default_forum_layout));
    APPEND(DCC_CHANNEL_GUILD_PRESENT_DEFAULT_THREAD_RATE_LIMIT_PER_USER,
        dcc_rest_json_append_u64_member(body, first, "default_thread_rate_limit_per_user", guild->default_thread_rate_limit_per_user));
    APPEND(DCC_CHANNEL_GUILD_PRESENT_FLAGS,
        dcc_rest_json_append_u64_member(body, first, "flags", guild->flags));
#undef APPEND
    return status;
}

static dcc_status_t append_group_dm(
    dcc_rest_buffer_t *body, int *first,
    const dcc_channel_group_dm_params_t *group
) {
    const uint64_t known = DCC_CHANNEL_GROUP_DM_PRESENT_NAME |
        DCC_CHANNEL_GROUP_DM_PRESENT_ICON;
    if ((group->present & ~known) != 0U ||
        (HAS(group->present, DCC_CHANNEL_GROUP_DM_PRESENT_NAME) &&
            !channel_name_valid(group->name)) ||
        (HAS(group->present, DCC_CHANNEL_GROUP_DM_PRESENT_ICON) &&
            (group->icon == NULL || strncmp(group->icon, "data:image/", 11U) != 0 ||
             strstr(group->icon, ";base64,") == NULL ||
             strstr(group->icon, ";base64,")[8] == '\0'))) return DCC_ERR_INVALID_ARG;
    dcc_status_t status = DCC_OK;
    if (HAS(group->present, DCC_CHANNEL_GROUP_DM_PRESENT_NAME))
        status = dcc_rest_json_append_string_member(body, first, "name", group->name);
    if (status == DCC_OK && HAS(group->present, DCC_CHANNEL_GROUP_DM_PRESENT_ICON))
        status = dcc_rest_json_append_nullable_string_member(body, first, "icon", group->icon);
    return status;
}

static dcc_status_t append_thread(
    dcc_rest_buffer_t *body, int *first,
    const dcc_channel_thread_params_t *thread
) {
    const uint64_t known = (UINT64_C(1) << 8U) - 1U;
    if ((thread->present & ~known) != 0U ||
        (HAS(thread->present, DCC_CHANNEL_THREAD_PRESENT_NAME) &&
            !channel_name_valid(thread->name)) ||
        (HAS(thread->present, DCC_CHANNEL_THREAD_PRESENT_RATE_LIMIT_PER_USER) &&
            thread->rate_limit_per_user > 21600U) ||
        (HAS(thread->present, DCC_CHANNEL_THREAD_PRESENT_ARCHIVED) &&
            thread->archived > 1U) ||
        (HAS(thread->present, DCC_CHANNEL_THREAD_PRESENT_LOCKED) &&
            thread->locked > 1U) ||
        (HAS(thread->present, DCC_CHANNEL_THREAD_PRESENT_INVITABLE) &&
            thread->invitable > 1U) ||
        (HAS(thread->present, DCC_CHANNEL_THREAD_PRESENT_FLAGS) &&
            (thread->flags & ~DCC_CHANNEL_FLAG_PINNED) != 0U) ||
        (HAS(thread->present, DCC_CHANNEL_THREAD_PRESENT_APPLIED_TAGS) &&
            (thread->applied_tag_count > 5U ||
             (thread->applied_tag_count != 0U && thread->applied_tags == NULL) ||
             thread->applied_tag_count > SIZE_MAX / sizeof(*thread->applied_tags))))
        return DCC_ERR_INVALID_ARG;
    if (HAS(thread->present, DCC_CHANNEL_THREAD_PRESENT_APPLIED_TAGS)) {
        for (size_t i = 0U; i < thread->applied_tag_count; ++i) {
            if (thread->applied_tags[i] == 0U) return DCC_ERR_INVALID_ARG;
            for (size_t j = 0U; j < i; ++j)
                if (thread->applied_tags[j] == thread->applied_tags[i])
                    return DCC_ERR_INVALID_ARG;
        }
    }
    dcc_status_t status = DCC_OK;
#define APPEND(bit_, expr_) do { if (status == DCC_OK && HAS(thread->present, bit_)) status = (expr_); } while (0)
    APPEND(DCC_CHANNEL_THREAD_PRESENT_NAME,
        dcc_rest_json_append_string_member(body, first, "name", thread->name));
    APPEND(DCC_CHANNEL_THREAD_PRESENT_ARCHIVED,
        dcc_rest_json_append_bool_member(body, first, "archived", thread->archived));
    if (HAS(thread->present, DCC_CHANNEL_THREAD_PRESENT_AUTO_ARCHIVE_DURATION) &&
        !dcc_rest_channel_archive_duration_valid(thread->auto_archive_duration))
        return DCC_ERR_INVALID_ARG;
    APPEND(DCC_CHANNEL_THREAD_PRESENT_AUTO_ARCHIVE_DURATION,
        dcc_rest_json_append_u64_member(body, first, "auto_archive_duration", thread->auto_archive_duration));
    APPEND(DCC_CHANNEL_THREAD_PRESENT_LOCKED,
        dcc_rest_json_append_bool_member(body, first, "locked", thread->locked));
    APPEND(DCC_CHANNEL_THREAD_PRESENT_INVITABLE,
        dcc_rest_json_append_bool_member(body, first, "invitable", thread->invitable));
    APPEND(DCC_CHANNEL_THREAD_PRESENT_RATE_LIMIT_PER_USER,
        dcc_rest_json_append_u64_member(body, first, "rate_limit_per_user", thread->rate_limit_per_user));
    APPEND(DCC_CHANNEL_THREAD_PRESENT_FLAGS,
        dcc_rest_json_append_u64_member(body, first, "flags", thread->flags));
    APPEND(DCC_CHANNEL_THREAD_PRESENT_APPLIED_TAGS,
        dcc_rest_json_append_snowflake_string_array_member(body, first, "applied_tags", thread->applied_tags, thread->applied_tag_count));
#undef APPEND
    return status;
}

static dcc_status_t channel_params_normalize(
    const dcc_channel_params_t *params,
    dcc_channel_params_t *out
) {
    if (out == NULL) return DCC_ERR_INVALID_ARG;
    *out = (dcc_channel_params_t)DCC_CHANNEL_PARAMS_INIT;
    dcc_endpoint_record_view_t view;
    if (dcc_endpoint_record_read(
            params, offsetof(dcc_channel_params_t, version), SIZE_MAX,
            DCC_ENDPOINT_FIELD_END(dcc_channel_params_t, kind),
            DCC_CHANNEL_PARAMS_VERSION, 0U, &view
        ) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    out->kind = params->kind;
    const size_t payload_base = offsetof(dcc_channel_params_t, payload);
#define COPY_FIELD(type_, source_, target_, field_) do { \
        const size_t offset_ = payload_base + offsetof(type_, field_); \
        if (!dcc_endpoint_field_covered(view.size, offset_, sizeof((target_)->field_))) \
            return DCC_ERR_INVALID_ARG; \
        memcpy(&(target_)->field_, (const unsigned char *)params + offset_, \
            sizeof((target_)->field_)); \
    } while (0)
#define COPY_PRESENT(type_, source_, target_, bit_, field_) do { \
    if (HAS((target_)->present, (bit_))) \
        COPY_FIELD(type_, source_, target_, field_); \
    } while (0)
#define PARTIAL_FIELD(type_, field_) \
    dcc_endpoint_field_partially_covered( \
        view.size, payload_base + offsetof(type_, field_), \
        sizeof(((type_ *)0)->field_) \
    )
    switch (params->kind) {
        case DCC_CHANNEL_PARAMS_GUILD: {
            dcc_channel_guild_params_t *target = &out->payload.guild;
            COPY_FIELD(dcc_channel_guild_params_t, &params->payload.guild, target, present);
            const uint64_t known = (UINT64_C(1) << 19U) - 1U;
            if ((target->present & ~known) != 0U ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, name) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, type) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, topic) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, bitrate) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, user_limit) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, rate_limit_per_user) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, position) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, permission_overwrites) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, permission_overwrite_count) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, parent_id) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, nsfw) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, rtc_region) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, video_quality_mode) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, default_auto_archive_duration) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, default_reaction_emoji) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, available_tags) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, available_tag_count) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, default_sort_order) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, default_forum_layout) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, default_thread_rate_limit_per_user) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, flags) ||
                PARTIAL_FIELD(dcc_channel_guild_params_t, nulls))
                return DCC_ERR_INVALID_ARG;
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_NAME, name);
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_TYPE, type);
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_TOPIC, topic);
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_BITRATE, bitrate);
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_USER_LIMIT, user_limit);
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_RATE_LIMIT_PER_USER, rate_limit_per_user);
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_POSITION, position);
            if (HAS(target->present, DCC_CHANNEL_GUILD_PRESENT_PERMISSION_OVERWRITES)) {
                COPY_FIELD(dcc_channel_guild_params_t, &params->payload.guild, target, permission_overwrites);
                COPY_FIELD(dcc_channel_guild_params_t, &params->payload.guild, target, permission_overwrite_count);
            }
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_PARENT_ID, parent_id);
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_NSFW, nsfw);
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_RTC_REGION, rtc_region);
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_VIDEO_QUALITY_MODE, video_quality_mode);
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_DEFAULT_AUTO_ARCHIVE_DURATION, default_auto_archive_duration);
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_DEFAULT_REACTION_EMOJI, default_reaction_emoji);
            if (HAS(target->present, DCC_CHANNEL_GUILD_PRESENT_AVAILABLE_TAGS)) {
                COPY_FIELD(dcc_channel_guild_params_t, &params->payload.guild, target, available_tags);
                COPY_FIELD(dcc_channel_guild_params_t, &params->payload.guild, target, available_tag_count);
            }
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_DEFAULT_SORT_ORDER, default_sort_order);
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_DEFAULT_FORUM_LAYOUT, default_forum_layout);
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_DEFAULT_THREAD_RATE_LIMIT_PER_USER, default_thread_rate_limit_per_user);
            COPY_PRESENT(dcc_channel_guild_params_t, &params->payload.guild, target, DCC_CHANNEL_GUILD_PRESENT_FLAGS, flags);
            const size_t nulls_offset = payload_base + offsetof(dcc_channel_guild_params_t, nulls);
            if (dcc_endpoint_field_covered(view.size, nulls_offset, sizeof(target->nulls)))
                memcpy(&target->nulls, (const unsigned char *)params + nulls_offset, sizeof(target->nulls));
            break;
        }
        case DCC_CHANNEL_PARAMS_GROUP_DM: {
            dcc_channel_group_dm_params_t *target = &out->payload.group_dm;
            COPY_FIELD(dcc_channel_group_dm_params_t, &params->payload.group_dm, target, present);
            if ((target->present & ~(DCC_CHANNEL_GROUP_DM_PRESENT_NAME |
                    DCC_CHANNEL_GROUP_DM_PRESENT_ICON)) != 0U ||
                PARTIAL_FIELD(dcc_channel_group_dm_params_t, name) ||
                PARTIAL_FIELD(dcc_channel_group_dm_params_t, icon))
                return DCC_ERR_INVALID_ARG;
            COPY_PRESENT(dcc_channel_group_dm_params_t, &params->payload.group_dm, target, DCC_CHANNEL_GROUP_DM_PRESENT_NAME, name);
            COPY_PRESENT(dcc_channel_group_dm_params_t, &params->payload.group_dm, target, DCC_CHANNEL_GROUP_DM_PRESENT_ICON, icon);
            break;
        }
        case DCC_CHANNEL_PARAMS_THREAD: {
            dcc_channel_thread_params_t *target = &out->payload.thread;
            COPY_FIELD(dcc_channel_thread_params_t, &params->payload.thread, target, present);
            if ((target->present & ~((UINT64_C(1) << 8U) - 1U)) != 0U ||
                PARTIAL_FIELD(dcc_channel_thread_params_t, name) ||
                PARTIAL_FIELD(dcc_channel_thread_params_t, archived) ||
                PARTIAL_FIELD(dcc_channel_thread_params_t, auto_archive_duration) ||
                PARTIAL_FIELD(dcc_channel_thread_params_t, locked) ||
                PARTIAL_FIELD(dcc_channel_thread_params_t, invitable) ||
                PARTIAL_FIELD(dcc_channel_thread_params_t, rate_limit_per_user) ||
                PARTIAL_FIELD(dcc_channel_thread_params_t, flags) ||
                PARTIAL_FIELD(dcc_channel_thread_params_t, applied_tags) ||
                PARTIAL_FIELD(dcc_channel_thread_params_t, applied_tag_count))
                return DCC_ERR_INVALID_ARG;
            COPY_PRESENT(dcc_channel_thread_params_t, &params->payload.thread, target, DCC_CHANNEL_THREAD_PRESENT_NAME, name);
            COPY_PRESENT(dcc_channel_thread_params_t, &params->payload.thread, target, DCC_CHANNEL_THREAD_PRESENT_ARCHIVED, archived);
            COPY_PRESENT(dcc_channel_thread_params_t, &params->payload.thread, target, DCC_CHANNEL_THREAD_PRESENT_AUTO_ARCHIVE_DURATION, auto_archive_duration);
            COPY_PRESENT(dcc_channel_thread_params_t, &params->payload.thread, target, DCC_CHANNEL_THREAD_PRESENT_LOCKED, locked);
            COPY_PRESENT(dcc_channel_thread_params_t, &params->payload.thread, target, DCC_CHANNEL_THREAD_PRESENT_INVITABLE, invitable);
            COPY_PRESENT(dcc_channel_thread_params_t, &params->payload.thread, target, DCC_CHANNEL_THREAD_PRESENT_RATE_LIMIT_PER_USER, rate_limit_per_user);
            COPY_PRESENT(dcc_channel_thread_params_t, &params->payload.thread, target, DCC_CHANNEL_THREAD_PRESENT_FLAGS, flags);
            if (HAS(target->present, DCC_CHANNEL_THREAD_PRESENT_APPLIED_TAGS)) {
                COPY_FIELD(dcc_channel_thread_params_t, &params->payload.thread, target, applied_tags);
                COPY_FIELD(dcc_channel_thread_params_t, &params->payload.thread, target, applied_tag_count);
            }
            break;
        }
        default:
            return DCC_ERR_INVALID_ARG;
    }
#undef COPY_PRESENT
#undef COPY_FIELD
#undef PARTIAL_FIELD
    return DCC_OK;
}

dcc_status_t dcc_rest_build_channel_body(
    const dcc_channel_params_t *params,
    uint8_t modify,
    char **out
) {
    if (out == NULL) return DCC_ERR_INVALID_ARG;
    *out = NULL;
    dcc_channel_params_t normalized;
    if (channel_params_normalize(params, &normalized) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    params = &normalized;
    if (!modify && params->kind != DCC_CHANNEL_PARAMS_GUILD)
        return DCC_ERR_INVALID_ARG;

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK) {
        switch (params->kind) {
            case DCC_CHANNEL_PARAMS_GUILD:
                status = append_guild(&body, &first, &params->payload.guild, modify);
                break;
            case DCC_CHANNEL_PARAMS_GROUP_DM:
                status = modify ? append_group_dm(&body, &first, &params->payload.group_dm) : DCC_ERR_INVALID_ARG;
                break;
            case DCC_CHANNEL_PARAMS_THREAD:
                status = modify ? append_thread(&body, &first, &params->payload.thread) : DCC_ERR_INVALID_ARG;
                break;
            default:
                status = DCC_ERR_INVALID_ARG;
        }
    }
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&body, "}");
    if (status == DCC_OK) {
        *out = body.data;
        return DCC_OK;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}
