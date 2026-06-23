#ifndef DCC_SUGAR_LISTENERS_BASE_ROUTES_H
#define DCC_SUGAR_LISTENERS_BASE_ROUTES_H

#include <dcc/sugar/listeners/types.h>

#define DCC_LISTEN_SLASH_ROUTE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_SLASH, \
        .slash = (route_) \
    })
#define DCC_LISTEN_COMMAND_SCHEMA(command_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_COMMAND_SCHEMA, \
        .command_schema = (command_) \
    })
#define DCC_LISTEN_COMMAND_BUILDER(command_) \
    DCC_LISTEN_COMMAND_SCHEMA(command_)
#define DCC_LISTEN_SCHEMA(command_) \
    DCC_LISTEN_COMMAND_SCHEMA(command_)
#define DCC_LISTEN_SLASH_BUILDER(builder_, handler_) \
    DCC_LISTEN_SLASH_ROUTE(DCC_SLASH_BUILDER_VALUE((builder_), (handler_)))
#define DCC_LISTEN_SLASH_BUILDER_DATA(builder_, handler_, user_data_) \
    DCC_LISTEN_SLASH_ROUTE(DCC_SLASH_BUILDER_VALUE_DATA((builder_), (handler_), (user_data_)))
#define DCC_LISTEN_SLASH_BUILDER_POLICY(builder_, handler_, policy_) \
    DCC_LISTEN_SLASH_ROUTE(DCC_SLASH_BUILDER_VALUE_POLICY((builder_), (handler_), (policy_)))
#define DCC_LISTEN_SLASH_BUILDER_DATA_POLICY(builder_, handler_, user_data_, policy_) \
    DCC_LISTEN_SLASH_ROUTE( \
        DCC_SLASH_BUILDER_VALUE_DATA_POLICY((builder_), (handler_), (user_data_), (policy_)) \
    )
#define DCC_LISTEN_COMMAND_BUILDER_VALUE(builder_, handler_) \
    DCC_LISTEN_SLASH_BUILDER((builder_), (handler_))
#define DCC_LISTEN_COMMAND_BUILDER_VALUE_DATA(builder_, handler_, user_data_) \
    DCC_LISTEN_SLASH_BUILDER_DATA((builder_), (handler_), (user_data_))
#define DCC_LISTEN_COMMAND_BUILDER_VALUE_POLICY(builder_, handler_, policy_) \
    DCC_LISTEN_SLASH_BUILDER_POLICY((builder_), (handler_), (policy_))
#define DCC_LISTEN_COMMAND_BUILDER_VALUE_DATA_POLICY(builder_, handler_, user_data_, policy_) \
    DCC_LISTEN_SLASH_BUILDER_DATA_POLICY((builder_), (handler_), (user_data_), (policy_))
#define DCC_LISTEN_SLASH(name_, description_, handler_) \
    DCC_LISTEN_SLASH_ROUTE(DCC_ON_SLASH((name_), (description_), (handler_)))
#define DCC_LISTEN_SLASH_DATA(name_, description_, handler_, user_data_) \
    DCC_LISTEN_SLASH_ROUTE(DCC_ON_SLASH_DATA((name_), (description_), (handler_), (user_data_)))
#define DCC_LISTEN_SLASH_GUARDED(name_, description_, handler_, ...) \
    DCC_LISTEN_SLASH_ROUTE(DCC_ON_SLASH_GUARDED((name_), (description_), (handler_), __VA_ARGS__))
#define DCC_LISTEN_SLASH_DATA_GUARDED(name_, description_, handler_, user_data_, ...) \
    DCC_LISTEN_SLASH_ROUTE( \
        DCC_ON_SLASH_DATA_GUARDED((name_), (description_), (handler_), (user_data_), __VA_ARGS__) \
    )
#define DCC_LISTEN_COMMAND(name_, description_, handler_) \
    DCC_LISTEN_SLASH((name_), (description_), (handler_))
