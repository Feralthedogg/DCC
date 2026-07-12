/* Included by <dcc/sugar.h>. */
#define DCC_MODAL_COMPONENTS_JSON(custom_id_, title_, components_json_) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components_json = (components_json_), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#define DCC_MODAL_COMPONENTS_V2_JSON(custom_id_, title_, components_json_) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components_v2_json = (components_json_), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#define DCC_MODAL(custom_id_, title_, ...) DCC_MODAL_BUILDER((custom_id_), (title_), __VA_ARGS__)
#define DCC_MODAL_V2(custom_id_, title_, ...) DCC_MODAL_V2_BUILDER((custom_id_), (title_), __VA_ARGS__)
#define DCC_SHOW_MODAL_BUILDER(ctx_, custom_id_, title_, ...) \
    DCC_SHOW_MODAL((ctx_), DCC_MODAL((custom_id_), (title_), __VA_ARGS__))
#define DCC_SHOW_MODAL_BUILDER_CB(ctx_, custom_id_, title_, cb_, user_data_, ...) \
    DCC_SHOW_MODAL_CB((ctx_), DCC_MODAL((custom_id_), (title_), __VA_ARGS__), (cb_), (user_data_))
#define DCC_SHOW_MODAL_V1(ctx_, custom_id_, title_, ...) \
    DCC_SHOW_MODAL_BUILDER((ctx_), (custom_id_), (title_), __VA_ARGS__)
#define DCC_SHOW_MODAL_V1_CB(ctx_, custom_id_, title_, cb_, user_data_, ...) \
    DCC_SHOW_MODAL_BUILDER_CB((ctx_), (custom_id_), (title_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_SHOW_MODAL_V2(ctx_, custom_id_, title_, ...) \
    DCC_SHOW_MODAL((ctx_), DCC_MODAL_V2((custom_id_), (title_), __VA_ARGS__))
#define DCC_SHOW_MODAL_V2_CB(ctx_, custom_id_, title_, cb_, user_data_, ...) \
    DCC_SHOW_MODAL_CB((ctx_), DCC_MODAL_V2((custom_id_), (title_), __VA_ARGS__), (cb_), (user_data_))
#define DCC_CTX_SHOW_MODAL_V2(ctx_, custom_id_, title_, ...) \
    DCC_SHOW_MODAL_V2((ctx_), (custom_id_), (title_), __VA_ARGS__)
#define DCC_CTX_SHOW_MODAL_V2_CB(ctx_, custom_id_, title_, cb_, user_data_, ...) \
    DCC_SHOW_MODAL_V2_CB((ctx_), (custom_id_), (title_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_MODAL_TEXT_INPUT(custom_id_, label_, required_) \
    DCC_TEXT_INPUT_REQUIRED((custom_id_), (label_), DCC_TEXT_INPUT_SHORT, (required_))
#define DCC_MODAL_TEXT_INPUT_PLACEHOLDER(custom_id_, label_, placeholder_, required_) \
    DCC_TEXT_INPUT_PLACEHOLDER((custom_id_), (label_), DCC_TEXT_INPUT_SHORT, (placeholder_), (required_))
#define DCC_MODAL_PARAGRAPH_INPUT(custom_id_, label_, required_) \
    DCC_TEXT_INPUT_REQUIRED((custom_id_), (label_), DCC_TEXT_INPUT_PARAGRAPH, (required_))
#define DCC_MODAL_PARAGRAPH_INPUT_PLACEHOLDER(custom_id_, label_, placeholder_, required_) \
    DCC_TEXT_INPUT_PLACEHOLDER((custom_id_), (label_), DCC_TEXT_INPUT_PARAGRAPH, (placeholder_), (required_))
#define DCC_MODAL_V2_TEXT_INPUT(custom_id_, label_, required_) \
    DCC_V2_TEXT_INPUT_REQUIRED((custom_id_), (label_), DCC_TEXT_INPUT_SHORT, (required_))
#define DCC_MODAL_V2_TEXT_INPUT_PLACEHOLDER(custom_id_, label_, placeholder_, required_) \
    DCC_V2_TEXT_INPUT_PLACEHOLDER((custom_id_), (label_), DCC_TEXT_INPUT_SHORT, (placeholder_), (required_))
#define DCC_MODAL_V2_PARAGRAPH_INPUT(custom_id_, label_, required_) \
    DCC_V2_TEXT_INPUT_REQUIRED((custom_id_), (label_), DCC_TEXT_INPUT_PARAGRAPH, (required_))
#define DCC_MODAL_V2_PARAGRAPH_INPUT_PLACEHOLDER(custom_id_, label_, placeholder_, required_) \
    DCC_V2_TEXT_INPUT_PLACEHOLDER((custom_id_), (label_), DCC_TEXT_INPUT_PARAGRAPH, (placeholder_), (required_))
#define DCC_MODAL_V2_CHECKBOX(custom_id_, label_, checked_) \
    DCC_MODAL_V2_FIELD_CHECKBOX((custom_id_), (label_), (checked_))

#define DCC_SLASH_COMMAND(name_, description_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .description = (description_), \
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
        .has_name = 1U, \
        .has_description = 1U, \
        .has_type = 1U \
    })
#define DCC_SLASH_COMMAND_OPTIONS(name_, description_, options_json_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .description = (description_), \
        .options_json = (options_json_), \
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
        .has_name = 1U, \
        .has_description = 1U, \
        .has_type = 1U \
    })
