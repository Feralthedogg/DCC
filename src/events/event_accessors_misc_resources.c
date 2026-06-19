#include "internal/events/dcc_event_state_internal.h"

const dcc_stage_instance_t *dcc_event_stage_instance(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_STAGE_INSTANCE_CREATE:
        case DCC_EVENT_STAGE_INSTANCE_UPDATE:
        case DCC_EVENT_STAGE_INSTANCE_DELETE:
            return &event->data.stage_instance;
        default:
            return NULL;
    }
}

const dcc_soundboard_sound_t *dcc_event_soundboard_sound(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_GUILD_SOUNDBOARD_SOUND_CREATE:
        case DCC_EVENT_GUILD_SOUNDBOARD_SOUND_UPDATE:
        case DCC_EVENT_GUILD_SOUNDBOARD_SOUND_DELETE:
            return &event->data.soundboard_sound;
        default:
            return NULL;
    }
}

const dcc_integration_t *dcc_event_integration(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_INTEGRATION_CREATE:
        case DCC_EVENT_INTEGRATION_UPDATE:
        case DCC_EVENT_INTEGRATION_DELETE:
            return &event->data.integration;
        default:
            return NULL;
    }
}

const dcc_webhooks_update_t *dcc_event_webhooks_update(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_WEBHOOKS_UPDATE ? &event->data.webhooks_update : NULL;
}
