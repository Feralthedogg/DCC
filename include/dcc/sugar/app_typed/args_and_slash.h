/* Included by <dcc/sugar/app_typed.h>. */
typedef struct dcc_no_args {
    unsigned char reserved;
} dcc_no_args_t;

typedef dcc_app_typed_slash_command_t dcc_command_route_t;
typedef dcc_app_typed_slash_command_t dcc_slash_route_t;
typedef dcc_app_typed_subcommand_t dcc_subcommand_route_t;
typedef dcc_app_typed_autocomplete_t dcc_autocomplete_route_t;
typedef dcc_app_typed_modal_t dcc_modal_route_t;
typedef dcc_app_typed_component_t dcc_component_route_t;
typedef dcc_app_typed_component_t dcc_button_route_t;
typedef dcc_app_typed_component_t dcc_select_route_t;
typedef dcc_app_modal_flow_t dcc_modal_flow_t;

#ifndef DCC_COMPONENT_ID
#define DCC_COMPONENT_ID(namespace_, action_) namespace_ "." action_
#endif
#ifndef DCC_COMPONENT_PREFIX
#define DCC_COMPONENT_PREFIX(namespace_) namespace_ "."
#endif
#ifndef DCC_COMPONENT_ACTION_PREFIX
#define DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_) namespace_ "." action_prefix_
#endif

#define DCC_CTX_OPTION_FIELD_BINDINGS(...) \
    ((dcc_ctx_option_field_binding_list_t){ \
        .items = (dcc_ctx_option_field_binding_t[]){ __VA_ARGS__ }, \
        .count = sizeof((dcc_ctx_option_field_binding_t[]){ __VA_ARGS__ }) / \
            sizeof(dcc_ctx_option_field_binding_t) \
    })

#define DCC_CTX_FORM_FIELD_BINDINGS(...) \
    ((dcc_ctx_form_field_binding_list_t){ \
        .items = (dcc_ctx_form_field_binding_t[]){ __VA_ARGS__ }, \
        .count = sizeof((dcc_ctx_form_field_binding_t[]){ __VA_ARGS__ }) / \
            sizeof(dcc_ctx_form_field_binding_t) \
    })

#define DCC_CTX_FIELD_VALIDATORS(...) \
    ((dcc_ctx_field_validator_list_t){ \
        .items = (dcc_ctx_field_validator_t[]){ __VA_ARGS__ }, \
        .count = sizeof((dcc_ctx_field_validator_t[]){ __VA_ARGS__ }) / \
            sizeof(dcc_ctx_field_validator_t) \
    })

#define DCC_CTX_COMPONENT_FIELD_BINDINGS(...) \
    ((dcc_ctx_component_field_binding_list_t){ \
        .items = (dcc_ctx_component_field_binding_t[]){ __VA_ARGS__ }, \
        .count = sizeof((dcc_ctx_component_field_binding_t[]){ __VA_ARGS__ }) / \
            sizeof(dcc_ctx_component_field_binding_t) \
    })

#define DCC_CTX_NO_OPTION_FIELD_BINDINGS() \
    ((dcc_ctx_option_field_binding_list_t){0})

#define DCC_CTX_NO_FORM_FIELD_BINDINGS() \
    ((dcc_ctx_form_field_binding_list_t){0})

#define DCC_CTX_NO_COMPONENT_FIELD_BINDINGS() \
    ((dcc_ctx_component_field_binding_list_t){0})

#define DCC_CTX_NO_FIELD_VALIDATORS() \
    ((dcc_ctx_field_validator_list_t){0})

#define DCC_ARGS(...) DCC_CTX_OPTION_FIELD_BINDINGS(__VA_ARGS__)
#define DCC_FORM_ARGS(...) DCC_CTX_FORM_FIELD_BINDINGS(__VA_ARGS__)
#define DCC_COMPONENT_ARGS(...) DCC_CTX_COMPONENT_FIELD_BINDINGS(__VA_ARGS__)
#define DCC_VALIDATORS(...) DCC_CTX_FIELD_VALIDATORS(__VA_ARGS__)
#define DCC_NO_ARGS() DCC_CTX_NO_OPTION_FIELD_BINDINGS()
#define DCC_NO_FORM_ARGS() DCC_CTX_NO_FORM_FIELD_BINDINGS()
#define DCC_NO_COMPONENT_ARGS() DCC_CTX_NO_COMPONENT_FIELD_BINDINGS()
#define DCC_NO_VALIDATORS() DCC_CTX_NO_FIELD_VALIDATORS()

