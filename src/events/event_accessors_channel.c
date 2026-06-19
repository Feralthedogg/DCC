#include "internal/events/dcc_event_state_internal.h"

const dcc_channel_t *dcc_event_channel(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_CHANNEL_CREATE:
        case DCC_EVENT_CHANNEL_UPDATE:
        case DCC_EVENT_CHANNEL_DELETE:
        case DCC_EVENT_CHANNEL_INFO:
        case DCC_EVENT_THREAD_CREATE:
        case DCC_EVENT_THREAD_UPDATE:
        case DCC_EVENT_THREAD_DELETE:
        case DCC_EVENT_CHANNEL_TOPIC_UPDATE:
            return &event->data.channel;
        default:
            return NULL;
    }
}

const dcc_thread_member_t *dcc_event_thread_member(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_THREAD_MEMBER_UPDATE
        ? &event->data.thread_member
        : NULL;
}

const dcc_thread_members_update_t *dcc_event_thread_members_update(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_THREAD_MEMBERS_UPDATE
        ? &event->data.thread_members_update
        : NULL;
}

const dcc_channel_pins_update_t *dcc_event_channel_pins_update(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_CHANNEL_PINS_UPDATE
        ? &event->data.channel_pins_update
        : NULL;
}

const dcc_typing_start_t *dcc_event_typing_start(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_TYPING_START
        ? &event->data.typing_start
        : NULL;
}

const dcc_thread_list_sync_t *dcc_event_thread_list_sync(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_THREAD_LIST_SYNC ? &event->data.thread_list_sync : NULL;
}
