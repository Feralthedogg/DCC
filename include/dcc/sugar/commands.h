#ifndef DCC_SUGAR_COMMANDS_H
#define DCC_SUGAR_COMMANDS_H

#include <dcc/application_command.h>
#include <dcc/sugar/app_params.h>
#include <dcc/sugar/app_typed.h>
#include <stdint.h>

#define DCC_COMMANDS(...) \
    ((dcc_application_command_builder_t[]){ __VA_ARGS__ })
#define DCC_COMMANDS_COUNT(...) \
    (sizeof((dcc_application_command_builder_t[]){ __VA_ARGS__ }) / sizeof(dcc_application_command_builder_t))

#define DCC_COMMAND_OPTIONS(...) \
    ((dcc_application_command_option_builder_t[]){ __VA_ARGS__ })
#define DCC_COMMAND_OPTIONS_COUNT(...) \
    (sizeof((dcc_application_command_option_builder_t[]){ __VA_ARGS__ }) / \
     sizeof(dcc_application_command_option_builder_t))

#define DCC_COMMAND_CHOICES(...) \
    ((dcc_autocomplete_choice_t[]){ __VA_ARGS__ })
#define DCC_COMMAND_CHOICES_COUNT(...) \
    (sizeof((dcc_autocomplete_choice_t[]){ __VA_ARGS__ }) / sizeof(dcc_autocomplete_choice_t))

#define DCC_STRING_CHOICE(name_, value_) \
    ((dcc_autocomplete_choice_t){ \
        .name = (name_), \
        .value_string = (value_), \
        .value_type = DCC_AUTOCOMPLETE_CHOICE_STRING, \
        .has_name = 1U, \
        .has_value = 1U \
    })

#define DCC_INT_CHOICE(name_, value_) \
    ((dcc_autocomplete_choice_t){ \
        .name = (name_), \
        .value_integer = (value_), \
        .value_type = DCC_AUTOCOMPLETE_CHOICE_INTEGER, \
        .has_name = 1U, \
        .has_value = 1U \
    })

#define DCC_INTEGER_CHOICE(name_, value_) DCC_INT_CHOICE((name_), (value_))

#define DCC_NUMBER_CHOICE(name_, value_) \
    ((dcc_autocomplete_choice_t){ \
        .name = (name_), \
        .value_number = (value_), \
        .value_type = DCC_AUTOCOMPLETE_CHOICE_NUMBER, \
        .has_name = 1U, \
        .has_value = 1U \
    })

#define DCC_STRING_CHOICE_LOCALIZED(name_, value_, name_localizations_json_) \
    ((dcc_autocomplete_choice_t){ \
        .name = (name_), \
        .name_localizations_json = (name_localizations_json_), \
        .value_string = (value_), \
        .value_type = DCC_AUTOCOMPLETE_CHOICE_STRING, \
        .has_name = 1U, \
        .has_value = 1U \
    })

#define DCC_INT_CHOICE_LOCALIZED(name_, value_, name_localizations_json_) \
    ((dcc_autocomplete_choice_t){ \
        .name = (name_), \
        .name_localizations_json = (name_localizations_json_), \
        .value_integer = (value_), \
        .value_type = DCC_AUTOCOMPLETE_CHOICE_INTEGER, \
        .has_name = 1U, \
        .has_value = 1U \
    })

#define DCC_INTEGER_CHOICE_LOCALIZED(name_, value_, name_localizations_json_) \
    DCC_INT_CHOICE_LOCALIZED((name_), (value_), (name_localizations_json_))

#define DCC_NUMBER_CHOICE_LOCALIZED(name_, value_, name_localizations_json_) \
    ((dcc_autocomplete_choice_t){ \
        .name = (name_), \
        .name_localizations_json = (name_localizations_json_), \
        .value_number = (value_), \
        .value_type = DCC_AUTOCOMPLETE_CHOICE_NUMBER, \
        .has_name = 1U, \
        .has_value = 1U \
    })

#define DCC_SLASH_CMD(name_, description_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .description = (description_), \
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
        .has_name = 1U, \
        .has_description = 1U, \
        .has_type = 1U \
    })

#define DCC_CMD(name_, description_) \
    DCC_SLASH_CMD((name_), (description_))

#define DCC_SLASH_OPTIONS(name_, description_, ...) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .description = (description_), \
        .options = DCC_COMMAND_OPTIONS(__VA_ARGS__), \
        .options_count = DCC_COMMAND_OPTIONS_COUNT(__VA_ARGS__), \
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
        .has_name = 1U, \
        .has_description = 1U, \
        .has_type = 1U \
    })

#define DCC_SLASH_GROUP(name_, description_, ...) \
    DCC_SLASH_OPTIONS((name_), (description_), __VA_ARGS__)

#define DCC_CMD_TREE(name_, description_, ...) \
    DCC_SLASH_GROUP((name_), (description_), __VA_ARGS__)

#define DCC_CMD_OPTIONS(name_, description_, ...) \
    DCC_SLASH_OPTIONS((name_), (description_), __VA_ARGS__)