#define DCC_ARG_STRING(type_, field_, name_, fallback_) \
    DCC_CTX_OPTION_FIELD_STRING(type_, field_, name_, fallback_)
#define DCC_ARG_REQUIRED_STRING(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_STRING(type_, field_, name_)
#define DCC_ARG_INT(type_, field_, name_, fallback_) \
    DCC_CTX_OPTION_FIELD_INT(type_, field_, name_, fallback_)
#define DCC_ARG_REQUIRED_INT(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_INT(type_, field_, name_)
#define DCC_ARG_NUMBER(type_, field_, name_, fallback_) \
    DCC_CTX_OPTION_FIELD_NUMBER(type_, field_, name_, fallback_)
#define DCC_ARG_REQUIRED_NUMBER(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_NUMBER(type_, field_, name_)
#define DCC_ARG_BOOL(type_, field_, name_, fallback_) \
    DCC_CTX_OPTION_FIELD_BOOL(type_, field_, name_, fallback_)
#define DCC_ARG_REQUIRED_BOOL(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_BOOL(type_, field_, name_)
#define DCC_ARG_SNOWFLAKE(type_, field_, name_, fallback_) \
    DCC_CTX_OPTION_FIELD_SNOWFLAKE(type_, field_, name_, fallback_)
#define DCC_ARG_REQUIRED_SNOWFLAKE(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_SNOWFLAKE(type_, field_, name_)
#define DCC_ARG_USER(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_USER(type_, field_, name_)
#define DCC_ARG_REQUIRED_USER(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_USER(type_, field_, name_)
#define DCC_ARG_MEMBER(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_MEMBER(type_, field_, name_)
#define DCC_ARG_REQUIRED_MEMBER(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_MEMBER(type_, field_, name_)
#define DCC_ARG_ROLE(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_ROLE(type_, field_, name_)
#define DCC_ARG_REQUIRED_ROLE(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_ROLE(type_, field_, name_)
#define DCC_ARG_CHANNEL(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_CHANNEL(type_, field_, name_)
#define DCC_ARG_REQUIRED_CHANNEL(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_CHANNEL(type_, field_, name_)
#define DCC_ARG_MESSAGE(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_MESSAGE(type_, field_, name_)
#define DCC_ARG_REQUIRED_MESSAGE(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_MESSAGE(type_, field_, name_)
#define DCC_ARG_ATTACHMENT(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_ATTACHMENT(type_, field_, name_)
#define DCC_ARG_REQUIRED_ATTACHMENT(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_ATTACHMENT(type_, field_, name_)

#define DCC_FORM_ARG_STRING(type_, field_, custom_id_, fallback_) \
    DCC_CTX_FORM_FIELD_STRING(type_, field_, custom_id_, fallback_)
#define DCC_FORM_ARG_REQUIRED_STRING(type_, field_, custom_id_) \
    DCC_CTX_FORM_FIELD_REQUIRED_STRING(type_, field_, custom_id_)
#define DCC_FORM_ARG_BOOL(type_, field_, custom_id_, fallback_) \
    DCC_CTX_FORM_FIELD_BOOL(type_, field_, custom_id_, fallback_)
#define DCC_FORM_ARG_REQUIRED_BOOL(type_, field_, custom_id_) \
    DCC_CTX_FORM_FIELD_REQUIRED_BOOL(type_, field_, custom_id_)
#define DCC_FORM_ARG_VALUES(type_, field_, count_field_, custom_id_, fallback_values_, fallback_count_) \
    DCC_CTX_FORM_FIELD_VALUES(type_, field_, count_field_, custom_id_, fallback_values_, fallback_count_)
#define DCC_FORM_ARG_REQUIRED_VALUES(type_, field_, count_field_, custom_id_) \
    DCC_CTX_FORM_FIELD_REQUIRED_VALUES(type_, field_, count_field_, custom_id_)

#define DCC_FORM_TEXT(type_, field_, custom_id_, fallback_) \
    DCC_FORM_ARG_STRING(type_, field_, custom_id_, fallback_)
#define DCC_FORM_REQUIRED_TEXT(type_, field_, custom_id_) \
    DCC_FORM_ARG_REQUIRED_STRING(type_, field_, custom_id_)
#define DCC_FORM_CHECKBOX(type_, field_, custom_id_, fallback_) \
    DCC_FORM_ARG_BOOL(type_, field_, custom_id_, fallback_)
