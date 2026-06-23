#ifndef DCC_SUGAR_GUILD_SHORTCUTS_SCHEDULED_EVENTS_H
#define DCC_SUGAR_GUILD_SHORTCUTS_SCHEDULED_EVENTS_H

#include <dcc/sugar/guild_shortcuts/base.h>

#define DCC_APP_GET_GUILD_SCHEDULED_EVENTS(app_, guild_id_) \
    dcc_app_get_guild_scheduled_events((app_), (guild_id_), NULL, NULL, NULL)
#define DCC_APP_GET_GUILD_SCHEDULED_EVENTS_QUERY(app_, guild_id_, query_) \
    dcc_app_get_guild_scheduled_events((app_), (guild_id_), (query_), NULL, NULL)
#define DCC_APP_GET_GUILD_SCHEDULED_EVENTS_WITH_USER_COUNT(app_, guild_id_) \
    dcc_app_get_guild_scheduled_events_with_user_count((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_SCHEDULED_EVENT(app_, guild_id_, event_id_) \
    dcc_app_get_guild_scheduled_event((app_), (guild_id_), (event_id_), NULL, NULL, NULL)
#define DCC_APP_GET_GUILD_SCHEDULED_EVENT_QUERY(app_, guild_id_, event_id_, query_) \
    dcc_app_get_guild_scheduled_event((app_), (guild_id_), (event_id_), (query_), NULL, NULL)
#define DCC_APP_GET_GUILD_SCHEDULED_EVENT_WITH_USER_COUNT(app_, guild_id_, event_id_) \
    dcc_app_get_guild_scheduled_event_with_user_count((app_), (guild_id_), (event_id_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_SCHEDULED_EVENT(app_, guild_id_, json_body_) \
    dcc_app_create_guild_scheduled_event((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_SCHEDULED_EVENT_PARAMS(app_, params_) \
    dcc_app_create_guild_scheduled_event_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_SCHEDULED_EVENT(app_, guild_id_, event_id_, json_body_) \
    dcc_app_modify_guild_scheduled_event((app_), (guild_id_), (event_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_SCHEDULED_EVENT_PARAMS(app_, params_) \
    dcc_app_modify_guild_scheduled_event_params((app_), (params_), NULL, NULL)
#define DCC_APP_DELETE_GUILD_SCHEDULED_EVENT(app_, guild_id_, event_id_) \
    dcc_app_delete_guild_scheduled_event((app_), (guild_id_), (event_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_SCHEDULED_EVENT_USERS(app_, guild_id_, event_id_) \
    dcc_app_get_guild_scheduled_event_users((app_), (guild_id_), (event_id_), NULL, NULL, NULL)
#define DCC_APP_GET_GUILD_SCHEDULED_EVENT_USERS_QUERY(app_, guild_id_, event_id_, query_) \
    dcc_app_get_guild_scheduled_event_users((app_), (guild_id_), (event_id_), (query_), NULL, NULL)
#define DCC_APP_GET_GUILD_SCHEDULED_EVENT_USERS_PAGE(app_, guild_id_, event_id_, limit_, before_, after_) \
    dcc_app_get_guild_scheduled_event_users_page( \
        (app_), \
        (guild_id_), \
        (event_id_), \
        (limit_), \
        (before_), \
        (after_), \
        NULL, \
        NULL \
    )
#define DCC_GUILD_SCHEDULED_EVENTS_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_SCHEDULED_EVENTS((app_), (guild_id_))
#define DCC_GUILD_SCHEDULED_EVENTS_FETCH_WITH_USER_COUNT(app_, guild_id_) \
    DCC_APP_GET_GUILD_SCHEDULED_EVENTS_WITH_USER_COUNT((app_), (guild_id_))
#define DCC_SCHEDULED_EVENT_FETCH(app_, guild_id_, event_id_) \
    DCC_APP_GET_GUILD_SCHEDULED_EVENT((app_), (guild_id_), (event_id_))
#define DCC_SCHEDULED_EVENT_FETCH_WITH_USER_COUNT(app_, guild_id_, event_id_) \
    DCC_APP_GET_GUILD_SCHEDULED_EVENT_WITH_USER_COUNT((app_), (guild_id_), (event_id_))
#define DCC_SCHEDULED_EVENT_CREATE(app_, guild_id_, json_body_) \
    DCC_APP_CREATE_GUILD_SCHEDULED_EVENT((app_), (guild_id_), (json_body_))
#define DCC_SCHEDULED_EVENT_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_GUILD_SCHEDULED_EVENT_PARAMS((app_), (params_))
#define DCC_SCHEDULED_EVENT_EDIT(app_, guild_id_, event_id_, json_body_) \
    DCC_APP_EDIT_GUILD_SCHEDULED_EVENT((app_), (guild_id_), (event_id_), (json_body_))
#define DCC_SCHEDULED_EVENT_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_GUILD_SCHEDULED_EVENT_PARAMS((app_), (params_))
#define DCC_SCHEDULED_EVENT_DELETE(app_, guild_id_, event_id_) \
    DCC_APP_DELETE_GUILD_SCHEDULED_EVENT((app_), (guild_id_), (event_id_))
#define DCC_SCHEDULED_EVENT_USERS_FETCH(app_, guild_id_, event_id_) \
    DCC_APP_GET_GUILD_SCHEDULED_EVENT_USERS((app_), (guild_id_), (event_id_))
#define DCC_SCHEDULED_EVENT_USERS_FETCH_PAGE(app_, guild_id_, event_id_, limit_, before_, after_) \
    DCC_APP_GET_GUILD_SCHEDULED_EVENT_USERS_PAGE((app_), (guild_id_), (event_id_), (limit_), (before_), (after_))

#endif /* DCC_SUGAR_GUILD_SHORTCUTS_SCHEDULED_EVENTS_H */
