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

static inline dcc_autocomplete_choice_t dcc_sugar_command_choice(
    const char *name,
    const char *name_localizations_json,
    dcc_autocomplete_choice_value_type_t type,
    const char *string_value,
    int64_t integer_value,
    double number_value
) {
    dcc_autocomplete_choice_t choice = DCC_AUTOCOMPLETE_CHOICE_INIT;
    choice.name = name;
    choice.present = DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME |
        DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE;
    if (name_localizations_json != NULL) {
        choice.present |= DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME_LOCALIZATIONS_JSON;
    }
    choice.name_localizations_json = name_localizations_json;
    choice.value_type = type;
    choice.value_string = string_value;
    choice.value_integer = integer_value;
    choice.value_number = number_value;
    return choice;
}

#define DCC_STRING_CHOICE(name_, value_) \
    dcc_sugar_command_choice( \
        (name_), NULL, DCC_AUTOCOMPLETE_CHOICE_STRING, (value_), 0, 0.0 \
    )

#define DCC_INT_CHOICE(name_, value_) \
    dcc_sugar_command_choice( \
        (name_), NULL, DCC_AUTOCOMPLETE_CHOICE_INTEGER, NULL, (value_), 0.0 \
    )

#define DCC_INTEGER_CHOICE(name_, value_) DCC_INT_CHOICE((name_), (value_))

#define DCC_NUMBER_CHOICE(name_, value_) \
    dcc_sugar_command_choice( \
        (name_), NULL, DCC_AUTOCOMPLETE_CHOICE_NUMBER, NULL, 0, (value_) \
    )

#define DCC_STRING_CHOICE_LOCALIZED(name_, value_, name_localizations_json_) \
    dcc_sugar_command_choice( \
        (name_), (name_localizations_json_), DCC_AUTOCOMPLETE_CHOICE_STRING, \
        (value_), 0, 0.0 \
    )

#define DCC_INT_CHOICE_LOCALIZED(name_, value_, name_localizations_json_) \
    dcc_sugar_command_choice( \
        (name_), (name_localizations_json_), DCC_AUTOCOMPLETE_CHOICE_INTEGER, \
        NULL, (value_), 0.0 \
    )

#define DCC_INTEGER_CHOICE_LOCALIZED(name_, value_, name_localizations_json_) \
    DCC_INT_CHOICE_LOCALIZED((name_), (value_), (name_localizations_json_))

#define DCC_NUMBER_CHOICE_LOCALIZED(name_, value_, name_localizations_json_) \
    dcc_sugar_command_choice( \
        (name_), (name_localizations_json_), DCC_AUTOCOMPLETE_CHOICE_NUMBER, \
        NULL, 0, (value_) \
    )

static inline dcc_application_command_builder_t dcc_sugar_command_make(
    const char *name,
    const char *description,
    dcc_application_command_type_t type,
    const dcc_application_command_option_builder_t *options,
    size_t options_count
) {
    dcc_application_command_builder_t command = DCC_APPLICATION_COMMAND_BUILDER_INIT;
    command.present = DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME |
        DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE;
    command.name = name;
    command.type = (uint32_t)type;
    if (type != DCC_APPLICATION_COMMAND_USER && type != DCC_APPLICATION_COMMAND_MESSAGE) {
        command.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION;
        command.description = description;
    }
    if (options_count != 0U) {
        command.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS;
        command.options = options;
        command.options_count = options_count;
    }
    return command;
}

#define DCC_SLASH_CMD(name_, description_) \
    dcc_sugar_command_make( \
        (name_), (description_), DCC_APPLICATION_COMMAND_CHAT_INPUT, NULL, 0U \
    )

#define DCC_CMD(name_, description_) \
    DCC_SLASH_CMD((name_), (description_))

#define DCC_SLASH_OPTIONS(name_, description_, ...) \
    dcc_sugar_command_make( \
        (name_), (description_), DCC_APPLICATION_COMMAND_CHAT_INPUT, \
        DCC_COMMAND_OPTIONS(__VA_ARGS__), DCC_COMMAND_OPTIONS_COUNT(__VA_ARGS__) \
    )

#define DCC_SLASH_GROUP(name_, description_, ...) \
    DCC_SLASH_OPTIONS((name_), (description_), __VA_ARGS__)

#define DCC_CMD_TREE(name_, description_, ...) \
    DCC_SLASH_GROUP((name_), (description_), __VA_ARGS__)

#define DCC_CMD_OPTIONS(name_, description_, ...) \
    DCC_SLASH_OPTIONS((name_), (description_), __VA_ARGS__)

#define DCC_USER_MENU(name_) \
    dcc_sugar_command_make((name_), NULL, DCC_APPLICATION_COMMAND_USER, NULL, 0U)

#define DCC_MESSAGE_MENU(name_) \
    dcc_sugar_command_make((name_), NULL, DCC_APPLICATION_COMMAND_MESSAGE, NULL, 0U)