#define DCC_USER_MENU(name_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .type = DCC_APPLICATION_COMMAND_USER, \
        .has_name = 1U, \
        .has_type = 1U \
    })

#define DCC_MESSAGE_MENU(name_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .type = DCC_APPLICATION_COMMAND_MESSAGE, \
        .has_name = 1U, \
        .has_type = 1U \
    })

#define DCC_COMMAND_OPTION_REQUIRED(type_, name_, description_) \
    ((dcc_application_command_option_builder_t){ \
        .type = (type_), \
        .name = (name_), \
        .description = (description_), \
        .required = 1U, \
        .has_required = 1U \
    })

#define DCC_COMMAND_OPTION_OPTIONAL(type_, name_, description_) \
    ((dcc_application_command_option_builder_t){ \
        .type = (type_), \
        .name = (name_), \
        .description = (description_), \
        .required = 0U, \
        .has_required = 1U \
    })

#define DCC_REQUIRED_STRING(name_, description_) \
    DCC_COMMAND_OPTION_REQUIRED(DCC_APPLICATION_COMMAND_OPTION_STRING, (name_), (description_))
#define DCC_OPTIONAL_STRING(name_, description_) \
    DCC_COMMAND_OPTION_OPTIONAL(DCC_APPLICATION_COMMAND_OPTION_STRING, (name_), (description_))
#define DCC_REQUIRED_INT(name_, description_) \
    DCC_COMMAND_OPTION_REQUIRED(DCC_APPLICATION_COMMAND_OPTION_INTEGER, (name_), (description_))
#define DCC_OPTIONAL_INT(name_, description_) \
    DCC_COMMAND_OPTION_OPTIONAL(DCC_APPLICATION_COMMAND_OPTION_INTEGER, (name_), (description_))
#define DCC_REQUIRED_INTEGER(name_, description_) DCC_REQUIRED_INT((name_), (description_))
#define DCC_OPTIONAL_INTEGER(name_, description_) DCC_OPTIONAL_INT((name_), (description_))
#define DCC_REQUIRED_BOOL(name_, description_) \
    DCC_COMMAND_OPTION_REQUIRED(DCC_APPLICATION_COMMAND_OPTION_BOOLEAN, (name_), (description_))
#define DCC_OPTIONAL_BOOL(name_, description_) \
    DCC_COMMAND_OPTION_OPTIONAL(DCC_APPLICATION_COMMAND_OPTION_BOOLEAN, (name_), (description_))
#define DCC_REQUIRED_BOOLEAN(name_, description_) DCC_REQUIRED_BOOL((name_), (description_))
#define DCC_OPTIONAL_BOOLEAN(name_, description_) DCC_OPTIONAL_BOOL((name_), (description_))
#define DCC_REQUIRED_USER(name_, description_) \
    DCC_COMMAND_OPTION_REQUIRED(DCC_APPLICATION_COMMAND_OPTION_USER, (name_), (description_))
#define DCC_OPTIONAL_USER(name_, description_) \
    DCC_COMMAND_OPTION_OPTIONAL(DCC_APPLICATION_COMMAND_OPTION_USER, (name_), (description_))
#define DCC_REQUIRED_CHANNEL(name_, description_) \
    DCC_COMMAND_OPTION_REQUIRED(DCC_APPLICATION_COMMAND_OPTION_CHANNEL, (name_), (description_))
#define DCC_OPTIONAL_CHANNEL(name_, description_) \
    DCC_COMMAND_OPTION_OPTIONAL(DCC_APPLICATION_COMMAND_OPTION_CHANNEL, (name_), (description_))
#define DCC_REQUIRED_ROLE(name_, description_) \
    DCC_COMMAND_OPTION_REQUIRED(DCC_APPLICATION_COMMAND_OPTION_ROLE, (name_), (description_))
#define DCC_OPTIONAL_ROLE(name_, description_) \
    DCC_COMMAND_OPTION_OPTIONAL(DCC_APPLICATION_COMMAND_OPTION_ROLE, (name_), (description_))
#define DCC_REQUIRED_MENTIONABLE(name_, description_) \
    DCC_COMMAND_OPTION_REQUIRED(DCC_APPLICATION_COMMAND_OPTION_MENTIONABLE, (name_), (description_))
#define DCC_OPTIONAL_MENTIONABLE(name_, description_) \
    DCC_COMMAND_OPTION_OPTIONAL(DCC_APPLICATION_COMMAND_OPTION_MENTIONABLE, (name_), (description_))
#define DCC_REQUIRED_NUMBER(name_, description_) \
    DCC_COMMAND_OPTION_REQUIRED(DCC_APPLICATION_COMMAND_OPTION_NUMBER, (name_), (description_))
#define DCC_OPTIONAL_NUMBER(name_, description_) \
    DCC_COMMAND_OPTION_OPTIONAL(DCC_APPLICATION_COMMAND_OPTION_NUMBER, (name_), (description_))
