#include "internal/dcc_new_app.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_NEW_APP_HANDLER_MARKER "/* dcc_new_app:command-handlers */"
#define DCC_NEW_APP_SLASH_ROUTE_MARKER "        /* dcc_new_app:slash-routes */"
#define DCC_NEW_APP_LATEST_ROUTE_MARKER "        /* dcc_new_app:latest-message-routes */"
#define DCC_NEW_APP_PRESET_SLASH_ROUTE_MARKER "        /* dcc_new_app:preset-slash-routes */"
#define DCC_NEW_APP_SUBCOMMAND_EXTENSION_MARKER "    /* dcc_new_app:subcommand-extension */"
#define DCC_NEW_APP_TASK_ROUTE_MARKER "        /* dcc_new_app:task-routes */"
#define DCC_NEW_APP_TASK_EXTENSION_MARKER "    /* dcc_new_app:task-extension */"

static char *dcc_new_app_latest_cog_source_path(const dcc_new_app_options_t *options) {
    char relative_path[320];
    snprintf(relative_path, sizeof(relative_path), "src/%s.c", options->cog_name);
    return dcc_new_app_join(options->path, relative_path);
}

static int dcc_new_app_latest_field_valid(const char *field) {
    if (field == NULL || field[0] == '\0') {
        return 0;
    }
    if (!(isalpha((unsigned char)field[0]) || field[0] == '_')) {
        return 0;
    }
    for (const unsigned char *p = (const unsigned char *)field; *p != '\0'; ++p) {
        if (!(isalnum(*p) || *p == '_')) {
            return 0;
        }
    }
    return 1;
}

static int dcc_new_app_latest_require_cog(char **out_path, const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_latest_cog_source_path(options);
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

static int dcc_new_app_latest_file_contains(const char *path, const char *needle) {
    size_t len = 0U;
    char *data = dcc_new_app_read_file(path, &len);
    (void)len;
    if (data == NULL) {
        fprintf(stderr, "could not read %s for marker update\n", path);
        return -1;
    }
    int found = strstr(data, needle) != NULL;
    free(data);
    return found;
}

static int dcc_new_app_latest_uint_literal_safe(const char *value) {
    if (value == NULL || value[0] == '\0') {
        return 0;
    }
    int saw_nonzero = 0;
    for (const unsigned char *p = (const unsigned char *)value; *p != '\0'; ++p) {
        if (!isdigit(*p)) {
            return 0;
        }
        saw_nonzero = saw_nonzero || *p != '0';
    }
    return saw_nonzero;
}

static int dcc_new_app_latest_hhmm_safe(const char *value) {
    if (value == NULL || !isdigit((unsigned char)value[0]) || !isdigit((unsigned char)value[1]) ||
        value[2] != ':' || !isdigit((unsigned char)value[3]) || !isdigit((unsigned char)value[4]) ||
        value[5] != '\0') {
        return 0;
    }
    unsigned hour = (unsigned)(value[0] - '0') * 10U + (unsigned)(value[1] - '0');
    unsigned minute = (unsigned)(value[3] - '0') * 10U + (unsigned)(value[4] - '0');
    return hour < 24U && minute < 60U;
}

static const char *dcc_new_app_latest_task_macro(dcc_new_app_task_schedule_t schedule) {
    switch (schedule) {
        case DCC_NEW_APP_TASK_MS:
            return "DCC_TASK_LOOP_MS_DATA";
        case DCC_NEW_APP_TASK_MINUTES:
            return "DCC_TASK_LOOP_MINUTES_DATA";
        case DCC_NEW_APP_TASK_HOURS:
            return "DCC_TASK_LOOP_HOURS_DATA";
        case DCC_NEW_APP_TASK_DAILY_KST:
            return "DCC_TASK_LOOP_KST_DATA";
        case DCC_NEW_APP_TASK_SECONDS:
        default:
            return "DCC_TASK_LOOP_SECONDS_DATA";
    }
}

static char *dcc_new_app_latest_last_before(char *haystack, const char *needle, const char *limit) {
    char *last = NULL;
    size_t needle_len = strlen(needle);
    for (char *p = strstr(haystack, needle); p != NULL && p < limit; p = strstr(p + needle_len, needle)) {
        last = p;
    }
    return last;
}

static int dcc_new_app_latest_insert_task_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    char *handler_name,
    size_t handler_name_len
) {
    char insertion[2300];
    snprintf(handler_name, handler_name_len, "on_%s_%s_latest_task", options->cog_name, options->command_name);
    snprintf(
        insertion,
        sizeof(insertion),
        "DCC_TASK_FN(%s) {\n"
        "    bot_config_t *config = BOT_CONFIG(user_data);\n"
        "    if (config == NULL || config->%s == 0U) {\n"
        "        return;\n"
        "    }\n"
        "\n"
        "    dcc_message_builder_t message =\n"
        "        DCC_MESSAGE_COMPONENTS_V2(\n"
        "            DCC_UI_CARD_ACCENT(\n"
        "                0x5865F2,\n"
        "                DCC_UI_TEXT(\"## %s\"),\n"
        "                DCC_UI_TEXT(\"Generated scheduled latest message. Edit this payload in src/%s.c.\")\n"
        "            )\n"
        "        );\n"
        "    (void)DCC_APP_PUBLISH_LATEST_STORE(\n"
        "        app,\n"
        "        \"%s.%s.scheduled_latest\",\n"
        "        config->%s,\n"
        "        &message,\n"
        "        NULL,\n"
        "        NULL\n"
        "    );\n"
        "}\n"
        "\n",
        handler_name,
        options->config_type_arg,
        options->command_name,
        options->cog_name,
        options->cog_name,
        options->command_name,
        options->config_type_arg
    );
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_HANDLER_MARKER, insertion, handler_name);
}