#define DCC_FORM_REQUIRED_CHECKBOX(type_, field_, custom_id_) \
    DCC_FORM_ARG_REQUIRED_BOOL(type_, field_, custom_id_)
#define DCC_FORM_VALUES(type_, field_, count_field_, custom_id_, fallback_values_, fallback_count_) \
    DCC_FORM_ARG_VALUES(type_, field_, count_field_, custom_id_, fallback_values_, fallback_count_)
#define DCC_FORM_REQUIRED_VALUES(type_, field_, count_field_, custom_id_) \
    DCC_FORM_ARG_REQUIRED_VALUES(type_, field_, count_field_, custom_id_)
#define DCC_MODAL_TEXT(type_, field_, custom_id_, fallback_) \
    DCC_FORM_TEXT(type_, field_, custom_id_, fallback_)
#define DCC_MODAL_REQUIRED_TEXT(type_, field_, custom_id_) \
    DCC_FORM_REQUIRED_TEXT(type_, field_, custom_id_)
#define DCC_MODAL_CHECKBOX(type_, field_, custom_id_, fallback_) \
    DCC_FORM_CHECKBOX(type_, field_, custom_id_, fallback_)
#define DCC_MODAL_REQUIRED_CHECKBOX(type_, field_, custom_id_) \
    DCC_FORM_REQUIRED_CHECKBOX(type_, field_, custom_id_)

#define DCC_COMPONENT_ARG_CUSTOM_ID_SUFFIX(type_, field_, prefix_, fallback_) \
    DCC_CTX_COMPONENT_FIELD_CUSTOM_ID_SUFFIX(type_, field_, prefix_, fallback_)
#define DCC_COMPONENT_ARG_REQUIRED_CUSTOM_ID_SUFFIX(type_, field_, prefix_) \
    DCC_CTX_COMPONENT_FIELD_REQUIRED_CUSTOM_ID_SUFFIX(type_, field_, prefix_)
#define DCC_COMPONENT_ARG_FIRST_VALUE(type_, field_, fallback_) \
    DCC_CTX_COMPONENT_FIELD_FIRST_VALUE(type_, field_, fallback_)
#define DCC_COMPONENT_ARG_REQUIRED_FIRST_VALUE(type_, field_) \
    DCC_CTX_COMPONENT_FIELD_REQUIRED_FIRST_VALUE(type_, field_)
#define DCC_COMPONENT_ARG_VALUES(type_, field_, count_field_, fallback_values_, fallback_count_) \
    DCC_CTX_COMPONENT_FIELD_VALUES(type_, field_, count_field_, fallback_values_, fallback_count_)
#define DCC_COMPONENT_ARG_REQUIRED_VALUES(type_, field_, count_field_) \
    DCC_CTX_COMPONENT_FIELD_REQUIRED_VALUES(type_, field_, count_field_)

#define DCC_COMPONENT_CUSTOM_ID_SUFFIX(type_, field_, prefix_, fallback_) \
    DCC_COMPONENT_ARG_CUSTOM_ID_SUFFIX(type_, field_, prefix_, fallback_)
#define DCC_COMPONENT_REQUIRED_CUSTOM_ID_SUFFIX(type_, field_, prefix_) \
    DCC_COMPONENT_ARG_REQUIRED_CUSTOM_ID_SUFFIX(type_, field_, prefix_)
#define DCC_COMPONENT_ACTION(type_, field_, prefix_, fallback_) \
    DCC_COMPONENT_ARG_CUSTOM_ID_SUFFIX(type_, field_, prefix_, fallback_)
#define DCC_COMPONENT_REQUIRED_ACTION(type_, field_, prefix_) \
    DCC_COMPONENT_ARG_REQUIRED_CUSTOM_ID_SUFFIX(type_, field_, prefix_)
#define DCC_COMPONENT_VALUE(type_, field_, fallback_) \
    DCC_COMPONENT_ARG_FIRST_VALUE(type_, field_, fallback_)
#define DCC_COMPONENT_REQUIRED_VALUE(type_, field_) \
    DCC_COMPONENT_ARG_REQUIRED_FIRST_VALUE(type_, field_)
#define DCC_COMPONENT_VALUES(type_, field_, count_field_, fallback_values_, fallback_count_) \
    DCC_COMPONENT_ARG_VALUES(type_, field_, count_field_, fallback_values_, fallback_count_)
