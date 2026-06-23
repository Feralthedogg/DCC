#ifndef DCC_SUGAR_COMMAND_ROUTES_COMPONENTS_H
#define DCC_SUGAR_COMMAND_ROUTES_COMPONENTS_H

#include <dcc/sugar/command_routes/base.h>

#define DCC_APP_ON_USER_CONTEXT_MENU(app_, name_, handler_) \
    dcc_app_user_context_menu((app_), (name_), (handler_), NULL)
#define DCC_APP_ON_USER_CONTEXT_MENU_DATA(app_, name_, handler_, user_data_) \
    dcc_app_user_context_menu((app_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU(app_, name_, handler_) \
    dcc_app_message_context_menu((app_), (name_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_DATA(app_, name_, handler_, user_data_) \
    dcc_app_message_context_menu((app_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_USER_CONTEXT_MENU_FN(app_, handler_) \
    DCC_APP_ON_USER_CONTEXT_MENU((app_), #handler_, (handler_))
#define DCC_APP_ON_USER_CONTEXT_MENU_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_USER_CONTEXT_MENU_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_FN(app_, handler_) \
    DCC_APP_ON_MESSAGE_CONTEXT_MENU((app_), #handler_, (handler_))
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_CONTEXT_MENU_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_BUTTON(app_, custom_id_, handler_) \
    dcc_app_button((app_), (custom_id_), (handler_), NULL)
#define DCC_APP_ON_BUTTON_DATA(app_, custom_id_, handler_, user_data_) \
    dcc_app_button((app_), (custom_id_), (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_PREFIX(app_, custom_id_prefix_, handler_) \
    dcc_app_button_prefix((app_), (custom_id_prefix_), (handler_), NULL)
#define DCC_APP_ON_BUTTON_PREFIX_DATA(app_, custom_id_prefix_, handler_, user_data_) \
    dcc_app_button_prefix((app_), (custom_id_prefix_), (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_FN(app_, handler_) \
    DCC_APP_ON_BUTTON((app_), #handler_, (handler_))
#define DCC_APP_ON_BUTTON_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_PREFIX_FN(app_, handler_) \
    DCC_APP_ON_BUTTON_PREFIX((app_), #handler_, (handler_))
#define DCC_APP_ON_BUTTON_PREFIX_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_PREFIX_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_NS(app_, namespace_, action_, handler_) \
    DCC_APP_ON_BUTTON((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_APP_ON_BUTTON_NS_DATA(app_, namespace_, action_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_DATA((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_NS_PREFIX(app_, namespace_, action_prefix_, handler_) \
    DCC_APP_ON_BUTTON_PREFIX((app_), DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_APP_ON_BUTTON_NS_PREFIX_DATA(app_, namespace_, action_prefix_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_PREFIX_DATA( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_) \
    )
#define DCC_APP_ON_BUTTON_NS_FN(app_, namespace_, handler_) \
    DCC_APP_ON_BUTTON_NS((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_BUTTON_NS_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_NS_DATA((app_), namespace_, #handler_, (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_NS_PREFIX_FN(app_, namespace_, handler_) \
    DCC_APP_ON_BUTTON_NS_PREFIX((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_BUTTON_NS_PREFIX_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_NS_PREFIX_DATA((app_), namespace_, #handler_, (handler_), (user_data_))
#define DCC_APP_ON_SELECT(app_, custom_id_, handler_) \
    dcc_app_select((app_), (custom_id_), (handler_), NULL)
#define DCC_APP_ON_SELECT_DATA(app_, custom_id_, handler_, user_data_) \
    dcc_app_select((app_), (custom_id_), (handler_), (user_data_))
#define DCC_APP_ON_SELECT_PREFIX(app_, custom_id_prefix_, handler_) \
    dcc_app_select_prefix((app_), (custom_id_prefix_), (handler_), NULL)
#define DCC_APP_ON_SELECT_PREFIX_DATA(app_, custom_id_prefix_, handler_, user_data_) \
    dcc_app_select_prefix((app_), (custom_id_prefix_), (handler_), (user_data_))
#define DCC_APP_ON_SELECT_FN(app_, handler_) \
    DCC_APP_ON_SELECT((app_), #handler_, (handler_))
#define DCC_APP_ON_SELECT_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_SELECT_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_SELECT_PREFIX_FN(app_, handler_) \
    DCC_APP_ON_SELECT_PREFIX((app_), #handler_, (handler_))
#define DCC_APP_ON_SELECT_PREFIX_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_SELECT_PREFIX_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_SELECT_NS(app_, namespace_, action_, handler_) \
    DCC_APP_ON_SELECT((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_APP_ON_SELECT_NS_DATA(app_, namespace_, action_, handler_, user_data_) \
    DCC_APP_ON_SELECT_DATA((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))
#define DCC_APP_ON_SELECT_NS_PREFIX(app_, namespace_, action_prefix_, handler_) \
    DCC_APP_ON_SELECT_PREFIX((app_), DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_APP_ON_SELECT_NS_PREFIX_DATA(app_, namespace_, action_prefix_, handler_, user_data_) \
    DCC_APP_ON_SELECT_PREFIX_DATA( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_) \
    )
#define DCC_APP_ON_SELECT_NS_FN(app_, namespace_, handler_) \
    DCC_APP_ON_SELECT_NS((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_SELECT_NS_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_SELECT_NS_DATA((app_), namespace_, #handler_, (handler_), (user_data_))
#define DCC_APP_ON_SELECT_NS_PREFIX_FN(app_, namespace_, handler_) \
    DCC_APP_ON_SELECT_NS_PREFIX((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_SELECT_NS_PREFIX_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_SELECT_NS_PREFIX_DATA((app_), namespace_, #handler_, (handler_), (user_data_))
#define DCC_APP_ON_MODAL(app_, custom_id_, handler_) \
    dcc_app_modal((app_), (custom_id_), (handler_), NULL)
#define DCC_APP_ON_MODAL_DATA(app_, custom_id_, handler_, user_data_) \
    dcc_app_modal((app_), (custom_id_), (handler_), (user_data_))
#define DCC_APP_ON_MODAL_PREFIX(app_, custom_id_prefix_, handler_) \
    dcc_app_modal_prefix((app_), (custom_id_prefix_), (handler_), NULL)
#define DCC_APP_ON_MODAL_PREFIX_DATA(app_, custom_id_prefix_, handler_, user_data_) \
    dcc_app_modal_prefix((app_), (custom_id_prefix_), (handler_), (user_data_))
#define DCC_APP_ON_MODAL_FN(app_, handler_) \
    DCC_APP_ON_MODAL((app_), #handler_, (handler_))
#define DCC_APP_ON_MODAL_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MODAL_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_MODAL_PREFIX_FN(app_, handler_) \
    DCC_APP_ON_MODAL_PREFIX((app_), #handler_, (handler_))
#define DCC_APP_ON_MODAL_PREFIX_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MODAL_PREFIX_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_MODAL_NS(app_, namespace_, action_, handler_) \
    DCC_APP_ON_MODAL((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_APP_ON_MODAL_NS_DATA(app_, namespace_, action_, handler_, user_data_) \
    DCC_APP_ON_MODAL_DATA((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))
#define DCC_APP_ON_MODAL_NS_PREFIX(app_, namespace_, action_prefix_, handler_) \
    DCC_APP_ON_MODAL_PREFIX((app_), DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_APP_ON_MODAL_NS_PREFIX_DATA(app_, namespace_, action_prefix_, handler_, user_data_) \
    DCC_APP_ON_MODAL_PREFIX_DATA( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_) \
    )
#define DCC_APP_ON_MODAL_NS_FN(app_, namespace_, handler_) \
    DCC_APP_ON_MODAL_NS((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_MODAL_NS_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_MODAL_NS_DATA((app_), namespace_, #handler_, (handler_), (user_data_))
#define DCC_APP_ON_MODAL_NS_PREFIX_FN(app_, namespace_, handler_) \
    DCC_APP_ON_MODAL_NS_PREFIX((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_MODAL_NS_PREFIX_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_MODAL_NS_PREFIX_DATA((app_), namespace_, #handler_, (handler_), (user_data_))

#endif /* DCC_SUGAR_COMMAND_ROUTES_COMPONENTS_H */