static int dcc_new_app_latest_insert_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    char *handler_name,
    size_t handler_name_len
) {
    char insertion[2600];
    snprintf(handler_name, handler_name_len, "on_%s_%s_latest_message", options->cog_name, options->command_name);
    snprintf(
        insertion,
        sizeof(insertion),
        "DCC_HANDLER(%s) {\n"
        "    bot_config_t *config = BOT_CONFIG(user_data);\n"
        "    DCC_CTX_REQUIRE_BAD_INPUT(\n"
        "        ctx,\n"
        "        config != NULL && config->%s != 0U,\n"
        "        \"Missing latest-message channel config.\"\n"
        "    );\n"
        "\n"
        "    dcc_message_builder_t message =\n"
        "        DCC_MESSAGE_COMPONENTS_V2(\n"
        "            DCC_UI_CARD_ACCENT(\n"
        "                0x5865F2,\n"
        "                DCC_UI_TEXT(\"## %s\"),\n"
        "                DCC_UI_TEXT(\"Generated latest message. Edit this payload in src/%s.c.\")\n"
        "            )\n"
        "        );\n"
        "    DCC_CTX_TRY(ctx, DCC_APP_PUBLISH_LATEST_STORE(\n"
        "        DCC_CTX_APP(ctx),\n"
        "        \"%s.%s.latest\",\n"
        "        config->%s,\n"
        "        &message,\n"
        "        NULL,\n"
        "        NULL\n"
        "    ));\n"
        "    (void)DCC_CTX_OK(ctx, \"Latest message queued.\");\n"
        "}\n"
        "\n",
        handler_name,
        options->config_type_arg,
        options->command_name,
        options->cog_name,
        options->cog_name,
        options->command_name,
        options->config_type_arg
    );
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_HANDLER_MARKER, insertion, handler_name);
}

