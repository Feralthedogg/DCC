#include <dcc/dcc.h>

#include <stdio.h>
#include <string.h>

static const char *const dpp_router_event_names[] = {
    "VOICE_CHANNEL_EFFECT_SEND",
    "VOICE_STATE_UPDATE",
    "VOICE_CLIENT_PLATFORM",
    "VOICE_CLIENT_DISCONNECT",
    "VOICE_CLIENT_SPEAKING",
    "__LOG__",
    "SOCKET_CLOSE",
    "GUILD_JOIN_REQUEST_DELETE",
    "GUILD_JOIN_REQUEST_UPDATE",
    "INTERACTION_CREATE",
    "SLASH_COMMAND",
    "BUTTON_CLICK",
    "AUTOCOMPLETE",
    "SELECT_CLICK",
    "MESSAGE_CONTEXT_MENU",
    "USER_CONTEXT_MENU",
    "FORM_SUBMIT",
    "GUILD_DELETE",
    "CHANNEL_DELETE",
    "CHANNEL_UPDATE",
    "READY",
    "MESSAGE_DELETE",
    "GUILD_MEMBER_REMOVE",
    "RESUMED",
    "GUILD_ROLE_CREATE",
    "TYPING_START",
    "MESSAGE_REACTION_ADD",
    "GUILD_MEMBERS_CHUNK",
    "MESSAGE_REACTION_REMOVE",
    "GUILD_CREATE",
    "CHANNEL_CREATE",
    "MESSAGE_REACTION_REMOVE_EMOJI",
    "MESSAGE_DELETE_BULK",
    "GUILD_ROLE_UPDATE",
    "GUILD_ROLE_DELETE",
    "CHANNEL_PINS_UPDATE",
    "MESSAGE_REACTION_REMOVE_ALL",
    "VOICE_SERVER_UPDATE",
    "GUILD_EMOJIS_UPDATE",
    "GUILD_STICKERS_UPDATE",
    "PRESENCE_UPDATE",
    "WEBHOOKS_UPDATE",
    "APPLICATION_COMMAND_PERMISSIONS_UPDATE",
    "AUTO_MODERATION_RULE_CREATE",
    "AUTO_MODERATION_RULE_UPDATE",
    "AUTO_MODERATION_RULE_DELETE",
    "AUTO_MODERATION_ACTION_EXECUTION",
    "GUILD_MEMBER_ADD",
    "INVITE_DELETE",
    "GUILD_UPDATE",
    "GUILD_INTEGRATIONS_UPDATE",
    "GUILD_MEMBER_UPDATE",
    "INVITE_CREATE",
    "MESSAGE_UPDATE",
    "USER_UPDATE",
    "MESSAGE_CREATE",
    "MESSAGE_POLL_VOTE_ADD",
    "MESSAGE_POLL_VOTE_REMOVE",
    "GUILD_APPLICATION_COMMAND_COUNTS_UPDATE",
    "GUILD_APPLICATION_COMMAND_INDEX_UPDATE",
    "GUILD_AUDIT_LOG_ENTRY_CREATE",
    "GUILD_BAN_ADD",
    "GUILD_BAN_REMOVE",
    "INTEGRATION_CREATE",
    "INTEGRATION_UPDATE",
    "INTEGRATION_DELETE",
    "THREAD_CREATE",
    "THREAD_UPDATE",
    "THREAD_DELETE",
    "THREAD_LIST_SYNC",
    "THREAD_MEMBER_UPDATE",
    "THREAD_MEMBERS_UPDATE",
    "CHANNEL_TOPIC_UPDATE",
    "EMBEDDED_ACTIVITY_UPDATE",
    "GUILD_SOUNDBOARD_SOUND_CREATE",
    "GUILD_SOUNDBOARD_SOUND_UPDATE",
    "GUILD_SOUNDBOARD_SOUND_DELETE",
    "GUILD_SOUNDBOARD_SOUNDS_UPDATE",
    "VOICE_CHANNEL_START_TIME_UPDATE",
    "VOICE_CHANNEL_STATUS_UPDATE",
    "GUILD_SCHEDULED_EVENT_CREATE",
    "GUILD_SCHEDULED_EVENT_UPDATE",
    "GUILD_SCHEDULED_EVENT_DELETE",
    "GUILD_SCHEDULED_EVENT_USER_ADD",
    "GUILD_SCHEDULED_EVENT_USER_REMOVE",
    "VOICE_BUFFER_SEND",
    "VOICE_READY",
    "VOICE_RECEIVE",
    "VOICE_RECEIVE_COMBINED",
    "VOICE_TRACK_MARKER",
    "STAGE_INSTANCE_CREATE",
    "STAGE_INSTANCE_UPDATE",
    "STAGE_INSTANCE_DELETE",
    "ENTITLEMENT_CREATE",
    "ENTITLEMENT_UPDATE",
    "ENTITLEMENT_DELETE",
};