#define DCC_REQUIRED_ATTACHMENT(name_, description_) \
    DCC_COMMAND_OPTION_REQUIRED(DCC_APPLICATION_COMMAND_OPTION_ATTACHMENT, (name_), (description_))
#define DCC_OPTIONAL_ATTACHMENT(name_, description_) \
    DCC_COMMAND_OPTION_OPTIONAL(DCC_APPLICATION_COMMAND_OPTION_ATTACHMENT, (name_), (description_))

#define DCC_REQUIRED_STRING_AUTOCOMPLETE(name_, description_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_STRING, \
        .name = (name_), \
        .description = (description_), \
        .required = 1U, \
        .has_required = 1U, \
        .autocomplete = 1U, \
        .has_autocomplete = 1U \
    })
#define DCC_OPTIONAL_STRING_AUTOCOMPLETE(name_, description_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_STRING, \
        .name = (name_), \
        .description = (description_), \
        .required = 0U, \
        .has_required = 1U, \
        .autocomplete = 1U, \
        .has_autocomplete = 1U \
    })
#define DCC_REQUIRED_STRING_CHOICES_JSON(name_, description_, choices_json_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_STRING, \
        .name = (name_), \
        .description = (description_), \
        .choices_json = (choices_json_), \
        .required = 1U, \
        .has_required = 1U \
    })
#define DCC_OPTIONAL_STRING_CHOICES_JSON(name_, description_, choices_json_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_STRING, \
        .name = (name_), \
        .description = (description_), \
        .choices_json = (choices_json_), \
        .required = 0U, \
        .has_required = 1U \
    })
#define DCC_REQUIRED_STRING_CHOICES(name_, description_, ...) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_STRING, \
        .name = (name_), \
        .description = (description_), \
        .choices = DCC_COMMAND_CHOICES(__VA_ARGS__), \
        .choices_count = DCC_COMMAND_CHOICES_COUNT(__VA_ARGS__), \
        .required = 1U, \
        .has_required = 1U \
    })
#define DCC_OPTIONAL_STRING_CHOICES(name_, description_, ...) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_STRING, \
        .name = (name_), \
        .description = (description_), \
        .choices = DCC_COMMAND_CHOICES(__VA_ARGS__), \
        .choices_count = DCC_COMMAND_CHOICES_COUNT(__VA_ARGS__), \
        .required = 0U, \
        .has_required = 1U \
    })
#define DCC_REQUIRED_INT_CHOICES(name_, description_, ...) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_INTEGER, \
        .name = (name_), \
        .description = (description_), \
        .choices = DCC_COMMAND_CHOICES(__VA_ARGS__), \
        .choices_count = DCC_COMMAND_CHOICES_COUNT(__VA_ARGS__), \
        .required = 1U, \
        .has_required = 1U \
    })
#define DCC_OPTIONAL_INT_CHOICES(name_, description_, ...) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_INTEGER, \
        .name = (name_), \
        .description = (description_), \
        .choices = DCC_COMMAND_CHOICES(__VA_ARGS__), \
        .choices_count = DCC_COMMAND_CHOICES_COUNT(__VA_ARGS__), \
        .required = 0U, \
        .has_required = 1U \
    })
#define DCC_REQUIRED_INTEGER_CHOICES(name_, description_, ...) \
    DCC_REQUIRED_INT_CHOICES((name_), (description_), __VA_ARGS__)
#define DCC_OPTIONAL_INTEGER_CHOICES(name_, description_, ...) \
    DCC_OPTIONAL_INT_CHOICES((name_), (description_), __VA_ARGS__)
#define DCC_REQUIRED_NUMBER_CHOICES(name_, description_, ...) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_NUMBER, \
        .name = (name_), \
        .description = (description_), \
        .choices = DCC_COMMAND_CHOICES(__VA_ARGS__), \
        .choices_count = DCC_COMMAND_CHOICES_COUNT(__VA_ARGS__), \
        .required = 1U, \
        .has_required = 1U \
    })
#define DCC_OPTIONAL_NUMBER_CHOICES(name_, description_, ...) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_NUMBER, \
        .name = (name_), \
        .description = (description_), \
        .choices = DCC_COMMAND_CHOICES(__VA_ARGS__), \
        .choices_count = DCC_COMMAND_CHOICES_COUNT(__VA_ARGS__), \
        .required = 0U, \
        .has_required = 1U \
    })

#define DCC_REQUIRED_INT_RANGE(name_, description_, min_, max_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_INTEGER, \
        .name = (name_), \
        .description = (description_), \
        .required = 1U, \
        .has_required = 1U, \
        .min_integer_value = (min_), \
        .max_integer_value = (max_), \
        .has_min_integer_value = 1U, \
        .has_max_integer_value = 1U \
    })
#define DCC_OPTIONAL_INT_RANGE(name_, description_, min_, max_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_INTEGER, \
        .name = (name_), \
        .description = (description_), \
        .required = 0U, \
        .has_required = 1U, \
        .min_integer_value = (min_), \
        .max_integer_value = (max_), \
        .has_min_integer_value = 1U, \
        .has_max_integer_value = 1U \
    })
