#include "gateway_smoke_handlers.h"

#include <stdio.h>
#include <string.h>

void on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_ready_event_t *ready = dcc_event_ready(event);
    if (ready != NULL &&
        strcmp(ready->session_id, "mock-session") == 0 &&
        ready->guild_ids != NULL &&
        ready->guild_ids_count == 2 &&
        ready->guild_count == 2 &&
        ready->guild_ids_truncated == 0 &&
        ready->guild_ids[0] == 333 &&
        ready->guild_ids[1] == 444) {
        state->ready_seen = 1;
        snprintf(state->session_id, sizeof(state->session_id), "%s", ready->session_id);
        if (state->voice_client != NULL &&
            dcc_voice_client_connect(state->voice_client, 333, 222, 0, 0, 0) == DCC_OK) {
            state->voice_connect_seen = 1;
        }
    }
}

void on_resumed(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_ready_event_t *resumed = dcc_event_resumed(event);
    if (resumed != NULL && strcmp(resumed->session_id, "mock-session") == 0) {
        state->resumed_seen = 1;
    }
}

void on_typed_dispatch(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    if (event == NULL) {
        return;
    }

    dcc_event_type_t type = dcc_event_type(event);
    gateway_state_mark_typed_event(state, type);

    int meta_ok = gateway_smoke_typed_metadata_ok(event, type);
    int ok = 0;
    switch (type) {
        case DCC_EVENT_READY:
            ok = dcc_event_ready(event) != NULL &&
                dcc_event_data(event) != NULL &&
                dcc_event_data(event)->user_id == 444 &&
                dcc_event_data(event)->user.id == 444 &&
                dcc_event_data(event)->user.username != NULL &&
                strcmp(dcc_event_data(event)->user.username, "self") == 0 &&
                dcc_event_data(event)->user.bot == 1;
            break;
        case DCC_EVENT_RESUMED:
            ok = dcc_event_resumed(event) != NULL;
            break;
        case DCC_EVENT_MESSAGE_CREATE:
        case DCC_EVENT_MESSAGE_UPDATE:
        case DCC_EVENT_MESSAGE_DELETE:
            ok = dcc_event_message(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_INTERACTION_CREATE:
        case DCC_EVENT_SLASH_COMMAND:
        case DCC_EVENT_BUTTON_CLICK:
        case DCC_EVENT_AUTOCOMPLETE:
        case DCC_EVENT_SELECT_CLICK:
        case DCC_EVENT_MESSAGE_CONTEXT_MENU:
        case DCC_EVENT_USER_CONTEXT_MENU:
        case DCC_EVENT_FORM_SUBMIT:
            ok = dcc_event_interaction(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_GUILD_CREATE:
        case DCC_EVENT_GUILD_UPDATE:
        case DCC_EVENT_GUILD_DELETE:
            ok = dcc_event_guild(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_VOICE_STATE_UPDATE:
            ok = dcc_event_voice_state(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_APPLICATION_COMMAND_PERMISSIONS_UPDATE:
            ok = dcc_event_application_command_permissions_update(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_AUTO_MODERATION_RULE_CREATE:
        case DCC_EVENT_AUTO_MODERATION_RULE_UPDATE:
        case DCC_EVENT_AUTO_MODERATION_RULE_DELETE:
            ok = dcc_event_auto_moderation_rule(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_AUTO_MODERATION_ACTION_EXECUTION:
            ok = dcc_event_auto_moderation_action(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_CHANNEL_CREATE:
        case DCC_EVENT_CHANNEL_UPDATE:
        case DCC_EVENT_CHANNEL_DELETE:
        case DCC_EVENT_CHANNEL_INFO:
        case DCC_EVENT_THREAD_CREATE:
        case DCC_EVENT_THREAD_UPDATE:
        case DCC_EVENT_THREAD_DELETE:
        case DCC_EVENT_CHANNEL_TOPIC_UPDATE:
            ok = dcc_event_channel(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_CHANNEL_PINS_UPDATE:
            ok = dcc_event_channel_pins_update(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_THREAD_LIST_SYNC:
            ok = dcc_event_thread_list_sync(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_THREAD_MEMBER_UPDATE:
            ok = dcc_event_thread_member(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_THREAD_MEMBERS_UPDATE:
            ok = dcc_event_thread_members_update(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_ENTITLEMENT_CREATE:
        case DCC_EVENT_ENTITLEMENT_UPDATE:
        case DCC_EVENT_ENTITLEMENT_DELETE:
            ok = dcc_event_entitlement(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_GUILD_AUDIT_LOG_ENTRY_CREATE:
            ok = dcc_event_guild_audit_log_entry(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_GUILD_BAN_ADD:
        case DCC_EVENT_GUILD_BAN_REMOVE:
            ok = dcc_event_guild_ban(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_GUILD_EMOJIS_UPDATE:
        case DCC_EVENT_GUILD_STICKERS_UPDATE:
        case DCC_EVENT_GUILD_SOUNDBOARD_SOUNDS_UPDATE:
        case DCC_EVENT_SOUNDBOARD_SOUNDS:
            ok = dcc_event_guild_id_list_update(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_GUILD_INTEGRATIONS_UPDATE:
            ok = dcc_event_guild_update_notice(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_GUILD_MEMBER_ADD:
        case DCC_EVENT_GUILD_MEMBER_REMOVE:
        case DCC_EVENT_GUILD_MEMBER_UPDATE:
            ok = dcc_event_member(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_TYPING_START:
            ok = dcc_event_typing_start(event) != NULL &&
                dcc_event_member(event) != NULL &&
                dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_GUILD_MEMBERS_CHUNK:
            ok = dcc_event_guild_members_chunk(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_GUILD_ROLE_CREATE:
        case DCC_EVENT_GUILD_ROLE_UPDATE:
        case DCC_EVENT_GUILD_ROLE_DELETE:
            ok = dcc_event_role(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_GUILD_SCHEDULED_EVENT_CREATE:
        case DCC_EVENT_GUILD_SCHEDULED_EVENT_UPDATE:
        case DCC_EVENT_GUILD_SCHEDULED_EVENT_DELETE:
            ok = dcc_event_scheduled_event(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_GUILD_SCHEDULED_EVENT_USER_ADD:
        case DCC_EVENT_GUILD_SCHEDULED_EVENT_USER_REMOVE:
            ok = dcc_event_scheduled_event_user_update(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_GUILD_SOUNDBOARD_SOUND_CREATE:
        case DCC_EVENT_GUILD_SOUNDBOARD_SOUND_UPDATE:
        case DCC_EVENT_GUILD_SOUNDBOARD_SOUND_DELETE:
            ok = dcc_event_soundboard_sound(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_INTEGRATION_CREATE:
        case DCC_EVENT_INTEGRATION_UPDATE:
        case DCC_EVENT_INTEGRATION_DELETE:
            ok = dcc_event_integration(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_INVITE_CREATE:
        case DCC_EVENT_INVITE_DELETE:
            ok = dcc_event_invite(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_MESSAGE_DELETE_BULK:
            ok = dcc_event_message_delete_bulk(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_MESSAGE_REACTION_ADD:
        case DCC_EVENT_MESSAGE_REACTION_REMOVE:
        case DCC_EVENT_MESSAGE_REACTION_REMOVE_ALL:
        case DCC_EVENT_MESSAGE_REACTION_REMOVE_EMOJI:
            ok = dcc_event_message_reaction(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_MESSAGE_POLL_VOTE_ADD:
        case DCC_EVENT_MESSAGE_POLL_VOTE_REMOVE:
            ok = dcc_event_poll_vote(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_PRESENCE_UPDATE:
            ok = dcc_event_presence(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_STAGE_INSTANCE_CREATE:
        case DCC_EVENT_STAGE_INSTANCE_UPDATE:
        case DCC_EVENT_STAGE_INSTANCE_DELETE:
            ok = dcc_event_stage_instance(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_SUBSCRIPTION_CREATE:
        case DCC_EVENT_SUBSCRIPTION_UPDATE:
        case DCC_EVENT_SUBSCRIPTION_DELETE:
            ok = dcc_event_subscription(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_USER_UPDATE:
            ok = dcc_event_user(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_VOICE_CHANNEL_EFFECT_SEND:
            ok = dcc_event_voice_channel_effect(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_VOICE_CHANNEL_START_TIME_UPDATE:
            ok = dcc_event_voice_channel_start_time_update(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_VOICE_CHANNEL_STATUS_UPDATE:
            ok = dcc_event_voice_channel_status_update(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_VOICE_SERVER_UPDATE:
            ok = dcc_event_voice_server_update(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_WEBHOOKS_UPDATE:
            ok = dcc_event_webhooks_update(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_GUILD_JOIN_REQUEST_DELETE:
        case DCC_EVENT_GUILD_JOIN_REQUEST_UPDATE:
            ok = dcc_event_guild_join_request_update(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_GUILD_APPLICATION_COMMAND_COUNTS_UPDATE:
        case DCC_EVENT_GUILD_APPLICATION_COMMAND_INDEX_UPDATE:
            ok = dcc_event_guild_application_command_update(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_EMBEDDED_ACTIVITY_UPDATE:
            ok = dcc_event_embedded_activity_update(event) != NULL && dcc_event_data(event) != NULL;
            break;
        case DCC_EVENT_RATE_LIMITED: {
            const dcc_rate_limited_event_t *limited = dcc_event_rate_limited(event);
            ok = limited != NULL &&
                limited->retry_after > 1.24 &&
                limited->retry_after < 1.26 &&
                limited->body != NULL &&
                limited->body_len > 0 &&
                dcc_event_data(event) != NULL;
            break;
        }
        case DCC_EVENT_SOCKET_CLOSE:
            ok = dcc_event_socket_close(event) != NULL && dcc_event_data(event) == NULL;
            break;
        case DCC_EVENT_VOICE_BUFFER_SEND:
            ok = dcc_event_voice_buffer_send(event) != NULL && dcc_event_data(event) == NULL;
            break;
        case DCC_EVENT_VOICE_READY:
            ok = dcc_event_voice_ready(event) != NULL && dcc_event_data(event) == NULL;
            break;
        case DCC_EVENT_VOICE_RECEIVE:
        case DCC_EVENT_VOICE_RECEIVE_COMBINED:
            ok = dcc_event_voice_receive(event) != NULL && dcc_event_data(event) == NULL;
            break;
        case DCC_EVENT_VOICE_TRACK_MARKER:
            ok = dcc_event_voice_track_marker(event) != NULL && dcc_event_data(event) == NULL;
            break;
        case DCC_EVENT_VOICE_CLIENT_SPEAKING:
            ok = dcc_event_voice_client_speaking(event) != NULL && dcc_event_data(event) == NULL;
            break;
        case DCC_EVENT_VOICE_CLIENT_DISCONNECT:
            ok = dcc_event_voice_client_disconnect(event) != NULL && dcc_event_data(event) == NULL;
            break;
        case DCC_EVENT_VOICE_CLIENT_PLATFORM:
            ok = dcc_event_voice_client_platform(event) != NULL && dcc_event_data(event) == NULL;
            break;
        case DCC_EVENT_LOG:
        case DCC_EVENT_RAW:
        case DCC_EVENT_MAX:
            break;
    }
    if (ok && meta_ok) {
        gateway_state_mark_typed_accessor_event(state, type);
        if (type == DCC_EVENT_RATE_LIMITED) {
            state->rate_limited_seen = 1;
        }
    }
}