#define DCC_SLASH_COMMAND_WITH_OPTIONS(name_, description_, ...) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .description = (description_), \
        .options = DCC_SUGAR_ARRAY(dcc_application_command_option_builder_t, __VA_ARGS__), \
        .options_count = DCC_SUGAR_ARRAY_LEN(dcc_application_command_option_builder_t, __VA_ARGS__), \
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
        .has_name = 1U, \
        .has_description = 1U, \
        .has_type = 1U \
    })
#define DCC_SLASH_WITH_OPTIONS(name_, description_, ...) \
    DCC_SLASH_COMMAND_WITH_OPTIONS((name_), (description_), __VA_ARGS__)
#ifndef DCC_SLASH_GROUP
#define DCC_SLASH_GROUP(name_, description_, ...) \
    DCC_SLASH_COMMAND_WITH_OPTIONS((name_), (description_), __VA_ARGS__)
#endif
#define DCC_SLASH_COMMAND_NSFW(name_, description_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .description = (description_), \
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
        .nsfw = 1U, \
        .has_name = 1U, \
        .has_description = 1U, \
        .has_type = 1U, \
        .has_nsfw = 1U \
    })
#define DCC_SLASH_COMMAND_DM(name_, description_, dm_permission_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .description = (description_), \
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
        .dm_permission = (dm_permission_), \
        .has_name = 1U, \
        .has_description = 1U, \
        .has_type = 1U, \
        .has_dm_permission = 1U \
    })
#define DCC_SLASH_COMMAND_DEFAULT_PERMISSIONS(name_, description_, permissions_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .description = (description_), \
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT, \
        .default_member_permissions = (permissions_), \
        .has_name = 1U, \
        .has_description = 1U, \
        .has_type = 1U, \
        .has_default_member_permissions = 1U \
    })
#define DCC_USER_COMMAND_BUILDER(name_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .type = DCC_APPLICATION_COMMAND_USER, \
        .has_name = 1U, \
        .has_type = 1U \
    })
#define DCC_MESSAGE_COMMAND_BUILDER(name_) \
    ((dcc_application_command_builder_t){ \
        .name = (name_), \
        .type = DCC_APPLICATION_COMMAND_MESSAGE, \
        .has_name = 1U, \
        .has_type = 1U \
    })

#define DCC_COMMAND_OPTION(type_, name_, description_, required_) \
    ((dcc_application_command_option_builder_t){ \
        .type = (type_), \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U \
    })
#define DCC_COMMAND_STRING_OPTION(name_, description_, required_) \
    DCC_COMMAND_OPTION(DCC_APPLICATION_COMMAND_OPTION_STRING, (name_), (description_), (required_))
#define DCC_COMMAND_INTEGER_OPTION(name_, description_, required_) \
    DCC_COMMAND_OPTION(DCC_APPLICATION_COMMAND_OPTION_INTEGER, (name_), (description_), (required_))