#define DCC_LISTEN_COMMAND_DATA(name_, description_, handler_, user_data_) \
    DCC_LISTEN_SLASH_DATA((name_), (description_), (handler_), (user_data_))

#define DCC_LISTEN_SUBCOMMAND_ROUTE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_SUBCOMMAND, \
        .subcommand = (route_) \
    })
#define DCC_LISTEN_SUBCOMMAND(command_name_, subcommand_path_, handler_) \
    DCC_LISTEN_SUBCOMMAND_ROUTE(DCC_SUBCOMMAND((command_name_), (subcommand_path_), (handler_)))
#define DCC_LISTEN_SUBCOMMAND_DATA(command_name_, subcommand_path_, handler_, user_data_) \
    DCC_LISTEN_SUBCOMMAND_ROUTE(DCC_SUBCOMMAND_DATA((command_name_), (subcommand_path_), (handler_), (user_data_)))

#define DCC_LISTEN_AUTOCOMPLETE_ROUTE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_AUTOCOMPLETE, \
        .autocomplete = (route_) \
    })
#define DCC_LISTEN_AUTOCOMPLETE(name_, handler_) \
    DCC_LISTEN_AUTOCOMPLETE_ROUTE(DCC_ON_AUTOCOMPLETE((name_), (handler_)))
#define DCC_LISTEN_AUTOCOMPLETE_DATA(name_, handler_, user_data_) \
    DCC_LISTEN_AUTOCOMPLETE_ROUTE(DCC_ON_AUTOCOMPLETE_DATA((name_), (handler_), (user_data_)))

#define DCC_LISTEN_CONTEXT_MENU_ROUTE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_CONTEXT_MENU, \
        .context_menu = (route_) \
    })
#define DCC_LISTEN_USER_CONTEXT_MENU(name_, handler_) \
    DCC_LISTEN_CONTEXT_MENU_ROUTE(DCC_ON_USER_CONTEXT_MENU((name_), (handler_)))
#define DCC_LISTEN_USER_CONTEXT_MENU_DATA(name_, handler_, user_data_) \
    DCC_LISTEN_CONTEXT_MENU_ROUTE(DCC_ON_USER_CONTEXT_MENU_DATA((name_), (handler_), (user_data_)))
#define DCC_LISTEN_MESSAGE_CONTEXT_MENU(name_, handler_) \
    DCC_LISTEN_CONTEXT_MENU_ROUTE(DCC_ON_MESSAGE_CONTEXT_MENU((name_), (handler_)))
#define DCC_LISTEN_MESSAGE_CONTEXT_MENU_DATA(name_, handler_, user_data_) \
    DCC_LISTEN_CONTEXT_MENU_ROUTE(DCC_ON_MESSAGE_CONTEXT_MENU_DATA((name_), (handler_), (user_data_)))

#define DCC_LISTEN_COMPONENT_ROUTE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_COMPONENT, \
        .component = (route_) \
    })