static int dcc_new_app_latest_insert_task_route(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    char route[760];
    char duplicate_needle[360];
    const char *macro = dcc_new_app_latest_task_macro(options->task_schedule);
    if (options->task_schedule == DCC_NEW_APP_TASK_DAILY_KST) {
        snprintf(duplicate_needle, sizeof(duplicate_needle), "%s(\"%s\", %s", macro, options->task_interval_arg, handler_name);
        snprintf(route, sizeof(route), "        %s(\"%s\", %s, user_data)\n", macro, options->task_interval_arg, handler_name);
    } else {
        snprintf(duplicate_needle, sizeof(duplicate_needle), "%s(%sU, %s", macro, options->task_interval_arg, handler_name);
        snprintf(route, sizeof(route), "        %s(%sU, %s, user_data)\n", macro, options->task_interval_arg, handler_name);
    }

    int has_route_marker = dcc_new_app_latest_file_contains(path, DCC_NEW_APP_TASK_ROUTE_MARKER);
    if (has_route_marker < 0) {
        return -1;
    }
    if (has_route_marker) {
        char insertion[980];
        snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
        return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_TASK_ROUTE_MARKER, insertion, duplicate_needle);
    }
    char block[1200];
    snprintf(
        block,
        sizeof(block),
        "    ,\n"
        "    DCC_FEATURE_TASKS(\n"
        "%s"
        "%s\n"
        "    )\n",
        route,
        DCC_NEW_APP_TASK_ROUTE_MARKER
    );
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_TASK_EXTENSION_MARKER, block, duplicate_needle);
}

static int dcc_new_app_latest_insert_route(
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

    char route[900];
    char duplicate_needle[320];
    char policy[1000];
    int guarded = dcc_new_app_guard_policy_literal(options, policy, sizeof(policy));
    if (guarded < 0) {
        free(data);
        return -1;
    }
    snprintf(
        duplicate_needle,
        sizeof(duplicate_needle),
        "DCC_COMMAND_DATA%s(\"%s\"",
        guarded ? "_POLICY" : "",
        options->command_name
    );
    snprintf(
        route,
        sizeof(route),
        "        %s(\"%s\", \"Publish latest %s message\", %s, user_data%s%s%s)\n",
        guarded ? "DCC_COMMAND_DATA_POLICY" : "DCC_COMMAND_DATA",
        options->command_name,
        options->command_name,
        handler_name,
        guarded ? ", " : "",
        guarded ? policy : "",
        ""
    );
    if (strstr(data, duplicate_needle) != NULL) {
        free(data);
        return 0;
    }

    char *latest_marker = strstr(data, DCC_NEW_APP_LATEST_ROUTE_MARKER);
    if (latest_marker != NULL) {
        char insertion[1100];
        snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
        int status = dcc_new_app_replace_range(path, data, len, latest_marker, 0U, insertion);
        free(data);
        return status;
    }
    char *preset_marker = strstr(data, DCC_NEW_APP_PRESET_SLASH_ROUTE_MARKER);
    if (preset_marker != NULL) {
        char insertion[1100];
        snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
        int status = dcc_new_app_replace_range(path, data, len, preset_marker, 0U, insertion);
        free(data);
        return status;
    }

    char *slash_marker = strstr(data, DCC_NEW_APP_SLASH_ROUTE_MARKER);
    if (slash_marker != NULL) {
        char *plain_slashes = dcc_new_app_latest_last_before(data, "DCC_FEATURE_SLASHES(", slash_marker);
        char *plain_commands = dcc_new_app_latest_last_before(data, "DCC_FEATURE_COMMANDS(", slash_marker);
        char *plain = plain_commands != NULL && (plain_slashes == NULL || plain_commands > plain_slashes)
            ? plain_commands
            : plain_slashes;
        char *typed_slashes = dcc_new_app_latest_last_before(data, "DCC_FEATURE_TYPED_SLASHES(", slash_marker);
        char *typed_command_routes = dcc_new_app_latest_last_before(data, "DCC_FEATURE_COMMAND_ROUTES(", slash_marker);
        char *typed_commands_old = dcc_new_app_latest_last_before(data, "DCC_FEATURE_TYPED_COMMANDS(", slash_marker);
        char *typed_commands = typed_command_routes != NULL &&
            (typed_commands_old == NULL || typed_command_routes > typed_commands_old)
            ? typed_command_routes
            : typed_commands_old;
        char *typed = typed_commands != NULL && (typed_slashes == NULL || typed_commands > typed_slashes)
            ? typed_commands
            : typed_slashes;
        if (plain != NULL && (typed == NULL || plain > typed)) {
            char insertion[1100];
            snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
            int status = dcc_new_app_replace_range(path, data, len, slash_marker, 0U, insertion);
            free(data);
            return status;
        }
    }

    char *extension = strstr(data, DCC_NEW_APP_SUBCOMMAND_EXTENSION_MARKER);
    if (extension == NULL) {
        fprintf(stderr, "warning: latest-message marker target not found in %s\n", path);
        free(data);
        return 0;
    }
    char block[1300];
    snprintf(
        block,
        sizeof(block),
        "    ,\n"
        "    DCC_FEATURE_COMMANDS(\n"
        "%s"
        "%s\n"
        "%s\n"
        "    )\n",
        route,
        DCC_NEW_APP_PRESET_SLASH_ROUTE_MARKER,
        DCC_NEW_APP_LATEST_ROUTE_MARKER
    );
    int status = dcc_new_app_replace_range(path, data, len, extension, 0U, block);
    free(data);
    return status;
}