#define DCC_COMMAND_BOOLEAN_OPTION(name_, description_, required_) \
    DCC_COMMAND_OPTION(DCC_APPLICATION_COMMAND_OPTION_BOOLEAN, (name_), (description_), (required_))
#define DCC_COMMAND_USER_OPTION(name_, description_, required_) \
    DCC_COMMAND_OPTION(DCC_APPLICATION_COMMAND_OPTION_USER, (name_), (description_), (required_))
#define DCC_COMMAND_CHANNEL_OPTION(name_, description_, required_) \
    DCC_COMMAND_OPTION(DCC_APPLICATION_COMMAND_OPTION_CHANNEL, (name_), (description_), (required_))
#define DCC_COMMAND_ROLE_OPTION(name_, description_, required_) \
    DCC_COMMAND_OPTION(DCC_APPLICATION_COMMAND_OPTION_ROLE, (name_), (description_), (required_))
#define DCC_COMMAND_MENTIONABLE_OPTION(name_, description_, required_) \
    DCC_COMMAND_OPTION(DCC_APPLICATION_COMMAND_OPTION_MENTIONABLE, (name_), (description_), (required_))
#define DCC_COMMAND_NUMBER_OPTION(name_, description_, required_) \
    DCC_COMMAND_OPTION(DCC_APPLICATION_COMMAND_OPTION_NUMBER, (name_), (description_), (required_))
#define DCC_COMMAND_ATTACHMENT_OPTION(name_, description_, required_) \
    DCC_COMMAND_OPTION(DCC_APPLICATION_COMMAND_OPTION_ATTACHMENT, (name_), (description_), (required_))
#define DCC_OPT_STRING(name_, description_, required_) \
    DCC_COMMAND_STRING_OPTION((name_), (description_), (required_))
#define DCC_OPT_INT(name_, description_, required_) \
    DCC_COMMAND_INTEGER_OPTION((name_), (description_), (required_))
#define DCC_OPT_INTEGER(name_, description_, required_) \
    DCC_COMMAND_INTEGER_OPTION((name_), (description_), (required_))
#define DCC_OPT_BOOL(name_, description_, required_) \
    DCC_COMMAND_BOOLEAN_OPTION((name_), (description_), (required_))
#define DCC_OPT_BOOLEAN(name_, description_, required_) \
    DCC_COMMAND_BOOLEAN_OPTION((name_), (description_), (required_))
#define DCC_OPT_USER(name_, description_, required_) \
    DCC_COMMAND_USER_OPTION((name_), (description_), (required_))
#define DCC_OPT_CHANNEL(name_, description_, required_) \
    DCC_COMMAND_CHANNEL_OPTION((name_), (description_), (required_))
#define DCC_OPT_ROLE(name_, description_, required_) \
    DCC_COMMAND_ROLE_OPTION((name_), (description_), (required_))
#define DCC_OPT_MENTIONABLE(name_, description_, required_) \
    DCC_COMMAND_MENTIONABLE_OPTION((name_), (description_), (required_))
#define DCC_OPT_NUMBER(name_, description_, required_) \
    DCC_COMMAND_NUMBER_OPTION((name_), (description_), (required_))
#define DCC_OPT_ATTACHMENT(name_, description_, required_) \
    DCC_COMMAND_ATTACHMENT_OPTION((name_), (description_), (required_))
#define DCC_COMMAND_STRING_OPTION_CHOICES_JSON(name_, description_, required_, choices_json_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_STRING, \
        .name = (name_), \
        .description = (description_), \
        .choices_json = (choices_json_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U \
    })
#define DCC_COMMAND_STRING_OPTION_AUTOCOMPLETE(name_, description_, required_) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_STRING, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U, \
        .autocomplete = 1U, \
        .has_autocomplete = 1U \
    })
#define DCC_OPT_STRING_CHOICES_JSON(name_, description_, required_, choices_json_) \
    DCC_COMMAND_STRING_OPTION_CHOICES_JSON((name_), (description_), (required_), (choices_json_))
#define DCC_OPT_STRING_AUTOCOMPLETE(name_, description_, required_) \
    DCC_COMMAND_STRING_OPTION_AUTOCOMPLETE((name_), (description_), (required_))
