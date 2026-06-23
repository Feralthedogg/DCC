#ifndef DCC_SUGAR_APP_TYPED_SUBCOMMANDS_AND_AUTOCOMPLETE_SUBCOMMAND_PARAMS_H
#define DCC_SUGAR_APP_TYPED_SUBCOMMANDS_AND_AUTOCOMPLETE_SUBCOMMAND_PARAMS_H

#include <dcc/sugar/app_typed/subcommands_and_autocomplete/typed_subcommands.h>

#define DCC_ON_SUBCOMMAND_PARAMS_DATA_VALIDATED( \
    command_name_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    params_, \
    validators_ \
) \
    DCC_TYPED_SUBCOMMAND( \
        (command_name_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        DCC_SLASH_PARAMS_BINDINGS(args_type_, params_), \
        (validators_) \
    )

#define DCC_ON_SUBCOMMAND_PARAMS_VALIDATED( \
    command_name_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    params_, \
    validators_ \
) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_VALIDATED( \
        (command_name_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        NULL, \
        params_, \
        (validators_) \
    )

#define DCC_ON_SUBCOMMAND_PARAMS_DATA(command_name_, subcommand_path_, args_type_, handler_, user_data_, params_) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_VALIDATED( \
        (command_name_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )

#define DCC_ON_SUBCOMMAND_PARAMS(command_name_, subcommand_path_, args_type_, handler_, params_) \
    DCC_ON_SUBCOMMAND_PARAMS_VALIDATED( \
        (command_name_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )
#define DCC_ON_SUBCOMMAND_PARAMS_FN_DATA_VALIDATED( \
    command_name_, \
    handler_, \
    args_type_, \
    user_data_, \
    params_, \
    validators_ \
) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_VALIDATED( \
        (command_name_), \
        #handler_, \
        args_type_, \
        (handler_), \
        (user_data_), \
        params_, \
        (validators_) \
    )
#define DCC_ON_SUBCOMMAND_PARAMS_FN_VALIDATED(command_name_, handler_, args_type_, params_, validators_) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_VALIDATED( \
        (command_name_), \
        #handler_, \
        args_type_, \
        (handler_), \
        NULL, \
        params_, \
        (validators_) \
    )
#define DCC_ON_SUBCOMMAND_PARAMS_FN_DATA(command_name_, handler_, args_type_, user_data_, params_) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_VALIDATED( \
        (command_name_), \
        #handler_, \
        args_type_, \
        (handler_), \
        (user_data_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )
#define DCC_ON_SUBCOMMAND_PARAMS_FN(command_name_, handler_, args_type_, params_) \
    DCC_ON_SUBCOMMAND_PARAMS_VALIDATED( \
        (command_name_), \
        #handler_, \
        args_type_, \
        (handler_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )

#endif /* DCC_SUGAR_APP_TYPED_SUBCOMMANDS_AND_AUTOCOMPLETE_SUBCOMMAND_PARAMS_H */
