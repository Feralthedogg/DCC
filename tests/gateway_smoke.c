#include <dcc/dcc.h>

#include "support/gateway_smoke_api_server.h"
#include "support/gateway_smoke_cache.h"
#include "support/gateway_smoke_handlers.h"
#include "support/gateway_smoke_server.h"
#include "support/gateway_smoke_state.h"

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    (void)signal(SIGPIPE, SIG_IGN);

    gateway_server_t server;
    pthread_t gateway_thread;
    if (start_server(&server, &gateway_thread) != 0) {
        fprintf(stderr, "failed to start gateway server\n");
        return 1;
    }

    api_server_t api;
    pthread_t api_thread;
    if (start_api_server(&api, &api_thread, server.port) != 0) {
        fprintf(stderr, "failed to start api server\n");
        close(server.fd);
        return 1;
    }

    char api_base[128];
    snprintf(api_base, sizeof(api_base), "http://127.0.0.1:%u", (unsigned)api.port);
    (void)setenv("DCC_DISCORD_API_BASE", api_base, 1);

    gateway_state_t state;
    memset(&state, 0, sizeof(state));

    dcc_client_t *client = NULL;
    dcc_voice_client_t *voice_client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "mock-token",
        .intents = DCC_INTENT_GUILDS |
            DCC_INTENT_GUILD_MESSAGES |
            DCC_INTENT_MESSAGE_CONTENT |
            DCC_INTENT_GUILD_VOICE_STATES,
        .shard_count = 1,
        .enable_cache = 1,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        close(api.fd);
        close(server.fd);
        return 1;
    }
    st = dcc_voice_client_create(client, &voice_client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_voice_client_create failed: %s\n", dcc_status_string(st));
        dcc_client_destroy(client);
        close(api.fd);
        close(server.fd);
        return 1;
    }
    state.voice_client = voice_client;

    for (int type = 0; type < (int)DCC_EVENT_MAX; type++) {
        if (gateway_smoke_event_is_not_gateway_dispatch((dcc_event_type_t)type)) {
            continue;
        }
        (void)dcc_client_on(client, (dcc_event_type_t)type, on_typed_dispatch, &state, NULL);
    }

    (void)dcc_client_on(client, DCC_EVENT_READY, on_ready, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_RESUMED, on_resumed, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_MESSAGE_CREATE, on_message_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_MESSAGE_UPDATE, on_message_update, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_MESSAGE_DELETE, on_message_delete, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_CREATE, on_guild_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_UPDATE, on_guild_update, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_DELETE, on_guild_delete, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_INTERACTION_CREATE, on_interaction_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_VOICE_STATE_UPDATE, on_voice_state_update, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_CHANNEL_CREATE, on_channel_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_CHANNEL_UPDATE, on_channel_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_CHANNEL_DELETE, on_channel_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_CHANNEL_INFO, on_channel_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_THREAD_CREATE, on_channel_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_THREAD_UPDATE, on_channel_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_THREAD_DELETE, on_channel_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_CHANNEL_TOPIC_UPDATE, on_channel_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_SUBSCRIPTION_CREATE, on_subscription_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_SUBSCRIPTION_UPDATE, on_subscription_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_SUBSCRIPTION_DELETE, on_subscription_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_MESSAGE_REACTION_ADD, on_message_reaction_add, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_MESSAGE_REACTION_REMOVE, on_message_reaction_add, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_MESSAGE_REACTION_REMOVE_ALL, on_message_reaction_add, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_MESSAGE_REACTION_REMOVE_EMOJI, on_message_reaction_add, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_TYPING_START, on_typing_start, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_MESSAGE_DELETE_BULK, on_message_delete_bulk, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_THREAD_MEMBERS_UPDATE, on_thread_members_update, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_MEMBER_ADD, on_guild_member_add, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_MEMBER_REMOVE, on_guild_member_add, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_ROLE_CREATE, on_guild_role_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_USER_UPDATE, on_user_update, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_AUTO_MODERATION_ACTION_EXECUTION, on_automod_action, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_AUTO_MODERATION_RULE_CREATE, on_automod_rule_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_AUTO_MODERATION_RULE_UPDATE, on_automod_rule_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_AUTO_MODERATION_RULE_DELETE, on_automod_rule_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_MESSAGE_POLL_VOTE_ADD, on_poll_vote_add, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_MESSAGE_POLL_VOTE_REMOVE, on_poll_vote_add, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_INVITE_CREATE, on_invite_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_INVITE_DELETE, on_invite_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_VOICE_CHANNEL_EFFECT_SEND, on_voice_channel_effect_send, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_SCHEDULED_EVENT_CREATE, on_scheduled_event_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_SCHEDULED_EVENT_UPDATE, on_scheduled_event_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_SCHEDULED_EVENT_DELETE, on_scheduled_event_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_MEMBER_UPDATE, on_guild_member_update, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_MEMBERS_CHUNK, on_guild_members_chunk, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_CHANNEL_PINS_UPDATE, on_channel_pins_update, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_STAGE_INSTANCE_CREATE, on_stage_instance_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_STAGE_INSTANCE_UPDATE, on_stage_instance_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_STAGE_INSTANCE_DELETE, on_stage_instance_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_SOUNDBOARD_SOUND_CREATE, on_soundboard_sound_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_SOUNDBOARD_SOUND_UPDATE, on_soundboard_sound_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_SOUNDBOARD_SOUND_DELETE, on_soundboard_sound_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_ENTITLEMENT_CREATE, on_entitlement_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_ENTITLEMENT_UPDATE, on_entitlement_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_ENTITLEMENT_DELETE, on_entitlement_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_VOICE_SERVER_UPDATE, on_voice_server_update, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_INTEGRATION_CREATE, on_integration_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_INTEGRATION_UPDATE, on_integration_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_INTEGRATION_DELETE, on_integration_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_PRESENCE_UPDATE, on_presence_update, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_THREAD_MEMBER_UPDATE, on_thread_member_update, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_EMOJIS_UPDATE, on_guild_emojis_update, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_THREAD_LIST_SYNC, on_thread_list_sync, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_SOUNDBOARD_SOUNDS_UPDATE, on_soundboard_sounds_update, &state, NULL);
    (void)dcc_client_on(
        client,
        DCC_EVENT_APPLICATION_COMMAND_PERMISSIONS_UPDATE,
        on_application_command_permissions_update,
        &state,
        NULL
    );
    (void)dcc_client_on(client, DCC_EVENT_GUILD_AUDIT_LOG_ENTRY_CREATE, on_guild_audit_log_entry_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_BAN_ADD, on_guild_ban, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_BAN_REMOVE, on_guild_ban, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_STICKERS_UPDATE, on_guild_stickers_update, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_INTEGRATIONS_UPDATE, on_guild_integrations_update, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_ROLE_UPDATE, on_guild_role_create, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_GUILD_ROLE_DELETE, on_guild_role_create, &state, NULL);
    (void)dcc_client_on(
        client,
        DCC_EVENT_GUILD_SCHEDULED_EVENT_USER_ADD,
        on_scheduled_event_user_update,
        &state,
        NULL
    );
    (void)dcc_client_on(
        client,
        DCC_EVENT_GUILD_SCHEDULED_EVENT_USER_REMOVE,
        on_scheduled_event_user_update,
        &state,
        NULL
    );
    (void)dcc_client_on(client, DCC_EVENT_SOUNDBOARD_SOUNDS, on_soundboard_sounds, &state, NULL);
    (void)dcc_client_on(
        client,
        DCC_EVENT_VOICE_CHANNEL_START_TIME_UPDATE,
        on_voice_channel_start_time_update,
        &state,
        NULL
    );
    (void)dcc_client_on(
        client,
        DCC_EVENT_VOICE_CHANNEL_STATUS_UPDATE,
        on_voice_channel_status_update,
        &state,
        NULL
    );
    (void)dcc_client_on(client, DCC_EVENT_WEBHOOKS_UPDATE, on_webhooks_update, &state, NULL);
    (void)dcc_client_on(
        client,
        DCC_EVENT_GUILD_JOIN_REQUEST_DELETE,
        on_guild_join_request_update,
        &state,
        NULL
    );
    (void)dcc_client_on(
        client,
        DCC_EVENT_GUILD_JOIN_REQUEST_UPDATE,
        on_guild_join_request_update,
        &state,
        NULL
    );
    (void)dcc_client_on(
        client,
        DCC_EVENT_GUILD_APPLICATION_COMMAND_COUNTS_UPDATE,
        on_guild_application_command_update,
        &state,
        NULL
    );
    (void)dcc_client_on(
        client,
        DCC_EVENT_GUILD_APPLICATION_COMMAND_INDEX_UPDATE,
        on_guild_application_command_update,
        &state,
        NULL
    );
    (void)dcc_client_on(client, DCC_EVENT_EMBEDDED_ACTIVITY_UPDATE, on_embedded_activity_update, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_SLASH_COMMAND, on_interaction_subtype, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_BUTTON_CLICK, on_interaction_subtype, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_AUTOCOMPLETE, on_interaction_subtype, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_SELECT_CLICK, on_interaction_subtype, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_MESSAGE_CONTEXT_MENU, on_interaction_subtype, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_USER_CONTEXT_MENU, on_interaction_subtype, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_FORM_SUBMIT, on_interaction_subtype, &state, NULL);
    (void)dcc_client_on_slash_command(client, "ping", on_named_interaction_route, &state.named_slash_command_seen, NULL);
    (void)dcc_client_on_button(client, "button-primary", on_named_interaction_route, &state.named_button_click_seen, NULL);
    (void)dcc_client_on_autocomplete(client, "complete", on_named_interaction_route, &state.named_autocomplete_seen, NULL);
    (void)dcc_client_on_select(client, "select-colour", on_named_interaction_route, &state.named_select_click_seen, NULL);
    (void)dcc_client_on_message_context_menu(
        client,
        "inspect_message",
        on_named_interaction_route,
        &state.named_message_context_menu_seen,
        NULL
    );
    (void)dcc_client_on_user_context_menu(
        client,
        "inspect_user",
        on_named_interaction_route,
        &state.named_user_context_menu_seen,
        NULL
    );
    (void)dcc_client_on_form_submit(
        client,
        "modal-submit",
        on_named_interaction_route,
        &state.named_form_submit_seen,
        NULL
    );
    (void)dcc_client_on_slash_command(
        client,
        "missing_command",
        on_named_interaction_route,
        &state.named_interaction_unexpected_seen,
        NULL
    );
    (void)dcc_client_on_button(
        client,
        "missing-button",
        on_named_interaction_route,
        &state.named_interaction_unexpected_seen,
        NULL
    );
    dcc_listener_id_t disabled_named_id = 0;
    if (dcc_client_on_slash_command(
            client,
            "ping",
            on_named_interaction_route,
            &state.named_interaction_off_seen,
            &disabled_named_id
        ) == DCC_OK) {
        (void)dcc_client_off_interaction(client, DCC_EVENT_SLASH_COMMAND, disabled_named_id);
    }
    (void)dcc_client_on(client, DCC_EVENT_RAW, on_raw_dispatch, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_LOG, on_log, &state, NULL);
    (void)dcc_client_on(client, DCC_EVENT_SOCKET_CLOSE, on_socket_close, &state, NULL);

    st = dcc_client_start(client);
    if (st == DCC_OK) {
        st = dcc_client_wait(client);
    }

    (void)pthread_join(api_thread, NULL);
    (void)pthread_join(gateway_thread, NULL);
    close(api.fd);
    close(server.fd);
    (void)unsetenv("DCC_DISCORD_API_BASE");
    char last_error[256];
    snprintf(last_error, sizeof(last_error), "%s", dcc_client_last_error(client));
    dcc_gateway_info_t gateway_info = {
        .size = sizeof(gateway_info),
    };
    dcc_status_t gateway_info_status = dcc_client_gateway_info(client, &gateway_info);
    uint32_t gateway_recommended_shards = gateway_info.recommended_shards;
    uint32_t gateway_max_concurrency = gateway_info.max_concurrency;
    uint32_t gateway_remaining_identifies = gateway_info.remaining_identifies;
    uint32_t gateway_total_identifies = gateway_info.total_identifies;
    uint64_t gateway_reset_after_ms = gateway_info.reset_after_ms;
    uint64_t gateway_reset_at_ms = gateway_info.reset_at_ms;
    int gateway_info_ok =
        gateway_info_status == DCC_OK &&
        gateway_info.url != NULL &&
        strstr(gateway_info.url, "?v=10&encoding=json") != NULL &&
        gateway_recommended_shards == 3 &&
        gateway_max_concurrency == 2 &&
        gateway_remaining_identifies == 998 &&
        gateway_total_identifies == 1000 &&
        gateway_reset_after_ms == 60000 &&
        gateway_reset_at_ms >= gateway_reset_after_ms;
    int cache_ok = gateway_smoke_cache_ok(client);
    dcc_voice_session_info_t voice_final = {
        .size = sizeof(voice_final),
    };
    dcc_status_t voice_final_status = dcc_voice_client_session_info(voice_client, &voice_final);
    int voice_auto_ok =
        state.voice_auto_ready_seen &&
        state.voice_auto_disconnected_seen &&
        voice_final_status == DCC_OK &&
        voice_final.state == DCC_VOICE_SESSION_DISCONNECTED &&
        !voice_final.ready &&
        voice_final.guild_id == 0 &&
        voice_final.channel_id == 0;
    dcc_voice_client_destroy(voice_client);
    state.voice_client = NULL;
    dcc_client_destroy(client);

    if (st != DCC_OK || !state.ready_seen || !state.resumed_seen ||
        !state.message_create_seen || !state.message_update_seen || !state.message_delete_seen ||
        !state.guild_create_seen || !state.guild_update_seen || !state.guild_delete_seen ||
        !state.interaction_seen || !state.voice_state_seen ||
        !state.channel_create_seen || !state.channel_update_seen || !state.channel_delete_seen ||
        !state.channel_info_seen || !state.thread_create_seen || !state.thread_update_seen ||
        !state.thread_delete_seen || !state.channel_topic_update_seen ||
        !state.subscription_create_seen || !state.raw_subscription_seen ||
        !state.reaction_add_seen || !state.reaction_remove_seen ||
        !state.reaction_remove_all_seen || !state.reaction_remove_emoji_seen || !state.typing_start_seen ||
        !state.bulk_delete_seen || !state.thread_members_seen ||
        !state.guild_member_add_seen || !state.guild_member_remove_seen ||
        !state.guild_role_create_seen || !state.guild_role_update_seen || !state.guild_role_delete_seen ||
        !state.user_update_seen ||
        !state.automod_action_seen || !state.automod_rule_seen ||
        !state.automod_rule_update_seen || !state.automod_rule_delete_seen ||
        !state.poll_vote_seen || !state.poll_vote_remove_seen ||
        !state.invite_create_seen || !state.invite_delete_seen ||
        !state.voice_effect_seen || !state.scheduled_event_seen ||
        !state.scheduled_event_update_seen || !state.scheduled_event_delete_seen ||
        !state.guild_member_update_seen || !state.members_chunk_seen || !state.channel_pins_seen ||
        !state.stage_instance_seen || !state.soundboard_sound_seen || !state.entitlement_create_seen ||
        !state.stage_instance_update_seen || !state.stage_instance_delete_seen ||
        !state.soundboard_sound_update_seen || !state.soundboard_sound_delete_seen ||
        !state.entitlement_update_seen || !state.entitlement_delete_seen ||
        !state.subscription_update_seen || !state.subscription_delete_seen ||
        !state.voice_server_update_seen || !state.integration_create_seen || !state.integration_update_seen ||
        !state.integration_delete_seen || !state.presence_update_seen ||
        !state.application_command_permissions_update_seen ||
        !state.audit_log_entry_seen || !state.guild_ban_add_seen || !state.guild_ban_remove_seen ||
        !state.guild_stickers_update_seen || !state.guild_integrations_update_seen ||
        !state.scheduled_event_user_add_seen || !state.scheduled_event_user_remove_seen ||
        !state.soundboard_sounds_seen ||
        !state.voice_channel_start_time_update_seen || !state.voice_channel_status_update_seen ||
        !state.webhooks_update_seen ||
        !state.guild_join_request_delete_seen || !state.guild_join_request_update_seen ||
        !state.guild_application_command_counts_seen || !state.guild_application_command_index_seen ||
        !state.embedded_activity_update_seen ||
        !state.rate_limited_seen || !state.log_seen ||
        !state.thread_member_update_seen || !state.emojis_update_seen || !state.thread_list_sync_seen ||
        !state.soundboard_sounds_update_seen ||
        !state.slash_command_seen || !state.button_click_seen || !state.autocomplete_seen ||
        !state.select_click_seen || !state.message_context_menu_seen || !state.user_context_menu_seen ||
        !state.form_submit_seen ||
        !state.named_slash_command_seen || !state.named_button_click_seen || !state.named_autocomplete_seen ||
        !state.named_select_click_seen || !state.named_message_context_menu_seen ||
        !state.named_user_context_menu_seen || !state.named_form_submit_seen ||
        state.named_interaction_unexpected_seen || state.named_interaction_off_seen ||
        !state.socket_close_seen ||
        !gateway_state_required_typed_events_seen(&state) ||
        !gateway_state_required_typed_accessors_seen(&state) ||
        !gateway_state_required_raw_events_seen(&state) ||
        !gateway_info_ok ||
        !api.saw_request ||
        !server.saw_identify || !server.saw_heartbeat || !server.saw_resume || !server.saw_voice_state_update ||
        !cache_ok ||
        !state.voice_connect_seen ||
        !voice_auto_ok) {
        fprintf(stderr,
                "gateway smoke failed: status=%s api=%d gateway_info=%d gw_status=%s gw_shards=%u gw_concurrency=%u gw_remaining=%u gw_total=%u gw_reset_after=%llu gw_reset_at=%llu ready=%d resumed=%d create=%d update=%d delete=%d guild_create=%d guild_update=%d guild_delete=%d interaction=%d voice=%d voice_connect=%d voice_op4=%d voice_auto_ready=%d voice_auto_disconnect=%d voice_final=%s channel=%d channel_update=%d channel_delete=%d channel_info=%d thread_create=%d thread_update=%d thread_delete=%d channel_topic=%d subscription=%d raw_subscription=%d reaction=%d reaction_remove=%d reaction_remove_all=%d reaction_remove_emoji=%d typing=%d bulk=%d thread_members=%d guild_member=%d guild_member_remove=%d guild_role=%d guild_role_update=%d guild_role_delete=%d user_update=%d automod=%d automod_rule=%d automod_rule_update=%d automod_rule_delete=%d poll=%d poll_remove=%d invite=%d invite_delete=%d voice_effect=%d scheduled=%d member_update=%d members_chunk=%d pins=%d stage=%d soundboard=%d entitlement=%d voice_server=%d integration=%d integration_update=%d integration_delete=%d presence=%d audit_log=%d ban_add=%d ban_remove=%d sched_user_add=%d sched_user_remove=%d thread_member=%d emojis=%d thread_list=%d soundboard_list=%d slash=%d button=%d autocomplete=%d select=%d message_menu=%d user_menu=%d form=%d named_slash=%d named_button=%d named_autocomplete=%d named_select=%d named_message_menu=%d named_user_menu=%d named_form=%d named_unexpected=%d named_off=%d socket_close=%d typed_low=%llx typed_high=%llx accessor_low=%llx accessor_high=%llx raw_low=%llx raw_high=%llx identify=%d heartbeat=%d resume=%d session=%s\n",
                dcc_status_string(st),
                api.saw_request,
                gateway_info_ok,
                dcc_status_string(gateway_info_status),
                gateway_recommended_shards,
                gateway_max_concurrency,
                gateway_remaining_identifies,
                gateway_total_identifies,
                (unsigned long long)gateway_reset_after_ms,
                (unsigned long long)gateway_reset_at_ms,
                state.ready_seen,
                state.resumed_seen,
                state.message_create_seen,
                state.message_update_seen,
                state.message_delete_seen,
                state.guild_create_seen,
                state.guild_update_seen,
                state.guild_delete_seen,
                state.interaction_seen,
                state.voice_state_seen,
                state.voice_connect_seen,
                server.saw_voice_state_update,
                state.voice_auto_ready_seen,
                state.voice_auto_disconnected_seen,
                dcc_status_string(voice_final_status),
                state.channel_create_seen,
                state.channel_update_seen,
                state.channel_delete_seen,
                state.channel_info_seen,
                state.thread_create_seen,
                state.thread_update_seen,
                state.thread_delete_seen,
                state.channel_topic_update_seen,
                state.subscription_create_seen,
                state.raw_subscription_seen,
                state.reaction_add_seen,
                state.reaction_remove_seen,
                state.reaction_remove_all_seen,
                state.reaction_remove_emoji_seen,
                state.typing_start_seen,
                state.bulk_delete_seen,
                state.thread_members_seen,
                state.guild_member_add_seen,
                state.guild_member_remove_seen,
                state.guild_role_create_seen,
                state.guild_role_update_seen,
                state.guild_role_delete_seen,
                state.user_update_seen,
                state.automod_action_seen,
                state.automod_rule_seen,
                state.automod_rule_update_seen,
                state.automod_rule_delete_seen,
                state.poll_vote_seen,
                state.poll_vote_remove_seen,
                state.invite_create_seen,
                state.invite_delete_seen,
                state.voice_effect_seen,
                state.scheduled_event_seen,
                state.guild_member_update_seen,
                state.members_chunk_seen,
                state.channel_pins_seen,
                state.stage_instance_seen,
                state.soundboard_sound_seen,
                state.entitlement_create_seen,
                state.voice_server_update_seen,
                state.integration_create_seen,
                state.integration_update_seen,
                state.integration_delete_seen,
                state.presence_update_seen,
                state.audit_log_entry_seen,
                state.guild_ban_add_seen,
                state.guild_ban_remove_seen,
                state.scheduled_event_user_add_seen,
                state.scheduled_event_user_remove_seen,
                state.thread_member_update_seen,
                state.emojis_update_seen,
                state.thread_list_sync_seen,
                state.soundboard_sounds_update_seen,
                state.slash_command_seen,
                state.button_click_seen,
                state.autocomplete_seen,
                state.select_click_seen,
                state.message_context_menu_seen,
                state.user_context_menu_seen,
                state.form_submit_seen,
                state.named_slash_command_seen,
                state.named_button_click_seen,
                state.named_autocomplete_seen,
                state.named_select_click_seen,
                state.named_message_context_menu_seen,
                state.named_user_context_menu_seen,
                state.named_form_submit_seen,
                state.named_interaction_unexpected_seen,
                state.named_interaction_off_seen,
                state.socket_close_seen,
                (unsigned long long)state.typed_seen_low,
                (unsigned long long)state.typed_seen_high,
                (unsigned long long)state.typed_accessor_seen_low,
                (unsigned long long)state.typed_accessor_seen_high,
                (unsigned long long)state.raw_seen_low,
                (unsigned long long)state.raw_seen_high,
                server.saw_identify,
                server.saw_heartbeat,
                server.saw_resume,
                state.session_id);
        fprintf(stderr,
                "extra_gateway_flags: app_perms=%d stickers=%d guild_integrations=%d soundboard=%d voice_start=%d voice_status=%d webhooks=%d join_delete=%d join_update=%d app_cmd_counts=%d app_cmd_index=%d embedded=%d rate_limited=%d log=%d cache=%d\n",
                state.application_command_permissions_update_seen,
                state.guild_stickers_update_seen,
                state.guild_integrations_update_seen,
                state.soundboard_sounds_seen,
                state.voice_channel_start_time_update_seen,
                state.voice_channel_status_update_seen,
                state.webhooks_update_seen,
                state.guild_join_request_delete_seen,
                state.guild_join_request_update_seen,
                state.guild_application_command_counts_seen,
                state.guild_application_command_index_seen,
                state.embedded_activity_update_seen,
                state.rate_limited_seen,
                state.log_seen,
                cache_ok);
        fprintf(stderr, "last_error=%s last_log=%s\n", last_error, state.last_log);
        return 1;
    }
    return 0;
}

#endif