#define DCC_COMMAND_INTEGER_OPTION_RANGE(name_, description_, required_, min_, max_) \
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
#define DCC_OPT_INT_RANGE(name_, description_, required_, min_, max_) \
    DCC_COMMAND_INTEGER_OPTION_RANGE((name_), (description_), (required_), (min_), (max_))
#define DCC_OPT_INTEGER_RANGE(name_, description_, required_, min_, max_) \
    DCC_COMMAND_INTEGER_OPTION_RANGE((name_), (description_), (required_), (min_), (max_))
#define DCC_COMMAND_NUMBER_OPTION_RANGE(name_, description_, required_, min_, max_) \
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
#define DCC_OPT_NUMBER_RANGE(name_, description_, required_, min_, max_) \
    DCC_COMMAND_NUMBER_OPTION_RANGE((name_), (description_), (required_), (min_), (max_))
#define DCC_COMMAND_CHANNEL_OPTION_TYPES(name_, description_, required_, ...) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_CHANNEL, \
        .name = (name_), \
        .description = (description_), \
        .required = (required_) ? 1U : 0U, \
        .has_required = 1U, \
        .channel_types = DCC_SUGAR_ARRAY(uint32_t, __VA_ARGS__), \
        .channel_types_count = DCC_SUGAR_ARRAY_LEN(uint32_t, __VA_ARGS__) \
    })
#define DCC_OPT_CHANNEL_TYPES(name_, description_, required_, ...) \
    DCC_COMMAND_CHANNEL_OPTION_TYPES((name_), (description_), (required_), __VA_ARGS__)
#define DCC_COMMAND_SUBCOMMAND(name_, description_, ...) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND, \
        .name = (name_), \
        .description = (description_), \
        .options = DCC_SUGAR_ARRAY(dcc_application_command_option_builder_t, __VA_ARGS__), \
        .options_count = DCC_SUGAR_ARRAY_LEN(dcc_application_command_option_builder_t, __VA_ARGS__) \
    })
#ifndef DCC_CMD_SUB
#define DCC_CMD_SUB(name_, description_, ...) \
    DCC_COMMAND_SUBCOMMAND((name_), (description_), __VA_ARGS__)
#endif
#define DCC_COMMAND_SUBCOMMAND_GROUP(name_, description_, ...) \
    ((dcc_application_command_option_builder_t){ \
        .type = DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND_GROUP, \
        .name = (name_), \
        .description = (description_), \
        .options = DCC_SUGAR_ARRAY(dcc_application_command_option_builder_t, __VA_ARGS__), \
        .options_count = DCC_SUGAR_ARRAY_LEN(dcc_application_command_option_builder_t, __VA_ARGS__) \
    })
#ifndef DCC_CMD_GROUP
#define DCC_CMD_GROUP(name_, description_, ...) \
    DCC_COMMAND_SUBCOMMAND_GROUP((name_), (description_), __VA_ARGS__)
#endif

#define DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL() \
    ((dcc_command_registry_options_t){ \
        .size = sizeof(dcc_command_registry_options_t) \
    })
#define DCC_COMMAND_REGISTRY_OPTIONS_GUILD(guild_id_) \
    ((dcc_command_registry_options_t){ \
        .size = sizeof(dcc_command_registry_options_t), \
        .guild_id = (guild_id_) \
    })
#define DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL_DRY_RUN() \
    ((dcc_command_registry_options_t){ \
        .size = sizeof(dcc_command_registry_options_t), \
        .dry_run = 1U \
    })
#define DCC_COMMAND_REGISTRY_OPTIONS_GUILD_DRY_RUN(guild_id_) \
    ((dcc_command_registry_options_t){ \
        .size = sizeof(dcc_command_registry_options_t), \
        .guild_id = (guild_id_), \
        .dry_run = 1U \
    })
#define DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL_DELETE_STALE() \
    ((dcc_command_registry_options_t){ \
        .size = sizeof(dcc_command_registry_options_t), \
        .delete_stale = 1U \
    })
#define DCC_COMMAND_REGISTRY_OPTIONS_GUILD_DELETE_STALE(guild_id_) \
    ((dcc_command_registry_options_t){ \
        .size = sizeof(dcc_command_registry_options_t), \
        .guild_id = (guild_id_), \
        .delete_stale = 1U \
    })
