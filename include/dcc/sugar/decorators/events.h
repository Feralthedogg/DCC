#ifndef DCC_SUGAR_DECORATORS_EVENTS_H
#define DCC_SUGAR_DECORATORS_EVENTS_H

#include <dcc/sugar/decorators/base.h>

#define DCC_DECORATE_EVENT(handler_, event_type_) \
    static void handler_(dcc_app_t *app, const dcc_event_t *event, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_EVENT, \
        .event_type = (event_type_), \
        .event_handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, const dcc_event_t *event, void *user_data)

#define DCC_DECORATE_EVENT_DATA(handler_, event_type_, user_data_) \
    static void handler_(dcc_app_t *app, const dcc_event_t *event, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_EVENT, \
        .event_type = (event_type_), \
        .event_handler = (handler_), \
        .user_data = (user_data_) \
    }; \
    static void handler_(dcc_app_t *app, const dcc_event_t *event, void *user_data)

#define DCC_DECORATE_EVENT_PAYLOAD(handler_, event_type_, payload_type_, payload_name_, accessor_) \
    static void handler_( \
        dcc_app_t *app, \
        const payload_type_ *payload_name_, \
        const dcc_event_t *event, \
        void *user_data \
    ); \
    static void DCC_EVENT_DISPATCH_NAME(handler_)( \
        dcc_app_t *app, \
        const dcc_event_t *event, \
        void *user_data \
    ) { \
        const payload_type_ *payload_name_ = accessor_((event)); \
        if (payload_name_ != NULL) { \
            handler_((app), payload_name_, (event), (user_data)); \
        } \
    } \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_EVENT, \
        .event_type = (event_type_), \
        .event_handler = DCC_EVENT_DISPATCH_NAME(handler_) \
    }; \
    static void handler_( \
        dcc_app_t *app, \
        const payload_type_ *payload_name_, \
        const dcc_event_t *event, \
        void *user_data \
    )

#define DCC_DECORATE_EVENT_PAYLOAD_DATA( \
    handler_, \
    event_type_, \
    payload_type_, \
    payload_name_, \
    accessor_, \
    user_data_ \
) \
    static void handler_( \
        dcc_app_t *app, \
        const payload_type_ *payload_name_, \
        const dcc_event_t *event, \
        void *user_data \
    ); \
    static void DCC_EVENT_DISPATCH_NAME(handler_)( \
        dcc_app_t *app, \
        const dcc_event_t *event, \
        void *user_data \
    ) { \
        const payload_type_ *payload_name_ = accessor_((event)); \
        if (payload_name_ != NULL) { \
            handler_((app), payload_name_, (event), (user_data)); \
        } \
    } \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_EVENT, \
        .event_type = (event_type_), \
        .event_handler = DCC_EVENT_DISPATCH_NAME(handler_), \
        .user_data = (user_data_) \
    }; \
    static void handler_( \
        dcc_app_t *app, \
        const payload_type_ *payload_name_, \
        const dcc_event_t *event, \
        void *user_data \
    )

#define DCC_DECORATE_GUILD_CREATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_CREATE, dcc_guild_t, guild, dcc_event_guild)
#define DCC_DECORATE_GUILD_CREATE_DATA(handler_, user_data_) \
    DCC_DECORATE_EVENT_PAYLOAD_DATA( \
        handler_, DCC_EVENT_GUILD_CREATE, dcc_guild_t, guild, dcc_event_guild, (user_data_) \
    )
#define DCC_DECORATE_GUILD_UPDATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_UPDATE, dcc_guild_t, guild, dcc_event_guild)
#define DCC_DECORATE_GUILD_DELETE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_DELETE, dcc_guild_t, guild, dcc_event_guild)

#define DCC_DECORATE_MEMBER_JOIN(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_MEMBER_ADD, dcc_member_t, member, dcc_event_member)
#define DCC_DECORATE_MEMBER_JOIN_DATA(handler_, user_data_) \
    DCC_DECORATE_EVENT_PAYLOAD_DATA( \
        handler_, DCC_EVENT_GUILD_MEMBER_ADD, dcc_member_t, member, dcc_event_member, (user_data_) \
    )
#define DCC_DECORATE_MEMBER_ADD(handler_) DCC_DECORATE_MEMBER_JOIN(handler_)
#define DCC_DECORATE_MEMBER_ADD_DATA(handler_, user_data_) DCC_DECORATE_MEMBER_JOIN_DATA(handler_, (user_data_))
#define DCC_DECORATE_MEMBER_UPDATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_MEMBER_UPDATE, dcc_member_t, member, dcc_event_member)
#define DCC_DECORATE_MEMBER_REMOVE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_MEMBER_REMOVE, dcc_member_t, member, dcc_event_member)

#define DCC_DECORATE_ROLE_CREATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_ROLE_CREATE, dcc_role_t, role, dcc_event_role)
#define DCC_DECORATE_ROLE_UPDATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_ROLE_UPDATE, dcc_role_t, role, dcc_event_role)
#define DCC_DECORATE_ROLE_DELETE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_ROLE_DELETE, dcc_role_t, role, dcc_event_role)

