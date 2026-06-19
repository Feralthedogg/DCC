#include "internal/events/dcc_event_state_internal.h"

const dcc_guild_t *dcc_event_guild(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_GUILD_CREATE:
        case DCC_EVENT_GUILD_UPDATE:
        case DCC_EVENT_GUILD_DELETE:
            return &event->data.guild;
        default:
            return NULL;
    }
}

const dcc_role_t *dcc_event_role(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_GUILD_ROLE_CREATE:
        case DCC_EVENT_GUILD_ROLE_UPDATE:
        case DCC_EVENT_GUILD_ROLE_DELETE:
            return &event->data.role;
        default:
            return NULL;
    }
}

const dcc_member_t *dcc_event_member(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_GUILD_MEMBER_ADD:
        case DCC_EVENT_GUILD_MEMBER_REMOVE:
        case DCC_EVENT_GUILD_MEMBER_UPDATE:
            return &event->data.member;
        case DCC_EVENT_TYPING_START:
            return &event->data.typing_start.member;
        default:
            return NULL;
    }
}

const dcc_guild_members_chunk_t *dcc_event_guild_members_chunk(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_GUILD_MEMBERS_CHUNK
        ? &event->data.guild_members_chunk
        : NULL;
}

const dcc_presence_t *dcc_event_presence(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_PRESENCE_UPDATE ? &event->data.presence : NULL;
}

const dcc_user_t *dcc_event_user(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_USER_UPDATE ? &event->data.user : NULL;
}

const dcc_guild_ban_t *dcc_event_guild_ban(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_GUILD_BAN_ADD:
        case DCC_EVENT_GUILD_BAN_REMOVE:
            return &event->data.guild_ban;
        default:
            return NULL;
    }
}
