#include "internal/events/dcc_event_state_internal.h"

const dcc_message_t *dcc_event_message(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_MESSAGE_CREATE:
        case DCC_EVENT_MESSAGE_UPDATE:
        case DCC_EVENT_MESSAGE_DELETE:
            return &event->data.message;
        default:
            return NULL;
    }
}

const dcc_message_reaction_t *dcc_event_message_reaction(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_MESSAGE_REACTION_ADD:
        case DCC_EVENT_MESSAGE_REACTION_REMOVE:
        case DCC_EVENT_MESSAGE_REACTION_REMOVE_ALL:
        case DCC_EVENT_MESSAGE_REACTION_REMOVE_EMOJI:
            return &event->data.message_reaction;
        default:
            return NULL;
    }
}

const dcc_poll_vote_t *dcc_event_poll_vote(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_MESSAGE_POLL_VOTE_ADD:
        case DCC_EVENT_MESSAGE_POLL_VOTE_REMOVE:
            return &event->data.poll_vote;
        default:
            return NULL;
    }
}

const dcc_message_delete_bulk_t *dcc_event_message_delete_bulk(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_MESSAGE_DELETE_BULK
        ? &event->data.message_delete_bulk
        : NULL;
}