#define DCC_REQUIRED_INTEGER_RANGE(name_, description_, min_, max_) \
    DCC_REQUIRED_INT_RANGE((name_), (description_), (min_), (max_))
#define DCC_OPTIONAL_INTEGER_RANGE(name_, description_, min_, max_) \
    DCC_OPTIONAL_INT_RANGE((name_), (description_), (min_), (max_))
#define DCC_REQUIRED_NUMBER_RANGE(name_, description_, min_, max_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_NUMBER, \
        .name = (name_), \
        .description = (description_), \
        .required = 1U, \
        .has_required = 1U, \
        .min_number_value = (min_), \
        .max_number_value = (max_), \
        .has_min_number_value = 1U, \
        .has_max_number_value = 1U \
    })
#define DCC_OPTIONAL_NUMBER_RANGE(name_, description_, min_, max_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_NUMBER, \
        .name = (name_), \
        .description = (description_), \
        .required = 0U, \
        .has_required = 1U, \
        .min_number_value = (min_), \
        .max_number_value = (max_), \
        .has_min_number_value = 1U, \
        .has_max_number_value = 1U \
    })
#define DCC_REQUIRED_CHANNEL_TYPES(name_, description_, ...) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_CHANNEL, \
        .name = (name_), \
        .description = (description_), \
        .required = 1U, \
        .has_required = 1U, \
        .channel_types = (uint32_t[]){ __VA_ARGS__ }, \
        .channel_types_count = sizeof((uint32_t[]){ __VA_ARGS__ }) / sizeof(uint32_t) \
    })
#define DCC_OPTIONAL_CHANNEL_TYPES(name_, description_, ...) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_CHANNEL, \
        .name = (name_), \
        .description = (description_), \
        .required = 0U, \
        .has_required = 1U, \
        .channel_types = (uint32_t[]){ __VA_ARGS__ }, \
        .channel_types_count = sizeof((uint32_t[]){ __VA_ARGS__ }) / sizeof(uint32_t) \
    })

#define DCC_SUB_CMD(name_, description_, ...) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND, \
        .name = (name_), \
        .description = (description_), \
        .options = DCC_COMMAND_OPTIONS(__VA_ARGS__), \
        .options_count = DCC_COMMAND_OPTIONS_COUNT(__VA_ARGS__) \
    })
#define DCC_CMD_SUB(name_, description_, ...) \
    DCC_SUB_CMD((name_), (description_), __VA_ARGS__)
#define DCC_COMMAND_SUB(name_, description_, ...) \
    DCC_SUB_CMD((name_), (description_), __VA_ARGS__)
#define DCC_SUBCOMMAND_SCHEMA(name_, description_, ...) \
    DCC_SUB_CMD((name_), (description_), __VA_ARGS__)

#define DCC_SUB_GROUP(name_, description_, ...) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND_GROUP, \
        .name = (name_), \
        .description = (description_), \
        .options = DCC_COMMAND_OPTIONS(__VA_ARGS__), \
        .options_count = DCC_COMMAND_OPTIONS_COUNT(__VA_ARGS__) \
    })
#define DCC_CMD_GROUP(name_, description_, ...) \
    DCC_SUB_GROUP((name_), (description_), __VA_ARGS__)
#define DCC_COMMAND_GROUP(name_, description_, ...) \
    DCC_SUB_GROUP((name_), (description_), __VA_ARGS__)
#define DCC_CMD_SUBGROUP(name_, description_, ...) \
    DCC_SUB_GROUP((name_), (description_), __VA_ARGS__)
#define DCC_SUBCOMMAND_GROUP_SCHEMA(name_, description_, ...) \
    DCC_SUB_GROUP((name_), (description_), __VA_ARGS__)

#define DCC_SUBCOMMAND_PATH(group_, subcommand_) group_ "/" subcommand_
#define DCC_SUB_PATH(group_, subcommand_) DCC_SUBCOMMAND_PATH(group_, subcommand_)
#define DCC_CMD_PATH(group_, subcommand_) DCC_SUBCOMMAND_PATH(group_, subcommand_)

#define DCC_SLASH_ROUTE(name_, description_, args_type_, handler_, params_) \
    DCC_ON_SLASH_PARAMS((name_), (description_), args_type_, (handler_), params_)
#define DCC_SLASH_ROUTE_DATA(name_, description_, args_type_, handler_, user_data_, params_) \
    DCC_ON_SLASH_PARAMS_DATA((name_), (description_), args_type_, (handler_), (user_data_), params_)
#define DCC_SLASH_ROUTE_VALIDATED(name_, description_, args_type_, handler_, params_, validators_) \
    DCC_ON_SLASH_PARAMS_VALIDATED((name_), (description_), args_type_, (handler_), params_, (validators_))
