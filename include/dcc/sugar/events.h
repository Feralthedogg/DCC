#ifndef DCC_SUGAR_EVENTS_H
#define DCC_SUGAR_EVENTS_H

#include <dcc/app.h>
#include <dcc/events/accessors.h>
#include <dcc/sugar/app_extension.h>
#include <dcc/sugar/handlers.h>

#define DCC_EVENT_DISPATCH_NAME(name_) name_ ## _dcc_event_dispatch

#define DCC_EVENT_PAYLOAD_FN(name_, payload_type_, payload_name_, accessor_) \
    static void name_( \
        dcc_app_t *app, \
        const payload_type_ *payload_name_, \
        const dcc_event_t *event, \
        void *user_data \
    ); \
    static void DCC_EVENT_DISPATCH_NAME(name_)( \
        dcc_app_t *app, \
        const dcc_event_t *event, \
        void *user_data \
    ) { \
        const payload_type_ *payload_name_ = accessor_((event)); \
        if (payload_name_ != NULL) { \
            name_((app), payload_name_, (event), (user_data)); \
        } \
    } \
    static void name_( \
        dcc_app_t *app, \
        const payload_type_ *payload_name_, \
        const dcc_event_t *event, \
        void *user_data \
    )

#define DCC_PUBLIC_EVENT_PAYLOAD_FN(name_, payload_type_, payload_name_, accessor_) \
    void name_( \
        dcc_app_t *app, \
        const payload_type_ *payload_name_, \
        const dcc_event_t *event, \
        void *user_data \
    ); \
    void DCC_EVENT_DISPATCH_NAME(name_)( \
        dcc_app_t *app, \
        const dcc_event_t *event, \
        void *user_data \
    ) { \
        const payload_type_ *payload_name_ = accessor_((event)); \
        if (payload_name_ != NULL) { \
            name_((app), payload_name_, (event), (user_data)); \
        } \
    } \
    void name_( \
        dcc_app_t *app, \
        const payload_type_ *payload_name_, \
        const dcc_event_t *event, \
        void *user_data \
    )

#define DCC_EVENT_ROUTE(type_, handler_) \
    DCC_ON_EVENT((type_), DCC_EVENT_DISPATCH_NAME(handler_))

#define DCC_EVENT_ROUTE_DATA(type_, handler_, user_data_) \
    DCC_ON_EVENT_DATA((type_), DCC_EVENT_DISPATCH_NAME(handler_), (user_data_))

#define DCC_APP_ON_EVENT_PAYLOAD(app_, type_, handler_) \
    dcc_app_on((app_), (type_), DCC_EVENT_DISPATCH_NAME(handler_), NULL)

#define DCC_APP_ON_EVENT_PAYLOAD_DATA(app_, type_, handler_, user_data_) \
    dcc_app_on((app_), (type_), DCC_EVENT_DISPATCH_NAME(handler_), (user_data_))

#define DCC_MESSAGE_CREATE_FN(name_) DCC_MESSAGE_FN(name_)
#define DCC_MESSAGE_UPDATE_FN(name_) DCC_MESSAGE_FN(name_)
#define DCC_MESSAGE_DELETE_FN(name_) DCC_MESSAGE_FN(name_)

#define DCC_GUILD_CREATE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_guild_t, guild, dcc_event_guild)
#define DCC_GUILD_UPDATE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_guild_t, guild, dcc_event_guild)
#define DCC_GUILD_DELETE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_guild_t, guild, dcc_event_guild)

#define DCC_MEMBER_JOIN_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_member_t, member, dcc_event_member)
#define DCC_MEMBER_ADD_FN(name_) DCC_MEMBER_JOIN_FN(name_)
#define DCC_MEMBER_UPDATE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_member_t, member, dcc_event_member)
#define DCC_MEMBER_REMOVE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_member_t, member, dcc_event_member)