#define DCC_COMPONENT_REQUIRED_VALUES(type_, field_, count_field_) \
    DCC_COMPONENT_ARG_REQUIRED_VALUES(type_, field_, count_field_)
#define DCC_SELECT_VALUE(type_, field_, fallback_) \
    DCC_COMPONENT_VALUE(type_, field_, fallback_)
#define DCC_SELECT_REQUIRED_VALUE(type_, field_) \
    DCC_COMPONENT_REQUIRED_VALUE(type_, field_)
#define DCC_SELECT_VALUES(type_, field_, count_field_, fallback_values_, fallback_count_) \
    DCC_COMPONENT_VALUES(type_, field_, count_field_, fallback_values_, fallback_count_)
#define DCC_SELECT_REQUIRED_VALUES(type_, field_, count_field_) \
    DCC_COMPONENT_REQUIRED_VALUES(type_, field_, count_field_)

#define DCC_TYPED_SLASH(command_, args_type_, handler_, user_data_, bindings_, validators_) \
    ((dcc_app_typed_slash_command_t){ \
        .size = sizeof(dcc_app_typed_slash_command_t), \
        .command = (command_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_TYPED_SLASH_POLICY(command_, args_type_, handler_, user_data_, bindings_, validators_, policy_) \
    ((dcc_app_typed_slash_command_t){ \
        .size = sizeof(dcc_app_typed_slash_command_t), \
        .command = (command_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_TYPED_SLASH_ROUTE(command_, args_type_, handler_, user_data_, out_route_, bindings_, validators_) \
    ((dcc_app_typed_slash_command_t){ \
        .size = sizeof(dcc_app_typed_slash_command_t), \
        .command = (command_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_) \
    })

#define DCC_TYPED_SLASH_ROUTE_POLICY( \
    command_, \
    args_type_, \
    handler_, \
    user_data_, \
    out_route_, \
    bindings_, \
    validators_, \
    policy_ \
) \
    ((dcc_app_typed_slash_command_t){ \
        .size = sizeof(dcc_app_typed_slash_command_t), \
        .command = (command_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_), \
        .policy = (policy_) \
    })

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

#define DCC_SLASH_PARAM_BIND(type_, kind_, ...) DCC_SLASH_PARAM_BIND_##kind_(type_, __VA_ARGS__),
#define DCC_SLASH_PARAM_OPTION(type_, kind_, ...) DCC_SLASH_PARAM_OPTION_##kind_(type_, __VA_ARGS__),

#define DCC_SLASH_PARAM_BIND_STRING(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_STRING, \
        .field_offset = offsetof(type_, field_), \
        .required = (required_) ? 1U : 0U, \
        .fallback_string = (fallback_) \
    })
#define DCC_SLASH_PARAM_OPTION_STRING(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_STRING, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U \
    })
#define DCC_SLASH_PARAM_BIND_REQUIRED_STRING(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_STRING(type_, field_, (name_), (description_), 1U, NULL)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_STRING(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_STRING(type_, field_, (name_), (description_), 1U, NULL)
#define DCC_SLASH_PARAM_BIND_STRING_AUTOCOMPLETE(type_, field_, name_, description_, required_, fallback_) \
    DCC_SLASH_PARAM_BIND_STRING(type_, field_, (name_), (description_), (required_), (fallback_))
#define DCC_SLASH_PARAM_OPTION_STRING_AUTOCOMPLETE(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_STRING, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U, \
        .autocomplete = 1U, \
        .has_autocomplete = 1U \
    })
#define DCC_SLASH_PARAM_BIND_REQUIRED_STRING_AUTOCOMPLETE(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_STRING_AUTOCOMPLETE(type_, field_, (name_), (description_), 1U, NULL)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_STRING_AUTOCOMPLETE(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_STRING_AUTOCOMPLETE(type_, field_, (name_), (description_), 1U, NULL)

#define DCC_SLASH_PARAM_BIND_INT(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_INTEGER, \
        .field_offset = offsetof(type_, field_), \
        .required = (required_) ? 1U : 0U, \
        .fallback_integer = (fallback_) \
    })
#define DCC_SLASH_PARAM_OPTION_INT(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_INTEGER, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U \
    })
#define DCC_SLASH_PARAM_BIND_INTEGER(type_, field_, name_, description_, required_, fallback_) \
    DCC_SLASH_PARAM_BIND_INT(type_, field_, (name_), (description_), (required_), (fallback_))
