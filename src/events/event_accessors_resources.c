#include "internal/events/dcc_event_state_internal.h"

const dcc_entitlement_t *dcc_event_entitlement(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_ENTITLEMENT_CREATE:
        case DCC_EVENT_ENTITLEMENT_UPDATE:
        case DCC_EVENT_ENTITLEMENT_DELETE:
            return &event->data.entitlement;
        default:
            return NULL;
    }
}

const dcc_subscription_t *dcc_event_subscription(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_SUBSCRIPTION_CREATE:
        case DCC_EVENT_SUBSCRIPTION_UPDATE:
        case DCC_EVENT_SUBSCRIPTION_DELETE:
            return &event->data.subscription;
        default:
            return NULL;
    }
}

const dcc_invite_t *dcc_event_invite(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_INVITE_CREATE:
        case DCC_EVENT_INVITE_DELETE:
            return &event->data.invite;
        default:
            return NULL;
    }
}