#define DCC_ROLE_CREATE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_role_t, role, dcc_event_role)
#define DCC_ROLE_UPDATE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_role_t, role, dcc_event_role)
#define DCC_ROLE_DELETE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_role_t, role, dcc_event_role)

#define DCC_CHANNEL_CREATE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_channel_t, channel, dcc_event_channel)
#define DCC_CHANNEL_UPDATE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_channel_t, channel, dcc_event_channel)
#define DCC_CHANNEL_DELETE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_channel_t, channel, dcc_event_channel)
#define DCC_THREAD_CREATE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_channel_t, thread, dcc_event_channel)
#define DCC_THREAD_UPDATE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_channel_t, thread, dcc_event_channel)
#define DCC_THREAD_DELETE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_channel_t, thread, dcc_event_channel)

#define DCC_REACTION_ADD_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_message_reaction_t, reaction, dcc_event_message_reaction)
#define DCC_REACTION_REMOVE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_message_reaction_t, reaction, dcc_event_message_reaction)
#define DCC_REACTION_CLEAR_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_message_reaction_t, reaction, dcc_event_message_reaction)
#define DCC_REACTION_EMOJI_CLEAR_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_message_reaction_t, reaction, dcc_event_message_reaction)

#define DCC_POLL_VOTE_ADD_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_poll_vote_t, poll_vote, dcc_event_poll_vote)
#define DCC_POLL_VOTE_REMOVE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_poll_vote_t, poll_vote, dcc_event_poll_vote)

#define DCC_VOICE_STATE_UPDATE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_voice_state_t, voice_state, dcc_event_voice_state)
#define DCC_TYPING_START_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_typing_start_t, typing, dcc_event_typing_start)
#define DCC_PRESENCE_UPDATE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_presence_t, presence, dcc_event_presence)
#define DCC_USER_UPDATE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_user_t, user, dcc_event_user)
#define DCC_GUILD_BAN_ADD_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_guild_ban_t, ban, dcc_event_guild_ban)
#define DCC_GUILD_BAN_REMOVE_FN(name_) \
    DCC_EVENT_PAYLOAD_FN(name_, dcc_guild_ban_t, ban, dcc_event_guild_ban)

#define DCC_ON_GUILD_CREATE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_GUILD_CREATE, handler_)
#define DCC_ON_GUILD_CREATE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_GUILD_CREATE, handler_, user_data_)
#define DCC_ON_GUILD_UPDATE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_GUILD_UPDATE, handler_)
#define DCC_ON_GUILD_UPDATE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_GUILD_UPDATE, handler_, user_data_)
#define DCC_ON_GUILD_DELETE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_GUILD_DELETE, handler_)
#define DCC_ON_GUILD_DELETE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_GUILD_DELETE, handler_, user_data_)

#define DCC_ON_MEMBER_JOIN(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_GUILD_MEMBER_ADD, handler_)
#define DCC_ON_MEMBER_JOIN_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_GUILD_MEMBER_ADD, handler_, user_data_)
#define DCC_ON_MEMBER_ADD(handler_) DCC_ON_MEMBER_JOIN(handler_)
#define DCC_ON_MEMBER_ADD_DATA(handler_, user_data_) DCC_ON_MEMBER_JOIN_DATA(handler_, user_data_)
#define DCC_ON_MEMBER_UPDATE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_GUILD_MEMBER_UPDATE, handler_)
#define DCC_ON_MEMBER_UPDATE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_GUILD_MEMBER_UPDATE, handler_, user_data_)
#define DCC_ON_MEMBER_REMOVE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_GUILD_MEMBER_REMOVE, handler_)
#define DCC_ON_MEMBER_REMOVE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_GUILD_MEMBER_REMOVE, handler_, user_data_)