#define DCC_LISTEN_BUTTON(custom_id_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_BUTTON((custom_id_), (handler_)))
#define DCC_LISTEN_BUTTON_DATA(custom_id_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_BUTTON_DATA((custom_id_), (handler_), (user_data_)))
#define DCC_LISTEN_BUTTON_PREFIX(custom_id_prefix_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_BUTTON_PREFIX((custom_id_prefix_), (handler_)))
#define DCC_LISTEN_BUTTON_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_BUTTON_PREFIX_DATA((custom_id_prefix_), (handler_), (user_data_)))
#define DCC_LISTEN_BUTTON_NS(namespace_, action_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_BUTTON_NS(namespace_, action_, (handler_)))
#define DCC_LISTEN_BUTTON_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_BUTTON_NS_DATA(namespace_, action_, (handler_), (user_data_)))
#define DCC_LISTEN_BUTTON_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_BUTTON_NS_PREFIX(namespace_, action_prefix_, (handler_)))
#define DCC_LISTEN_BUTTON_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_BUTTON_NS_PREFIX_DATA(namespace_, action_prefix_, (handler_), (user_data_)))
#define DCC_LISTEN_SELECT(custom_id_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_SELECT((custom_id_), (handler_)))
#define DCC_LISTEN_SELECT_DATA(custom_id_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_SELECT_DATA((custom_id_), (handler_), (user_data_)))
#define DCC_LISTEN_SELECT_PREFIX(custom_id_prefix_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_SELECT_PREFIX((custom_id_prefix_), (handler_)))
#define DCC_LISTEN_SELECT_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_SELECT_PREFIX_DATA((custom_id_prefix_), (handler_), (user_data_)))
#define DCC_LISTEN_SELECT_NS(namespace_, action_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_SELECT_NS(namespace_, action_, (handler_)))
#define DCC_LISTEN_SELECT_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_SELECT_NS_DATA(namespace_, action_, (handler_), (user_data_)))
#define DCC_LISTEN_SELECT_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_SELECT_NS_PREFIX(namespace_, action_prefix_, (handler_)))
#define DCC_LISTEN_SELECT_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_SELECT_NS_PREFIX_DATA(namespace_, action_prefix_, (handler_), (user_data_)))
#define DCC_LISTEN_MODAL(custom_id_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_MODAL((custom_id_), (handler_)))
#define DCC_LISTEN_MODAL_DATA(custom_id_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_MODAL_DATA((custom_id_), (handler_), (user_data_)))
#define DCC_LISTEN_MODAL_PREFIX(custom_id_prefix_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_MODAL_PREFIX((custom_id_prefix_), (handler_)))
#define DCC_LISTEN_MODAL_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_MODAL_PREFIX_DATA((custom_id_prefix_), (handler_), (user_data_)))
#define DCC_LISTEN_MODAL_NS(namespace_, action_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_MODAL_NS(namespace_, action_, (handler_)))
#define DCC_LISTEN_MODAL_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_MODAL_NS_DATA(namespace_, action_, (handler_), (user_data_)))
#define DCC_LISTEN_MODAL_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_MODAL_NS_PREFIX(namespace_, action_prefix_, (handler_)))
#define DCC_LISTEN_MODAL_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_MODAL_NS_PREFIX_DATA(namespace_, action_prefix_, (handler_), (user_data_)))

#define DCC_LISTEN_EVENT_ROUTE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_EVENT, \
        .event = (route_) \
    })
#define DCC_LISTEN_EVENT(type_, handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_EVENT((type_), (handler_)))
#define DCC_LISTEN_EVENT_DATA(type_, handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_EVENT_DATA((type_), (handler_), (user_data_)))
#define DCC_LISTEN_READY(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_READY((handler_)))
#define DCC_LISTEN_READY_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_READY_DATA((handler_), (user_data_)))
#define DCC_LISTEN_READY_ONCE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_READY_ONCE((handler_)))
#define DCC_LISTEN_READY_ONCE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_READY_ONCE_DATA((handler_), (user_data_)))
#define DCC_LISTEN_MESSAGE_CREATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MESSAGE_CREATE((handler_)))
#define DCC_LISTEN_MESSAGE_CREATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MESSAGE_CREATE_DATA((handler_), (user_data_)))
#define DCC_LISTEN_MESSAGE_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MESSAGE_UPDATE((handler_)))
#define DCC_LISTEN_MESSAGE_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MESSAGE_UPDATE_DATA((handler_), (user_data_)))
#define DCC_LISTEN_MESSAGE_DELETE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MESSAGE_DELETE((handler_)))
#define DCC_LISTEN_MESSAGE_DELETE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MESSAGE_DELETE_DATA((handler_), (user_data_)))

#define DCC_LISTEN_GUILD_CREATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_CREATE(handler_))
#define DCC_LISTEN_GUILD_CREATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_CREATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_GUILD_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_UPDATE(handler_))
#define DCC_LISTEN_GUILD_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_UPDATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_GUILD_DELETE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_DELETE(handler_))
#define DCC_LISTEN_GUILD_DELETE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_DELETE_DATA(handler_, (user_data_)))

