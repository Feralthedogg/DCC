#include "internal/events/dcc_event_state_internal.h"

const dcc_scheduled_event_t *dcc_event_scheduled_event(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_GUILD_SCHEDULED_EVENT_CREATE:
        case DCC_EVENT_GUILD_SCHEDULED_EVENT_UPDATE:
        case DCC_EVENT_GUILD_SCHEDULED_EVENT_DELETE:
            return &event->data.scheduled_event;
        default:
            return NULL;
    }
}

const dcc_application_command_permissions_update_t *dcc_event_application_command_permissions_update(
    const dcc_event_t *event
) {
    return event != NULL && event->type == DCC_EVENT_APPLICATION_COMMAND_PERMISSIONS_UPDATE
        ? &event->data.application_command_permissions_update
        : NULL;
}

const dcc_guild_audit_log_entry_t *dcc_event_guild_audit_log_entry(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_GUILD_AUDIT_LOG_ENTRY_CREATE
        ? &event->data.guild_audit_log_entry
        : NULL;
}

const dcc_guild_id_list_update_t *dcc_event_guild_id_list_update(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_GUILD_EMOJIS_UPDATE:
        case DCC_EVENT_GUILD_STICKERS_UPDATE:
        case DCC_EVENT_GUILD_SOUNDBOARD_SOUNDS_UPDATE:
        case DCC_EVENT_SOUNDBOARD_SOUNDS:
            return &event->data.guild_id_list_update;
        default:
            return NULL;
    }
}

const dcc_guild_update_notice_t *dcc_event_guild_update_notice(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_GUILD_INTEGRATIONS_UPDATE
        ? &event->data.guild_update_notice
        : NULL;
}

const dcc_scheduled_event_user_update_t *dcc_event_scheduled_event_user_update(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_GUILD_SCHEDULED_EVENT_USER_ADD:
        case DCC_EVENT_GUILD_SCHEDULED_EVENT_USER_REMOVE:
            return &event->data.scheduled_event_user_update;
        default:
            return NULL;
    }
}

const dcc_guild_join_request_update_t *dcc_event_guild_join_request_update(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_GUILD_JOIN_REQUEST_DELETE:
        case DCC_EVENT_GUILD_JOIN_REQUEST_UPDATE:
            return &event->data.guild_join_request_update;
        default:
            return NULL;
    }
}

const dcc_guild_application_command_update_t *dcc_event_guild_application_command_update(
    const dcc_event_t *event
) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_GUILD_APPLICATION_COMMAND_COUNTS_UPDATE:
        case DCC_EVENT_GUILD_APPLICATION_COMMAND_INDEX_UPDATE:
            return &event->data.guild_application_command_update;
        default:
            return NULL;
    }
}

const dcc_embedded_activity_update_t *dcc_event_embedded_activity_update(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_EMBEDDED_ACTIVITY_UPDATE
        ? &event->data.embedded_activity_update
        : NULL;
}