#define DCC_DECORATE_CHANNEL_CREATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_CHANNEL_CREATE, dcc_channel_t, channel, dcc_event_channel)
#define DCC_DECORATE_CHANNEL_UPDATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_CHANNEL_UPDATE, dcc_channel_t, channel, dcc_event_channel)
#define DCC_DECORATE_CHANNEL_DELETE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_CHANNEL_DELETE, dcc_channel_t, channel, dcc_event_channel)
#define DCC_DECORATE_THREAD_CREATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_THREAD_CREATE, dcc_channel_t, thread, dcc_event_channel)
#define DCC_DECORATE_THREAD_UPDATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_THREAD_UPDATE, dcc_channel_t, thread, dcc_event_channel)
#define DCC_DECORATE_THREAD_DELETE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_THREAD_DELETE, dcc_channel_t, thread, dcc_event_channel)

#define DCC_DECORATE_REACTION_ADD(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD( \
        handler_, \
        DCC_EVENT_MESSAGE_REACTION_ADD, \
        dcc_message_reaction_t, \
        reaction, \
        dcc_event_message_reaction \
    )
#define DCC_DECORATE_REACTION_REMOVE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD( \
        handler_, \
        DCC_EVENT_MESSAGE_REACTION_REMOVE, \
        dcc_message_reaction_t, \
        reaction, \
        dcc_event_message_reaction \
    )
#define DCC_DECORATE_REACTION_CLEAR(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD( \
        handler_, \
        DCC_EVENT_MESSAGE_REACTION_REMOVE_ALL, \
        dcc_message_reaction_t, \
        reaction, \
        dcc_event_message_reaction \
    )
#define DCC_DECORATE_REACTION_EMOJI_CLEAR(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD( \
        handler_, \
        DCC_EVENT_MESSAGE_REACTION_REMOVE_EMOJI, \
        dcc_message_reaction_t, \
        reaction, \
        dcc_event_message_reaction \
    )

#define DCC_DECORATE_POLL_VOTE_ADD(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD( \
        handler_, \
        DCC_EVENT_MESSAGE_POLL_VOTE_ADD, \
        dcc_poll_vote_t, \
        poll_vote, \
        dcc_event_poll_vote \
    )
#define DCC_DECORATE_POLL_VOTE_REMOVE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD( \
        handler_, \
        DCC_EVENT_MESSAGE_POLL_VOTE_REMOVE, \
        dcc_poll_vote_t, \
        poll_vote, \
        dcc_event_poll_vote \
    )

#define DCC_DECORATE_VOICE_STATE_UPDATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD( \
        handler_, \
        DCC_EVENT_VOICE_STATE_UPDATE, \
        dcc_voice_state_t, \
        voice_state, \
        dcc_event_voice_state \
    )
#define DCC_DECORATE_TYPING_START(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD( \
        handler_, \
        DCC_EVENT_TYPING_START, \
        dcc_typing_start_t, \
        typing, \
        dcc_event_typing_start \
    )
#define DCC_DECORATE_PRESENCE_UPDATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_PRESENCE_UPDATE, dcc_presence_t, presence, dcc_event_presence)
#define DCC_DECORATE_USER_UPDATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_USER_UPDATE, dcc_user_t, user, dcc_event_user)
#define DCC_DECORATE_GUILD_BAN_ADD(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_BAN_ADD, dcc_guild_ban_t, ban, dcc_event_guild_ban)
#define DCC_DECORATE_GUILD_BAN_REMOVE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_BAN_REMOVE, dcc_guild_ban_t, ban, dcc_event_guild_ban)

#define DCC_DECORATE_READY(handler_) \
    static void handler_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_READY, \
        .ready_handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data)

#define DCC_DECORATE_READY_DATA(handler_, user_data_) \
    static void handler_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_READY, \
        .ready_handler = (handler_), \
        .user_data = (user_data_) \
    }; \
    static void handler_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data)

#define DCC_DECORATE_READY_ONCE(handler_) \
    static void handler_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_READY, \
        .ready_handler = (handler_), \
        .once = 1U \
    }; \
    static void handler_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data)

#define DCC_DECORATE_READY_ONCE_DATA(handler_, user_data_) \
    static void handler_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_READY, \
        .ready_handler = (handler_), \
        .user_data = (user_data_), \
        .once = 1U \
    }; \
    static void handler_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data)

#define DCC_DECORATE_MESSAGE_CREATE(handler_) \
    static void handler_(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_MESSAGE_CREATE, \
        .message_handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data)

#define DCC_DECORATE_MESSAGE_CREATE_DATA(handler_, user_data_) \
    static void handler_(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_MESSAGE_CREATE, \
        .message_handler = (handler_), \
        .user_data = (user_data_) \
    }; \
    static void handler_(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data)

#define DCC_DECORATE_MESSAGE(handler_) DCC_DECORATE_MESSAGE_CREATE(handler_)
#define DCC_DECORATE_MESSAGE_DATA(handler_, user_data_) DCC_DECORATE_MESSAGE_CREATE_DATA(handler_, (user_data_))

#define DCC_DECORATE_MESSAGE_UPDATE(handler_) \
    static void handler_(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_MESSAGE_UPDATE, \
        .message_handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data)

#define DCC_DECORATE_MESSAGE_DELETE(handler_) \
    static void handler_(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_MESSAGE_DELETE, \
        .message_handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data)


#endif /* DCC_SUGAR_DECORATORS_EVENTS_H */
