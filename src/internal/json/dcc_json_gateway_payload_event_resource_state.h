#ifndef DCC_JSON_GATEWAY_PAYLOAD_EVENT_RESOURCE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_EVENT_RESOURCE_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_EVENT_RESOURCE_FIELDS \
    int has_stage_instance; \
    dcc_stage_instance_t stage_instance; \
    int has_soundboard_sound; \
    dcc_soundboard_sound_t soundboard_sound; \
    int has_message_reaction; \
    dcc_message_reaction_t message_reaction; \
    int has_poll_vote; \
    dcc_poll_vote_t poll_vote; \
    int has_voice_channel_effect; \
    dcc_voice_channel_effect_t voice_channel_effect; \
    int has_auto_moderation_action; \
    dcc_auto_moderation_action_t auto_moderation_action; \
    int has_auto_moderation_rule; \
    dcc_auto_moderation_rule_t auto_moderation_rule; \
    int has_message_delete_bulk; \
    dcc_message_delete_bulk_t message_delete_bulk

#endif
