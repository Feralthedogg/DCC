#ifndef DCC_OBJECT_CORE_FIELDS_INTERNAL_H
#define DCC_OBJECT_CORE_FIELDS_INTERNAL_H

#include <dcc/objects.h>

#ifdef __cplusplus
extern "C" {
#endif

int dcc_clone_user_fields(dcc_user_t *copy, const dcc_user_t *user);
void dcc_free_user_fields(dcc_user_t *user);
int dcc_clone_attachment_fields(dcc_attachment_t *copy, const dcc_attachment_t *attachment);
void dcc_free_attachment_fields(dcc_attachment_t *attachment);
int dcc_clone_message_fields(dcc_message_t *copy, const dcc_message_t *message);
void dcc_free_message_fields(dcc_message_t *message);
int dcc_clone_channel_fields(dcc_channel_t *copy, const dcc_channel_t *channel);
void dcc_free_channel_fields(dcc_channel_t *channel);
int dcc_clone_role_fields(dcc_role_t *copy, const dcc_role_t *role);
void dcc_free_role_fields(dcc_role_t *role);
int dcc_clone_scheduled_event_fields(
    dcc_scheduled_event_t *copy,
    const dcc_scheduled_event_t *scheduled_event
);
void dcc_free_scheduled_event_fields(dcc_scheduled_event_t *scheduled_event);
int dcc_clone_member_fields(dcc_member_t *copy, const dcc_member_t *member);
void dcc_free_member_fields(dcc_member_t *member);
int dcc_clone_guild_string_fields(dcc_guild_t *copy, const dcc_guild_t *guild);
void dcc_free_guild_string_fields(dcc_guild_t *guild);
int dcc_clone_guild_collection_fields(dcc_guild_t *copy, const dcc_guild_t *guild);
int dcc_clone_guild_core_collection_fields(dcc_guild_t *copy, const dcc_guild_t *guild);
int dcc_clone_guild_resource_collection_fields(dcc_guild_t *copy, const dcc_guild_t *guild);
void dcc_free_guild_collection_fields(dcc_guild_t *guild);
int dcc_clone_thread_member_fields(dcc_thread_member_t *copy, const dcc_thread_member_t *thread_member);
void dcc_free_thread_member_fields(dcc_thread_member_t *thread_member);

#ifdef __cplusplus
}
#endif

#endif
