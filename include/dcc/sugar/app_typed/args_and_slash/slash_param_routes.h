#ifndef DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_ROUTES_H
#define DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_ROUTES_H

#include <dcc/sugar/app_typed/args_and_slash/slash_param_commands.h>

#define DCC_ON_SLASH_PARAMS_DATA_VALIDATED( \
    name_, \
    description_, \
    args_type_, \
    handler_, \
    user_data_, \
    params_, \
    validators_ \
) \
    ((dcc_app_typed_slash_command_t){ \
        .size = sizeof(dcc_app_typed_slash_command_t), \
        .command = &((dcc_application_command_builder_t[]){ \
            DCC_SLASH_COMMAND_PARAMS((name_), (description_), args_type_, params_) \
        })[0], \
        .args_size = sizeof(args_type_), \
        .bindings = DCC_SLASH_PARAMS_BINDINGS(args_type_, params_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_ON_SLASH_PARAMS_VALIDATED(name_, description_, args_type_, handler_, params_, validators_) \
    DCC_ON_SLASH_PARAMS_DATA_VALIDATED( \
        (name_), \
        (description_), \
        args_type_, \
        (handler_), \
        NULL, \
        params_, \
        (validators_) \
    )

#define DCC_ON_SLASH_PARAMS_DATA(name_, description_, args_type_, handler_, user_data_, params_) \
    DCC_ON_SLASH_PARAMS_DATA_VALIDATED( \
        (name_), \
        (description_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )

#define DCC_ON_SLASH_PARAMS(name_, description_, args_type_, handler_, params_) \
    DCC_ON_SLASH_PARAMS_VALIDATED( \
        (name_), \
        (description_), \
        args_type_, \
        (handler_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )
#define DCC_ON_SLASH_PARAMS_FN_DATA_VALIDATED( \
    handler_, \
    description_, \
    args_type_, \
    user_data_, \
    params_, \
    validators_ \
) \
    DCC_ON_SLASH_PARAMS_DATA_VALIDATED( \
        #handler_, \
        (description_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        params_, \
        (validators_) \
    )
#define DCC_ON_SLASH_PARAMS_FN_VALIDATED(handler_, description_, args_type_, params_, validators_) \
    DCC_ON_SLASH_PARAMS_DATA_VALIDATED( \
        #handler_, \
        (description_), \
        args_type_, \
        (handler_), \
        NULL, \
        params_, \
        (validators_) \
    )
#define DCC_ON_SLASH_PARAMS_FN_DATA(handler_, description_, args_type_, user_data_, params_) \
    DCC_ON_SLASH_PARAMS_DATA_VALIDATED( \
        #handler_, \
        (description_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )
#define DCC_ON_SLASH_PARAMS_FN(handler_, description_, args_type_, params_) \
    DCC_ON_SLASH_PARAMS_VALIDATED( \
        #handler_, \
        (description_), \
        args_type_, \
        (handler_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )

#endif /* DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_ROUTES_H */
