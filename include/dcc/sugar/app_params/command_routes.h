#ifndef DCC_SUGAR_APP_PARAMS_COMMAND_ROUTES_H
#define DCC_SUGAR_APP_PARAMS_COMMAND_ROUTES_H

#include <dcc/sugar/app_params/base.h>

#define DCC_ON_SLASH_PARAMS_DATA_POLICY(name_, description_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_TYPED_SLASH_OPTIONS_DATA_POLICY( \
        (name_), (description_), args_type_, (handler_), (user_data_), \
        DCC_SLASH_PARAMS_BINDINGS(args_type_, params_), DCC_NO_VALIDATORS(), (policy_), \
        params_(DCC_SLASH_PARAM_OPTION, args_type_) \
    )
#define DCC_ON_SLASH_PARAMS_DATA_GUARDED(name_, description_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_SLASH_PARAMS_DATA_POLICY( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_ON_SLASH_PARAMS_FN_DATA_POLICY(handler_, description_, args_type_, user_data_, params_, policy_) \
    DCC_ON_SLASH_PARAMS_DATA_POLICY( \
        #handler_, (description_), args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_SLASH_PARAMS_FN_DATA_GUARDED(handler_, description_, args_type_, user_data_, params_, ...) \
    DCC_ON_SLASH_PARAMS_FN_DATA_POLICY( \
        handler_, (description_), args_type_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_SUBCOMMAND_PARAMS_DATA_POLICY(command_name_, subcommand_path_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_TYPED_SUBCOMMAND_POLICY( \
        (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), \
        DCC_SLASH_PARAMS_BINDINGS(args_type_, params_), DCC_NO_VALIDATORS(), (policy_) \
    )
#define DCC_ON_SUBCOMMAND_PARAMS_DATA_GUARDED(command_name_, subcommand_path_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_POLICY( \
        (command_name_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        params_, \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_ON_SUBCOMMAND_PARAMS_FN_DATA_POLICY(command_name_, handler_, args_type_, user_data_, params_, policy_) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_POLICY( \
        (command_name_), #handler_, args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_SUBCOMMAND_PARAMS_FN_DATA_GUARDED(command_name_, handler_, args_type_, user_data_, params_, ...) \
    DCC_ON_SUBCOMMAND_PARAMS_FN_DATA_POLICY( \
        (command_name_), handler_, args_type_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_TYPED_SLASH_BUILDER_DATA_POLICY(command_, args_type_, handler_, user_data_, bindings_, validators_, policy_) \
    DCC_TYPED_SLASH_POLICY((command_), args_type_, (handler_), (user_data_), (bindings_), (validators_), (policy_))
#define DCC_ON_TYPED_SLASH_BUILDER_DATA_GUARDED(command_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    DCC_ON_TYPED_SLASH_BUILDER_DATA_POLICY( \
        (command_), args_type_, (handler_), (user_data_), (bindings_), (validators_), DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_AUTOCOMPLETE_PARAMS_DATA_POLICY(command_name_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_TYPED_AUTOCOMPLETE_POLICY( \
        (command_name_), args_type_, (handler_), (user_data_), \
        DCC_SLASH_PARAMS_BINDINGS(args_type_, params_), DCC_NO_VALIDATORS(), (policy_) \
    )
#define DCC_ON_AUTOCOMPLETE_PARAMS_DATA_GUARDED(command_name_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_AUTOCOMPLETE_PARAMS_DATA_POLICY( \
        (command_name_), args_type_, (handler_), (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_ON_AUTOCOMPLETE_PARAMS_FN_DATA_POLICY(handler_, args_type_, user_data_, params_, policy_) \
    DCC_ON_AUTOCOMPLETE_PARAMS_DATA_POLICY( \
        #handler_, args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_AUTOCOMPLETE_PARAMS_FN_DATA_GUARDED(handler_, args_type_, user_data_, params_, ...) \
    DCC_ON_AUTOCOMPLETE_PARAMS_FN_DATA_POLICY( \
        handler_, args_type_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#endif /* DCC_SUGAR_APP_PARAMS_COMMAND_ROUTES_H */