#define DCC_SLASH_ROUTE_DATA_VALIDATED(name_, description_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_ON_SLASH_PARAMS_DATA_VALIDATED( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_SLASH_ROUTE_DATA_POLICY(name_, description_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_ON_SLASH_PARAMS_DATA_POLICY((name_), (description_), args_type_, (handler_), (user_data_), params_, (policy_))
#define DCC_SLASH_ROUTE_GUARDED(name_, description_, args_type_, handler_, params_, ...) \
    DCC_ON_SLASH_PARAMS_DATA_GUARDED( \
        (name_), (description_), args_type_, (handler_), NULL, params_, __VA_ARGS__ \
    )
#define DCC_SLASH_ROUTE_DATA_GUARDED(name_, description_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_SLASH_PARAMS_DATA_GUARDED( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_SLASH_ROUTE_FN(handler_, description_, args_type_, params_) \
    DCC_ON_SLASH_PARAMS_FN(handler_, (description_), args_type_, params_)
#define DCC_SLASH_ROUTE_FN_DATA(handler_, description_, args_type_, user_data_, params_) \
    DCC_ON_SLASH_PARAMS_FN_DATA(handler_, (description_), args_type_, (user_data_), params_)
#define DCC_SLASH_ROUTE_FN_VALIDATED(handler_, description_, args_type_, params_, validators_) \
    DCC_ON_SLASH_PARAMS_FN_VALIDATED(handler_, (description_), args_type_, params_, (validators_))
#define DCC_SLASH_ROUTE_FN_DATA_VALIDATED(handler_, description_, args_type_, user_data_, params_, validators_) \
    DCC_ON_SLASH_PARAMS_FN_DATA_VALIDATED( \
        handler_, (description_), args_type_, (user_data_), params_, (validators_) \
    )
#define DCC_SLASH_ROUTE_FN_DATA_POLICY(handler_, description_, args_type_, user_data_, params_, policy_) \
    DCC_ON_SLASH_PARAMS_FN_DATA_POLICY(handler_, (description_), args_type_, (user_data_), params_, (policy_))
#define DCC_SLASH_ROUTE_FN_GUARDED(handler_, description_, args_type_, params_, ...) \
    DCC_ON_SLASH_PARAMS_FN_DATA_GUARDED(handler_, (description_), args_type_, NULL, params_, __VA_ARGS__)
#define DCC_SLASH_ROUTE_FN_DATA_GUARDED(handler_, description_, args_type_, user_data_, params_, ...) \
    DCC_ON_SLASH_PARAMS_FN_DATA_GUARDED( \
        handler_, (description_), args_type_, (user_data_), params_, __VA_ARGS__ \
    )

#define DCC_SLASH_ROUTE_NO_OPTIONS(name_, description_, handler_) \
    DCC_TYPED_SLASH_NO_OPTIONS((name_), (description_), dcc_no_args_t, (handler_), DCC_NO_ARGS(), DCC_NO_VALIDATORS())
#define DCC_SLASH_ROUTE_NO_OPTIONS_DATA(name_, description_, handler_, user_data_) \
    DCC_TYPED_SLASH_NO_OPTIONS_DATA( \
        (name_), (description_), dcc_no_args_t, (handler_), (user_data_), DCC_NO_ARGS(), DCC_NO_VALIDATORS() \
    )
#define DCC_SLASH_ROUTE_NO_OPTIONS_POLICY(name_, description_, handler_, policy_) \
    DCC_TYPED_SLASH_NO_OPTIONS_POLICY( \
        (name_), (description_), dcc_no_args_t, (handler_), DCC_NO_ARGS(), DCC_NO_VALIDATORS(), (policy_) \
    )
#define DCC_SLASH_ROUTE_NO_OPTIONS_DATA_POLICY(name_, description_, handler_, user_data_, policy_) \
    DCC_TYPED_SLASH_NO_OPTIONS_DATA_POLICY( \
        (name_), (description_), dcc_no_args_t, (handler_), (user_data_), DCC_NO_ARGS(), DCC_NO_VALIDATORS(), (policy_) \
    )
#define DCC_SLASH_ROUTE_NO_OPTIONS_GUARDED(name_, description_, handler_, ...) \
    DCC_SLASH_ROUTE_NO_OPTIONS_POLICY((name_), (description_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))