#define DCC_APPLICATION_COMMAND_REGISTRATION_GLOBAL() \
    ((dcc_application_command_registration_options_t){ \
        .size = sizeof(dcc_application_command_registration_options_t) \
    })
#define DCC_APPLICATION_COMMAND_REGISTRATION_GUILD(guild_id_) \
    ((dcc_application_command_registration_options_t){ \
        .size = sizeof(dcc_application_command_registration_options_t), \
        .guild_id = (guild_id_) \
    })

#define DCC_AUTOCOMPLETE_STRING_CHOICE(name_, value_) \
    ((dcc_autocomplete_choice_t){ \
        .name = (name_), \
        .value_string = (value_), \
        .value_type = DCC_AUTOCOMPLETE_CHOICE_STRING, \
        .has_name = 1U, \
        .has_value = 1U \
    })
#define DCC_AUTOCOMPLETE_INTEGER_CHOICE(name_, value_) \
    ((dcc_autocomplete_choice_t){ \
        .name = (name_), \
        .value_integer = (value_), \
        .value_type = DCC_AUTOCOMPLETE_CHOICE_INTEGER, \
        .has_name = 1U, \
        .has_value = 1U \
    })
#define DCC_AUTOCOMPLETE_NUMBER_CHOICE(name_, value_) \
    ((dcc_autocomplete_choice_t){ \
        .name = (name_), \
        .value_number = (value_), \
        .value_type = DCC_AUTOCOMPLETE_CHOICE_NUMBER, \
        .has_name = 1U, \
        .has_value = 1U \
    })
#define DCC_AUTOCOMPLETE_CHOICES(...) \
    ((dcc_autocomplete_builder_t){ \
        .choices = DCC_SUGAR_ARRAY(dcc_autocomplete_choice_t, __VA_ARGS__), \
        .choices_count = DCC_SUGAR_ARRAY_LEN(dcc_autocomplete_choice_t, __VA_ARGS__) \
    })
#define DCC_AUTOCOMPLETE_CHOICES_ARRAY(choices_, choices_count_) \
    ((dcc_autocomplete_builder_t){ \
        .choices = (choices_), \
        .choices_count = (choices_count_) \
    })

#define DCC_COMPONENT_SESSION_OPTIONS(secret_, secret_len_) \
    ((dcc_component_session_options_t){ \
        .size = sizeof(dcc_component_session_options_t), \
        .secret = (secret_), \
        .secret_len = (secret_len_), \
        .ttl_ms = UINT64_C(900000) \
    })
#define DCC_COMPONENT_SESSION_OPTIONS_TTL(secret_, secret_len_, now_ms_, ttl_ms_) \
    ((dcc_component_session_options_t){ \
        .size = sizeof(dcc_component_session_options_t), \
        .secret = (secret_), \
        .secret_len = (secret_len_), \
        .now_ms = (now_ms_), \
        .ttl_ms = (ttl_ms_) \
    })
#define DCC_COMPONENT_SESSION_OPTIONS_USER(secret_, secret_len_, user_id_) \
    ((dcc_component_session_options_t){ \
        .size = sizeof(dcc_component_session_options_t), \
        .secret = (secret_), \
        .secret_len = (secret_len_), \
        .ttl_ms = UINT64_C(900000), \
        .user_id = (user_id_), \
        .lock_user = 1U \
    })
#define DCC_COMPONENT_SESSION_CHECK(custom_id_, now_ms_, user_id_, channel_id_, guild_id_) \
    ((dcc_component_session_check_t){ \
        .size = sizeof(dcc_component_session_check_t), \
        .custom_id = (custom_id_), \
        .now_ms = (now_ms_), \
        .user_id = (user_id_), \
        .channel_id = (channel_id_), \
        .guild_id = (guild_id_) \
    })
#define DCC_COMPONENT_SESSION_LISTENER_OPTIONS() \
    ((dcc_component_session_listener_options_t){ \
        .size = sizeof(dcc_component_session_listener_options_t), \
        .listen_buttons = 1U, \
        .listen_selects = 1U, \
        .listen_forms = 1U \
    })

#define DCC_HOT_RELOAD_OPTIONS() \
    ((dcc_hot_reload_options_t){ \
        .size = sizeof(dcc_hot_reload_options_t) \
    })