static const char *const event_surface_expected_names[DCC_EVENT_MAX] = {
    [DCC_EVENT_READY] = "READY",
    [DCC_EVENT_RESUMED] = "RESUMED",
    [DCC_EVENT_MESSAGE_CREATE] = "MESSAGE_CREATE",
    [DCC_EVENT_MESSAGE_UPDATE] = "MESSAGE_UPDATE",
    [DCC_EVENT_MESSAGE_DELETE] = "MESSAGE_DELETE",
    [DCC_EVENT_INTERACTION_CREATE] = "INTERACTION_CREATE",
    [DCC_EVENT_SLASH_COMMAND] = "SLASH_COMMAND",
    [DCC_EVENT_BUTTON_CLICK] = "BUTTON_CLICK",
    [DCC_EVENT_AUTOCOMPLETE] = "AUTOCOMPLETE",
    [DCC_EVENT_SELECT_CLICK] = "SELECT_CLICK",
    [DCC_EVENT_MESSAGE_CONTEXT_MENU] = "MESSAGE_CONTEXT_MENU",
    [DCC_EVENT_USER_CONTEXT_MENU] = "USER_CONTEXT_MENU",
    [DCC_EVENT_FORM_SUBMIT] = "FORM_SUBMIT",
    [DCC_EVENT_GUILD_CREATE] = "GUILD_CREATE",
    [DCC_EVENT_GUILD_DELETE] = "GUILD_DELETE",
    [DCC_EVENT_VOICE_STATE_UPDATE] = "VOICE_STATE_UPDATE",
    [DCC_EVENT_LOG] = "__LOG__",
    [DCC_EVENT_RAW] = "__RAW__",
    [DCC_EVENT_SOCKET_CLOSE] = "SOCKET_CLOSE",
    [DCC_EVENT_RATE_LIMITED] = "RATE_LIMITED",
    [DCC_EVENT_APPLICATION_COMMAND_PERMISSIONS_UPDATE] = "APPLICATION_COMMAND_PERMISSIONS_UPDATE",
    [DCC_EVENT_AUTO_MODERATION_RULE_CREATE] = "AUTO_MODERATION_RULE_CREATE",
    [DCC_EVENT_AUTO_MODERATION_RULE_UPDATE] = "AUTO_MODERATION_RULE_UPDATE",
    [DCC_EVENT_AUTO_MODERATION_RULE_DELETE] = "AUTO_MODERATION_RULE_DELETE",
    [DCC_EVENT_AUTO_MODERATION_ACTION_EXECUTION] = "AUTO_MODERATION_ACTION_EXECUTION",
    [DCC_EVENT_CHANNEL_CREATE] = "CHANNEL_CREATE",
    [DCC_EVENT_CHANNEL_UPDATE] = "CHANNEL_UPDATE",
    [DCC_EVENT_CHANNEL_DELETE] = "CHANNEL_DELETE",
    [DCC_EVENT_CHANNEL_INFO] = "CHANNEL_INFO",
    [DCC_EVENT_CHANNEL_PINS_UPDATE] = "CHANNEL_PINS_UPDATE",
    [DCC_EVENT_THREAD_CREATE] = "THREAD_CREATE",
    [DCC_EVENT_THREAD_UPDATE] = "THREAD_UPDATE",
    [DCC_EVENT_THREAD_DELETE] = "THREAD_DELETE",
    [DCC_EVENT_THREAD_LIST_SYNC] = "THREAD_LIST_SYNC",
    [DCC_EVENT_THREAD_MEMBER_UPDATE] = "THREAD_MEMBER_UPDATE",
    [DCC_EVENT_THREAD_MEMBERS_UPDATE] = "THREAD_MEMBERS_UPDATE",
    [DCC_EVENT_ENTITLEMENT_CREATE] = "ENTITLEMENT_CREATE",
    [DCC_EVENT_ENTITLEMENT_UPDATE] = "ENTITLEMENT_UPDATE",
    [DCC_EVENT_ENTITLEMENT_DELETE] = "ENTITLEMENT_DELETE",
    [DCC_EVENT_GUILD_UPDATE] = "GUILD_UPDATE",
    [DCC_EVENT_GUILD_AUDIT_LOG_ENTRY_CREATE] = "GUILD_AUDIT_LOG_ENTRY_CREATE",
    [DCC_EVENT_GUILD_BAN_ADD] = "GUILD_BAN_ADD",
    [DCC_EVENT_GUILD_BAN_REMOVE] = "GUILD_BAN_REMOVE",
    [DCC_EVENT_GUILD_EMOJIS_UPDATE] = "GUILD_EMOJIS_UPDATE",
    [DCC_EVENT_GUILD_STICKERS_UPDATE] = "GUILD_STICKERS_UPDATE",
    [DCC_EVENT_GUILD_INTEGRATIONS_UPDATE] = "GUILD_INTEGRATIONS_UPDATE",
    [DCC_EVENT_GUILD_MEMBER_ADD] = "GUILD_MEMBER_ADD",
    [DCC_EVENT_GUILD_MEMBER_REMOVE] = "GUILD_MEMBER_REMOVE",
    [DCC_EVENT_GUILD_MEMBER_UPDATE] = "GUILD_MEMBER_UPDATE",
    [DCC_EVENT_GUILD_MEMBERS_CHUNK] = "GUILD_MEMBERS_CHUNK",
    [DCC_EVENT_GUILD_ROLE_CREATE] = "GUILD_ROLE_CREATE",
    [DCC_EVENT_GUILD_ROLE_UPDATE] = "GUILD_ROLE_UPDATE",
    [DCC_EVENT_GUILD_ROLE_DELETE] = "GUILD_ROLE_DELETE",
    [DCC_EVENT_GUILD_SCHEDULED_EVENT_CREATE] = "GUILD_SCHEDULED_EVENT_CREATE",
    [DCC_EVENT_GUILD_SCHEDULED_EVENT_UPDATE] = "GUILD_SCHEDULED_EVENT_UPDATE",
    [DCC_EVENT_GUILD_SCHEDULED_EVENT_DELETE] = "GUILD_SCHEDULED_EVENT_DELETE",
    [DCC_EVENT_GUILD_SCHEDULED_EVENT_USER_ADD] = "GUILD_SCHEDULED_EVENT_USER_ADD",
    [DCC_EVENT_GUILD_SCHEDULED_EVENT_USER_REMOVE] = "GUILD_SCHEDULED_EVENT_USER_REMOVE",
    [DCC_EVENT_GUILD_SOUNDBOARD_SOUND_CREATE] = "GUILD_SOUNDBOARD_SOUND_CREATE",
    [DCC_EVENT_GUILD_SOUNDBOARD_SOUND_UPDATE] = "GUILD_SOUNDBOARD_SOUND_UPDATE",
    [DCC_EVENT_GUILD_SOUNDBOARD_SOUND_DELETE] = "GUILD_SOUNDBOARD_SOUND_DELETE",
    [DCC_EVENT_GUILD_SOUNDBOARD_SOUNDS_UPDATE] = "GUILD_SOUNDBOARD_SOUNDS_UPDATE",
    [DCC_EVENT_SOUNDBOARD_SOUNDS] = "SOUNDBOARD_SOUNDS",
    [DCC_EVENT_INTEGRATION_CREATE] = "INTEGRATION_CREATE",
    [DCC_EVENT_INTEGRATION_UPDATE] = "INTEGRATION_UPDATE",
    [DCC_EVENT_INTEGRATION_DELETE] = "INTEGRATION_DELETE",
    [DCC_EVENT_INVITE_CREATE] = "INVITE_CREATE",
    [DCC_EVENT_INVITE_DELETE] = "INVITE_DELETE",
    [DCC_EVENT_MESSAGE_DELETE_BULK] = "MESSAGE_DELETE_BULK",
    [DCC_EVENT_MESSAGE_REACTION_ADD] = "MESSAGE_REACTION_ADD",
    [DCC_EVENT_MESSAGE_REACTION_REMOVE] = "MESSAGE_REACTION_REMOVE",
    [DCC_EVENT_MESSAGE_REACTION_REMOVE_ALL] = "MESSAGE_REACTION_REMOVE_ALL",
    [DCC_EVENT_MESSAGE_REACTION_REMOVE_EMOJI] = "MESSAGE_REACTION_REMOVE_EMOJI",
    [DCC_EVENT_MESSAGE_POLL_VOTE_ADD] = "MESSAGE_POLL_VOTE_ADD",
    [DCC_EVENT_MESSAGE_POLL_VOTE_REMOVE] = "MESSAGE_POLL_VOTE_REMOVE",
    [DCC_EVENT_PRESENCE_UPDATE] = "PRESENCE_UPDATE",
    [DCC_EVENT_STAGE_INSTANCE_CREATE] = "STAGE_INSTANCE_CREATE",
    [DCC_EVENT_STAGE_INSTANCE_UPDATE] = "STAGE_INSTANCE_UPDATE",
    [DCC_EVENT_STAGE_INSTANCE_DELETE] = "STAGE_INSTANCE_DELETE",
    [DCC_EVENT_SUBSCRIPTION_CREATE] = "SUBSCRIPTION_CREATE",
    [DCC_EVENT_SUBSCRIPTION_UPDATE] = "SUBSCRIPTION_UPDATE",
    [DCC_EVENT_SUBSCRIPTION_DELETE] = "SUBSCRIPTION_DELETE",
    [DCC_EVENT_TYPING_START] = "TYPING_START",
    [DCC_EVENT_USER_UPDATE] = "USER_UPDATE",
    [DCC_EVENT_VOICE_CHANNEL_EFFECT_SEND] = "VOICE_CHANNEL_EFFECT_SEND",
    [DCC_EVENT_VOICE_CHANNEL_START_TIME_UPDATE] = "VOICE_CHANNEL_START_TIME_UPDATE",
    [DCC_EVENT_VOICE_CHANNEL_STATUS_UPDATE] = "VOICE_CHANNEL_STATUS_UPDATE",
    [DCC_EVENT_VOICE_SERVER_UPDATE] = "VOICE_SERVER_UPDATE",
    [DCC_EVENT_VOICE_BUFFER_SEND] = "VOICE_BUFFER_SEND",
    [DCC_EVENT_VOICE_READY] = "VOICE_READY",
    [DCC_EVENT_VOICE_RECEIVE] = "VOICE_RECEIVE",
    [DCC_EVENT_VOICE_RECEIVE_COMBINED] = "VOICE_RECEIVE_COMBINED",
    [DCC_EVENT_VOICE_TRACK_MARKER] = "VOICE_TRACK_MARKER",
    [DCC_EVENT_VOICE_CLIENT_SPEAKING] = "VOICE_CLIENT_SPEAKING",
    [DCC_EVENT_VOICE_CLIENT_DISCONNECT] = "VOICE_CLIENT_DISCONNECT",
    [DCC_EVENT_VOICE_CLIENT_PLATFORM] = "VOICE_CLIENT_PLATFORM",
    [DCC_EVENT_WEBHOOKS_UPDATE] = "WEBHOOKS_UPDATE",
    [DCC_EVENT_GUILD_JOIN_REQUEST_DELETE] = "GUILD_JOIN_REQUEST_DELETE",
    [DCC_EVENT_GUILD_JOIN_REQUEST_UPDATE] = "GUILD_JOIN_REQUEST_UPDATE",
    [DCC_EVENT_GUILD_APPLICATION_COMMAND_COUNTS_UPDATE] = "GUILD_APPLICATION_COMMAND_COUNTS_UPDATE",
    [DCC_EVENT_GUILD_APPLICATION_COMMAND_INDEX_UPDATE] = "GUILD_APPLICATION_COMMAND_INDEX_UPDATE",
    [DCC_EVENT_EMBEDDED_ACTIVITY_UPDATE] = "EMBEDDED_ACTIVITY_UPDATE",
    [DCC_EVENT_CHANNEL_TOPIC_UPDATE] = "CHANNEL_TOPIC_UPDATE",
};

