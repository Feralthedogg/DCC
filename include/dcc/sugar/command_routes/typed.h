#ifndef DCC_SUGAR_COMMAND_ROUTES_TYPED_H
#define DCC_SUGAR_COMMAND_ROUTES_TYPED_H

#include <dcc/sugar/command_routes/base.h>

#define DCC_APP_TYPED_SLASH(app_, route_) \
    dcc_app_slash_typed((app_), DCC_SUGAR_PTR(dcc_app_typed_slash_command_t, (route_)))
#define DCC_APP_TYPED_SUBCOMMAND(app_, route_) \
    dcc_app_subcommand_typed((app_), DCC_SUGAR_PTR(dcc_app_typed_subcommand_t, (route_)))
#define DCC_APP_TYPED_AUTOCOMPLETE(app_, route_) \
    dcc_app_autocomplete_typed((app_), DCC_SUGAR_PTR(dcc_app_typed_autocomplete_t, (route_)))
#define DCC_APP_TYPED_MODAL(app_, route_) \
    dcc_app_modal_typed((app_), DCC_SUGAR_PTR(dcc_app_typed_modal_t, (route_)))
#define DCC_APP_MODAL_FLOW(app_, flow_) \
    dcc_app_modal_flow_register((app_), DCC_SUGAR_PTR(dcc_app_modal_flow_t, (flow_)))
#define DCC_APP_TYPED_BUTTON(app_, route_) \
    dcc_app_button_typed((app_), DCC_SUGAR_PTR(dcc_app_typed_component_t, (route_)))
#define DCC_APP_TYPED_SELECT(app_, route_) \
    dcc_app_select_typed((app_), DCC_SUGAR_PTR(dcc_app_typed_component_t, (route_)))
#define DCC_APP_REGISTER_SLASH(app_, route_) \
    DCC_APP_TYPED_SLASH((app_), (route_))
#define DCC_APP_REGISTER_COMMAND(app_, route_) \
    DCC_APP_TYPED_SLASH((app_), (route_))
#define DCC_APP_REGISTER_SUBCOMMAND(app_, route_) \
    DCC_APP_TYPED_SUBCOMMAND((app_), (route_))
#define DCC_APP_REGISTER_AUTOCOMPLETE(app_, route_) \
    DCC_APP_TYPED_AUTOCOMPLETE((app_), (route_))
#define DCC_APP_REGISTER_MODAL(app_, route_) \
    DCC_APP_TYPED_MODAL((app_), (route_))
#define DCC_APP_REGISTER_MODAL_FLOW(app_, flow_) \
    DCC_APP_MODAL_FLOW((app_), (flow_))
#define DCC_APP_REGISTER_BUTTON(app_, route_) \
    DCC_APP_TYPED_BUTTON((app_), (route_))
#define DCC_APP_REGISTER_SELECT(app_, route_) \
    DCC_APP_TYPED_SELECT((app_), (route_))

#endif /* DCC_SUGAR_COMMAND_ROUTES_TYPED_H */