#define DCC_HOT_RELOAD_ISOLATED_OPTIONS(worker_path_) \
    ((dcc_hot_reload_options_t){ \
        .size = sizeof(dcc_hot_reload_options_t), \
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER, \
        .worker_path = (worker_path_) \
    })
#define DCC_HOT_RELOAD_ISOLATED_CANARY_OPTIONS(worker_path_, canary_options_) \
    ((dcc_hot_reload_options_t){ \
        .size = sizeof(dcc_hot_reload_options_t), \
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER, \
        .worker_path = (worker_path_), \
        .worker_canary_enabled = 1U, \
        .worker_canary_options = (canary_options_) \
    })
#define DCC_HOT_RELOAD_CANARY_OPTIONS(percent_, promote_after_ms_, max_error_rate_, max_consecutive_failures_) \
    ((dcc_hot_reload_canary_options_t){ \
        .size = sizeof(dcc_hot_reload_canary_options_t), \
        .canary_percent = (percent_), \
        .promote_after_ms = (promote_after_ms_), \
        .max_error_rate = (max_error_rate_), \
        .max_consecutive_failures = (max_consecutive_failures_) \
    })
#define DCC_HOT_RELOAD_CANARY_OPTIONS_DEFAULT() \
    DCC_HOT_RELOAD_CANARY_OPTIONS(5U, 30000U, 0.05, 3U)
#define DCC_HOT_RELOAD_RUN_OPTIONS(client_options_, library_path_, hot_reload_options_) \
    ((dcc_hot_reload_run_options_t){ \
        .size = sizeof(dcc_hot_reload_run_options_t), \
        .client_options = (client_options_), \
        .library_path = (library_path_), \
        .hot_reload_options = (hot_reload_options_) \
    })

#define DCC_REST_FIREWALL_OPTIONS() \
    ((dcc_rest_firewall_options_t){ \
        .size = sizeof(dcc_rest_firewall_options_t), \
        .invalid_request_soft_limit = DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_SOFT_LIMIT, \
        .invalid_request_hard_limit = DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_HARD_LIMIT, \
        .invalid_request_window_ms = DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_WINDOW_MS, \
        .soft_limit_delay_ms = DCC_REST_FIREWALL_DEFAULT_SOFT_LIMIT_DELAY_MS, \
        .on_hard_limit = DCC_REST_FIREWALL_HARD_LIMIT_REJECT_NONCRITICAL \
    })
#define DCC_REST_FIREWALL_OPTIONS_STRICT() \
    ((dcc_rest_firewall_options_t){ \
        .size = sizeof(dcc_rest_firewall_options_t), \
        .invalid_request_soft_limit = DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_SOFT_LIMIT, \
        .invalid_request_hard_limit = DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_HARD_LIMIT, \
        .invalid_request_window_ms = DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_WINDOW_MS, \
        .soft_limit_delay_ms = DCC_REST_FIREWALL_DEFAULT_SOFT_LIMIT_DELAY_MS, \
        .on_hard_limit = DCC_REST_FIREWALL_HARD_LIMIT_REJECT_ALL \
    })
#define DCC_REST_FIREWALL_REQUEST(method_, path_, now_ms_, critical_) \
    ((dcc_rest_firewall_request_t){ \
        .size = sizeof(dcc_rest_firewall_request_t), \
        .method = (method_), \
        .path = (path_), \
        .now_ms = (now_ms_), \
        .critical = (critical_) \
    })

#define DCC_REPLAY_GATEWAY_RECORD(event_, ts_ms_, payload_, payload_len_) \
    ((dcc_replay_record_t){ \
        .size = sizeof(dcc_replay_record_t), \
        .kind = DCC_REPLAY_GATEWAY, \
        .ts_ms = (ts_ms_), \
        .event = (event_), \
        .payload = (payload_), \
        .payload_len = (payload_len_) \
    })
#define DCC_REPLAY_INTERACTION_RECORD(ts_ms_, payload_, payload_len_) \
    ((dcc_replay_record_t){ \
        .size = sizeof(dcc_replay_record_t), \
        .kind = DCC_REPLAY_INTERACTION, \
        .ts_ms = (ts_ms_), \
        .payload = (payload_), \
        .payload_len = (payload_len_) \
    })
