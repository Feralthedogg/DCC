#include "json_smoke_support.h"

int dcc_json_smoke_str_eq(const char *actual, const char *expected) {
    return actual != NULL && strcmp(actual, expected) == 0;
}

int dcc_json_smoke_arena_allocator_ok(void) {
    dcc_json_arena_t arena = {0};
    size_t alignment = _Alignof(max_align_t);
    size_t align_overflow_size =
        alignment > 1U ? (size_t)-1 - (alignment - 2U) : (size_t)-1;
    size_t chunk_overflow_size = ((size_t)-1 - sizeof(dcc_json_arena_chunk_t)) + 1U;
    void *first = NULL;
    void *second = NULL;
    int ok = 0;

    if (dcc_json_arena_alloc(NULL, 1U) != NULL ||
        dcc_json_arena_alloc(&arena, 0U) != NULL ||
        dcc_json_arena_alloc(&arena, (size_t)-1) != NULL ||
        dcc_json_arena_alloc(&arena, align_overflow_size) != NULL ||
        dcc_json_arena_alloc(&arena, chunk_overflow_size) != NULL ||
        dcc_json_arena_copy(&arena, "", (size_t)-1) != NULL ||
        arena.head != NULL) {
        dcc_json_arena_deinit(&arena);
        return 0;
    }

    first = dcc_json_arena_alloc(&arena, 1U);
    second = dcc_json_arena_alloc(&arena, 3U);
    ok = first != NULL &&
         second != NULL &&
         alignment != 0U &&
         ((uintptr_t)first % alignment) == 0U &&
         ((uintptr_t)second % alignment) == 0U;

    dcc_json_arena_deinit(&arena);
    return ok && arena.head == NULL;
}

