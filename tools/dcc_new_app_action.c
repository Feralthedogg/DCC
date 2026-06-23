#include "internal/dcc_new_app.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_NEW_APP_HANDLER_MARKER "/* dcc_new_app:command-handlers */"
#define DCC_NEW_APP_SLASH_ROUTE_MARKER "        /* dcc_new_app:slash-routes */"
#define DCC_NEW_APP_PRESET_SLASH_ROUTE_MARKER "        /* dcc_new_app:preset-slash-routes */"
#define DCC_NEW_APP_ACTION_ROUTE_MARKER "        /* dcc_new_app:action-routes */"
#define DCC_NEW_APP_SUBCOMMAND_EXTENSION_MARKER "    /* dcc_new_app:subcommand-extension */"

static char *dcc_new_app_action_cog_source_path(const dcc_new_app_options_t *options) {
    char relative_path[320];
    snprintf(relative_path, sizeof(relative_path), "src/%s.c", options->cog_name);
    return dcc_new_app_join(options->path, relative_path);
}

static const char *dcc_new_app_action_kind(const dcc_new_app_options_t *options) {
    if (options == NULL || options->subcommand_arg == NULL) {
        return "send";
    }
    if (strcmp(options->subcommand_arg, "voice") == 0 ||
        strcmp(options->subcommand_arg, "voice_regions") == 0 ||
        strcmp(options->subcommand_arg, "guild-voice-regions") == 0) {
        return "voice-regions";
    }
    if (strcmp(options->subcommand_arg, "direct-message") == 0 ||
        strcmp(options->subcommand_arg, "direct_message") == 0) {
        return "dm";
    }
    if (strcmp(options->subcommand_arg, "role_add") == 0 ||
        strcmp(options->subcommand_arg, "add-role") == 0) {
        return "role-add";
    }
    if (strcmp(options->subcommand_arg, "role_remove") == 0 ||
        strcmp(options->subcommand_arg, "remove-role") == 0) {
        return "role-remove";
    }
    return options->subcommand_arg;
}

static int dcc_new_app_action_uses_typed_options(const dcc_new_app_options_t *options) {
    const char *kind = dcc_new_app_action_kind(options);
    return strcmp(kind, "dm") == 0 ||
           strcmp(kind, "role-add") == 0 ||
           strcmp(kind, "role-remove") == 0;
}

static char *dcc_new_app_action_last_before(char *haystack, const char *needle, const char *limit) {
    char *last = NULL;
    size_t needle_len = strlen(needle);
    for (char *p = strstr(haystack, needle); p != NULL && p < limit; p = strstr(p + needle_len, needle)) {
        last = p;
    }
    return last;
}

static int dcc_new_app_action_require_cog(char **out_path, const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_action_cog_source_path(options);
    if (path == NULL) {
        return -1;
    }
    if (!dcc_new_app_path_exists(path)) {
        fprintf(stderr, "%s does not exist; add the feature first\n", path);
        free(path);
        return -1;
    }
    *out_path = path;
    return 0;
}

static int dcc_new_app_action_insert_send_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    char insertion[1900];
    snprintf(
        insertion,
        sizeof(insertion),
        "DCC_HANDLER(%s) {\n"
        "    (void)user_data;\n"
        "\n"
        "    DCC_CTX_TRY(ctx, DCC_CTX_SEND_UI(\n"
        "        ctx,\n"
        "        DCC_UI_CARD_ACCENT(\n"
        "            0x5865F2,\n"
        "            DCC_UI_TEXT(\"## %s\"),\n"
        "            DCC_UI_TEXT(\"Generated send action. Edit this payload in src/%s.c.\")\n"
        "        )\n"
        "    ));\n"
        "    (void)DCC_CTX_DONE(ctx, \"Message queued.\");\n"
        "}\n"
        "\n",
        handler_name,
        options->command_name,
        options->cog_name
    );
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_HANDLER_MARKER, insertion, handler_name);
}

static int dcc_new_app_action_insert_gateway_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    (void)options;
    char insertion[900];
    snprintf(
        insertion,
        sizeof(insertion),
        "DCC_HANDLER(%s) {\n"
        "    (void)user_data;\n"
        "\n"
        "    DCC_CTX_TRY(ctx, DCC_GATEWAY_BOT_FETCH(DCC_CTX_APP(ctx)));\n"
        "    (void)DCC_CTX_DONE(ctx, \"Gateway bot fetch queued.\");\n"
        "}\n"
        "\n",
        handler_name
    );
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_HANDLER_MARKER, insertion, handler_name);
}

