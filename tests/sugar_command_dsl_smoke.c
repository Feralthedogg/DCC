#include <dcc/sugar.h>

#include <stdio.h>
#include <string.h>

typedef struct command_dsl_search_args {
    const char *query;
    int64_t limit;
    const dcc_user_t *user;
} command_dsl_search_args_t;

static int command_dsl_listener_state;

#define COMMAND_DSL_SEARCH_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_STRING, query, "query", "Search query") \
    PARAM(type, INT_RANGE, limit, "limit", "Result count", 0U, 10, 1, 25) \
    PARAM(type, USER, user, "user", "Restrict to user", 0U)

DCC_TYPED_HANDLER(on_command_dsl_search) {
    DCC_TYPED_ARGS(command_dsl_search_args_t, search);
    (void)ctx;
    (void)search;
    (void)user_data;
}

static int require_contains(const char *json, const char *needle) {
    if (json == NULL || strstr(json, needle) == NULL) {
        fprintf(stderr, "missing JSON fragment: %s\njson=%s\n", needle, json != NULL ? json : "(null)");
        return 1;
    }
    return 0;
}

static int check_manual_command_dsl(void) {
    dcc_application_command_builder_t ping = DCC_SLASH_CMD("ping", "Latency check");
    if (ping.type != DCC_APPLICATION_COMMAND_CHAT_INPUT ||
        ping.has_name != 1U ||
        ping.has_description != 1U ||
        strcmp(ping.name, "ping") != 0) {
        return 1;
    }

    dcc_application_command_builder_t command =
        DCC_SLASH_OPTIONS(
            "search",
            "Search members",
            DCC_REQUIRED_STRING("query", "Search query"),
            DCC_OPTIONAL_INT_RANGE("limit", "Result count", 1, 25),
            DCC_OPTIONAL_CHANNEL_TYPES("channel", "Restrict to channel", 0U, 5U),
            DCC_OPTIONAL_STRING_AUTOCOMPLETE("tag", "Tag filter")
        );

    if (command.options_count != 4U ||
        command.options[0].required != 1U ||
        command.options[1].min_integer_value != 1 ||
        command.options[1].max_integer_value != 25 ||
        command.options[2].channel_types_count != 2U ||
        command.options[3].autocomplete != 1U) {
        return 1;
    }

    char *json = NULL;
    if (dcc_application_command_builder_build_json(&command, &json) != DCC_OK) {
        return 1;
    }

    int failed =
        require_contains(json, "\"name\":\"search\"") ||
        require_contains(json, "\"name\":\"query\"") ||
        require_contains(json, "\"required\":true") ||
        require_contains(json, "\"name\":\"channel\"") ||
        require_contains(json, "\"channel_types\":[0,5]") ||
        require_contains(json, "\"autocomplete\":true");
    dcc_application_command_builder_json_free(json);
    return failed;
}

static int check_subcommand_dsl(void) {
    dcc_application_command_builder_t command =
        DCC_SLASH_OPTIONS(
            "admin",
            "Admin tools",
            DCC_SUB_GROUP(
                "moderation",
                "Moderation",
                DCC_SUB_CMD(
                    "ban",
                    "Ban a member",
                    DCC_REQUIRED_USER("member", "Member"),
                    DCC_OPTIONAL_STRING("reason", "Reason")
                )
            )
        );

    if (command.options_count != 1U ||
        command.options[0].type != DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND_GROUP ||
        command.options[0].options_count != 1U ||
        command.options[0].options[0].type != DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND ||
        command.options[0].options[0].options_count != 2U) {
        return 1;
    }

    char *json = NULL;
    if (dcc_application_command_builder_build_json(&command, &json) != DCC_OK) {
        return 1;
    }

    int failed =
        require_contains(json, "\"name\":\"admin\"") ||
        require_contains(json, "\"name\":\"moderation\"") ||
        require_contains(json, "\"type\":2") ||
        require_contains(json, "\"name\":\"ban\"") ||
        require_contains(json, "\"type\":1") ||
        require_contains(json, "\"name\":\"member\"");
    dcc_application_command_builder_json_free(json);
    return failed;
}