#define DCC_ON_ROLE_CREATE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_GUILD_ROLE_CREATE, handler_)
#define DCC_ON_ROLE_CREATE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_GUILD_ROLE_CREATE, handler_, user_data_)
#define DCC_ON_ROLE_UPDATE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_GUILD_ROLE_UPDATE, handler_)
#define DCC_ON_ROLE_UPDATE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_GUILD_ROLE_UPDATE, handler_, user_data_)
#define DCC_ON_ROLE_DELETE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_GUILD_ROLE_DELETE, handler_)
#define DCC_ON_ROLE_DELETE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_GUILD_ROLE_DELETE, handler_, user_data_)

#define DCC_ON_CHANNEL_CREATE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_CHANNEL_CREATE, handler_)
#define DCC_ON_CHANNEL_CREATE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_CHANNEL_CREATE, handler_, user_data_)
#define DCC_ON_CHANNEL_UPDATE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_CHANNEL_UPDATE, handler_)
#define DCC_ON_CHANNEL_UPDATE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_CHANNEL_UPDATE, handler_, user_data_)
#define DCC_ON_CHANNEL_DELETE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_CHANNEL_DELETE, handler_)
#define DCC_ON_CHANNEL_DELETE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_CHANNEL_DELETE, handler_, user_data_)

#define DCC_ON_THREAD_CREATE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_THREAD_CREATE, handler_)
#define DCC_ON_THREAD_CREATE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_THREAD_CREATE, handler_, user_data_)
#define DCC_ON_THREAD_UPDATE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_THREAD_UPDATE, handler_)
#define DCC_ON_THREAD_UPDATE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_THREAD_UPDATE, handler_, user_data_)
#define DCC_ON_THREAD_DELETE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_THREAD_DELETE, handler_)
#define DCC_ON_THREAD_DELETE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_THREAD_DELETE, handler_, user_data_)

#define DCC_ON_REACTION_ADD(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_MESSAGE_REACTION_ADD, handler_)
#define DCC_ON_REACTION_ADD_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_MESSAGE_REACTION_ADD, handler_, user_data_)
#define DCC_ON_REACTION_REMOVE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_MESSAGE_REACTION_REMOVE, handler_)
#define DCC_ON_REACTION_REMOVE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_MESSAGE_REACTION_REMOVE, handler_, user_data_)
#define DCC_ON_REACTION_CLEAR(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_MESSAGE_REACTION_REMOVE_ALL, handler_)
#define DCC_ON_REACTION_CLEAR_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_MESSAGE_REACTION_REMOVE_ALL, handler_, user_data_)
#define DCC_ON_REACTION_EMOJI_CLEAR(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_MESSAGE_REACTION_REMOVE_EMOJI, handler_)
#define DCC_ON_REACTION_EMOJI_CLEAR_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_MESSAGE_REACTION_REMOVE_EMOJI, handler_, user_data_)

#define DCC_ON_POLL_VOTE_ADD(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_MESSAGE_POLL_VOTE_ADD, handler_)
#define DCC_ON_POLL_VOTE_ADD_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_MESSAGE_POLL_VOTE_ADD, handler_, user_data_)
#define DCC_ON_POLL_VOTE_REMOVE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_MESSAGE_POLL_VOTE_REMOVE, handler_)
#define DCC_ON_POLL_VOTE_REMOVE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_MESSAGE_POLL_VOTE_REMOVE, handler_, user_data_)

#define DCC_ON_VOICE_STATE_UPDATE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_VOICE_STATE_UPDATE, handler_)
#define DCC_ON_VOICE_STATE_UPDATE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_VOICE_STATE_UPDATE, handler_, user_data_)
#define DCC_ON_TYPING_START(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_TYPING_START, handler_)
#define DCC_ON_TYPING_START_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_TYPING_START, handler_, user_data_)
#define DCC_ON_PRESENCE_UPDATE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_PRESENCE_UPDATE, handler_)
#define DCC_ON_PRESENCE_UPDATE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_PRESENCE_UPDATE, handler_, user_data_)
#define DCC_ON_USER_UPDATE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_USER_UPDATE, handler_)
#define DCC_ON_USER_UPDATE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_USER_UPDATE, handler_, user_data_)
#define DCC_ON_GUILD_BAN_ADD(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_GUILD_BAN_ADD, handler_)
#define DCC_ON_GUILD_BAN_ADD_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_GUILD_BAN_ADD, handler_, user_data_)
#define DCC_ON_GUILD_BAN_REMOVE(handler_) \
    DCC_EVENT_ROUTE(DCC_EVENT_GUILD_BAN_REMOVE, handler_)
