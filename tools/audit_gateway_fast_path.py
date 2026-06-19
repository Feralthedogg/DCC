#!/usr/bin/env python3
"""Audit invariants for the gateway JSON hot path.

The gateway direct parser is intentionally a static-buffer parser:
- it must not allocate through malloc/calloc/realloc/free,
- it must not run the stage1 scanner by default,
- gateway dispatch events must reference payload->gateway instead of copying it.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
JSON_FAST_PATH_SOURCES = [
    ROOT / "src/json/dcc_json_gateway_payload_fields.c",
    ROOT / "src/json/dcc_json_gateway_payload_parse.c",
    ROOT / "src/json/dcc_json_keys.c",
    ROOT / "src/json/dcc_json_keys_extended.c",
    ROOT / "src/json/dcc_json_keys_extended_tail.c",
    ROOT / "src/json/dcc_json_keys_len3.c",
    ROOT / "src/json/dcc_json_keys_len4.c",
    ROOT / "src/json/dcc_json_keys_len5.c",
    ROOT / "src/json/dcc_json_keys_len6.c",
    ROOT / "src/json/dcc_json_keys_len7.c",
    ROOT / "src/json/dcc_json_keys_len8.c",
    ROOT / "src/json/dcc_json_keys_len9.c",
    ROOT / "src/json/dcc_json_keys_len10.c",
    ROOT / "src/json/dcc_json_keys_len11.c",
    ROOT / "src/json/dcc_json_keys_len12.c",
    ROOT / "src/json/dcc_json_keys_len13.c",
    ROOT / "src/json/dcc_json_keys_len14.c",
    ROOT / "src/json/dcc_json_keys_len15.c",
    ROOT / "src/json/dcc_json_keys_len16.c",
    ROOT / "src/json/dcc_json_keys_len17.c",
    ROOT / "src/json/dcc_json_keys_len18.c",
    ROOT / "src/json/dcc_json_keys_long.c",
    ROOT / "src/json/dcc_json_keys_medium.c",
    ROOT / "src/json/dcc_json_keys_medium_tail.c",
    ROOT / "src/json/dcc_json_keys_short.c",
    ROOT / "src/json/dcc_json_parse_core.c",
    ROOT / "src/json/dcc_json_parse_nullable_strings.c",
    ROOT / "src/json/dcc_json_parse_number_ascii.c",
    ROOT / "src/json/dcc_json_parse_number_direct.c",
    ROOT / "src/json/dcc_json_parse_number_span.c",
    ROOT / "src/json/dcc_json_parse_nested_user_id.c",
    ROOT / "src/json/dcc_json_parse_object_id.c",
    ROOT / "src/json/dcc_json_parse_object_ids.c",
    ROOT / "src/json/dcc_json_parse_skip.c",
    ROOT / "src/json/dcc_json_parse_skip_array.c",
    ROOT / "src/json/dcc_json_parse_skip_object.c",
    ROOT / "src/json/dcc_json_parse_snowflake_arrays.c",
    ROOT / "src/json/dcc_json_parse_tokens.c",
    ROOT / "src/json/dcc_json_parse_utils.c",
    ROOT / "src/json/dcc_json_payload.c",
    ROOT / "src/json/dcc_json_string.c",
    ROOT / "src/json/dcc_json_string_decode.c",
    ROOT / "src/json/dcc_json_string_escape.c",
    ROOT / "src/json/dcc_json_string_parse.c",
    ROOT / "src/json/dcc_json_string_utf8.c",
    ROOT / "src/json/dcc_json_gateway_d.c",
    ROOT / "src/json/dcc_json_gateway_activities.c",
    ROOT / "src/json/dcc_json_gateway_activity_array_finish.c",
    ROOT / "src/json/dcc_json_gateway_activity_array_items.c",
    ROOT / "src/json/dcc_json_gateway_activity_array_loop.c",
    ROOT / "src/json/dcc_json_gateway_activity_arrays.c",
    ROOT / "src/json/dcc_json_gateway_activity_buffers.c",
    ROOT / "src/json/dcc_json_gateway_activity_emoji.c",
    ROOT / "src/json/dcc_json_gateway_activity_emoji_fields.c",
    ROOT / "src/json/dcc_json_gateway_activity_object_dispatch.c",
    ROOT / "src/json/dcc_json_gateway_activity_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_activity_objects.c",
    ROOT / "src/json/dcc_json_gateway_activity_scalar_fields.c",
    ROOT / "src/json/dcc_json_gateway_activity_text_fields.c",
    ROOT / "src/json/dcc_json_gateway_application_permission_fields.c",
    ROOT / "src/json/dcc_json_gateway_application_permission_object.c",
    ROOT / "src/json/dcc_json_gateway_application_permission_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_application_permissions.c",
    ROOT / "src/json/dcc_json_gateway_client_status.c",
    ROOT / "src/json/dcc_json_gateway_client_status_entries.c",
    ROOT / "src/json/dcc_json_gateway_client_status_fields.c",
    ROOT / "src/json/dcc_json_gateway_d_auto_moderation_text.c",
    ROOT / "src/json/dcc_json_gateway_d_channel_invite_text.c",
    ROOT / "src/json/dcc_json_gateway_d_collections.c",
    ROOT / "src/json/dcc_json_gateway_d_channel_numeric_scalar_apply.c",
    ROOT / "src/json/dcc_json_gateway_d_channel_numeric_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_context_text.c",
    ROOT / "src/json/dcc_json_gateway_d_member_context_text.c",
    ROOT / "src/json/dcc_json_gateway_d_core.c",
    ROOT / "src/json/dcc_json_gateway_d_core_application_id.c",
    ROOT / "src/json/dcc_json_gateway_d_core_application_type.c",
    ROOT / "src/json/dcc_json_gateway_d_core_type.c",
    ROOT / "src/json/dcc_json_gateway_d_core_guild_bools.c",
    ROOT / "src/json/dcc_json_gateway_d_core_guild_capacity_limit_keys.c",
    ROOT / "src/json/dcc_json_gateway_d_core_guild_capacity_limits.c",
    ROOT / "src/json/dcc_json_gateway_d_core_guild_limit_values.c",
    ROOT / "src/json/dcc_json_gateway_d_core_guild_limits.c",
    ROOT / "src/json/dcc_json_gateway_d_core_guild_policy_apply.c",
    ROOT / "src/json/dcc_json_gateway_d_core_guild_policy_limits.c",
    ROOT / "src/json/dcc_json_gateway_d_core_guild_user.c",
    ROOT / "src/json/dcc_json_gateway_d_core_channel_id.c",
    ROOT / "src/json/dcc_json_gateway_d_core_guild_id.c",
    ROOT / "src/json/dcc_json_gateway_d_core_guild_id_fields.c",
    ROOT / "src/json/dcc_json_gateway_d_core_guild_id_flags.c",
    ROOT / "src/json/dcc_json_gateway_d_core_id.c",
    ROOT / "src/json/dcc_json_gateway_d_core_identity.c",
    ROOT / "src/json/dcc_json_gateway_d_core_interaction.c",
    ROOT / "src/json/dcc_json_gateway_d_core_interaction_objects.c",
    ROOT / "src/json/dcc_json_gateway_d_core_interaction_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_core_interaction_text.c",
    ROOT / "src/json/dcc_json_gateway_d_core_message.c",
    ROOT / "src/json/dcc_json_gateway_d_core_message_author.c",
    ROOT / "src/json/dcc_json_gateway_d_core_message_bool.c",
    ROOT / "src/json/dcc_json_gateway_d_core_message_flags.c",
    ROOT / "src/json/dcc_json_gateway_d_core_message_scheduled.c",
    ROOT / "src/json/dcc_json_gateway_d_core_message_scheduled_object.c",
    ROOT / "src/json/dcc_json_gateway_d_core_message_scheduled_text.c",
    ROOT / "src/json/dcc_json_gateway_d_core_message_text.c",
    ROOT / "src/json/dcc_json_gateway_d_core_member_count.c",
    ROOT / "src/json/dcc_json_gateway_d_core_name.c",
    ROOT / "src/json/dcc_json_gateway_d_core_session.c",
    ROOT / "src/json/dcc_json_gateway_d_core_user.c",
    ROOT / "src/json/dcc_json_gateway_d_core_webhook_id.c",
    ROOT / "src/json/dcc_json_gateway_d_bool_scalar_fields.c",
    ROOT / "src/json/dcc_json_gateway_d_bool_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_bulk_id_collections.c",
    ROOT / "src/json/dcc_json_gateway_d_channel_id_collections.c",
    ROOT / "src/json/dcc_json_gateway_d_entitlement_ids.c",
    ROOT / "src/json/dcc_json_gateway_d_entitlement_nullable_ids.c",
    ROOT / "src/json/dcc_json_gateway_d_entitlement_values.c",
    ROOT / "src/json/dcc_json_gateway_d_emoji_voice_text.c",
    ROOT / "src/json/dcc_json_gateway_d_event_ids.c",
    ROOT / "src/json/dcc_json_gateway_d_moderation_event_ids.c",
    ROOT / "src/json/dcc_json_gateway_d_resource_event_ids.c",
    ROOT / "src/json/dcc_json_gateway_d_scheduled_event_ids.c",
    ROOT / "src/json/dcc_json_gateway_d_guild_avatar.c",
    ROOT / "src/json/dcc_json_gateway_d_guild_channel_id_apply.c",
    ROOT / "src/json/dcc_json_gateway_d_guild_channel_ids.c",
    ROOT / "src/json/dcc_json_gateway_d_guild_icon_banner.c",
    ROOT / "src/json/dcc_json_gateway_d_guild_media_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_guild_media_text.c",
    ROOT / "src/json/dcc_json_gateway_d_id_collections.c",
    ROOT / "src/json/dcc_json_gateway_d_ids.c",
    ROOT / "src/json/dcc_json_gateway_d_invite_numeric_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_invite_thread_time_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_guild_collections.c",
    ROOT / "src/json/dcc_json_gateway_d_member_array_collections.c",
    ROOT / "src/json/dcc_json_gateway_d_member_collections.c",
    ROOT / "src/json/dcc_json_gateway_d_member_id_collections.c",
    ROOT / "src/json/dcc_json_gateway_d_member_numeric_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_member_time_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_member_voice_time_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_media_collections.c",
    ROOT / "src/json/dcc_json_gateway_d_media_ids.c",
    ROOT / "src/json/dcc_json_gateway_d_message_timestamp.c",
    ROOT / "src/json/dcc_json_gateway_d_message_timestamp_text.c",
    ROOT / "src/json/dcc_json_gateway_d_message_time_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_moderation_numeric_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_numeric_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_object_collections.c",
    ROOT / "src/json/dcc_json_gateway_d_permission_collections.c",
    ROOT / "src/json/dcc_json_gateway_d_primary_message_ids.c",
    ROOT / "src/json/dcc_json_gateway_d_primary_ids.c",
    ROOT / "src/json/dcc_json_gateway_d_primary_resource_ids.c",
    ROOT / "src/json/dcc_json_gateway_d_primary_user_id.c",
    ROOT / "src/json/dcc_json_gateway_d_role_id_collections.c",
    ROOT / "src/json/dcc_json_gateway_d_schedule_entitlement_time_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_scheduled_event_numeric_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_subscription_id_collections.c",
    ROOT / "src/json/dcc_json_gateway_d_subscription_time_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_status_text.c",
    ROOT / "src/json/dcc_json_gateway_d_text_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_time_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_voice_flags.c",
    ROOT / "src/json/dcc_json_gateway_d_voice_presence_double.c",
    ROOT / "src/json/dcc_json_gateway_d_voice_presence_numeric_scalars.c",
    ROOT / "src/json/dcc_json_gateway_d_voice_presence_time.c",
    ROOT / "src/json/dcc_json_gateway_d_voice_presence_u32.c",
    ROOT / "src/json/dcc_json_gateway_d_voice_time_scalars.c",
    ROOT / "src/json/dcc_json_gateway_emoji_apply.c",
    ROOT / "src/json/dcc_json_gateway_emoji.c",
    ROOT / "src/json/dcc_json_gateway_emoji_fields.c",
    ROOT / "src/json/dcc_json_gateway_emoji_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_entitlement_array_finish.c",
    ROOT / "src/json/dcc_json_gateway_entitlement_array_items.c",
    ROOT / "src/json/dcc_json_gateway_entitlement_ids.c",
    ROOT / "src/json/dcc_json_gateway_entitlement_object_dispatch.c",
    ROOT / "src/json/dcc_json_gateway_entitlement_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_entitlement_objects.c",
    ROOT / "src/json/dcc_json_gateway_entitlement_scalars.c",
    ROOT / "src/json/dcc_json_gateway_entitlement_times.c",
    ROOT / "src/json/dcc_json_gateway_entitlements.c",
    ROOT / "src/json/dcc_json_gateway_entity_metadata.c",
    ROOT / "src/json/dcc_json_gateway_entity_metadata_fields.c",
    ROOT / "src/json/dcc_json_gateway_event_guild_id_aliases.c",
    ROOT / "src/json/dcc_json_gateway_event_id_aliases.c",
    ROOT / "src/json/dcc_json_gateway_event_names.c",
    ROOT / "src/json/dcc_json_gateway_events.c",
    ROOT / "src/json/dcc_json_gateway_guild_channel_array_finish.c",
    ROOT / "src/json/dcc_json_gateway_guild_channel_array_items.c",
    ROOT / "src/json/dcc_json_gateway_guilds.c",
    ROOT / "src/json/dcc_json_gateway_interaction_data_fields.c",
    ROOT / "src/json/dcc_json_gateway_interaction_data_id_fields.c",
    ROOT / "src/json/dcc_json_gateway_interaction_data_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_interaction_data_scalar_fields.c",
    ROOT / "src/json/dcc_json_gateway_interaction_data_text_fields.c",
    ROOT / "src/json/dcc_json_gateway_interaction_owner_entries.c",
    ROOT / "src/json/dcc_json_gateway_interaction_owners.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_array_loop.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_arrays.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_array_items.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_array_lists.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_child_fields.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_fields.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_finalize.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_meta_fields.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_number_text.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_objects.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_value_bool.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_value_fields.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_value_null.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_value_number.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_value_parse_bool.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_value_parse_number.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_value_parse_string.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_value_string.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_value_utils.c",
    ROOT / "src/json/dcc_json_gateway_interaction_option_values.c",
    ROOT / "src/json/dcc_json_gateway_interaction_options.c",
    ROOT / "src/json/dcc_json_gateway_interaction_values.c",
    ROOT / "src/json/dcc_json_gateway_interactions.c",
    ROOT / "src/json/dcc_json_gateway_list_emoji_arrays.c",
    ROOT / "src/json/dcc_json_gateway_list_emoji_bools.c",
    ROOT / "src/json/dcc_json_gateway_list_emoji_buffers.c",
    ROOT / "src/json/dcc_json_gateway_list_emoji_ids.c",
    ROOT / "src/json/dcc_json_gateway_list_emoji_array_items.c",
    ROOT / "src/json/dcc_json_gateway_list_emoji_object_dispatch.c",
    ROOT / "src/json/dcc_json_gateway_list_emoji_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_emoji_objects.c",
    ROOT / "src/json/dcc_json_gateway_list_emoji_text_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_emoji_user_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_objects.c",
    ROOT / "src/json/dcc_json_gateway_list_soundboard_array_items.c",
    ROOT / "src/json/dcc_json_gateway_list_soundboard_arrays.c",
    ROOT / "src/json/dcc_json_gateway_list_soundboard.c",
    ROOT / "src/json/dcc_json_gateway_list_soundboard_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_soundboard_id_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_soundboard_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_soundboard_objects.c",
    ROOT / "src/json/dcc_json_gateway_list_soundboard_scalar_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_soundboard_text_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_soundboard_user_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_sticker_array_items.c",
    ROOT / "src/json/dcc_json_gateway_list_sticker_arrays.c",
    ROOT / "src/json/dcc_json_gateway_list_sticker_buffers.c",
    ROOT / "src/json/dcc_json_gateway_list_sticker_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_sticker_id_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_sticker_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_sticker_objects.c",
    ROOT / "src/json/dcc_json_gateway_list_sticker_scalar_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_sticker_text_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_sticker_user_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_stickers.c",
    ROOT / "src/json/dcc_json_gateway_list_thread_array_finish.c",
    ROOT / "src/json/dcc_json_gateway_list_thread_member_arrays.c",
    ROOT / "src/json/dcc_json_gateway_list_thread_member_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_thread_member_ids.c",
    ROOT / "src/json/dcc_json_gateway_list_thread_member_objects.c",
    ROOT / "src/json/dcc_json_gateway_list_thread_member_scalars.c",
    ROOT / "src/json/dcc_json_gateway_list_thread_member_times.c",
    ROOT / "src/json/dcc_json_gateway_list_thread_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_thread_ids.c",
    ROOT / "src/json/dcc_json_gateway_list_thread_members.c",
    ROOT / "src/json/dcc_json_gateway_list_thread_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_thread_objects.c",
    ROOT / "src/json/dcc_json_gateway_list_thread_scalars.c",
    ROOT / "src/json/dcc_json_gateway_list_thread_text_fields.c",
    ROOT / "src/json/dcc_json_gateway_list_threads.c",
    ROOT / "src/json/dcc_json_gateway_member_list_arrays.c",
    ROOT / "src/json/dcc_json_gateway_member_list_buffers.c",
    ROOT / "src/json/dcc_json_gateway_member_list_entries.c",
    ROOT / "src/json/dcc_json_gateway_member_list_fields.c",
    ROOT / "src/json/dcc_json_gateway_member_list_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_member_list_object_parse.c",
    ROOT / "src/json/dcc_json_gateway_member_list_object_prepare.c",
    ROOT / "src/json/dcc_json_gateway_member_list_scalar_fields.c",
    ROOT / "src/json/dcc_json_gateway_member_list_objects.c",
    ROOT / "src/json/dcc_json_gateway_member_list_text_fields.c",
    ROOT / "src/json/dcc_json_gateway_member_list_time_fields.c",
    ROOT / "src/json/dcc_json_gateway_member_list_user_fields.c",
    ROOT / "src/json/dcc_json_gateway_member_lists.c",
    ROOT / "src/json/dcc_json_gateway_member_fields.c",
    ROOT / "src/json/dcc_json_gateway_member_finish.c",
    ROOT / "src/json/dcc_json_gateway_member_scalar_fields.c",
    ROOT / "src/json/dcc_json_gateway_member_text_fields.c",
    ROOT / "src/json/dcc_json_gateway_member_time_fields.c",
    ROOT / "src/json/dcc_json_gateway_member_user_fields.c",
    ROOT / "src/json/dcc_json_gateway_members.c",
    ROOT / "src/json/dcc_json_gateway_normalize.c",
    ROOT / "src/json/dcc_json_gateway_normalize_collections.c",
    ROOT / "src/json/dcc_json_gateway_normalize_identity.c",
    ROOT / "src/json/dcc_json_gateway_normalize_message_reactions.c",
    ROOT / "src/json/dcc_json_gateway_normalize_messages.c",
    ROOT / "src/json/dcc_json_gateway_normalize_moderation.c",
    ROOT / "src/json/dcc_json_gateway_normalize_poll_votes.c",
    ROOT / "src/json/dcc_json_gateway_normalize_resource_commerce.c",
    ROOT / "src/json/dcc_json_gateway_normalize_resource_invite_stage.c",
    ROOT / "src/json/dcc_json_gateway_normalize_resource_scheduled.c",
    ROOT / "src/json/dcc_json_gateway_normalize_resource_soundboard.c",
    ROOT / "src/json/dcc_json_gateway_normalize_resources.c",
    ROOT / "src/json/dcc_json_gateway_normalize_threads.c",
    ROOT / "src/json/dcc_json_gateway_normalize_update_channel.c",
    ROOT / "src/json/dcc_json_gateway_normalize_update_core.c",
    ROOT / "src/json/dcc_json_gateway_normalize_update_guild.c",
    ROOT / "src/json/dcc_json_gateway_normalize_updates.c",
    ROOT / "src/json/dcc_json_gateway_nullable_string_fields.c",
    ROOT / "src/json/dcc_json_gateway_presence.c",
    ROOT / "src/json/dcc_json_gateway_presence_client_status.c",
    ROOT / "src/json/dcc_json_gateway_presence_client_status_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_presence_client_status_fields.c",
    ROOT / "src/json/dcc_json_gateway_presence_list.c",
    ROOT / "src/json/dcc_json_gateway_presence_list_activity_fields.c",
    ROOT / "src/json/dcc_json_gateway_presence_list_arrays.c",
    ROOT / "src/json/dcc_json_gateway_presence_list_buffers.c",
    ROOT / "src/json/dcc_json_gateway_presence_list_entries.c",
    ROOT / "src/json/dcc_json_gateway_presence_list_fields.c",
    ROOT / "src/json/dcc_json_gateway_presence_list_identity_fields.c",
    ROOT / "src/json/dcc_json_gateway_presence_list_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_presence_list_object_parse.c",
    ROOT / "src/json/dcc_json_gateway_presence_list_objects.c",
    ROOT / "src/json/dcc_json_gateway_presence_list_status_fields.c",
    ROOT / "src/json/dcc_json_gateway_resolved.c",
    ROOT / "src/json/dcc_json_gateway_resolved_fields.c",
    ROOT / "src/json/dcc_json_gateway_resolved_attachment_arrays.c",
    ROOT / "src/json/dcc_json_gateway_resolved_attachment_entries.c",
    ROOT / "src/json/dcc_json_gateway_resolved_attachment_ids.c",
    ROOT / "src/json/dcc_json_gateway_resolved_attachment_fields.c",
    ROOT / "src/json/dcc_json_gateway_resolved_attachment_media_fields.c",
    ROOT / "src/json/dcc_json_gateway_resolved_attachment_maps.c",
    ROOT / "src/json/dcc_json_gateway_resolved_attachment_numeric_fields.c",
    ROOT / "src/json/dcc_json_gateway_resolved_attachment_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_resolved_attachment_string_fields.c",
    ROOT / "src/json/dcc_json_gateway_resolved_attachments.c",
    ROOT / "src/json/dcc_json_gateway_resolved_channel_arrays.c",
    ROOT / "src/json/dcc_json_gateway_resolved_channel_entries.c",
    ROOT / "src/json/dcc_json_gateway_resolved_channel_maps.c",
    ROOT / "src/json/dcc_json_gateway_resolved_member_arrays.c",
    ROOT / "src/json/dcc_json_gateway_resolved_member_entries.c",
    ROOT / "src/json/dcc_json_gateway_resolved_member_map_items.c",
    ROOT / "src/json/dcc_json_gateway_resolved_members.c",
    ROOT / "src/json/dcc_json_gateway_resolved_message_arrays.c",
    ROOT / "src/json/dcc_json_gateway_resolved_message_author_fields.c",
    ROOT / "src/json/dcc_json_gateway_resolved_message_entries.c",
    ROOT / "src/json/dcc_json_gateway_resolved_message_fields.c",
    ROOT / "src/json/dcc_json_gateway_resolved_message_id_fields.c",
    ROOT / "src/json/dcc_json_gateway_resolved_message_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_resolved_message_objects.c",
    ROOT / "src/json/dcc_json_gateway_resolved_message_scalar_fields.c",
    ROOT / "src/json/dcc_json_gateway_resolved_message_text_fields.c",
    ROOT / "src/json/dcc_json_gateway_resolved_messages.c",
    ROOT / "src/json/dcc_json_gateway_resolved_object_map_items.c",
    ROOT / "src/json/dcc_json_gateway_resolved_object_maps.c",
    ROOT / "src/json/dcc_json_gateway_resolved_permissions.c",
    ROOT / "src/json/dcc_json_gateway_resolved_role_arrays.c",
    ROOT / "src/json/dcc_json_gateway_resolved_role_entries.c",
    ROOT / "src/json/dcc_json_gateway_resolved_role_maps.c",
    ROOT / "src/json/dcc_json_gateway_resolved_user_arrays.c",
    ROOT / "src/json/dcc_json_gateway_resolved_user_entries.c",
    ROOT / "src/json/dcc_json_gateway_resolved_users.c",
    ROOT / "src/json/dcc_json_gateway_role_array_finish.c",
    ROOT / "src/json/dcc_json_gateway_role_array_items.c",
    ROOT / "src/json/dcc_json_gateway_role_arrays.c",
    ROOT / "src/json/dcc_json_gateway_role_bool_fields.c",
    ROOT / "src/json/dcc_json_gateway_role_fields.c",
    ROOT / "src/json/dcc_json_gateway_role_id_fields.c",
    ROOT / "src/json/dcc_json_gateway_role_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_role_object_parse_fields.c",
    ROOT / "src/json/dcc_json_gateway_role_objects.c",
    ROOT / "src/json/dcc_json_gateway_role_scalar_fields.c",
    ROOT / "src/json/dcc_json_gateway_role_text_fields.c",
    ROOT / "src/json/dcc_json_gateway_roles.c",
    ROOT / "src/json/dcc_json_gateway_scheduled_event_array_finish.c",
    ROOT / "src/json/dcc_json_gateway_scheduled_event_array_items.c",
    ROOT / "src/json/dcc_json_gateway_scheduled_event_arrays.c",
    ROOT / "src/json/dcc_json_gateway_scheduled_event_buffers.c",
    ROOT / "src/json/dcc_json_gateway_scheduled_event_creator_fields.c",
    ROOT / "src/json/dcc_json_gateway_scheduled_event_ids.c",
    ROOT / "src/json/dcc_json_gateway_scheduled_event_identity_text_fields.c",
    ROOT / "src/json/dcc_json_gateway_scheduled_event_numeric_fields.c",
    ROOT / "src/json/dcc_json_gateway_scheduled_event_object_dispatch.c",
    ROOT / "src/json/dcc_json_gateway_scheduled_event_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_scheduled_event_text_fields.c",
    ROOT / "src/json/dcc_json_gateway_scheduled_event_time_text_fields.c",
    ROOT / "src/json/dcc_json_gateway_scheduled_events.c",
    ROOT / "src/json/dcc_json_gateway_stage_instance_array_finish.c",
    ROOT / "src/json/dcc_json_gateway_stage_instance_array_items.c",
    ROOT / "src/json/dcc_json_gateway_stage_instance_buffers.c",
    ROOT / "src/json/dcc_json_gateway_stage_instance_id_fields.c",
    ROOT / "src/json/dcc_json_gateway_stage_instance_object_dispatch.c",
    ROOT / "src/json/dcc_json_gateway_stage_instance_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_stage_instance_objects.c",
    ROOT / "src/json/dcc_json_gateway_stage_instance_scalar_fields.c",
    ROOT / "src/json/dcc_json_gateway_stage_instance_text_fields.c",
    ROOT / "src/json/dcc_json_gateway_stage_instances.c",
    ROOT / "src/json/dcc_json_gateway_user_buffers.c",
    ROOT / "src/json/dcc_json_gateway_user_flag_fields.c",
    ROOT / "src/json/dcc_json_gateway_user_id_fields.c",
    ROOT / "src/json/dcc_json_gateway_user_object_dispatch.c",
    ROOT / "src/json/dcc_json_gateway_user_object_fields.c",
    ROOT / "src/json/dcc_json_gateway_user_objects.c",
    ROOT / "src/json/dcc_json_gateway_user_text_fields.c",
    ROOT / "src/json/dcc_json_gateway_users.c",
]
GATEWAY_CONTRACT_SOURCES = [
    ROOT / "src/gateway/gateway.c",
    ROOT / "src/gateway/gateway_dispatch.c",
    ROOT / "src/gateway/gateway_emit_core.c",
    ROOT / "src/gateway/gateway_session.c",
    ROOT / "src/gateway/gateway_session_receive.c",
]
JSON_H = ROOT / "src/internal/json/dcc_json.h"
PAYLOAD_STATE_H = ROOT / "src/internal/json/dcc_json_gateway_payload_state.h"
PAYLOAD_CONTRACT_SOURCES = [
    PAYLOAD_STATE_H,
    ROOT / "src/internal/json/dcc_json_gateway_payload_limits.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_core_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_resource_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_interaction_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_interaction_core_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_interaction_option_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_interaction_resolved_user_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_interaction_resolved_member_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_interaction_resolved_resource_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_gateway_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_gateway_core_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_gateway_text_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_gateway_time_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_gateway_media_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_gateway_id_collection_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_collection_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_activity_collection_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_guild_collection_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_presence_collection_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_list_collection_state.h",
    ROOT / "src/internal/json/dcc_json_gateway_payload_member_collection_state.h",
]

DISALLOWED_CALLS = {
    "malloc",
    "calloc",
    "realloc",
    "free",
    "dcc_json_arena_alloc",
    "dcc_json_arena_calloc",
    "dcc_json_stage1_scan",
    "dcc_json_stage1_validate",
    "dcc_json_parse",
    "dcc_json_free",
}


def line_of(text: str, offset: int) -> int:
    return text.count("\n", 0, offset) + 1


def matching_brace(text: str, open_index: int) -> int:
    depth = 0
    i = open_index
    in_string = False
    in_char = False
    in_line_comment = False
    in_block_comment = False
    escaped = False
    while i < len(text):
        ch = text[i]
        nxt = text[i + 1] if i + 1 < len(text) else ""

        if in_line_comment:
            if ch == "\n":
                in_line_comment = False
            i += 1
            continue

        if in_block_comment:
            if ch == "*" and nxt == "/":
                in_block_comment = False
                i += 2
                continue
            i += 1
            continue

        if in_string:
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == '"':
                in_string = False
            i += 1
            continue

        if in_char:
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == "'":
                in_char = False
            i += 1
            continue

        if ch == "/" and nxt == "/":
            in_line_comment = True
            i += 2
            continue
        if ch == "/" and nxt == "*":
            in_block_comment = True
            i += 2
            continue
        if ch == '"':
            in_string = True
            i += 1
            continue
        if ch == "'":
            in_char = True
            i += 1
            continue

        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                return i
        i += 1
    raise ValueError(f"unmatched brace at byte {open_index}")


def collect_functions(path: Path) -> dict[str, tuple[str, int, Path]]:
    text = path.read_text()
    functions: dict[str, tuple[str, int, Path]] = {}
    function_start = re.compile(r"(?m)^[A-Za-z_][A-Za-z0-9_ \t\*]*\b(dcc_[A-Za-z0-9_]+)\s*\(")
    pos = 0
    while True:
        match = function_start.search(text, pos)
        if match is None:
            break

        sig_start = match.start()
        search = match.end()
        while search < len(text) and text[search].isspace():
            search += 1

        paren_depth = 1
        while search < len(text) and paren_depth > 0:
            if text[search] == "(":
                paren_depth += 1
            elif text[search] == ")":
                paren_depth -= 1
            search += 1

        while search < len(text) and text[search].isspace():
            search += 1

        if search >= len(text) or text[search] != "{":
            pos = match.end()
            continue

        close = matching_brace(text, search)
        name = match.group(1)
        functions[name] = (text[search + 1 : close], line_of(text, sig_start), path)
        pos = close + 1

    return functions


def collect_fast_path_functions() -> dict[str, tuple[str, int, Path]]:
    functions: dict[str, tuple[str, int, Path]] = {}
    for path in JSON_FAST_PATH_SOURCES:
        if not path.is_file():
            continue
        for name, data in collect_functions(path).items():
            if name in functions:
                previous = functions[name][2]
                raise ValueError(f"duplicate function in gateway fast-path audit: {name} in {previous} and {path}")
            functions[name] = data
    return functions


def reachable_functions(functions: dict[str, tuple[str, int, Path]], start: str) -> set[str]:
    names = set(functions)
    seen: set[str] = set()
    stack = [start]
    call_re = re.compile(r"\b(dcc_[A-Za-z0-9_]+)\s*\(")

    while stack:
        name = stack.pop()
        if name in seen or name not in functions:
            continue
        seen.add(name)
        body, _, _ = functions[name]
        for call in call_re.findall(body):
            if call in names and call not in seen:
                stack.append(call)
    return seen


def audit_json_fast_path() -> list[str]:
    functions = collect_fast_path_functions()
    errors: list[str] = []

    if "dcc_json_parse_gateway_payload" not in functions:
        return ["missing dcc_json_parse_gateway_payload definition"]

    reachable = reachable_functions(functions, "dcc_json_parse_gateway_payload")
    if len(reachable) < 20:
        errors.append(f"gateway parser call graph unexpectedly small: {len(reachable)} functions")

    call_re = re.compile(r"\b([A-Za-z_][A-Za-z0-9_]*)\s*\(")
    for name in sorted(reachable):
        body, start_line, path = functions[name]
        calls = set(call_re.findall(body))
        bad = sorted(calls & DISALLOWED_CALLS)
        if bad:
            errors.append(f"{name} at {path}:{start_line} reaches disallowed call(s): {', '.join(bad)}")

    return errors


def audit_static_storage_contract() -> list[str]:
    errors: list[str] = []
    gateway_text = "\n".join(path.read_text() for path in GATEWAY_CONTRACT_SOURCES if path.is_file())
    payload_state_header = "\n".join(path.read_text() for path in PAYLOAD_CONTRACT_SOURCES if path.is_file())

    required_gateway_snippets = [
        "_Thread_local dcc_json_gateway_payload_t dcc_gateway_payload_scratch",
        "event->gateway = payload != NULL ? &payload->gateway : NULL;",
    ]
    for snippet in required_gateway_snippets:
        if snippet not in gateway_text:
            errors.append(f"missing gateway static-buffer invariant: {snippet}")

    required_header_snippets = [
        "#define DCC_JSON_GATEWAY_ID_LIST_CAP",
        "#define DCC_JSON_GATEWAY_ACTIVITY_CAP",
        "#define DCC_JSON_GATEWAY_OBJECT_LIST_CAP",
        "dcc_snowflake_t gateway_ids[DCC_JSON_GATEWAY_ID_LIST_CAP]",
        "dcc_gateway_activity_t gateway_activities[DCC_JSON_GATEWAY_ACTIVITY_CAP]",
        "dcc_member_t gateway_members[DCC_JSON_GATEWAY_OBJECT_LIST_CAP]",
    ]
    for snippet in required_header_snippets:
        if snippet not in payload_state_header:
            errors.append(f"missing gateway payload fixed-storage invariant: {snippet}")

    return errors


def main() -> int:
    errors = audit_json_fast_path() + audit_static_storage_contract()
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print("gateway fast path audit passed: no reachable heap allocation or stage1 call")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
