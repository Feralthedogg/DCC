#include "internal/events/dcc_event_state_internal.h"

const dcc_auto_moderation_action_t *dcc_event_auto_moderation_action(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_AUTO_MODERATION_ACTION_EXECUTION
        ? &event->data.auto_moderation_action
        : NULL;
}

const dcc_auto_moderation_rule_t *dcc_event_auto_moderation_rule(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_AUTO_MODERATION_RULE_CREATE:
        case DCC_EVENT_AUTO_MODERATION_RULE_UPDATE:
        case DCC_EVENT_AUTO_MODERATION_RULE_DELETE:
            return &event->data.auto_moderation_rule;
        default:
            return NULL;
    }
}