int dcc_json_smoke_interaction_resolved_ok(const dcc_interaction_t *interaction) {
    const dcc_user_t *user = dcc_interaction_resolved_user(interaction, 446);
    const dcc_member_t *member = dcc_interaction_resolved_member(interaction, 446);
    const dcc_role_t *role = dcc_interaction_resolved_role(interaction, 555);
    const dcc_channel_t *channel = dcc_interaction_resolved_channel(interaction, 222);
    const dcc_message_t *message = dcc_interaction_resolved_message(interaction, 111);
    const dcc_attachment_t *attachment = dcc_interaction_resolved_attachment(interaction, 888);
    const dcc_interaction_resolved_permission_t *permission =
        dcc_interaction_resolved_permission(interaction, 446);

    return interaction != NULL &&
           interaction->resolved.users_count == 1 &&
           interaction->resolved.members_count == 1 &&
           interaction->resolved.roles_count == 1 &&
           interaction->resolved.channels_count == 1 &&
           interaction->resolved.messages_count == 1 &&
           interaction->resolved.attachments_count == 1 &&
           interaction->resolved.member_permissions_count == 1 &&
           interaction->resolved.users_truncated == 0 &&
           interaction->resolved.members_truncated == 0 &&
           interaction->resolved.roles_truncated == 0 &&
           interaction->resolved.channels_truncated == 0 &&
           interaction->resolved.messages_truncated == 0 &&
           interaction->resolved.attachments_truncated == 0 &&
           interaction->resolved.member_permissions_truncated == 0 &&
           user != NULL &&
           user->id == 446 &&
           dcc_json_smoke_str_eq(user->username, "target-user") &&
           dcc_json_smoke_str_eq(user->global_name, "Target") &&
           dcc_json_smoke_str_eq(user->avatar, "target-avatar") &&
           member != NULL &&
           member->guild_id == 333 &&
           member->user.id == 446 &&
           dcc_json_smoke_str_eq(member->user.username, "target-user") &&
           dcc_json_smoke_str_eq(member->user.global_name, "Target") &&
           dcc_json_smoke_str_eq(member->nick, "TargetNick") &&
           member->role_ids != NULL &&
           member->role_ids_count == 1 &&
           member->role_ids[0] == 555 &&
           member->pending == 0 &&
           member->flags == 2 &&
           dcc_json_smoke_str_eq(member->joined_at, "2026-06-15T00:00:00.000000+00:00") &&
           member->premium_since == NULL &&
           member->communication_disabled_until == NULL &&
           role != NULL &&
           role->id == 555 &&
           role->guild_id == 333 &&
           role->permissions == 8 &&
           role->color == 123 &&
           role->position == 4 &&
           role->hoist == 1 &&
           role->managed == 0 &&
           role->mentionable == 1 &&
           dcc_json_smoke_str_eq(role->name, "mods") &&
           channel != NULL &&
           channel->id == 222 &&
           channel->guild_id == 333 &&
           channel->type == 0 &&
           channel->position == 2 &&
           dcc_json_smoke_str_eq(channel->name, "general") &&
           dcc_json_smoke_str_eq(channel->topic, "ops") &&
           message != NULL &&
           message->id == 111 &&
           message->channel_id == 222 &&
           message->guild_id == 333 &&
           message->author.id == 446 &&
           dcc_json_smoke_str_eq(message->author.username, "target-user") &&
           dcc_json_smoke_str_eq(message->author.global_name, "Target") &&
           dcc_json_smoke_str_eq(message->author.avatar, "target-avatar") &&
           dcc_json_smoke_str_eq(message->content, "hello resolved") &&
           dcc_json_smoke_str_eq(message->timestamp, "2026-06-15T00:00:00.000000+00:00") &&
           message->edited_timestamp == NULL &&
           message->flags == 64 &&
           message->type == 0 &&
           message->tts == 0 &&
           message->mention_everyone == 0 &&
           message->pinned == 1 &&
           attachment != NULL &&
           attachment->id == 888 &&
           attachment->size == 4096 &&
           attachment->width == 640 &&
           attachment->height == 480 &&
           attachment->flags == 4 &&
           attachment->ephemeral == 1 &&
           attachment->duration_secs > 1.24 &&
           attachment->duration_secs < 1.26 &&
           dcc_json_smoke_str_eq(attachment->filename, "clip.ogg") &&
           dcc_json_smoke_str_eq(attachment->description, "voice clip") &&
           dcc_json_smoke_str_eq(attachment->url, "https://cdn.discordapp.com/attachments/clip.ogg") &&
           dcc_json_smoke_str_eq(attachment->proxy_url, "https://media.discordapp.net/attachments/clip.ogg") &&
           dcc_json_smoke_str_eq(attachment->content_type, "audio/ogg") &&
           dcc_json_smoke_str_eq(attachment->waveform, "AAEC") &&
           permission != NULL &&
           permission->user_id == 446 &&
           permission->permissions == 2048 &&
           dcc_interaction_resolved_user(interaction, 999) == NULL &&
           dcc_interaction_resolved_message(interaction, 999) == NULL &&
           dcc_interaction_resolved_attachment(interaction, 999) == NULL &&
           dcc_interaction_resolved_permission(interaction, 999) == NULL;
}

