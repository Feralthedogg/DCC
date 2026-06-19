#ifndef DCC_OBJECTS_CLONE_RESOURCES_H
#define DCC_OBJECTS_CLONE_RESOURCES_H

#include <dcc/objects/resources.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_guild_t *dcc_guild_clone(const dcc_guild_t *guild);
DCC_API void dcc_guild_free(dcc_guild_t *guild);
DCC_API dcc_channel_t *dcc_channel_clone(const dcc_channel_t *channel);
DCC_API void dcc_channel_free(dcc_channel_t *channel);
DCC_API dcc_role_t *dcc_role_clone(const dcc_role_t *role);
DCC_API void dcc_role_free(dcc_role_t *role);
DCC_API dcc_member_t *dcc_member_clone(const dcc_member_t *member);
DCC_API void dcc_member_free(dcc_member_t *member);
DCC_API dcc_scheduled_event_t *dcc_scheduled_event_clone(const dcc_scheduled_event_t *scheduled_event);
DCC_API void dcc_scheduled_event_free(dcc_scheduled_event_t *scheduled_event);
DCC_API dcc_entitlement_t *dcc_entitlement_clone(const dcc_entitlement_t *entitlement);
DCC_API void dcc_entitlement_free(dcc_entitlement_t *entitlement);
DCC_API dcc_subscription_t *dcc_subscription_clone(const dcc_subscription_t *subscription);
DCC_API void dcc_subscription_free(dcc_subscription_t *subscription);
DCC_API dcc_invite_t *dcc_invite_clone(const dcc_invite_t *invite);
DCC_API void dcc_invite_free(dcc_invite_t *invite);
DCC_API dcc_stage_instance_t *dcc_stage_instance_clone(const dcc_stage_instance_t *stage_instance);
DCC_API void dcc_stage_instance_free(dcc_stage_instance_t *stage_instance);
DCC_API dcc_soundboard_sound_t *dcc_soundboard_sound_clone(const dcc_soundboard_sound_t *soundboard_sound);
DCC_API void dcc_soundboard_sound_free(dcc_soundboard_sound_t *soundboard_sound);
DCC_API dcc_emoji_t *dcc_emoji_clone(const dcc_emoji_t *emoji);
DCC_API void dcc_emoji_free(dcc_emoji_t *emoji);
DCC_API dcc_sticker_t *dcc_sticker_clone(const dcc_sticker_t *sticker);
DCC_API void dcc_sticker_free(dcc_sticker_t *sticker);

#ifdef __cplusplus
}
#endif

#endif