static int check_command_group_aliases(void) {
    dcc_application_command_builder_t ping = DCC_CMD("ping", "Latency check");
    dcc_application_command_builder_t command =
        DCC_SLASH_GROUP(
            "admin",
            "Admin tools",
            DCC_CMD_GROUP(
                "moderation",
                "Moderation",
                DCC_CMD_SUB(
                    "ban",
                    "Ban a member",
                    DCC_REQUIRED_USER("member", "Member")
                )
            )
        );
    dcc_application_command_builder_t options_command =
        DCC_CMD_OPTIONS(
            "lookup",
            "Lookup tools",
            DCC_COMMAND_GROUP(
                "member",
                "Member tools",
                DCC_COMMAND_SUB(
                    "show",
                    "Show a member",
                    DCC_OPTIONAL_USER("user", "Member")
                )
            )
        );

    if (ping.type != DCC_APPLICATION_COMMAND_CHAT_INPUT ||
        command.options_count != 1U ||
        command.options[0].type != DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND_GROUP ||
        command.options[0].options_count != 1U ||
        command.options[0].options[0].type != DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND ||
        options_command.options_count != 1U ||
        options_command.options[0].options[0].options_count != 1U) {
        return 1;
    }

    char *json = NULL;
    if (dcc_application_command_builder_build_json(&command, &json) != DCC_OK) {
        return 1;
    }

    int failed =
        require_contains(json, "\"name\":\"admin\"") ||
        require_contains(json, "\"name\":\"moderation\"") ||
        require_contains(json, "\"name\":\"ban\"");
    dcc_application_command_builder_json_free(json);
    return failed;
}

static int check_choice_dsl(void) {
    dcc_application_command_builder_t command =
        DCC_SLASH_OPTIONS(
            "report",
            "Report status",
            DCC_REQUIRED_STRING_CHOICES(
                "mode",
                "Mode",
                DCC_STRING_CHOICE("Fast", "fast"),
                DCC_STRING_CHOICE_LOCALIZED("Careful", "careful", "{\"ko\":\"Careful KO\"}")
            ),
            DCC_OPTIONAL_INT_CHOICES(
                "priority",
                "Priority",
                DCC_INT_CHOICE("Low", 1),
                DCC_INT_CHOICE("High", 5)
            ),
            DCC_OPTIONAL_NUMBER_CHOICES(
                "ratio",
                "Ratio",
                DCC_NUMBER_CHOICE("Half", 0.5),
                DCC_NUMBER_CHOICE("Full", 1.0)
            )
        );

    if (command.options_count != 3U ||
        command.options[0].choices_count != 2U ||
        command.options[1].choices[1].value_integer != 5 ||
        command.options[2].choices[0].value_number < 0.49 ||
        command.options[2].choices[0].value_number > 0.51) {
        return 1;
    }

    char *json = NULL;
    if (dcc_application_command_builder_build_json(&command, &json) != DCC_OK) {
        return 1;
    }

    int failed =
        require_contains(json, "\"name\":\"mode\"") ||
        require_contains(json, "\"choices\":[{\"name\":\"Fast\",\"value\":\"fast\"") ||
        require_contains(json, "\"name_localizations\":{\"ko\":\"Careful KO\"}") ||
        require_contains(json, "\"name\":\"priority\"") ||
        require_contains(json, "\"value\":5") ||
        require_contains(json, "\"name\":\"ratio\"") ||
        require_contains(json, "\"value\":0.5");
    dcc_application_command_builder_json_free(json);
    return failed;
}