static int dcc_new_app_action_insert_voice_regions_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    (void)options;
    char insertion[1300];
    snprintf(
        insertion,
        sizeof(insertion),
        "DCC_HANDLER(%s) {\n"
        "    (void)user_data;\n"
        "\n"
        "    DCC_CTX_REQUIRE_BAD_INPUT(ctx, DCC_CTX_IS_GUILD(ctx), \"Use this action in a guild.\");\n"
        "    dcc_snowflake_t guild_id = DCC_CTX_GUILD_ID(ctx);\n"
        "\n"
        "    DCC_CTX_TRY(ctx, DCC_GUILD_VOICE_REGIONS_FETCH(DCC_CTX_APP(ctx), guild_id));\n"
        "    (void)DCC_CTX_DONE(ctx, \"Guild voice regions fetch queued.\");\n"
        "}\n"
        "\n",
        handler_name
    );
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_HANDLER_MARKER, insertion, handler_name);
}

static int dcc_new_app_action_insert_dm_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    char args_type[360];
    char params_macro[360];
    snprintf(args_type, sizeof(args_type), "%s_args_t", handler_name);
    dcc_new_app_make_macro_name(handler_name, "_PARAMS", params_macro, sizeof(params_macro));

    char insertion[2500];
    snprintf(
        insertion,
        sizeof(insertion),
        "typedef struct %s_args {\n"
        "    const dcc_user_t *user;\n"
        "    const char *message;\n"
        "} %s_args_t;\n"
        "\n"
        "#define %s(PARAM, type) \\\n"
        "    PARAM(type, USER, user, \"user\", \"User to DM\", 1U) \\\n"
        "    PARAM(type, STRING, message, \"message\", \"Message to send\", 0U, \"Hello from DCC.\")\n"
        "\n"
        "DCC_COMMAND_ARGS_IMPL(%s, %s, command) {\n"
        "    (void)user_data;\n"
        "\n"
        "    DCC_CTX_REQUIRE_BAD_INPUT(\n"
        "        ctx,\n"
        "        command != NULL && command->user != NULL && command->user->id != 0U,\n"
        "        \"Choose a user to DM.\"\n"
        "    );\n"
        "    const char *message = command->message != NULL && command->message[0] != '\\0'\n"
        "        ? command->message\n"
        "        : \"Hello from DCC.\";\n"
        "\n"
        "    DCC_CTX_TRY(ctx, DCC_USER_DM_SEND_TEXT(DCC_CTX_APP(ctx), command->user->id, message));\n"
        "    (void)DCC_CTX_DONE(ctx, \"DM queued.\");\n"
        "}\n"
        "\n",
        handler_name,
        handler_name,
        params_macro,
        handler_name,
        args_type
    );
    (void)options;
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_HANDLER_MARKER, insertion, handler_name);
}

static int dcc_new_app_action_insert_role_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name,
    int add_role
) {
    char args_type[360];
    char params_macro[360];
    snprintf(args_type, sizeof(args_type), "%s_args_t", handler_name);
    dcc_new_app_make_macro_name(handler_name, "_PARAMS", params_macro, sizeof(params_macro));

    char insertion[2900];
    snprintf(
        insertion,
        sizeof(insertion),
        "typedef struct %s_args {\n"
        "    const dcc_user_t *user;\n"
        "    const dcc_role_t *role;\n"
        "} %s_args_t;\n"
        "\n"
        "#define %s(PARAM, type) \\\n"
        "    PARAM(type, USER, user, \"user\", \"Target user\", 1U) \\\n"
        "    PARAM(type, ROLE, role, \"role\", \"Role to %s\", 1U)\n"
        "\n"
        "DCC_COMMAND_ARGS_IMPL(%s, %s, command) {\n"
        "    (void)user_data;\n"
        "\n"
        "    DCC_CTX_REQUIRE_BAD_INPUT(ctx, DCC_CTX_IS_GUILD(ctx), \"Use this action in a guild.\");\n"
        "    dcc_snowflake_t guild_id = DCC_CTX_GUILD_ID(ctx);\n"
        "    DCC_CTX_REQUIRE_BAD_INPUT(\n"
        "        ctx,\n"
        "        command != NULL && command->user != NULL && command->user->id != 0U &&\n"
        "            command->role != NULL && command->role->id != 0U,\n"
        "        \"Choose a user and role.\"\n"
        "    );\n"
        "\n"
        "    DCC_CTX_TRY(ctx, %s(DCC_CTX_APP(ctx), guild_id, command->user->id, command->role->id));\n"
        "    (void)DCC_CTX_DONE(ctx, \"Role %s queued.\");\n"
        "}\n"
        "\n",
        handler_name,
        handler_name,
        params_macro,
        add_role ? "add" : "remove",
        handler_name,
        args_type,
        add_role ? "DCC_MEMBER_ADD_ROLE" : "DCC_MEMBER_REMOVE_ROLE",
        add_role ? "add" : "remove"
    );
    (void)options;
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_HANDLER_MARKER, insertion, handler_name);
}