#define DCC_ON_GUILD_BAN_REMOVE_DATA(handler_, user_data_) \
    DCC_EVENT_ROUTE_DATA(DCC_EVENT_GUILD_BAN_REMOVE, handler_, user_data_)

#define DCC_GUILD_CREATE(handler_) \
    DCC_ON_GUILD_CREATE(handler_)
#define DCC_GUILD_CREATE_DATA(handler_, user_data_) \
    DCC_ON_GUILD_CREATE_DATA(handler_, user_data_)
#define DCC_GUILD_UPDATE(handler_) \
    DCC_ON_GUILD_UPDATE(handler_)
#define DCC_GUILD_UPDATE_DATA(handler_, user_data_) \
    DCC_ON_GUILD_UPDATE_DATA(handler_, user_data_)
#define DCC_GUILD_DELETE(handler_) \
    DCC_ON_GUILD_DELETE(handler_)
#define DCC_GUILD_DELETE_DATA(handler_, user_data_) \
    DCC_ON_GUILD_DELETE_DATA(handler_, user_data_)

#define DCC_MEMBER_JOIN(handler_) \
    DCC_ON_MEMBER_JOIN(handler_)
#define DCC_MEMBER_JOIN_DATA(handler_, user_data_) \
    DCC_ON_MEMBER_JOIN_DATA(handler_, user_data_)
#define DCC_MEMBER_ADD(handler_) DCC_MEMBER_JOIN(handler_)
#define DCC_MEMBER_ADD_DATA(handler_, user_data_) DCC_MEMBER_JOIN_DATA(handler_, user_data_)
#define DCC_MEMBER_UPDATE(handler_) \
    DCC_ON_MEMBER_UPDATE(handler_)
#define DCC_MEMBER_UPDATE_DATA(handler_, user_data_) \
    DCC_ON_MEMBER_UPDATE_DATA(handler_, user_data_)
#define DCC_MEMBER_REMOVE(handler_) \
    DCC_ON_MEMBER_REMOVE(handler_)
#define DCC_MEMBER_REMOVE_DATA(handler_, user_data_) \
    DCC_ON_MEMBER_REMOVE_DATA(handler_, user_data_)

#define DCC_ROLE_CREATE(handler_) \
    DCC_ON_ROLE_CREATE(handler_)
#define DCC_ROLE_CREATE_DATA(handler_, user_data_) \
    DCC_ON_ROLE_CREATE_DATA(handler_, user_data_)
#define DCC_ROLE_UPDATE(handler_) \
    DCC_ON_ROLE_UPDATE(handler_)
#define DCC_ROLE_UPDATE_DATA(handler_, user_data_) \
    DCC_ON_ROLE_UPDATE_DATA(handler_, user_data_)
#define DCC_ROLE_DELETE_EVENT(handler_) \
    DCC_ON_ROLE_DELETE(handler_)
#define DCC_ROLE_DELETE_EVENT_DATA(handler_, user_data_) \
    DCC_ON_ROLE_DELETE_DATA(handler_, user_data_)

#define DCC_CHANNEL_CREATE(handler_) \
    DCC_ON_CHANNEL_CREATE(handler_)
#define DCC_CHANNEL_CREATE_DATA(handler_, user_data_) \
    DCC_ON_CHANNEL_CREATE_DATA(handler_, user_data_)
#define DCC_CHANNEL_UPDATE(handler_) \
    DCC_ON_CHANNEL_UPDATE(handler_)
