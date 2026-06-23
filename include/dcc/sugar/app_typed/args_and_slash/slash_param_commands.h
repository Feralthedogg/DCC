#ifndef DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_COMMANDS_H
#define DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_COMMANDS_H

#include <dcc/sugar/app_typed/args_and_slash/slash_param_objects.h>
#include <dcc/sugar/app_typed/args_and_slash/binding_lists.h>

#define DCC_SLASH_PARAMS_BINDINGS(args_type_, params_) \
    DCC_ARGS(params_(DCC_SLASH_PARAM_BIND, args_type_))
#define DCC_SLASH_PARAMS_OPTIONS(args_type_, params_) \
    (dcc_application_command_option_builder_t[]){ params_(DCC_SLASH_PARAM_OPTION, args_type_) }
#define DCC_SLASH_PARAMS_OPTIONS_COUNT(args_type_, params_) \
    (sizeof((dcc_application_command_option_builder_t[]){ params_(DCC_SLASH_PARAM_OPTION, args_type_) }) / \
     sizeof(dcc_application_command_option_builder_t))

#define DCC_SLASH_COMMAND_PARAMS(name_, description_, args_type_, params_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), .description = (description_), .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
        .has_name = 1U, .has_description = 1U, .has_type = 1U, \
        .options = DCC_SLASH_PARAMS_OPTIONS(args_type_, params_), \
        .options_count = DCC_SLASH_PARAMS_OPTIONS_COUNT(args_type_, params_), \
    })

#define DCC_COMMAND_PARAMS(name_, description_, args_type_, params_) DCC_SLASH_COMMAND_PARAMS((name_), (description_), args_type_, params_)
#define DCC_SLASH_COMMAND_PARAMS_FN(handler_, description_, args_type_, params_) \
    DCC_SLASH_COMMAND_PARAMS(#handler_, (description_), args_type_, params_)
#define DCC_COMMAND_PARAMS_FN(handler_, description_, args_type_, params_) \
    DCC_SLASH_COMMAND_PARAMS_FN(handler_, (description_), args_type_, params_)

#define DCC_COMMAND_SUBCOMMAND_PARAMS(name_, description_, args_type_, params_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND, .name = (name_), .description = (description_), \
        .options = DCC_SLASH_PARAMS_OPTIONS(args_type_, params_), \
        .options_count = DCC_SLASH_PARAMS_OPTIONS_COUNT(args_type_, params_) \
    })

#define DCC_CMD_SUB_PARAMS(name_, description_, args_type_, params_) DCC_COMMAND_SUBCOMMAND_PARAMS((name_), (description_), args_type_, params_)

#endif /* DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_COMMANDS_H */