#define DCC_LISTEN_MEMBER_JOIN(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MEMBER_JOIN(handler_))
#define DCC_LISTEN_MEMBER_JOIN_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MEMBER_JOIN_DATA(handler_, (user_data_)))
#define DCC_LISTEN_MEMBER_ADD(handler_) DCC_LISTEN_MEMBER_JOIN(handler_)
#define DCC_LISTEN_MEMBER_ADD_DATA(handler_, user_data_) \
    DCC_LISTEN_MEMBER_JOIN_DATA(handler_, (user_data_))
#define DCC_LISTEN_MEMBER_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MEMBER_UPDATE(handler_))
#define DCC_LISTEN_MEMBER_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MEMBER_UPDATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_MEMBER_REMOVE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MEMBER_REMOVE(handler_))
#define DCC_LISTEN_MEMBER_REMOVE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MEMBER_REMOVE_DATA(handler_, (user_data_)))

#define DCC_LISTEN_ROLE_CREATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_ROLE_CREATE(handler_))
#define DCC_LISTEN_ROLE_CREATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_ROLE_CREATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_ROLE_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_ROLE_UPDATE(handler_))
#define DCC_LISTEN_ROLE_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_ROLE_UPDATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_ROLE_DELETE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_ROLE_DELETE(handler_))
#define DCC_LISTEN_ROLE_DELETE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_ROLE_DELETE_DATA(handler_, (user_data_)))

#define DCC_LISTEN_CHANNEL_CREATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_CHANNEL_CREATE(handler_))
#define DCC_LISTEN_CHANNEL_CREATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_CHANNEL_CREATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_CHANNEL_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_CHANNEL_UPDATE(handler_))
#define DCC_LISTEN_CHANNEL_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_CHANNEL_UPDATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_CHANNEL_DELETE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_CHANNEL_DELETE(handler_))
#define DCC_LISTEN_CHANNEL_DELETE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_CHANNEL_DELETE_DATA(handler_, (user_data_)))

#define DCC_LISTEN_THREAD_CREATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_THREAD_CREATE(handler_))
#define DCC_LISTEN_THREAD_CREATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_THREAD_CREATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_THREAD_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_THREAD_UPDATE(handler_))
#define DCC_LISTEN_THREAD_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_THREAD_UPDATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_THREAD_DELETE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_THREAD_DELETE(handler_))
#define DCC_LISTEN_THREAD_DELETE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_THREAD_DELETE_DATA(handler_, (user_data_)))

#define DCC_LISTEN_REACTION_ADD(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_REACTION_ADD(handler_))
#define DCC_LISTEN_REACTION_ADD_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_REACTION_ADD_DATA(handler_, (user_data_)))
#define DCC_LISTEN_REACTION_REMOVE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_REACTION_REMOVE(handler_))
#define DCC_LISTEN_REACTION_REMOVE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_REACTION_REMOVE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_REACTION_CLEAR(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_REACTION_CLEAR(handler_))
#define DCC_LISTEN_REACTION_CLEAR_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_REACTION_CLEAR_DATA(handler_, (user_data_)))
#define DCC_LISTEN_REACTION_EMOJI_CLEAR(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_REACTION_EMOJI_CLEAR(handler_))
#define DCC_LISTEN_REACTION_EMOJI_CLEAR_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_REACTION_EMOJI_CLEAR_DATA(handler_, (user_data_)))

#define DCC_LISTEN_POLL_VOTE_ADD(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_POLL_VOTE_ADD(handler_))
#define DCC_LISTEN_POLL_VOTE_ADD_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_POLL_VOTE_ADD_DATA(handler_, (user_data_)))
#define DCC_LISTEN_POLL_VOTE_REMOVE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_POLL_VOTE_REMOVE(handler_))
#define DCC_LISTEN_POLL_VOTE_REMOVE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_POLL_VOTE_REMOVE_DATA(handler_, (user_data_)))

