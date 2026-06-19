#include "internal/events/dcc_events_internal.h"

#define DCC_EVENT_WAITER_DEFINE_CLONE(name, type, accessor, clone_fn) \
    void *name(const dcc_event_t *event, dcc_status_t *out_status) { \
        const type *object = accessor(event); \
        if (object == NULL) { \
            *out_status = DCC_ERR_STATE; \
            return NULL; \
        } \
        type *copy = clone_fn(object); \
        if (copy == NULL) { \
            *out_status = DCC_ERR_NOMEM; \
        } \
        return copy; \
    }

DCC_EVENT_WAITER_DEFINE_CLONE(
    dcc_event_waiter_clone_gateway_data,
    dcc_gateway_event_data_t,
    dcc_event_data,
    dcc_gateway_event_data_clone
)

DCC_EVENT_WAITER_DEFINE_CLONE(
    dcc_event_waiter_clone_message,
    dcc_message_t,
    dcc_event_message,
    dcc_message_clone
)

DCC_EVENT_WAITER_DEFINE_CLONE(
    dcc_event_waiter_clone_guild,
    dcc_guild_t,
    dcc_event_guild,
    dcc_guild_clone
)

DCC_EVENT_WAITER_DEFINE_CLONE(
    dcc_event_waiter_clone_channel,
    dcc_channel_t,
    dcc_event_channel,
    dcc_channel_clone
)

DCC_EVENT_WAITER_DEFINE_CLONE(
    dcc_event_waiter_clone_interaction,
    dcc_interaction_t,
    dcc_event_interaction,
    dcc_interaction_clone
)

DCC_EVENT_WAITER_DEFINE_CLONE(
    dcc_event_waiter_clone_voice_state,
    dcc_voice_state_t,
    dcc_event_voice_state,
    dcc_voice_state_clone
)

DCC_EVENT_WAITER_DEFINE_CLONE(
    dcc_event_waiter_clone_member,
    dcc_member_t,
    dcc_event_member,
    dcc_member_clone
)

DCC_EVENT_WAITER_DEFINE_CLONE(
    dcc_event_waiter_clone_role,
    dcc_role_t,
    dcc_event_role,
    dcc_role_clone
)

DCC_EVENT_WAITER_DEFINE_CLONE(
    dcc_event_waiter_clone_user,
    dcc_user_t,
    dcc_event_user,
    dcc_user_clone
)

#undef DCC_EVENT_WAITER_DEFINE_CLONE