static int dcc_new_app_latest_append_command_json(const dcc_new_app_options_t *options) {
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
    char command[360];
    snprintf(
        command,
        sizeof(command),
        "%s\n"
        "  {\n"
        "    \"name\": \"%s\",\n"
        "    \"description\": \"Publish latest %s message\",\n"
        "    \"type\": 1\n"
        "  }\n",
        has_existing ? "," : "",
        options->command_name,
        options->command_name
    );
    size_t prefix_len = (size_t)(last_bracket - data);
    while (prefix_len > 0U && isspace((unsigned char)data[prefix_len - 1U])) {
        --prefix_len;
    }
    size_t command_len = strlen(command);
    size_t suffix_len = len - (size_t)(last_bracket - data);
    char *next = (char *)malloc(prefix_len + command_len + suffix_len + 1U);
    if (next == NULL) {
        free(data);
        free(path);
        return -1;
    }
    memcpy(next, data, prefix_len);
    memcpy(next + prefix_len, command, command_len);
    memcpy(next + prefix_len + command_len, last_bracket, suffix_len);
    next[prefix_len + command_len + suffix_len] = '\0';
    int status = dcc_new_app_write_file_all(path, next, prefix_len + command_len + suffix_len);
    free(next);
    free(data);
    free(path);
    return status;
}

static int dcc_new_app_generate_scheduled_latest_message(const dcc_new_app_options_t *options) {
    if (options->task_schedule == DCC_NEW_APP_TASK_DAILY_KST) {
        if (!dcc_new_app_latest_hhmm_safe(options->task_interval_arg)) {
            fprintf(stderr, "--daily-kst must use HH:MM with a valid KST time\n");
            return -1;
        }
    } else if (!dcc_new_app_latest_uint_literal_safe(options->task_interval_arg)) {
        fprintf(stderr, "scheduled-latest interval must be a positive integer literal\n");
        return -1;
    }
    char *path = NULL;
    if (dcc_new_app_latest_require_cog(&path, options) != 0) {
        return -1;
    }
    char handler_name[320];
    int status = dcc_new_app_latest_insert_task_handler(path, options, handler_name, sizeof(handler_name));
    if (status == 0) {
        status = dcc_new_app_latest_insert_task_route(path, options, handler_name);
    }
    free(path);
    if (status != 0) {
        return -1;
    }
    printf("created DCC scheduled latest message %s in feature %s under %s\n", options->command_name, options->cog_name, options->path);
    return 0;
}

int dcc_new_app_generate_latest_message(const dcc_new_app_options_t *options) {
    if (!dcc_new_app_latest_field_valid(options->config_type_arg)) {
        fprintf(stderr, "CHANNEL_FIELD must be a C identifier in bot_config_t\n");
        return -1;
    }
    if (options->subcommand_arg != NULL && strcmp(options->subcommand_arg, "scheduled") == 0) {
        return dcc_new_app_generate_scheduled_latest_message(options);
    }
    char *path = NULL;
    if (dcc_new_app_latest_require_cog(&path, options) != 0) {
        return -1;
    }
    char handler_name[320];
    int status = dcc_new_app_latest_insert_handler(path, options, handler_name, sizeof(handler_name));
    if (status == 0) {
        status = dcc_new_app_latest_insert_route(path, options, handler_name);
    }
    free(path);
    if (status == 0) {
        status = dcc_new_app_latest_append_command_json(options);
    }
    if (status != 0) {
        return -1;
    }
    printf("created DCC latest message %s in feature %s under %s\n", options->command_name, options->cog_name, options->path);
    return 0;
}