static int check_invalid_choice_type(void) {
    dcc_application_command_builder_t command =
        DCC_SLASH_OPTIONS(
            "bad",
            "Bad command",
            DCC_REQUIRED_STRING_CHOICES("mode", "Mode", DCC_INT_CHOICE("Wrong", 1))
        );
    char *json = NULL;
    dcc_status_t status = dcc_application_command_builder_build_json(&command, &json);
    dcc_application_command_builder_json_free(json);
    return status == DCC_ERR_INVALID_ARG ? 0 : 1;
}

static int check_typed_route_aliases(void) {
    dcc_app_typed_slash_command_t route =
        DCC_COMMAND_ROUTE("search", "Search members", command_dsl_search_args_t, on_command_dsl_search, COMMAND_DSL_SEARCH_PARAMS);
    if (route.command == NULL ||
        route.args_size != sizeof(command_dsl_search_args_t) ||
        route.bindings.count != 3U ||
        route.command->options_count != 3U ||
        route.handler != on_command_dsl_search) {
        return 1;
    }

    dcc_app_typed_subcommand_t subcommand =
        DCC_SUBCOMMAND_ROUTE(
            "admin",
            "moderation/ban",
            command_dsl_search_args_t,
            on_command_dsl_search,
            COMMAND_DSL_SEARCH_PARAMS
        );
    return subcommand.command_name != NULL &&
        strcmp(subcommand.command_name, "admin") == 0 &&
        subcommand.subcommand_path != NULL &&
        strcmp(subcommand.subcommand_path, "moderation/ban") == 0 &&
        subcommand.bindings.count == 3U
        ? 0
        : 1;
}