static void event_surface_noop(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    (void)event;
    (void)user_data;
}

static int event_surface_has_name(const char *name) {
    for (int type = 0; type < (int)DCC_EVENT_MAX; ++type) {
        const char *event_name = dcc_event_type_name((dcc_event_type_t)type);
        if (event_name != NULL && strcmp(event_name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

static int event_surface_name_is_unique(dcc_event_type_t type, const char *name) {
    for (int other = 0; other < (int)DCC_EVENT_MAX; ++other) {
        if (other == (int)type) {
            continue;
        }

        const char *other_name = dcc_event_type_name((dcc_event_type_t)other);
        if (other_name != NULL && strcmp(other_name, name) == 0) {
            return 0;
        }
    }
    return 1;
}

static int event_surface_exercise_listener_growth(dcc_client_t *client) {
    enum {
        listener_count = 12
    };
    dcc_listener_id_t ids[listener_count];
    memset(ids, 0, sizeof(ids));

    for (int i = 0; i < listener_count; ++i) {
        dcc_status_t st = dcc_client_on(client, DCC_EVENT_READY, event_surface_noop, NULL, &ids[i]);
        if (st != DCC_OK || ids[i] == 0) {
            fprintf(stderr, "listener growth add %d failed: %s\n", i, dcc_status_string(st));
            return 0;
        }
    }

    for (int i = 0; i < listener_count; ++i) {
        dcc_status_t st = dcc_client_off(client, DCC_EVENT_READY, ids[i]);
        if (st != DCC_OK) {
            fprintf(stderr, "listener growth remove %d failed: %s\n", i, dcc_status_string(st));
            return 0;
        }
    }

    return 1;
}

int main(void) {
    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .shard_count = 1,
    };

    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        return 1;
    }

    for (int type = 0; type < (int)DCC_EVENT_MAX; ++type) {
        const char *name = dcc_event_type_name((dcc_event_type_t)type);
        const char *expected_name = event_surface_expected_names[type];
        if (expected_name == NULL) {
            fprintf(stderr, "event type %d has no expected public name\n", type);
            dcc_client_destroy(client);
            return 1;
        }
        if (name == NULL || *name == '\0') {
            fprintf(stderr, "event type %d has no public name\n", type);
            dcc_client_destroy(client);
            return 1;
        }
        if (strcmp(name, expected_name) != 0) {
            fprintf(
                stderr,
                "event type %d public name mismatch: got %s expected %s\n",
                type,
                name,
                expected_name
            );
            dcc_client_destroy(client);
            return 1;
        }
        if (!event_surface_name_is_unique((dcc_event_type_t)type, name)) {
            fprintf(stderr, "event type %d has duplicate public name: %s\n", type, name);
            dcc_client_destroy(client);
            return 1;
        }

        dcc_listener_id_t id = 0;
        st = dcc_client_on(client, (dcc_event_type_t)type, event_surface_noop, NULL, &id);
        if (st != DCC_OK || id == 0) {
            fprintf(stderr, "event type %d cannot register listener: %s\n", type, dcc_status_string(st));
            dcc_client_destroy(client);
            return 1;
        }

        st = dcc_client_off(client, (dcc_event_type_t)type, id);
        if (st != DCC_OK) {
            fprintf(stderr, "event type %d cannot unregister listener: %s\n", type, dcc_status_string(st));
            dcc_client_destroy(client);
            return 1;
        }
    }

    if (!event_surface_exercise_listener_growth(client)) {
        dcc_client_destroy(client);
        return 1;
    }

    if (dcc_event_type_name(DCC_EVENT_MAX) != NULL) {
        fprintf(stderr, "DCC_EVENT_MAX unexpectedly has a public name\n");
        dcc_client_destroy(client);
        return 1;
    }

    size_t router_count = sizeof(dpp_router_event_names) / sizeof(dpp_router_event_names[0]);
    for (size_t i = 0; i < router_count; ++i) {
        if (!event_surface_has_name(dpp_router_event_names[i])) {
            fprintf(stderr, "missing DPP router event surface: %s\n", dpp_router_event_names[i]);
            dcc_client_destroy(client);
            return 1;
        }
    }

    dcc_client_destroy(client);
    return 0;
}