#define DCC_SLASH_ROUTE_NO_OPTIONS_DATA_GUARDED(name_, description_, handler_, user_data_, ...) \
    DCC_SLASH_ROUTE_NO_OPTIONS_DATA_POLICY( \
        (name_), (description_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_SLASH_ROUTE_NO_OPTIONS_FN(handler_, description_) \
    DCC_SLASH_ROUTE_NO_OPTIONS(#handler_, (description_), (handler_))
#define DCC_SLASH_ROUTE_NO_OPTIONS_FN_DATA(handler_, description_, user_data_) \
    DCC_SLASH_ROUTE_NO_OPTIONS_DATA(#handler_, (description_), (handler_), (user_data_))
#define DCC_SLASH_ROUTE_NO_OPTIONS_FN_GUARDED(handler_, description_, ...) \
    DCC_SLASH_ROUTE_NO_OPTIONS_GUARDED(#handler_, (description_), (handler_), __VA_ARGS__)
#define DCC_SLASH_ROUTE_NO_OPTIONS_FN_DATA_GUARDED(handler_, description_, user_data_, ...) \
    DCC_SLASH_ROUTE_NO_OPTIONS_DATA_GUARDED(#handler_, (description_), (handler_), (user_data_), __VA_ARGS__)

#define DCC_COMMAND_ROUTE(name_, description_, args_type_, handler_, params_) \
    DCC_SLASH_ROUTE((name_), (description_), args_type_, (handler_), params_)
#define DCC_COMMAND_ROUTE_DATA(name_, description_, args_type_, handler_, user_data_, params_) \
    DCC_SLASH_ROUTE_DATA((name_), (description_), args_type_, (handler_), (user_data_), params_)
#define DCC_COMMAND_ROUTE_VALIDATED(name_, description_, args_type_, handler_, params_, validators_) \
    DCC_SLASH_ROUTE_VALIDATED((name_), (description_), args_type_, (handler_), params_, (validators_))
#define DCC_COMMAND_ROUTE_DATA_VALIDATED(name_, description_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_SLASH_ROUTE_DATA_VALIDATED( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_COMMAND_ROUTE_DATA_POLICY(name_, description_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_SLASH_ROUTE_DATA_POLICY((name_), (description_), args_type_, (handler_), (user_data_), params_, (policy_))
#define DCC_COMMAND_ROUTE_GUARDED(name_, description_, args_type_, handler_, params_, ...) \
    DCC_SLASH_ROUTE_GUARDED((name_), (description_), args_type_, (handler_), params_, __VA_ARGS__)
#define DCC_COMMAND_ROUTE_DATA_GUARDED(name_, description_, args_type_, handler_, user_data_, params_, ...) \
    DCC_SLASH_ROUTE_DATA_GUARDED((name_), (description_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__)
#define DCC_COMMAND_ROUTE_FN(handler_, description_, args_type_, params_) \
    DCC_SLASH_ROUTE_FN(handler_, (description_), args_type_, params_)
#define DCC_COMMAND_ROUTE_FN_DATA(handler_, description_, args_type_, user_data_, params_) \
    DCC_SLASH_ROUTE_FN_DATA(handler_, (description_), args_type_, (user_data_), params_)
#define DCC_COMMAND_ROUTE_FN_VALIDATED(handler_, description_, args_type_, params_, validators_) \
    DCC_SLASH_ROUTE_FN_VALIDATED(handler_, (description_), args_type_, params_, (validators_))
#define DCC_COMMAND_ROUTE_FN_DATA_VALIDATED(handler_, description_, args_type_, user_data_, params_, validators_) \
    DCC_SLASH_ROUTE_FN_DATA_VALIDATED(handler_, (description_), args_type_, (user_data_), params_, (validators_))
#define DCC_COMMAND_ROUTE_FN_DATA_POLICY(handler_, description_, args_type_, user_data_, params_, policy_) \
    DCC_SLASH_ROUTE_FN_DATA_POLICY(handler_, (description_), args_type_, (user_data_), params_, (policy_))
#define DCC_COMMAND_ROUTE_FN_GUARDED(handler_, description_, args_type_, params_, ...) \
    DCC_SLASH_ROUTE_FN_GUARDED(handler_, (description_), args_type_, params_, __VA_ARGS__)
#define DCC_COMMAND_ROUTE_FN_DATA_GUARDED(handler_, description_, args_type_, user_data_, params_, ...) \
    DCC_SLASH_ROUTE_FN_DATA_GUARDED(handler_, (description_), args_type_, (user_data_), params_, __VA_ARGS__)

#define DCC_COMMAND_ROUTE_NO_OPTIONS(name_, description_, handler_) \
    DCC_SLASH_ROUTE_NO_OPTIONS((name_), (description_), (handler_))
#define DCC_COMMAND_ROUTE_NO_OPTIONS_DATA(name_, description_, handler_, user_data_) \
    DCC_SLASH_ROUTE_NO_OPTIONS_DATA((name_), (description_), (handler_), (user_data_))
#define DCC_COMMAND_ROUTE_NO_OPTIONS_POLICY(name_, description_, handler_, policy_) \
    DCC_SLASH_ROUTE_NO_OPTIONS_POLICY((name_), (description_), (handler_), (policy_))
#define DCC_COMMAND_ROUTE_NO_OPTIONS_DATA_POLICY(name_, description_, handler_, user_data_, policy_) \
    DCC_SLASH_ROUTE_NO_OPTIONS_DATA_POLICY((name_), (description_), (handler_), (user_data_), (policy_))
#define DCC_COMMAND_ROUTE_NO_OPTIONS_GUARDED(name_, description_, handler_, ...) \
    DCC_SLASH_ROUTE_NO_OPTIONS_GUARDED((name_), (description_), (handler_), __VA_ARGS__)
#define DCC_COMMAND_ROUTE_NO_OPTIONS_DATA_GUARDED(name_, description_, handler_, user_data_, ...) \
    DCC_SLASH_ROUTE_NO_OPTIONS_DATA_GUARDED((name_), (description_), (handler_), (user_data_), __VA_ARGS__)
#define DCC_COMMAND_ROUTE_NO_OPTIONS_FN(handler_, description_) \
    DCC_SLASH_ROUTE_NO_OPTIONS_FN(handler_, (description_))
#define DCC_COMMAND_ROUTE_NO_OPTIONS_FN_DATA(handler_, description_, user_data_) \
    DCC_SLASH_ROUTE_NO_OPTIONS_FN_DATA(handler_, (description_), (user_data_))
#define DCC_COMMAND_ROUTE_NO_OPTIONS_FN_GUARDED(handler_, description_, ...) \
    DCC_SLASH_ROUTE_NO_OPTIONS_FN_GUARDED(handler_, (description_), __VA_ARGS__)
#define DCC_COMMAND_ROUTE_NO_OPTIONS_FN_DATA_GUARDED(handler_, description_, user_data_, ...) \
    DCC_SLASH_ROUTE_NO_OPTIONS_FN_DATA_GUARDED(handler_, (description_), (user_data_), __VA_ARGS__)

#define DCC_SUBCOMMAND_ROUTE(command_name_, subcommand_path_, args_type_, handler_, params_) \
    DCC_ON_SUBCOMMAND_PARAMS((command_name_), (subcommand_path_), args_type_, (handler_), params_)
#define DCC_SUBCOMMAND_ROUTE_DATA(command_name_, subcommand_path_, args_type_, handler_, user_data_, params_) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA( \
        (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_ \
    )
#define DCC_SUBCOMMAND_ROUTE_DATA_POLICY(command_name_, subcommand_path_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_POLICY( \
        (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_SUBCOMMAND_ROUTE_GUARDED(command_name_, subcommand_path_, args_type_, handler_, params_, ...) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_GUARDED( \
        (command_name_), (subcommand_path_), args_type_, (handler_), NULL, params_, __VA_ARGS__ \
    )
#define DCC_SUBCOMMAND_ROUTE_DATA_GUARDED(command_name_, subcommand_path_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_GUARDED( \
        (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_SUBCOMMAND_ROUTE_FN(command_name_, handler_, args_type_, params_) \
    DCC_ON_SUBCOMMAND_PARAMS_FN((command_name_), handler_, args_type_, params_)
#define DCC_SUBCOMMAND_ROUTE_FN_DATA(command_name_, handler_, args_type_, user_data_, params_) \
    DCC_ON_SUBCOMMAND_PARAMS_FN_DATA((command_name_), handler_, args_type_, (user_data_), params_)
#define DCC_SUBCOMMAND_ROUTE_FN_VALIDATED(command_name_, handler_, args_type_, params_, validators_) \
    DCC_ON_SUBCOMMAND_PARAMS_FN_VALIDATED((command_name_), handler_, args_type_, params_, (validators_))
#define DCC_SUBCOMMAND_ROUTE_FN_DATA_VALIDATED(command_name_, handler_, args_type_, user_data_, params_, validators_) \
    DCC_ON_SUBCOMMAND_PARAMS_FN_DATA_VALIDATED( \
        (command_name_), handler_, args_type_, (user_data_), params_, (validators_) \
    )
#define DCC_SUBCOMMAND_ROUTE_FN_DATA_POLICY(command_name_, handler_, args_type_, user_data_, params_, policy_) \
    DCC_ON_SUBCOMMAND_PARAMS_FN_DATA_POLICY((command_name_), handler_, args_type_, (user_data_), params_, (policy_))
#define DCC_SUBCOMMAND_ROUTE_FN_GUARDED(command_name_, handler_, args_type_, params_, ...) \
    DCC_ON_SUBCOMMAND_PARAMS_FN_DATA_GUARDED((command_name_), handler_, args_type_, NULL, params_, __VA_ARGS__)
#define DCC_SUBCOMMAND_ROUTE_FN_DATA_GUARDED(command_name_, handler_, args_type_, user_data_, params_, ...) \
    DCC_ON_SUBCOMMAND_PARAMS_FN_DATA_GUARDED( \
        (command_name_), handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
    )

#define DCC_SUBCOMMAND_ROUTE_IN(command_name_, group_, subcommand_, args_type_, handler_, params_) \
    DCC_SUBCOMMAND_ROUTE((command_name_), DCC_SUBCOMMAND_PATH(group_, subcommand_), args_type_, (handler_), params_)
#define DCC_SUBCOMMAND_ROUTE_IN_DATA(command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_) \
    DCC_SUBCOMMAND_ROUTE_DATA( \
        (command_name_), DCC_SUBCOMMAND_PATH(group_, subcommand_), args_type_, (handler_), (user_data_), params_ \
    )
#define DCC_SUBCOMMAND_ROUTE_IN_DATA_POLICY( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_, policy_ \
) \
    DCC_SUBCOMMAND_ROUTE_DATA_POLICY( \
        (command_name_), DCC_SUBCOMMAND_PATH(group_, subcommand_), args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_SUBCOMMAND_ROUTE_IN_GUARDED(command_name_, group_, subcommand_, args_type_, handler_, params_, ...) \
    DCC_SUBCOMMAND_ROUTE_GUARDED( \
        (command_name_), DCC_SUBCOMMAND_PATH(group_, subcommand_), args_type_, (handler_), params_, __VA_ARGS__ \
    )
#define DCC_SUBCOMMAND_ROUTE_IN_DATA_GUARDED( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_SUBCOMMAND_ROUTE_DATA_GUARDED( \
        (command_name_), DCC_SUBCOMMAND_PATH(group_, subcommand_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_SUBCOMMAND_ROUTE_IN_FN(command_name_, group_, handler_, args_type_, params_) \
    DCC_SUBCOMMAND_ROUTE_IN((command_name_), group_, #handler_, args_type_, (handler_), params_)
#define DCC_SUBCOMMAND_ROUTE_IN_FN_DATA(command_name_, group_, handler_, args_type_, user_data_, params_) \
    DCC_SUBCOMMAND_ROUTE_IN_DATA( \
        (command_name_), group_, #handler_, args_type_, (handler_), (user_data_), params_ \
    )
#define DCC_SUBCOMMAND_ROUTE_IN_FN_DATA_POLICY(command_name_, group_, handler_, args_type_, user_data_, params_, policy_) \
    DCC_SUBCOMMAND_ROUTE_IN_DATA_POLICY( \
        (command_name_), group_, #handler_, args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_SUBCOMMAND_ROUTE_IN_FN_GUARDED(command_name_, group_, handler_, args_type_, params_, ...) \
    DCC_SUBCOMMAND_ROUTE_IN_GUARDED((command_name_), group_, #handler_, args_type_, (handler_), params_, __VA_ARGS__)
#define DCC_SUBCOMMAND_ROUTE_IN_FN_DATA_GUARDED(command_name_, group_, handler_, args_type_, user_data_, params_, ...) \
    DCC_SUBCOMMAND_ROUTE_IN_DATA_GUARDED( \
        (command_name_), group_, #handler_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )

#define DCC_ON_SUBCOMMAND_BUILDER_PARAMS_IN(command_, group_, subcommand_, args_type_, handler_, params_) \
    DCC_ON_SUBCOMMAND_BUILDER_PARAMS( \
        (command_), DCC_SUBCOMMAND_PATH(group_, subcommand_), args_type_, (handler_), params_ \
    )
#define DCC_ON_SUBCOMMAND_BUILDER_PARAMS_IN_DATA(command_, group_, subcommand_, args_type_, handler_, user_data_, params_) \
    DCC_ON_SUBCOMMAND_BUILDER_PARAMS_DATA( \
        (command_), DCC_SUBCOMMAND_PATH(group_, subcommand_), args_type_, (handler_), (user_data_), params_ \
    )
#define DCC_ON_SUBCOMMAND_BUILDER_PARAMS_IN_VALIDATED( \
    command_, group_, subcommand_, args_type_, handler_, params_, validators_ \
) \
    DCC_ON_SUBCOMMAND_BUILDER_PARAMS_VALIDATED( \
        (command_), DCC_SUBCOMMAND_PATH(group_, subcommand_), args_type_, (handler_), params_, (validators_) \
    )
#define DCC_ON_SUBCOMMAND_BUILDER_PARAMS_IN_DATA_VALIDATED( \
    command_, group_, subcommand_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_ON_SUBCOMMAND_BUILDER_PARAMS_DATA_VALIDATED( \
        (command_), DCC_SUBCOMMAND_PATH(group_, subcommand_), args_type_, (handler_), (user_data_), params_, (validators_) \
    )

#define DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_IN(command_name_, group_, subcommand_, args_type_, handler_, params_) \
    DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS( \
        (command_name_), DCC_SUBCOMMAND_PATH(group_, subcommand_), args_type_, (handler_), params_ \
    )
#define DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_IN_DATA( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_ \
) \
    DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_DATA( \
        (command_name_), DCC_SUBCOMMAND_PATH(group_, subcommand_), args_type_, (handler_), (user_data_), params_ \
    )
#define DCC_ON_SUBCOMMAND_AUTOCOMPLETE_BUILDER_PARAMS_IN(command_, group_, subcommand_, args_type_, handler_, params_) \
    DCC_ON_SUBCOMMAND_AUTOCOMPLETE_BUILDER_PARAMS( \
        (command_), DCC_SUBCOMMAND_PATH(group_, subcommand_), args_type_, (handler_), params_ \
    )
#define DCC_ON_SUBCOMMAND_AUTOCOMPLETE_BUILDER_PARAMS_IN_DATA( \
    command_, group_, subcommand_, args_type_, handler_, user_data_, params_ \
) \
    DCC_ON_SUBCOMMAND_AUTOCOMPLETE_BUILDER_PARAMS_DATA( \
        (command_), DCC_SUBCOMMAND_PATH(group_, subcommand_), args_type_, (handler_), (user_data_), params_ \
    )

#endif
