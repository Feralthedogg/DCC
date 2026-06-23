#ifndef DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_TYPED_SLASH_ALIASES_H
#define DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_TYPED_SLASH_ALIASES_H

#include <dcc/sugar/app_typed/args_and_slash/typed_slash_builders.h>

#define DCC_ON_TYPED_SLASH_BUILDER_DATA(command_, args_type_, handler_, user_data_, bindings_, validators_) \
    DCC_TYPED_SLASH((command_), args_type_, (handler_), (user_data_), (bindings_), (validators_))

#define DCC_ON_TYPED_SLASH_BUILDER(command_, args_type_, handler_, bindings_, validators_) \
    DCC_TYPED_SLASH((command_), args_type_, (handler_), NULL, (bindings_), (validators_))

#define DCC_ON_TYPED_SLASH_NO_OPTIONS_DATA(name_, description_, args_type_, handler_, user_data_, bindings_, validators_) \
    DCC_TYPED_SLASH_NO_OPTIONS_DATA( \
        (name_), \
        (description_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_) \
    )

#define DCC_ON_TYPED_SLASH_NO_OPTIONS(name_, description_, args_type_, handler_, bindings_, validators_) \
    DCC_TYPED_SLASH_NO_OPTIONS((name_), (description_), args_type_, (handler_), (bindings_), (validators_))

#define DCC_ON_TYPED_SLASH_DATA(name_, description_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    DCC_TYPED_SLASH_OPTIONS_DATA( \
        (name_), \
        (description_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        __VA_ARGS__ \
    )

#define DCC_ON_TYPED_SLASH(name_, description_, args_type_, handler_, bindings_, validators_, ...) \
    DCC_TYPED_SLASH_OPTIONS((name_), (description_), args_type_, (handler_), (bindings_), (validators_), __VA_ARGS__)

#define DCC_ON_TYPED_SLASH_DATA_POLICY( \
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
    DCC_TYPED_SLASH_OPTIONS_DATA_POLICY( \
        (name_), \
        (description_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        (policy_), \
        __VA_ARGS__ \
    )

#define DCC_ON_TYPED_SLASH_POLICY(name_, description_, args_type_, handler_, bindings_, validators_, policy_, ...) \
    DCC_TYPED_SLASH_OPTIONS_POLICY( \
        (name_), \
        (description_), \
        args_type_, \
        (handler_), \
        (bindings_), \
        (validators_), \
        (policy_), \
        __VA_ARGS__ \
    )

#endif /* DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_TYPED_SLASH_ALIASES_H */
