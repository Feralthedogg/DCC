#ifndef DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_TYPED_SLASH_BUILDERS_H
#define DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_TYPED_SLASH_BUILDERS_H

#include <dcc/sugar/app_typed/args_and_slash/typed_slash_core.h>

#define DCC_TYPED_SLASH_NO_OPTIONS_DATA(name_, description_, args_type_, handler_, user_data_, bindings_, validators_) \
    ((dcc_app_typed_slash_command_t){ \
        .size = sizeof(dcc_app_typed_slash_command_t), \
        .command = &((dcc_application_command_builder_t[]){ \
            { \
                .name = (name_), \
                .description = (description_), \
                .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
                .has_name = 1U, \
                .has_description = 1U, \
                .has_type = 1U \
            } \
        })[0], \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_TYPED_SLASH_NO_OPTIONS_DATA_POLICY( \
    name_, \
    description_, \
    args_type_, \
    handler_, \
    user_data_, \
    bindings_, \
    validators_, \
    policy_ \
) \
    ((dcc_app_typed_slash_command_t){ \
        .size = sizeof(dcc_app_typed_slash_command_t), \
        .command = &((dcc_application_command_builder_t[]){ \
            { \
                .name = (name_), \
                .description = (description_), \
                .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
                .has_name = 1U, \
                .has_description = 1U, \
                .has_type = 1U \
            } \
        })[0], \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_TYPED_SLASH_NO_OPTIONS(name_, description_, args_type_, handler_, bindings_, validators_) \
    DCC_TYPED_SLASH_NO_OPTIONS_DATA((name_), (description_), args_type_, (handler_), NULL, (bindings_), (validators_))

#define DCC_TYPED_SLASH_NO_OPTIONS_POLICY(name_, description_, args_type_, handler_, bindings_, validators_, policy_) \
    DCC_TYPED_SLASH_NO_OPTIONS_DATA_POLICY( \
        (name_), \
        (description_), \
        args_type_, \
        (handler_), \
        NULL, \
        (bindings_), \
        (validators_), \
        (policy_) \
    )

#define DCC_TYPED_SLASH_OPTIONS_DATA( \
    name_, \
    description_, \
    args_type_, \
    handler_, \
    user_data_, \
    bindings_, \
    validators_, \
    ... \
) \
    ((dcc_app_typed_slash_command_t){ \
        .size = sizeof(dcc_app_typed_slash_command_t), \
        .command = &((dcc_application_command_builder_t[]){ \
            { \
                .name = (name_), \
                .description = (description_), \
                .options = (dcc_application_command_option_builder_t[]){ __VA_ARGS__ }, \
                .options_count = sizeof((dcc_application_command_option_builder_t[]){ __VA_ARGS__ }) / \
                    sizeof(dcc_application_command_option_builder_t), \
                .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
                .has_name = 1U, \
                .has_description = 1U, \
                .has_type = 1U \
            } \
        })[0], \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_TYPED_SLASH_OPTIONS_DATA_POLICY( \
    name_, \
    description_, \
    args_type_, \
    handler_, \
    user_data_, \
    bindings_, \
    validators_, \
    policy_, \
    ... \
) \
    ((dcc_app_typed_slash_command_t){ \
        .size = sizeof(dcc_app_typed_slash_command_t), \
        .command = &((dcc_application_command_builder_t[]){ \
            { \
                .name = (name_), \
                .description = (description_), \
                .options = (dcc_application_command_option_builder_t[]){ __VA_ARGS__ }, \
                .options_count = sizeof((dcc_application_command_option_builder_t[]){ __VA_ARGS__ }) / \
                    sizeof(dcc_application_command_option_builder_t), \
                .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
                .has_name = 1U, \
                .has_description = 1U, \
                .has_type = 1U \
            } \
        })[0], \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_TYPED_SLASH_OPTIONS(name_, description_, args_type_, handler_, bindings_, validators_, ...) \
    DCC_TYPED_SLASH_OPTIONS_DATA( \
        (name_), \
        (description_), \
        args_type_, \
        (handler_), \
        NULL, \
        (bindings_), \
        (validators_), \
        __VA_ARGS__ \
    )

#define DCC_TYPED_SLASH_OPTIONS_POLICY( \
    name_, \
    description_, \
    args_type_, \
    handler_, \
    bindings_, \
    validators_, \
    policy_, \
    ... \
) \
    DCC_TYPED_SLASH_OPTIONS_DATA_POLICY( \
        (name_), \
        (description_), \
        args_type_, \
        (handler_), \
        NULL, \
        (bindings_), \
        (validators_), \
        (policy_), \
        __VA_ARGS__ \
    )

#endif /* DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_TYPED_SLASH_BUILDERS_H */
