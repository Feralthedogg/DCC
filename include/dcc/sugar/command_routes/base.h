#ifndef DCC_SUGAR_COMMAND_ROUTES_BASE_H
#define DCC_SUGAR_COMMAND_ROUTES_BASE_H

#include <dcc/app.h>
#include <dcc/sugar/app_extension/components.h>
#include <dcc/sugar/app_params.h>
#include <dcc/sugar/commands.h>
#include <dcc/sugar/core_options.h>

/* Included by <dcc/sugar.h>. */
#define DCC_APP_ON_SLASH(app_, name_, description_, handler_) \
    dcc_app_slash((app_), (name_), (description_), (handler_), NULL)
#define DCC_APP_ON_SLASH_DATA(app_, name_, description_, handler_, user_data_) \
    dcc_app_slash((app_), (name_), (description_), (handler_), (user_data_))
#define DCC_APP_ON_SLASH_BUILDER(app_, builder_, handler_) \
    dcc_app_slash_builder((app_), (builder_), (handler_), NULL)
#define DCC_APP_ON_SLASH_BUILDER_DATA(app_, builder_, handler_, user_data_) \
    dcc_app_slash_builder((app_), (builder_), (handler_), (user_data_))
#define DCC_APP_ON_COMMAND(app_, name_, description_, handler_) \
    DCC_APP_ON_SLASH((app_), (name_), (description_), (handler_))
#define DCC_APP_ON_COMMAND_DATA(app_, name_, description_, handler_, user_data_) \
    DCC_APP_ON_SLASH_DATA((app_), (name_), (description_), (handler_), (user_data_))
#define DCC_APP_ON_COMMAND_BUILDER(app_, builder_, handler_) \
    DCC_APP_ON_SLASH_BUILDER((app_), (builder_), (handler_))
#define DCC_APP_ON_COMMAND_BUILDER_DATA(app_, builder_, handler_, user_data_) \
    DCC_APP_ON_SLASH_BUILDER_DATA((app_), (builder_), (handler_), (user_data_))
#define DCC_APP_ON_SLASH_FN(app_, handler_, description_) \
    DCC_APP_ON_SLASH((app_), #handler_, (description_), (handler_))
#define DCC_APP_ON_SLASH_FN_DATA(app_, handler_, description_, user_data_) \
    DCC_APP_ON_SLASH_DATA((app_), #handler_, (description_), (handler_), (user_data_))
#define DCC_APP_ON_COMMAND_FN(app_, handler_, description_) \
    DCC_APP_ON_SLASH_FN((app_), handler_, (description_))
#define DCC_APP_ON_COMMAND_FN_DATA(app_, handler_, description_, user_data_) \
    DCC_APP_ON_SLASH_FN_DATA((app_), handler_, (description_), (user_data_))
#define DCC_APP_ON_SUBCOMMAND(app_, command_name_, subcommand_path_, handler_) \
    dcc_app_subcommand((app_), (command_name_), (subcommand_path_), (handler_), NULL)
#define DCC_APP_ON_SUBCOMMAND_DATA(app_, command_name_, subcommand_path_, handler_, user_data_) \
    dcc_app_subcommand((app_), (command_name_), (subcommand_path_), (handler_), (user_data_))
#define DCC_APP_ON_AUTOCOMPLETE(app_, name_, handler_) \
    dcc_app_autocomplete((app_), (name_), (handler_), NULL)
#define DCC_APP_ON_AUTOCOMPLETE_DATA(app_, name_, handler_, user_data_) \
    dcc_app_autocomplete((app_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_AUTOCOMPLETE_FN(app_, handler_) \
    DCC_APP_ON_AUTOCOMPLETE((app_), #handler_, (handler_))
#define DCC_APP_ON_AUTOCOMPLETE_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_AUTOCOMPLETE_DATA((app_), #handler_, (handler_), (user_data_))

#endif /* DCC_SUGAR_COMMAND_ROUTES_BASE_H */