#define DCC_SLASH_PARAM_OPTION_INTEGER(type_, field_, name_, description_, required_, fallback_) \
    DCC_SLASH_PARAM_OPTION_INT(type_, field_, (name_), (description_), (required_), (fallback_))
#define DCC_SLASH_PARAM_BIND_REQUIRED_INT(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_INT(type_, field_, (name_), (description_), 1U, 0)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_INT(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_INT(type_, field_, (name_), (description_), 1U, 0)
#define DCC_SLASH_PARAM_BIND_REQUIRED_INTEGER(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_REQUIRED_INT(type_, field_, (name_), (description_))
#define DCC_SLASH_PARAM_OPTION_REQUIRED_INTEGER(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_REQUIRED_INT(type_, field_, (name_), (description_))
#define DCC_SLASH_PARAM_BIND_INT_RANGE(type_, field_, name_, description_, required_, fallback_, min_, max_) \
    DCC_SLASH_PARAM_BIND_INT(type_, field_, (name_), (description_), (required_), (fallback_))
#define DCC_SLASH_PARAM_OPTION_INT_RANGE(type_, field_, name_, description_, required_, fallback_, min_, max_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_INTEGER, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U, \
        .min_integer_value = (min_), \
        .max_integer_value = (max_), \
        .has_min_integer_value = 1U, \
        .has_max_integer_value = 1U \
    })
#define DCC_SLASH_PARAM_BIND_INTEGER_RANGE(type_, field_, name_, description_, required_, fallback_, min_, max_) \
    DCC_SLASH_PARAM_BIND_INT_RANGE(type_, field_, (name_), (description_), (required_), (fallback_), (min_), (max_))
#define DCC_SLASH_PARAM_OPTION_INTEGER_RANGE(type_, field_, name_, description_, required_, fallback_, min_, max_) \
    DCC_SLASH_PARAM_OPTION_INT_RANGE(type_, field_, (name_), (description_), (required_), (fallback_), (min_), (max_))

#define DCC_SLASH_PARAM_BIND_NUMBER(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_NUMBER, \
        .field_offset = offsetof(type_, field_), \
        .required = (required_) ? 1U : 0U, \
        .fallback_number = (fallback_) \
    })
#define DCC_SLASH_PARAM_OPTION_NUMBER(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_NUMBER, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U \
    })
#define DCC_SLASH_PARAM_BIND_REQUIRED_NUMBER(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_NUMBER(type_, field_, (name_), (description_), 1U, 0.0)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_NUMBER(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_NUMBER(type_, field_, (name_), (description_), 1U, 0.0)
#define DCC_SLASH_PARAM_BIND_NUMBER_RANGE(type_, field_, name_, description_, required_, fallback_, min_, max_) \
    DCC_SLASH_PARAM_BIND_NUMBER(type_, field_, (name_), (description_), (required_), (fallback_))
#define DCC_SLASH_PARAM_OPTION_NUMBER_RANGE(type_, field_, name_, description_, required_, fallback_, min_, max_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_NUMBER, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U, \
        .min_number_value = (min_), \
        .max_number_value = (max_), \
        .has_min_number_value = 1U, \
        .has_max_number_value = 1U \
    })

#define DCC_SLASH_PARAM_BIND_BOOL(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = DCC_CTX_OPTION_BIND_BOOLEAN, \
        .field_offset = offsetof(type_, field_), \
        .required = (required_) ? 1U : 0U, \
        .fallback_boolean = (fallback_) ? 1U : 0U \
    })
#define DCC_SLASH_PARAM_OPTION_BOOL(type_, field_, name_, description_, required_, fallback_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_BOOLEAN, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U \
    })
#define DCC_SLASH_PARAM_BIND_BOOLEAN(type_, field_, name_, description_, required_, fallback_) \
    DCC_SLASH_PARAM_BIND_BOOL(type_, field_, (name_), (description_), (required_), (fallback_))
#define DCC_SLASH_PARAM_OPTION_BOOLEAN(type_, field_, name_, description_, required_, fallback_) \
    DCC_SLASH_PARAM_OPTION_BOOL(type_, field_, (name_), (description_), (required_), (fallback_))
#define DCC_SLASH_PARAM_BIND_REQUIRED_BOOL(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_BOOL(type_, field_, (name_), (description_), 1U, 0U)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_BOOL(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_BOOL(type_, field_, (name_), (description_), 1U, 0U)
#define DCC_SLASH_PARAM_BIND_REQUIRED_BOOLEAN(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_REQUIRED_BOOL(type_, field_, (name_), (description_))
#define DCC_SLASH_PARAM_OPTION_REQUIRED_BOOLEAN(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_REQUIRED_BOOL(type_, field_, (name_), (description_))

