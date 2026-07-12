#include "internal/dcc_new_app.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_NEW_APP_HANDLER_MARKER "/* dcc_new_app:command-handlers */"
#define DCC_NEW_APP_PRESET_SLASH_ROUTE_MARKER "        /* dcc_new_app:preset-slash-routes */"
#define DCC_NEW_APP_EVENT_ROUTE_MARKER "        /* dcc_new_app:event-routes */"
#define DCC_NEW_APP_EVENT_EXTENSION_MARKER "    /* dcc_new_app:event-extension */"

static char *dcc_new_app_welcome_cog_source_path(const dcc_new_app_options_t *options) {
    char relative_path[320];
    snprintf(relative_path, sizeof(relative_path), "src/%s.c", options->cog_name);
    return dcc_new_app_join(options->path, relative_path);
}

static int dcc_new_app_welcome_file_contains(const char *path, const char *needle) {
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

static int dcc_new_app_welcome_insert_route(
    const char *path,
    const char *route,
    const char *duplicate_needle,
    const char *route_marker,
    const char *extension_marker,
    const char *cog_macro
) {
    int has_route_marker = dcc_new_app_welcome_file_contains(path, route_marker);
    if (has_route_marker < 0) {
        return -1;
    }
    if (has_route_marker) {
        char insertion[1400];
        snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
        return dcc_new_app_insert_before_marker(path, route_marker, insertion, duplicate_needle);
    }
    char insertion[1600];
    snprintf(insertion, sizeof(insertion), "    ,\n    %s(\n%s%s\n    )\n", cog_macro, route, route_marker);
    return dcc_new_app_insert_before_marker(path, extension_marker, insertion, duplicate_needle);
}

static int dcc_new_app_welcome_append_command_json(const dcc_new_app_options_t *options) {
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
    char needle[192];
    snprintf(needle, sizeof(needle), "\"name\": \"%s\"", options->command_name);
    if (strstr(data, needle) != NULL) {
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
    size_t prefix_len = (size_t)(last_bracket - data);
    while (prefix_len > 0U && isspace((unsigned char)data[prefix_len - 1U])) {
        --prefix_len;
    }
    char command[480];
    snprintf(
        command,
        sizeof(command),
        "%s\n  {\n"
        "    \"name\": \"%s\",\n"
        "    \"description\": \"Generated welcome preset\",\n"
        "    \"type\": 1\n"
        "  }\n",
        strchr(data, '{') != NULL ? "," : "",
        options->command_name
    );
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

static int dcc_new_app_welcome_insert_handlers(const char *path, const dcc_new_app_options_t *options) {
    char handler[320];
    char event_handler[320];
    char channel_macro[320];
    char insertion[5400];
    snprintf(handler, sizeof(handler), "on_%s_%s", options->cog_name, options->command_name);
    snprintf(event_handler, sizeof(event_handler), "on_%s_%s_member", options->cog_name, options->command_name);
    dcc_new_app_make_macro_name(handler, "_CHANNEL_ID", channel_macro, sizeof(channel_macro));

    snprintf(
        insertion,
        sizeof(insertion),
        "#ifndef %s\n"
        "#define %s 0ULL\n"
        "#endif\n"
        "\n"
        "static void %s_build_text(char *out, size_t out_size, dcc_snowflake_t user_id, const char *name) {\n"
        "    const char *display = name != NULL && name[0] != '\\0' ? name : \"there\";\n"
        "    snprintf(\n"
        "        out,\n"
        "        out_size,\n"
        "        \"## Welcome <@%%llu>\\nGlad you're here, %%s.\",\n"
        "        (unsigned long long)user_id,\n"
        "        display\n"
        "    );\n"
        "}\n"
        "\n"
        "DCC_HANDLER(%s) {\n"
        "    (void)user_data;\n"
        "\n"
        "    char text[256];\n"
        "    %s_build_text(text, sizeof(text), DCC_CTX_AUTHOR_ID(ctx), \"preview\");\n"
        "    (void)DCC_CTX_REPLY_UI(ctx, DCC_UI_CARD_ACCENT(0x5865F2, DCC_UI_TEXT(text)));\n"
        "}\n"
        "\n"
        "DCC_MEMBER_JOIN_FN(%s) {\n"
        "    (void)event;\n"
        "    (void)user_data;\n"
        "\n"
        "    dcc_snowflake_t channel_id = (dcc_snowflake_t)%s;\n"
        "    if (channel_id == 0U) {\n"
        "        return;\n"
        "    }\n"
        "    if (member == NULL || member->user.id == 0U) {\n"
        "        return;\n"
        "    }\n"
        "    const char *name = member->nick != NULL ? member->nick :\n"
        "        (member->user.global_name != NULL ? member->user.global_name : member->user.username);\n"
        "    char text[256];\n"
        "    %s_build_text(text, sizeof(text), member->user.id, name);\n"
        "    (void)DCC_APP_SEND_UI(app, channel_id, DCC_UI_CARD_ACCENT(0x5865F2, DCC_UI_TEXT(text)));\n"
        "}\n"
        "\n",
        channel_macro,
        channel_macro,
        handler,
        handler,
        handler,
        event_handler,
        channel_macro,
        handler
    );
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_HANDLER_MARKER, insertion, handler);
}

static int dcc_new_app_welcome_insert_routes(const char *path, const dcc_new_app_options_t *options) {
    char policy[1000];
    int guarded = dcc_new_app_guard_policy_literal(options, policy, sizeof(policy));
    if (guarded < 0) {
        return -1;
    }
    char handler[320];
    char event_handler[320];
    snprintf(handler, sizeof(handler), "on_%s_%s", options->cog_name, options->command_name);
    snprintf(event_handler, sizeof(event_handler), "on_%s_%s_member", options->cog_name, options->command_name);

    char slash_route[1200];
    snprintf(
        slash_route,
        sizeof(slash_route),
        "        %s(\"%s\", \"Generated welcome preset\", %s, user_data%s%s%s)\n",
        guarded ? "DCC_COMMAND_DATA_POLICY" : "DCC_COMMAND_DATA",
        options->command_name,
        handler,
        guarded ? ", " : "",
        guarded ? policy : "",
        ""
    );
    char slash_needle[320];
    snprintf(slash_needle, sizeof(slash_needle), "\"%s\", \"Generated welcome preset\"", options->command_name);
    int status = dcc_new_app_welcome_insert_route(
        path,
        slash_route,
        slash_needle,
        DCC_NEW_APP_PRESET_SLASH_ROUTE_MARKER,
        "    /* dcc_new_app:component-extension */",
        "DCC_FEATURE_COMMANDS"
    );
    if (status != 0) {
        return status;
    }

    char event_route[700];
    snprintf(event_route, sizeof(event_route), "        DCC_MEMBER_JOIN_DATA(%s, user_data)\n", event_handler);
    char event_needle[360];
    snprintf(event_needle, sizeof(event_needle), "DCC_MEMBER_JOIN_DATA(%s", event_handler);
    return dcc_new_app_welcome_insert_route(
        path,
        event_route,
        event_needle,
        DCC_NEW_APP_EVENT_ROUTE_MARKER,
        DCC_NEW_APP_EVENT_EXTENSION_MARKER,
        "DCC_FEATURE_EVENTS"
    );
}

int dcc_new_app_generate_welcome_preset(const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_welcome_cog_source_path(options);
    if (path == NULL) {
        return -1;
    }
    if (!dcc_new_app_path_exists(path)) {
        fprintf(stderr, "%s does not exist; add the feature first\n", path);
        free(path);
        return -1;
    }
    int status = dcc_new_app_welcome_insert_handlers(path, options);
    if (status == 0) {
        status = dcc_new_app_welcome_insert_routes(path, options);
    }
    free(path);
    if (status == 0) {
        status = dcc_new_app_welcome_append_command_json(options);
    }
    if (status != 0) {
        return -1;
    }
    printf("created DCC welcome preset %s in feature %s under %s\n", options->command_name, options->cog_name, options->path);
    return 0;
}
