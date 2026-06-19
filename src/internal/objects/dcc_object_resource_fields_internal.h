#ifndef DCC_OBJECT_RESOURCE_FIELDS_INTERNAL_H
#define DCC_OBJECT_RESOURCE_FIELDS_INTERNAL_H

#include <dcc/objects.h>

#ifdef __cplusplus
extern "C" {
#endif

int dcc_clone_entitlement_fields(dcc_entitlement_t *copy, const dcc_entitlement_t *entitlement);
void dcc_free_entitlement_fields(dcc_entitlement_t *entitlement);
int dcc_clone_stage_instance_fields(dcc_stage_instance_t *copy, const dcc_stage_instance_t *stage_instance);
void dcc_free_stage_instance_fields(dcc_stage_instance_t *stage_instance);
int dcc_clone_soundboard_sound_fields(
    dcc_soundboard_sound_t *copy,
    const dcc_soundboard_sound_t *sound
);
void dcc_free_soundboard_sound_fields(dcc_soundboard_sound_t *sound);
int dcc_clone_presence_fields(dcc_presence_t *copy, const dcc_presence_t *presence);
void dcc_free_presence_fields(dcc_presence_t *presence);
int dcc_clone_emoji_fields(dcc_emoji_t *copy, const dcc_emoji_t *emoji);
void dcc_free_emoji_fields(dcc_emoji_t *emoji);
int dcc_clone_sticker_fields(dcc_sticker_t *copy, const dcc_sticker_t *sticker);
void dcc_free_sticker_fields(dcc_sticker_t *sticker);

#ifdef __cplusplus
}
#endif

#endif