#define DCC_SLASH_PARAM_BIND_OBJECT(type_, field_, name_, description_, required_, bind_type_) \
    ((dcc_ctx_option_field_binding_t){ \
        .size = sizeof(dcc_ctx_option_field_binding_t), \
        .name = (name_), \
        .type = (bind_type_), \
        .field_offset = offsetof(type_, field_), \
        .required = (required_) ? 1U : 0U \
    })
#define DCC_SLASH_PARAM_OPTION_OBJECT(option_type_, name_, description_, required_) \
    ((dcc_application_command_option_builder_t){ \
        .type = (option_type_), \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U \
    })
#define DCC_SLASH_PARAM_BIND_USER(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_BIND_OBJECT(type_, field_, (name_), (description_), (required_), DCC_CTX_OPTION_BIND_USER)
#define DCC_SLASH_PARAM_OPTION_USER(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_OPTION_OBJECT(DCC_APPLICATION_COMMAND_OPTION_USER, (name_), (description_), (required_))
#define DCC_SLASH_PARAM_BIND_REQUIRED_USER(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_USER(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_USER(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_USER(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_BIND_MEMBER(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_BIND_OBJECT(type_, field_, (name_), (description_), (required_), DCC_CTX_OPTION_BIND_MEMBER)
#define DCC_SLASH_PARAM_OPTION_MEMBER(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_OPTION_OBJECT(DCC_APPLICATION_COMMAND_OPTION_USER, (name_), (description_), (required_))
#define DCC_SLASH_PARAM_BIND_REQUIRED_MEMBER(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_MEMBER(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_MEMBER(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_MEMBER(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_BIND_ROLE(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_BIND_OBJECT(type_, field_, (name_), (description_), (required_), DCC_CTX_OPTION_BIND_ROLE)
#define DCC_SLASH_PARAM_OPTION_ROLE(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_OPTION_OBJECT(DCC_APPLICATION_COMMAND_OPTION_ROLE, (name_), (description_), (required_))
#define DCC_SLASH_PARAM_BIND_REQUIRED_ROLE(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_ROLE(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_ROLE(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_ROLE(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_BIND_CHANNEL(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_BIND_OBJECT(type_, field_, (name_), (description_), (required_), DCC_CTX_OPTION_BIND_CHANNEL)
#define DCC_SLASH_PARAM_OPTION_CHANNEL(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_OPTION_OBJECT(DCC_APPLICATION_COMMAND_OPTION_CHANNEL, (name_), (description_), (required_))
#define DCC_SLASH_PARAM_BIND_REQUIRED_CHANNEL(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_CHANNEL(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_CHANNEL(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_CHANNEL(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_BIND_CHANNEL_TYPES(type_, field_, name_, description_, required_, ...) \
    DCC_SLASH_PARAM_BIND_CHANNEL(type_, field_, (name_), (description_), (required_))
#define DCC_SLASH_PARAM_OPTION_CHANNEL_TYPES(type_, field_, name_, description_, required_, ...) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_CHANNEL, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U, \
        .channel_types = (uint32_t[]){ __VA_ARGS__ }, \
        .channel_types_count = sizeof((uint32_t[]){ __VA_ARGS__ }) / sizeof(uint32_t) \
    })
#define DCC_SLASH_PARAM_BIND_ATTACHMENT(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_BIND_OBJECT(type_, field_, (name_), (description_), (required_), DCC_CTX_OPTION_BIND_ATTACHMENT)
#define DCC_SLASH_PARAM_OPTION_ATTACHMENT(type_, field_, name_, description_, required_) \
    DCC_SLASH_PARAM_OPTION_OBJECT(DCC_APPLICATION_COMMAND_OPTION_ATTACHMENT, (name_), (description_), (required_))
#define DCC_SLASH_PARAM_BIND_REQUIRED_ATTACHMENT(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_BIND_ATTACHMENT(type_, field_, (name_), (description_), 1U)
#define DCC_SLASH_PARAM_OPTION_REQUIRED_ATTACHMENT(type_, field_, name_, description_) \
    DCC_SLASH_PARAM_OPTION_ATTACHMENT(type_, field_, (name_), (description_), 1U)

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