#define DCC_CHANNEL_UPDATE_DATA(handler_, user_data_) \
    DCC_ON_CHANNEL_UPDATE_DATA(handler_, user_data_)
#define DCC_CHANNEL_DELETE_EVENT(handler_) \
    DCC_ON_CHANNEL_DELETE(handler_)
#define DCC_CHANNEL_DELETE_EVENT_DATA(handler_, user_data_) \
    DCC_ON_CHANNEL_DELETE_DATA(handler_, user_data_)

#define DCC_THREAD_CREATE(handler_) \
    DCC_ON_THREAD_CREATE(handler_)
#define DCC_THREAD_CREATE_DATA(handler_, user_data_) \
    DCC_ON_THREAD_CREATE_DATA(handler_, user_data_)
#define DCC_THREAD_UPDATE(handler_) \
    DCC_ON_THREAD_UPDATE(handler_)
#define DCC_THREAD_UPDATE_DATA(handler_, user_data_) \
    DCC_ON_THREAD_UPDATE_DATA(handler_, user_data_)
#define DCC_THREAD_DELETE_EVENT(handler_) \
    DCC_ON_THREAD_DELETE(handler_)
#define DCC_THREAD_DELETE_EVENT_DATA(handler_, user_data_) \
    DCC_ON_THREAD_DELETE_DATA(handler_, user_data_)

#define DCC_REACTION_ADD(handler_) \
    DCC_ON_REACTION_ADD(handler_)
#define DCC_REACTION_ADD_DATA(handler_, user_data_) \
    DCC_ON_REACTION_ADD_DATA(handler_, user_data_)
#define DCC_REACTION_REMOVE(handler_) \
    DCC_ON_REACTION_REMOVE(handler_)
#define DCC_REACTION_REMOVE_DATA(handler_, user_data_) \
    DCC_ON_REACTION_REMOVE_DATA(handler_, user_data_)
#define DCC_REACTION_CLEAR(handler_) \
    DCC_ON_REACTION_CLEAR(handler_)
#define DCC_REACTION_CLEAR_DATA(handler_, user_data_) \
    DCC_ON_REACTION_CLEAR_DATA(handler_, user_data_)
#define DCC_REACTION_EMOJI_CLEAR(handler_) \
    DCC_ON_REACTION_EMOJI_CLEAR(handler_)
#define DCC_REACTION_EMOJI_CLEAR_DATA(handler_, user_data_) \
    DCC_ON_REACTION_EMOJI_CLEAR_DATA(handler_, user_data_)

#define DCC_POLL_VOTE_ADD(handler_) \
    DCC_ON_POLL_VOTE_ADD(handler_)
#define DCC_POLL_VOTE_ADD_DATA(handler_, user_data_) \
    DCC_ON_POLL_VOTE_ADD_DATA(handler_, user_data_)
#define DCC_POLL_VOTE_REMOVE(handler_) \
    DCC_ON_POLL_VOTE_REMOVE(handler_)
#define DCC_POLL_VOTE_REMOVE_DATA(handler_, user_data_) \
    DCC_ON_POLL_VOTE_REMOVE_DATA(handler_, user_data_)

#define DCC_VOICE_STATE_UPDATE(handler_) \
    DCC_ON_VOICE_STATE_UPDATE(handler_)
#define DCC_VOICE_STATE_UPDATE_DATA(handler_, user_data_) \
    DCC_ON_VOICE_STATE_UPDATE_DATA(handler_, user_data_)
#define DCC_TYPING_START(handler_) \
    DCC_ON_TYPING_START(handler_)
#define DCC_TYPING_START_DATA(handler_, user_data_) \
    DCC_ON_TYPING_START_DATA(handler_, user_data_)
#define DCC_PRESENCE_UPDATE(handler_) \
    DCC_ON_PRESENCE_UPDATE(handler_)