#define DCC_LISTEN_VOICE_STATE_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_VOICE_STATE_UPDATE(handler_))
#define DCC_LISTEN_VOICE_STATE_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_VOICE_STATE_UPDATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_TYPING_START(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_TYPING_START(handler_))
#define DCC_LISTEN_TYPING_START_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_TYPING_START_DATA(handler_, (user_data_)))
#define DCC_LISTEN_PRESENCE_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_PRESENCE_UPDATE(handler_))
#define DCC_LISTEN_PRESENCE_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_PRESENCE_UPDATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_USER_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_USER_UPDATE(handler_))
#define DCC_LISTEN_USER_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_USER_UPDATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_GUILD_BAN_ADD(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_BAN_ADD(handler_))
#define DCC_LISTEN_GUILD_BAN_ADD_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_BAN_ADD_DATA(handler_, (user_data_)))
#define DCC_LISTEN_GUILD_BAN_REMOVE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_BAN_REMOVE(handler_))
#define DCC_LISTEN_GUILD_BAN_REMOVE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_BAN_REMOVE_DATA(handler_, (user_data_)))

#define DCC_LISTEN_MESSAGE_COMMAND_ROUTE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_MESSAGE_COMMAND, \
        .message_command = (route_) \
    })
#define DCC_LISTEN_MESSAGE_COMMAND(prefix_, name_, handler_) \
    DCC_LISTEN_MESSAGE_COMMAND_ROUTE(DCC_MESSAGE_COMMAND((prefix_), (name_), (handler_)))
#define DCC_LISTEN_MESSAGE_COMMAND_DATA(prefix_, name_, handler_, user_data_) \
    DCC_LISTEN_MESSAGE_COMMAND_ROUTE(DCC_MESSAGE_COMMAND_DATA((prefix_), (name_), (handler_), (user_data_)))
#define DCC_LISTEN_PREFIX_COMMAND(prefix_, name_, handler_) \
    DCC_LISTEN_MESSAGE_COMMAND((prefix_), (name_), (handler_))
#define DCC_LISTEN_PREFIX_COMMAND_DATA(prefix_, name_, handler_, user_data_) \
    DCC_LISTEN_MESSAGE_COMMAND_DATA((prefix_), (name_), (handler_), (user_data_))

#define DCC_LISTEN_TASK_ROUTE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_TASK, \
        .task = (route_) \
    })
#define DCC_LISTEN_TASK_MS(interval_ms_, handler_) \
    DCC_LISTEN_TASK_ROUTE(DCC_TASK_LOOP_MS((interval_ms_), (handler_)))
#define DCC_LISTEN_TASK_MS_DATA(interval_ms_, handler_, user_data_) \
    DCC_LISTEN_TASK_ROUTE(DCC_TASK_LOOP_MS_DATA((interval_ms_), (handler_), (user_data_)))
#define DCC_LISTEN_TASK_SECONDS(interval_seconds_, handler_) \
    DCC_LISTEN_TASK_ROUTE(DCC_TASK_LOOP_SECONDS((interval_seconds_), (handler_)))
#define DCC_LISTEN_TASK_SECONDS_DATA(interval_seconds_, handler_, user_data_) \
    DCC_LISTEN_TASK_ROUTE(DCC_TASK_LOOP_SECONDS_DATA((interval_seconds_), (handler_), (user_data_)))
#define DCC_LISTEN_TASK_DAILY_KST(hhmm_, handler_) \
    DCC_LISTEN_TASK_ROUTE(DCC_TASK_DAILY_KST((hhmm_), (handler_)))
#define DCC_LISTEN_TASK_DAILY_KST_DATA(hhmm_, handler_, user_data_) \
    DCC_LISTEN_TASK_ROUTE(DCC_TASK_DAILY_KST_DATA((hhmm_), (handler_), (user_data_)))


#endif