static int check_typed_listener_aliases(void) {
    dcc_sugar_listener_t slash =
        DCC_LISTEN_SLASH_PARAMS(
            "search",
            "Search members",
            command_dsl_search_args_t,
            on_command_dsl_search,
            COMMAND_DSL_SEARCH_PARAMS
        );
    if (slash.kind != DCC_SUGAR_LISTENER_TYPED_SLASH ||
        slash.typed_slash.command == NULL ||
        strcmp(slash.typed_slash.command->name, "search") != 0 ||
        slash.typed_slash.command->options_count != 3U ||
        slash.typed_slash.bindings.count != 3U ||
        slash.typed_slash.handler != on_command_dsl_search) {
        return 1;
    }

    dcc_sugar_listener_t slash_function_named =
        DCC_LISTEN_COMMAND_FN_DATA_GUARDED(
            on_command_dsl_search,
            "Search members",
            command_dsl_search_args_t,
            &command_dsl_listener_state,
            COMMAND_DSL_SEARCH_PARAMS,
            DCC_ONLY_GUILD()
        );
    if (slash_function_named.kind != DCC_SUGAR_LISTENER_TYPED_SLASH ||
        slash_function_named.typed_slash.command == NULL ||
        strcmp(slash_function_named.typed_slash.command->name, "on_command_dsl_search") != 0 ||
        slash_function_named.typed_slash.user_data != &command_dsl_listener_state ||
        slash_function_named.typed_slash.policy.guild_only != 1U) {
        return 1;
    }

    dcc_sugar_listener_t schema =
        DCC_LISTEN_COMMAND_SCHEMA(
            DCC_CMD_TREE(
                "admin",
                "Admin tools",
                DCC_CMD_GROUP(
                    "moderation",
                    "Moderation tools",
                    DCC_CMD_SUB(
                        "ban",
                        "Ban a member",
                        DCC_REQUIRED_USER("member", "Member")
                    )
                )
            )
        );
    if (schema.kind != DCC_SUGAR_LISTENER_COMMAND_SCHEMA ||
        strcmp(schema.command_schema.name, "admin") != 0 ||
        schema.command_schema.options_count != 1U ||
        schema.command_schema.options[0].options_count != 1U) {
        return 1;
    }

    dcc_sugar_listener_t subcommand =
        DCC_LISTEN_SUBCOMMAND_PARAMS(
            "admin",
            "moderation/ban",
            command_dsl_search_args_t,
            on_command_dsl_search,
            COMMAND_DSL_SEARCH_PARAMS
        );
    if (subcommand.kind != DCC_SUGAR_LISTENER_TYPED_SUBCOMMAND ||
        strcmp(subcommand.typed_subcommand.command_name, "admin") != 0 ||
        strcmp(subcommand.typed_subcommand.subcommand_path, "moderation/ban") != 0 ||
        subcommand.typed_subcommand.bindings.count != 3U ||
        subcommand.typed_subcommand.handler != on_command_dsl_search) {
        return 1;
    }

    dcc_sugar_listener_t grouped =
        DCC_LISTEN_SUBCOMMAND_IN_PARAMS_DATA_GUARDED(
            "admin",
            "moderation",
            "kick",
            command_dsl_search_args_t,
            on_command_dsl_search,
            &command_dsl_listener_state,
            COMMAND_DSL_SEARCH_PARAMS,
            DCC_ONLY_GUILD()
        );
    if (grouped.kind != DCC_SUGAR_LISTENER_TYPED_SUBCOMMAND ||
        strcmp(grouped.typed_subcommand.subcommand_path, "moderation/kick") != 0 ||
        grouped.typed_subcommand.user_data != &command_dsl_listener_state ||
        grouped.typed_subcommand.policy.guild_only != 1U) {
        return 1;
    }

    dcc_sugar_listener_t function_named =
        DCC_LISTEN_SUBCOMMAND_IN_FN_DATA(
            "admin",
            "moderation",
            on_command_dsl_search,
            command_dsl_search_args_t,
            &subcommand,
            COMMAND_DSL_SEARCH_PARAMS
        );
    if (function_named.kind != DCC_SUGAR_LISTENER_TYPED_SUBCOMMAND ||
        strcmp(function_named.typed_subcommand.subcommand_path, "moderation/on_command_dsl_search") != 0 ||
        function_named.typed_subcommand.user_data != &subcommand) {
        return 1;
    }

    dcc_sugar_listener_t autocomplete =
        DCC_LISTEN_SUBCOMMAND_AUTOCOMPLETE_IN_PARAMS_DATA(
            "admin",
            "moderation",
            "ban",
            command_dsl_search_args_t,
            on_command_dsl_search,
            &command_dsl_listener_state,
            COMMAND_DSL_SEARCH_PARAMS
        );
    if (autocomplete.kind != DCC_SUGAR_LISTENER_TYPED_AUTOCOMPLETE ||
        strcmp(autocomplete.typed_autocomplete.command_name, "admin") != 0 ||
        strcmp(autocomplete.typed_autocomplete.subcommand_path, "moderation/ban") != 0 ||
        autocomplete.typed_autocomplete.user_data != &command_dsl_listener_state ||
        autocomplete.typed_autocomplete.bindings.count != 3U) {
        return 1;
    }

    dcc_app_options_t options = DCC_APP_OPTIONS("token", DCC_INTENTS_DEFAULT);
    dcc_app_t *app = NULL;
    if (dcc_app_create(&options, &app) != DCC_OK) {
        return 1;
    }
    int ok =
        dcc_sugar_listener_register(app, &schema) == DCC_OK &&
        dcc_command_registry_count(dcc_app_command_registry(app)) == 1U;
    dcc_app_destroy(app);
    if (!ok) {
        return 1;
    }

    return 0;
}

int main(void) {
    if (check_manual_command_dsl() != 0) {
        return 1;
    }
    if (check_subcommand_dsl() != 0) {
        return 1;
    }
    if (check_command_group_aliases() != 0) {
        return 1;
    }
    if (check_choice_dsl() != 0) {
        return 1;
    }
    if (check_invalid_choice_type() != 0) {
        return 1;
    }
    if (check_typed_route_aliases() != 0) {
        return 1;
    }
    if (check_typed_listener_aliases() != 0) {
        return 1;
    }
    return 0;
}