static inline dcc_application_command_option_builder_t dcc_sugar_command_option_make(
    uint32_t type,
    const char *name,
    const char *description,
    uint8_t required,
    uint8_t has_required
) {
    dcc_application_command_option_builder_t option = DCC_APPLICATION_COMMAND_OPTION_BUILDER_INIT;
    option.present = DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME |
        DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION |
        DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_TYPE;
    option.type = type;
    option.name = name;
    option.description = description;
    option.required = required != 0U;
    if (has_required) {
        option.present |= DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_REQUIRED;
    }
    return option;
}

static inline dcc_application_command_option_builder_t dcc_sugar_option_autocomplete(
    dcc_application_command_option_builder_t option
) {
    option.present |= DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_AUTOCOMPLETE;
    option.autocomplete = 1U;
    return option;
}

static inline dcc_application_command_option_builder_t dcc_sugar_option_choices_json(
    dcc_application_command_option_builder_t option,
    const char *choices_json
) {
    option.choices_json = choices_json;
    if (choices_json != NULL) {
        option.present |= DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHOICES_JSON;
    }
    return option;
}

static inline dcc_application_command_option_builder_t dcc_sugar_option_choices(
    dcc_application_command_option_builder_t option,
    const dcc_autocomplete_choice_t *choices,
    size_t choices_count
) {
    option.choices = choices;
    option.choices_count = choices_count;
    if (choices_count != 0U) {
        option.present |= DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHOICES;
    }
    return option;
}

static inline dcc_application_command_option_builder_t dcc_sugar_option_integer_range(
    dcc_application_command_option_builder_t option,
    int64_t minimum,
    int64_t maximum
) {
    option.present |= DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MIN_INTEGER_VALUE |
        DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MAX_INTEGER_VALUE;
    option.min_integer_value = minimum;
    option.max_integer_value = maximum;
    return option;
}

static inline dcc_application_command_option_builder_t dcc_sugar_option_number_range(
    dcc_application_command_option_builder_t option,
    double minimum,
    double maximum
) {
    option.present |= DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MIN_NUMBER_VALUE |
        DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MAX_NUMBER_VALUE;
    option.min_number_value = minimum;
    option.max_number_value = maximum;
    return option;
}

static inline dcc_application_command_option_builder_t dcc_sugar_option_channel_types(
    dcc_application_command_option_builder_t option,
    const uint32_t *channel_types,
    size_t channel_types_count
) {
    option.channel_types = channel_types;
    option.channel_types_count = channel_types_count;
    if (channel_types_count != 0U) {
        option.present |= DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHANNEL_TYPES;
    }
    return option;
}

static inline dcc_application_command_option_builder_t dcc_sugar_option_nested(
    dcc_application_command_option_builder_t option,
    const dcc_application_command_option_builder_t *options,
    size_t options_count
) {
    option.options = options;
    option.options_count = options_count;
    if (options_count != 0U) {
        option.present |= DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_OPTIONS;
    }
    return option;
}

#define DCC_COMMAND_OPTION_REQUIRED(type_, name_, description_) \
    dcc_sugar_command_option_make((type_), (name_), (description_), 1U, 1U)

#define DCC_COMMAND_OPTION_OPTIONAL(type_, name_, description_) \
    dcc_sugar_command_option_make((type_), (name_), (description_), 0U, 1U)

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
    dcc_sugar_option_autocomplete(DCC_REQUIRED_STRING((name_), (description_)))
#define DCC_OPTIONAL_STRING_AUTOCOMPLETE(name_, description_) \
    dcc_sugar_option_autocomplete(DCC_OPTIONAL_STRING((name_), (description_)))
#define DCC_REQUIRED_STRING_CHOICES_JSON(name_, description_, choices_json_) \
    dcc_sugar_option_choices_json( \
        DCC_REQUIRED_STRING((name_), (description_)), (choices_json_) \
    )
#define DCC_OPTIONAL_STRING_CHOICES_JSON(name_, description_, choices_json_) \
    dcc_sugar_option_choices_json( \
        DCC_OPTIONAL_STRING((name_), (description_)), (choices_json_) \
    )
#define DCC_REQUIRED_STRING_CHOICES(name_, description_, ...) \
    dcc_sugar_option_choices( \
        DCC_REQUIRED_STRING((name_), (description_)), DCC_COMMAND_CHOICES(__VA_ARGS__), \
        DCC_COMMAND_CHOICES_COUNT(__VA_ARGS__) \
    )
#define DCC_OPTIONAL_STRING_CHOICES(name_, description_, ...) \
    dcc_sugar_option_choices( \
        DCC_OPTIONAL_STRING((name_), (description_)), DCC_COMMAND_CHOICES(__VA_ARGS__), \
        DCC_COMMAND_CHOICES_COUNT(__VA_ARGS__) \
    )