static int dcc_new_app_action_insert_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    const char *kind = dcc_new_app_action_kind(options);
    if (strcmp(kind, "gateway") == 0) {
        return dcc_new_app_action_insert_gateway_handler(path, options, handler_name);
    }
    if (strcmp(kind, "voice-regions") == 0) {
        return dcc_new_app_action_insert_voice_regions_handler(path, options, handler_name);
    }
    if (strcmp(kind, "dm") == 0) {
        return dcc_new_app_action_insert_dm_handler(path, options, handler_name);
    }
    if (strcmp(kind, "role-add") == 0) {
        return dcc_new_app_action_insert_role_handler(path, options, handler_name, 1);
    }
    if (strcmp(kind, "role-remove") == 0) {
        return dcc_new_app_action_insert_role_handler(path, options, handler_name, 0);
    }
    return dcc_new_app_action_insert_send_handler(path, options, handler_name);
}

static int dcc_new_app_action_insert_route(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    size_t len = 0U;
    char *data = dcc_new_app_read_file(path, &len);
    if (data == NULL) {
        fprintf(stderr, "could not read %s for marker update\n", path);
        return -1;
    }

    char policy[1000];
    int guarded = dcc_new_app_guard_policy_literal(options, policy, sizeof(policy));
    if (guarded < 0) {
        free(data);
        return -1;
    }

    char duplicate_needle[320];
    if (dcc_new_app_action_uses_typed_options(options)) {
        snprintf(
            duplicate_needle,
            sizeof(duplicate_needle),
            "\"%s\",\n"
            "            \"Generated %s action\"",
            options->command_name,
            dcc_new_app_action_kind(options)
        );
    } else {
        snprintf(
            duplicate_needle,
            sizeof(duplicate_needle),
            "DCC_COMMAND_DATA%s(\"%s\"",
            guarded ? "_POLICY" : "",
            options->command_name
        );
    }
    if (strstr(data, duplicate_needle) != NULL) {
        free(data);
        return 0;
    }

    char route[1800];
    if (dcc_new_app_action_uses_typed_options(options)) {
        char args_type[360];
        char params_macro[360];
        snprintf(args_type, sizeof(args_type), "%s_args_t", handler_name);
        dcc_new_app_make_macro_name(handler_name, "_PARAMS", params_macro, sizeof(params_macro));
        snprintf(
            route,
            sizeof(route),
            "        %s(\n"
            "            \"%s\",\n"
            "            \"Generated %s action\",\n"
            "            %s,\n"
            "            %s,\n"
            "            user_data,\n"
            "            %s%s%s\n"
            "        )\n",
            guarded ? "DCC_COMMAND_ROUTE_DATA_POLICY" : "DCC_COMMAND_ROUTE_DATA",
            options->command_name,
            dcc_new_app_action_kind(options),
            args_type,
            handler_name,
            params_macro,
            guarded ? ",\n            " : "",
            guarded ? policy : ""
        );
    } else {
        snprintf(
            route,
            sizeof(route),
            "        %s(\"%s\", \"Generated %s action\", %s, user_data%s%s%s)\n",
            guarded ? "DCC_COMMAND_DATA_POLICY" : "DCC_COMMAND_DATA",
            options->command_name,
            dcc_new_app_action_kind(options),
            handler_name,
            guarded ? ", " : "",
            guarded ? policy : "",
            ""
        );
    }

    char *slash_marker = strstr(data, DCC_NEW_APP_SLASH_ROUTE_MARKER);
    if (slash_marker != NULL) {
        char *plain_slashes = dcc_new_app_action_last_before(data, "DCC_FEATURE_SLASHES(", slash_marker);
        char *plain_commands = dcc_new_app_action_last_before(data, "DCC_FEATURE_COMMANDS(", slash_marker);
        char *plain = plain_commands != NULL && (plain_slashes == NULL || plain_commands > plain_slashes)
            ? plain_commands
            : plain_slashes;
        char *typed_slashes = dcc_new_app_action_last_before(data, "DCC_FEATURE_TYPED_SLASHES(", slash_marker);
        char *typed_command_routes = dcc_new_app_action_last_before(data, "DCC_FEATURE_COMMAND_ROUTES(", slash_marker);
        char *typed_commands_old = dcc_new_app_action_last_before(data, "DCC_FEATURE_TYPED_COMMANDS(", slash_marker);
        char *typed_commands = typed_command_routes != NULL &&
            (typed_commands_old == NULL || typed_command_routes > typed_commands_old)
            ? typed_command_routes
            : typed_commands_old;
        char *typed = typed_commands != NULL && (typed_slashes == NULL || typed_commands > typed_slashes)
            ? typed_commands
            : typed_slashes;
        int slash_marker_is_typed = typed != NULL && (plain == NULL || typed > plain);
        if (dcc_new_app_action_uses_typed_options(options) && slash_marker_is_typed) {
            char insertion[2100];
            snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
            int status = dcc_new_app_replace_range(path, data, len, slash_marker, 0U, insertion);
            free(data);
            return status;
        }
        if (!dcc_new_app_action_uses_typed_options(options) && plain != NULL && !slash_marker_is_typed) {
            char insertion[2100];
            snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
            int status = dcc_new_app_replace_range(path, data, len, slash_marker, 0U, insertion);
            free(data);
            return status;
        }
    }

    if (!dcc_new_app_action_uses_typed_options(options)) {
        char *action_marker = strstr(data, DCC_NEW_APP_ACTION_ROUTE_MARKER);
        if (action_marker != NULL) {
            char insertion[2100];
            snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
            int status = dcc_new_app_replace_range(path, data, len, action_marker, 0U, insertion);
            free(data);
            return status;
        }

        char *preset_marker = strstr(data, DCC_NEW_APP_PRESET_SLASH_ROUTE_MARKER);
        if (preset_marker != NULL) {
            char insertion[2100];
            snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
            int status = dcc_new_app_replace_range(path, data, len, preset_marker, 0U, insertion);
            free(data);
            return status;
        }
    }

    char *extension = strstr(data, DCC_NEW_APP_SUBCOMMAND_EXTENSION_MARKER);
    if (extension == NULL) {
        fprintf(stderr, "warning: action marker target not found in %s\n", path);
        free(data);
        return 0;
    }

    char block[2300];
    snprintf(
        block,
        sizeof(block),
        "    ,\n"
        "    %s(\n"
        "%s"
        "%s\n"
        "%s\n"
        "    )\n",
        dcc_new_app_action_uses_typed_options(options) ? "DCC_FEATURE_COMMAND_ROUTES" : "DCC_FEATURE_COMMANDS",
        route,
        dcc_new_app_action_uses_typed_options(options) ? DCC_NEW_APP_SLASH_ROUTE_MARKER : DCC_NEW_APP_PRESET_SLASH_ROUTE_MARKER,
        dcc_new_app_action_uses_typed_options(options) ? "" : DCC_NEW_APP_ACTION_ROUTE_MARKER
    );
    int status = dcc_new_app_replace_range(path, data, len, extension, 0U, block);
    free(data);
    return status;
}