int dcc_json_smoke_interaction_metadata_ok(const dcc_interaction_t *interaction) {
    const dcc_entitlement_t *entitlement = dcc_interaction_entitlement(interaction, 700);

    return interaction != NULL &&
           interaction->message_id == 111 &&
           interaction->app_permissions == 8192 &&
           interaction->attachment_size_limit == 26214400U &&
           interaction->version == 1 &&
           interaction->has_context == 1 &&
           interaction->context == 0 &&
           dcc_json_smoke_str_eq(interaction->locale, "ko") &&
           dcc_json_smoke_str_eq(interaction->guild_locale, "en-US") &&
           interaction->authorizing_integration_owners != NULL &&
           interaction->authorizing_integration_owners_count == 2 &&
           interaction->authorizing_integration_owners_truncated == 0 &&
           dcc_interaction_authorizing_integration_owner(interaction, 0) == 333 &&
           dcc_interaction_authorizing_integration_owner(interaction, 1) == 446 &&
           dcc_interaction_authorizing_integration_owner(interaction, 2) == 0 &&
           interaction->entitlements != NULL &&
           interaction->entitlements_count == 1 &&
           interaction->entitlements_truncated == 0 &&
           entitlement != NULL &&
           entitlement->id == 700 &&
           entitlement->sku_id == 123 &&
           entitlement->application_id == 666 &&
           entitlement->user_id == 446 &&
           entitlement->guild_id == 333 &&
           entitlement->type == 8 &&
           entitlement->flags == 2 &&
           dcc_json_smoke_str_eq(entitlement->start_time, "2026-06-15T00:00:00.000000+00:00") &&
           entitlement->end_time == NULL &&
           dcc_interaction_entitlement(interaction, 999) == NULL;
}

int dcc_json_smoke_interaction_resolved_clone_ok(
    const dcc_interaction_t *original,
    const dcc_interaction_t *copy
) {
    const dcc_user_t *original_user = dcc_interaction_resolved_user(original, 446);
    const dcc_user_t *copy_user = dcc_interaction_resolved_user(copy, 446);
    const dcc_member_t *original_member = dcc_interaction_resolved_member(original, 446);
    const dcc_member_t *copy_member = dcc_interaction_resolved_member(copy, 446);
    const dcc_message_t *original_message = dcc_interaction_resolved_message(original, 111);
    const dcc_message_t *copy_message = dcc_interaction_resolved_message(copy, 111);
    const dcc_attachment_t *original_attachment = dcc_interaction_resolved_attachment(original, 888);
    const dcc_attachment_t *copy_attachment = dcc_interaction_resolved_attachment(copy, 888);
    const dcc_interaction_resolved_permission_t *original_permission =
        dcc_interaction_resolved_permission(original, 446);
    const dcc_interaction_resolved_permission_t *copy_permission =
        dcc_interaction_resolved_permission(copy, 446);
    const dcc_entitlement_t *original_entitlement = dcc_interaction_entitlement(original, 700);
    const dcc_entitlement_t *copy_entitlement = dcc_interaction_entitlement(copy, 700);

    return dcc_json_smoke_interaction_resolved_ok(copy) &&
           dcc_json_smoke_interaction_metadata_ok(copy) &&
           original != NULL &&
           copy != NULL &&
           original->locale != copy->locale &&
           original->guild_locale != copy->guild_locale &&
           original->authorizing_integration_owners != copy->authorizing_integration_owners &&
           original->entitlements != copy->entitlements &&
           original->resolved.users != copy->resolved.users &&
           original->resolved.members != copy->resolved.members &&
           original->resolved.roles != copy->resolved.roles &&
           original->resolved.channels != copy->resolved.channels &&
           original->resolved.messages != copy->resolved.messages &&
           original->resolved.attachments != copy->resolved.attachments &&
           original->resolved.member_permissions != copy->resolved.member_permissions &&
           original_user != NULL &&
           copy_user != NULL &&
           original_user->username != copy_user->username &&
           original_member != NULL &&
           copy_member != NULL &&
           original_member->role_ids != copy_member->role_ids &&
           original_message != NULL &&
           copy_message != NULL &&
           original_message->content != copy_message->content &&
           original_message->author.username != copy_message->author.username &&
           original_attachment != NULL &&
           copy_attachment != NULL &&
           original_attachment->filename != copy_attachment->filename &&
           original_attachment->url != copy_attachment->url &&
           original_permission != NULL &&
           copy_permission != NULL &&
           original_permission != copy_permission &&
           original_entitlement != NULL &&
           copy_entitlement != NULL &&
           original_entitlement != copy_entitlement &&
           original_entitlement->start_time != copy_entitlement->start_time;
}