#define DCC_PRESENCE_UPDATE_DATA(handler_, user_data_) \
    DCC_ON_PRESENCE_UPDATE_DATA(handler_, user_data_)
#define DCC_USER_UPDATE(handler_) \
    DCC_ON_USER_UPDATE(handler_)
#define DCC_USER_UPDATE_DATA(handler_, user_data_) \
    DCC_ON_USER_UPDATE_DATA(handler_, user_data_)
#define DCC_GUILD_BAN_ADD(handler_) \
    DCC_ON_GUILD_BAN_ADD(handler_)
#define DCC_GUILD_BAN_ADD_DATA(handler_, user_data_) \
    DCC_ON_GUILD_BAN_ADD_DATA(handler_, user_data_)
#define DCC_GUILD_BAN_REMOVE(handler_) \
    DCC_ON_GUILD_BAN_REMOVE(handler_)
#define DCC_GUILD_BAN_REMOVE_DATA(handler_, user_data_) \
    DCC_ON_GUILD_BAN_REMOVE_DATA(handler_, user_data_)

#define DCC_APP_ON_GUILD_CREATE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_GUILD_CREATE, handler_)
#define DCC_APP_ON_GUILD_CREATE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_GUILD_CREATE, handler_, user_data_)
#define DCC_APP_ON_GUILD_UPDATE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_GUILD_UPDATE, handler_)
#define DCC_APP_ON_GUILD_UPDATE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_GUILD_UPDATE, handler_, user_data_)
#define DCC_APP_ON_GUILD_DELETE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_GUILD_DELETE, handler_)
#define DCC_APP_ON_GUILD_DELETE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_GUILD_DELETE, handler_, user_data_)

#define DCC_APP_ON_MEMBER_JOIN(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_GUILD_MEMBER_ADD, handler_)
#define DCC_APP_ON_MEMBER_JOIN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_GUILD_MEMBER_ADD, handler_, user_data_)
#define DCC_APP_ON_MEMBER_ADD(app_, handler_) DCC_APP_ON_MEMBER_JOIN((app_), handler_)
#define DCC_APP_ON_MEMBER_ADD_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MEMBER_JOIN_DATA((app_), handler_, user_data_)
#define DCC_APP_ON_MEMBER_UPDATE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_GUILD_MEMBER_UPDATE, handler_)
#define DCC_APP_ON_MEMBER_UPDATE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_GUILD_MEMBER_UPDATE, handler_, user_data_)
#define DCC_APP_ON_MEMBER_REMOVE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_GUILD_MEMBER_REMOVE, handler_)
#define DCC_APP_ON_MEMBER_REMOVE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_GUILD_MEMBER_REMOVE, handler_, user_data_)

#define DCC_APP_ON_ROLE_CREATE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_GUILD_ROLE_CREATE, handler_)
#define DCC_APP_ON_ROLE_CREATE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_GUILD_ROLE_CREATE, handler_, user_data_)
#define DCC_APP_ON_ROLE_UPDATE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_GUILD_ROLE_UPDATE, handler_)
#define DCC_APP_ON_ROLE_UPDATE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_GUILD_ROLE_UPDATE, handler_, user_data_)
#define DCC_APP_ON_ROLE_DELETE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_GUILD_ROLE_DELETE, handler_)
#define DCC_APP_ON_ROLE_DELETE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_GUILD_ROLE_DELETE, handler_, user_data_)

#define DCC_APP_ON_CHANNEL_CREATE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_CHANNEL_CREATE, handler_)
#define DCC_APP_ON_CHANNEL_CREATE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_CHANNEL_CREATE, handler_, user_data_)
#define DCC_APP_ON_CHANNEL_UPDATE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_CHANNEL_UPDATE, handler_)
#define DCC_APP_ON_CHANNEL_UPDATE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_CHANNEL_UPDATE, handler_, user_data_)
#define DCC_APP_ON_CHANNEL_DELETE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_CHANNEL_DELETE, handler_)
#define DCC_APP_ON_CHANNEL_DELETE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_CHANNEL_DELETE, handler_, user_data_)