static int dcc_new_app_action_append_command_json(const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_join(options->path, "commands.json");
    if (path == NULL) {
        return -1;
    }
    size_t len = 0U;
    char *data = dcc_new_app_read_file(path, &len);
    if (data == NULL) {
        free(path);
        return 0;
    }

    char name_needle[192];
    snprintf(name_needle, sizeof(name_needle), "\"name\": \"%s\"", options->command_name);
    if (strstr(data, name_needle) != NULL) {
        free(data);
        free(path);
        return 0;
    }

    char *last_bracket = strrchr(data, ']');
    if (last_bracket == NULL) {
        fprintf(stderr, "warning: commands.json is not a JSON array; %s command was not appended\n", options->command_name);
        free(data);
        free(path);
        return 0;
    }

    int has_existing = strchr(data, '{') != NULL;
    const char *kind = dcc_new_app_action_kind(options);
    char options_json[900];
    snprintf(options_json, sizeof(options_json), "\n");
    if (strcmp(kind, "dm") == 0) {
        snprintf(
            options_json,
            sizeof(options_json),
            ",\n"
            "    \"options\": [\n"
            "      {\n"
            "        \"name\": \"user\",\n"
            "        \"description\": \"User to DM\",\n"
            "        \"type\": 6,\n"
            "        \"required\": true\n"
            "      },\n"
            "      {\n"
            "        \"name\": \"message\",\n"
            "        \"description\": \"Message to send\",\n"
            "        \"type\": 3,\n"
            "        \"required\": false\n"
            "      }\n"
            "    ]\n"
        );
    } else if (strcmp(kind, "role-add") == 0 || strcmp(kind, "role-remove") == 0) {
        snprintf(
            options_json,
            sizeof(options_json),
            ",\n"
            "    \"options\": [\n"
            "      {\n"
            "        \"name\": \"user\",\n"
            "        \"description\": \"Target user\",\n"
            "        \"type\": 6,\n"
            "        \"required\": true\n"
            "      },\n"
            "      {\n"
            "        \"name\": \"role\",\n"
            "        \"description\": \"Role to %s\",\n"
            "        \"type\": 8,\n"
            "        \"required\": true\n"
            "      }\n"
            "    ]\n",
            strcmp(kind, "role-add") == 0 ? "add" : "remove"
        );
    }

    char command[1500];
    snprintf(
        command,
        sizeof(command),
        "%s\n"
        "  {\n"
        "    \"name\": \"%s\",\n"
        "    \"description\": \"Generated %s action\",\n"
        "    \"type\": 1%s"
        "  }\n",
        has_existing ? "," : "",
        options->command_name,
        kind,
        options_json
    );

    size_t prefix_len = (size_t)(last_bracket - data);
    while (prefix_len > 0U && isspace((unsigned char)data[prefix_len - 1U])) {
        --prefix_len;
    }
    size_t command_len = strlen(command);
    size_t suffix_len = len - (size_t)(last_bracket - data);
    size_t total = prefix_len + command_len + suffix_len;
    char *next = (char *)malloc(total + 1U);
    if (next == NULL) {
        free(data);
        free(path);
        return -1;
    }
    memcpy(next, data, prefix_len);
    memcpy(next + prefix_len, command, command_len);
    memcpy(next + prefix_len + command_len, last_bracket, suffix_len);
    next[total] = '\0';

    int status = dcc_new_app_write_file_all(path, next, total);
    free(next);
    free(data);
    free(path);
    return status;
}

int dcc_new_app_generate_action(const dcc_new_app_options_t *options) {
    if (!dcc_new_app_path_exists(options->path)) {
        fprintf(stderr, "%s does not exist; create the app first\n", options->path);
        return -1;
    }

    char *path = NULL;
    if (dcc_new_app_action_require_cog(&path, options) != 0) {
        return -1;
    }

    char handler_name[320];
    snprintf(handler_name, sizeof(handler_name), "on_%s_%s_action", options->cog_name, options->command_name);
    int status = dcc_new_app_action_insert_handler(path, options, handler_name);
    if (status == 0) {
        status = dcc_new_app_action_insert_route(path, options, handler_name);
    }
    free(path);
    if (status == 0) {
        status = dcc_new_app_action_append_command_json(options);
    }
    if (status != 0) {
        return -1;
    }

    printf(
        "created DCC action %s (%s) in feature %s under %s\n",
        options->command_name,
        dcc_new_app_action_kind(options),
        options->cog_name,
        options->path
    );
    return 0;
}
