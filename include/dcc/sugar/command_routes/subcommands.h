#ifndef DCC_SUGAR_COMMAND_ROUTES_SUBCOMMANDS_H
#define DCC_SUGAR_COMMAND_ROUTES_SUBCOMMANDS_H

#include <dcc/sugar/command_routes/typed.h>

#define DCC_APP_SUBCOMMAND_ROUTE(app_, command_name_, subcommand_path_, args_type_, handler_, params_) \
    DCC_APP_TYPED_SUBCOMMAND( \
        (app_), DCC_SUBCOMMAND_ROUTE((command_name_), (subcommand_path_), args_type_, (handler_), params_) \
    )
#define DCC_APP_SUBCOMMAND_ROUTE_DATA(app_, command_name_, subcommand_path_, args_type_, handler_, user_data_, params_) \
    DCC_APP_TYPED_SUBCOMMAND( \
        (app_), \
        DCC_SUBCOMMAND_ROUTE_DATA((command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_APP_SUBCOMMAND_ROUTE_FN(app_, command_name_, handler_, args_type_, params_) \
    DCC_APP_TYPED_SUBCOMMAND((app_), DCC_SUBCOMMAND_ROUTE_FN((command_name_), handler_, args_type_, params_))
#define DCC_APP_SUBCOMMAND_ROUTE_FN_DATA(app_, command_name_, handler_, args_type_, user_data_, params_) \
    DCC_APP_TYPED_SUBCOMMAND( \
        (app_), DCC_SUBCOMMAND_ROUTE_FN_DATA((command_name_), handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_SUBCOMMAND_ROUTE_FN_DATA_GUARDED(app_, command_name_, handler_, args_type_, user_data_, params_, ...) \
    DCC_APP_TYPED_SUBCOMMAND( \
        (app_), \
        DCC_SUBCOMMAND_ROUTE_FN_DATA_GUARDED((command_name_), handler_, args_type_, (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_APP_SUBCOMMAND_ROUTE_IN_FN_DATA( \
    app_, command_name_, group_, handler_, args_type_, user_data_, params_ \
) \
    DCC_APP_TYPED_SUBCOMMAND( \
        (app_), \
        DCC_SUBCOMMAND_ROUTE_IN_FN_DATA((command_name_), group_, handler_, args_type_, (user_data_), params_) \
    )

#endif /* DCC_SUGAR_COMMAND_ROUTES_SUBCOMMANDS_H */