#define DCC_REQUIRED_INT_CHOICES(name_, description_, ...) \
    dcc_sugar_option_choices( \
        DCC_REQUIRED_INT((name_), (description_)), DCC_COMMAND_CHOICES(__VA_ARGS__), \
        DCC_COMMAND_CHOICES_COUNT(__VA_ARGS__) \
    )
#define DCC_OPTIONAL_INT_CHOICES(name_, description_, ...) \
    dcc_sugar_option_choices( \
        DCC_OPTIONAL_INT((name_), (description_)), DCC_COMMAND_CHOICES(__VA_ARGS__), \
        DCC_COMMAND_CHOICES_COUNT(__VA_ARGS__) \
    )
#define DCC_REQUIRED_INTEGER_CHOICES(name_, description_, ...) \
    DCC_REQUIRED_INT_CHOICES((name_), (description_), __VA_ARGS__)
#define DCC_OPTIONAL_INTEGER_CHOICES(name_, description_, ...) \
    DCC_OPTIONAL_INT_CHOICES((name_), (description_), __VA_ARGS__)
#define DCC_REQUIRED_NUMBER_CHOICES(name_, description_, ...) \
    dcc_sugar_option_choices( \
        DCC_REQUIRED_NUMBER((name_), (description_)), DCC_COMMAND_CHOICES(__VA_ARGS__), \
        DCC_COMMAND_CHOICES_COUNT(__VA_ARGS__) \
    )
#define DCC_OPTIONAL_NUMBER_CHOICES(name_, description_, ...) \
    dcc_sugar_option_choices( \
        DCC_OPTIONAL_NUMBER((name_), (description_)), DCC_COMMAND_CHOICES(__VA_ARGS__), \
        DCC_COMMAND_CHOICES_COUNT(__VA_ARGS__) \
    )

#define DCC_REQUIRED_INT_RANGE(name_, description_, min_, max_) \
    dcc_sugar_option_integer_range( \
        DCC_REQUIRED_INT((name_), (description_)), (min_), (max_) \
    )
#define DCC_OPTIONAL_INT_RANGE(name_, description_, min_, max_) \
    dcc_sugar_option_integer_range( \
        DCC_OPTIONAL_INT((name_), (description_)), (min_), (max_) \
    )
#define DCC_REQUIRED_INTEGER_RANGE(name_, description_, min_, max_) \
    DCC_REQUIRED_INT_RANGE((name_), (description_), (min_), (max_))
#define DCC_OPTIONAL_INTEGER_RANGE(name_, description_, min_, max_) \
    DCC_OPTIONAL_INT_RANGE((name_), (description_), (min_), (max_))
#define DCC_REQUIRED_NUMBER_RANGE(name_, description_, min_, max_) \
    dcc_sugar_option_number_range( \
        DCC_REQUIRED_NUMBER((name_), (description_)), (min_), (max_) \
    )
#define DCC_OPTIONAL_NUMBER_RANGE(name_, description_, min_, max_) \
    dcc_sugar_option_number_range( \
        DCC_OPTIONAL_NUMBER((name_), (description_)), (min_), (max_) \
    )
#define DCC_REQUIRED_CHANNEL_TYPES(name_, description_, ...) \
    dcc_sugar_option_channel_types( \
        DCC_REQUIRED_CHANNEL((name_), (description_)), (uint32_t[]){ __VA_ARGS__ }, \
        sizeof((uint32_t[]){ __VA_ARGS__ }) / sizeof(uint32_t) \
    )
#define DCC_OPTIONAL_CHANNEL_TYPES(name_, description_, ...) \
    dcc_sugar_option_channel_types( \
        DCC_OPTIONAL_CHANNEL((name_), (description_)), (uint32_t[]){ __VA_ARGS__ }, \
        sizeof((uint32_t[]){ __VA_ARGS__ }) / sizeof(uint32_t) \
    )

#define DCC_SUB_CMD(name_, description_, ...) \
    dcc_sugar_option_nested( \
        dcc_sugar_command_option_make( \
            DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND, (name_), (description_), 0U, 0U \
        ), \
        DCC_COMMAND_OPTIONS(__VA_ARGS__), DCC_COMMAND_OPTIONS_COUNT(__VA_ARGS__) \
    )
#define DCC_CMD_SUB(name_, description_, ...) \
    DCC_SUB_CMD((name_), (description_), __VA_ARGS__)
#define DCC_COMMAND_SUB(name_, description_, ...) \
    DCC_SUB_CMD((name_), (description_), __VA_ARGS__)
#define DCC_SUBCOMMAND_SCHEMA(name_, description_, ...) \
    DCC_SUB_CMD((name_), (description_), __VA_ARGS__)

#define DCC_SUB_GROUP(name_, description_, ...) \
    dcc_sugar_option_nested( \
        dcc_sugar_command_option_make( \
            DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND_GROUP, (name_), (description_), 0U, 0U \
        ), \
        DCC_COMMAND_OPTIONS(__VA_ARGS__), DCC_COMMAND_OPTIONS_COUNT(__VA_ARGS__) \
    )
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