#define DCC_APP_ON_THREAD_CREATE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_THREAD_CREATE, handler_)
#define DCC_APP_ON_THREAD_CREATE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_THREAD_CREATE, handler_, user_data_)
#define DCC_APP_ON_THREAD_UPDATE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_THREAD_UPDATE, handler_)
#define DCC_APP_ON_THREAD_UPDATE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_THREAD_UPDATE, handler_, user_data_)
#define DCC_APP_ON_THREAD_DELETE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_THREAD_DELETE, handler_)
#define DCC_APP_ON_THREAD_DELETE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_THREAD_DELETE, handler_, user_data_)

#define DCC_APP_ON_REACTION_ADD(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_MESSAGE_REACTION_ADD, handler_)
#define DCC_APP_ON_REACTION_ADD_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_MESSAGE_REACTION_ADD, handler_, user_data_)
#define DCC_APP_ON_REACTION_REMOVE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_MESSAGE_REACTION_REMOVE, handler_)
#define DCC_APP_ON_REACTION_REMOVE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_MESSAGE_REACTION_REMOVE, handler_, user_data_)
#define DCC_APP_ON_REACTION_CLEAR(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_MESSAGE_REACTION_REMOVE_ALL, handler_)
#define DCC_APP_ON_REACTION_CLEAR_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_MESSAGE_REACTION_REMOVE_ALL, handler_, user_data_)
#define DCC_APP_ON_REACTION_EMOJI_CLEAR(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_MESSAGE_REACTION_REMOVE_EMOJI, handler_)
#define DCC_APP_ON_REACTION_EMOJI_CLEAR_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_MESSAGE_REACTION_REMOVE_EMOJI, handler_, user_data_)

#define DCC_APP_ON_POLL_VOTE_ADD(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_MESSAGE_POLL_VOTE_ADD, handler_)
#define DCC_APP_ON_POLL_VOTE_ADD_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_MESSAGE_POLL_VOTE_ADD, handler_, user_data_)
#define DCC_APP_ON_POLL_VOTE_REMOVE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_MESSAGE_POLL_VOTE_REMOVE, handler_)
#define DCC_APP_ON_POLL_VOTE_REMOVE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_MESSAGE_POLL_VOTE_REMOVE, handler_, user_data_)

#define DCC_APP_ON_VOICE_STATE_UPDATE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_VOICE_STATE_UPDATE, handler_)
#define DCC_APP_ON_VOICE_STATE_UPDATE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_VOICE_STATE_UPDATE, handler_, user_data_)
#define DCC_APP_ON_TYPING_START(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_TYPING_START, handler_)
#define DCC_APP_ON_TYPING_START_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_TYPING_START, handler_, user_data_)
#define DCC_APP_ON_PRESENCE_UPDATE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_PRESENCE_UPDATE, handler_)
#define DCC_APP_ON_PRESENCE_UPDATE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_PRESENCE_UPDATE, handler_, user_data_)
#define DCC_APP_ON_USER_UPDATE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_USER_UPDATE, handler_)
#define DCC_APP_ON_USER_UPDATE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_USER_UPDATE, handler_, user_data_)
#define DCC_APP_ON_GUILD_BAN_ADD(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_GUILD_BAN_ADD, handler_)
#define DCC_APP_ON_GUILD_BAN_ADD_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_GUILD_BAN_ADD, handler_, user_data_)
#define DCC_APP_ON_GUILD_BAN_REMOVE(app_, handler_) \
    DCC_APP_ON_EVENT_PAYLOAD((app_), DCC_EVENT_GUILD_BAN_REMOVE, handler_)
#define DCC_APP_ON_GUILD_BAN_REMOVE_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_EVENT_PAYLOAD_DATA((app_), DCC_EVENT_GUILD_BAN_REMOVE, handler_, user_data_)

#endif
