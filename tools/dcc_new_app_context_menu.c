#include "internal/dcc_new_app.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_NEW_APP_HANDLER_MARKER "/* dcc_new_app:command-handlers */"
#define DCC_NEW_APP_CONTEXT_MENU_ROUTE_MARKER "        /* dcc_new_app:context-menu-routes */"
#define DCC_NEW_APP_CONTEXT_MENU_EXTENSION_MARKER "    /* dcc_new_app:context-menu-extension */"
#define DCC_NEW_APP_MESSAGE_COMMAND_EXTENSION_MARKER "    /* dcc_new_app:message-command-extension */"

static char *dcc_new_app_context_menu_cog_source_path(const dcc_new_app_options_t *options) {
    char relative_path[320];
    snprintf(relative_path, sizeof(relative_path), "src/%s.c", options->cog_name);
    return dcc_new_app_join(options->path, relative_path);
}

static const char *dcc_new_app_context_menu_kind(const dcc_new_app_options_t *options) {
    if (options == NULL || options->subcommand_arg == NULL) {
        return "user";
    }
    if (strcmp(options->subcommand_arg, "message") == 0 ||
        strcmp(options->subcommand_arg, "message-menu") == 0 ||
        strcmp(options->subcommand_arg, "message-context") == 0 ||
        strcmp(options->subcommand_arg, "message_context") == 0) {
        return "message";
    }
    return "user";
}

static int dcc_new_app_context_menu_is_user(const dcc_new_app_options_t *options) {
    return strcmp(dcc_new_app_context_menu_kind(options), "user") == 0;
}

static int dcc_new_app_context_menu_require_cog(char **out_path, const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_context_menu_cog_source_path(options);
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

static int dcc_new_app_insert_context_menu_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    char *handler_name,
    size_t handler_name_len
) {
    int is_user = dcc_new_app_context_menu_is_user(options);
    snprintf(
        handler_name,
        handler_name_len,
        "on_%s_%s_%s_menu",
        options->cog_name,
        options->command_name,
        is_user ? "user" : "message"
    );

    char insertion[1900];
    if (is_user) {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_HANDLER(%s) {\n"
            "    (void)user_data;\n"
            "\n"
            "    dcc_snowflake_t target_id = DCC_CTX_TARGET_ID(ctx);\n"
            "    DCC_CTX_REQUIRE_BAD_INPUT(ctx, target_id != 0U, \"Choose a user from the context menu.\");\n"
            "\n"
            "    (void)DCC_REPLY_PRIVATEF(\n"
            "        ctx,\n"
            "        \"Selected user: %%s (%%s)\",\n"
            "        DCC_CTX_TARGET_DISPLAY_NAME_OR(ctx, \"unknown\"),\n"
            "        DCC_CTX_TARGET_USER_MENTION(ctx)\n"
            "    );\n"
            "}\n"
            "\n",
            handler_name
        );
    } else {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_HANDLER(%s) {\n"
            "    (void)user_data;\n"
            "\n"
            "    const dcc_message_t *message = DCC_CTX_TARGET_MESSAGE(ctx);\n"
            "    DCC_CTX_REQUIRE_BAD_INPUT(\n"
            "        ctx,\n"
            "        message != NULL && message->id != 0U,\n"
            "        \"Choose a message from the context menu.\"\n"
            "    );\n"
            "\n"
            "    const char *content = message->content != NULL && message->content[0] != '\\0'\n"
            "        ? message->content\n"
            "        : \"(no text content)\";\n"
            "    (void)DCC_REPLY_PRIVATEF(\n"
            "        ctx,\n"
            "        \"Selected message %%llu:\\n%%s\",\n"
            "        (unsigned long long)message->id,\n"
            "        content\n"
            "    );\n"
            "}\n"
            "\n",
            handler_name
        );
    }

    return dcc_new_app_insert_before_marker(
        path,
        DCC_NEW_APP_HANDLER_MARKER,
        insertion,
        handler_name
    );
}

static const char *dcc_new_app_context_menu_extension_anchor(const char *data) {
    const char *marker = strstr(data, DCC_NEW_APP_CONTEXT_MENU_EXTENSION_MARKER);
    if (marker != NULL) {
        return marker;
    }
    return strstr(data, DCC_NEW_APP_MESSAGE_COMMAND_EXTENSION_MARKER);
}

static int dcc_new_app_insert_context_menu_route(
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

    int is_user = dcc_new_app_context_menu_is_user(options);
    char plain_needle[420];
    char guarded_needle[420];
    snprintf(
        plain_needle,
        sizeof(plain_needle),
        "%s(\"%s\"",
        is_user ? "DCC_USER_CONTEXT_MENU_DATA" : "DCC_MESSAGE_CONTEXT_MENU_DATA",
        options->command_name
    );
    snprintf(
        guarded_needle,
        sizeof(guarded_needle),
        "%s(\n"
        "            \"%s\"",
        is_user ? "DCC_USER_CONTEXT_MENU_DATA_GUARDED" : "DCC_MESSAGE_CONTEXT_MENU_DATA_GUARDED",
        options->command_name
    );
    if (strstr(data, plain_needle) != NULL || strstr(data, guarded_needle) != NULL) {
        free(data);
        return 0;
    }

    char guards[1000];
    int guarded = dcc_new_app_guard_items_literal(options, guards, sizeof(guards));
    if (guarded < 0) {
        free(data);
        return -1;
    }

    char route[1500];
    if (guarded) {
        snprintf(
            route,
            sizeof(route),
            "        %s(\n"
            "            \"%s\",\n"
            "            %s,\n"
            "            user_data,\n"
            "            %s\n"
            "        )\n",
            is_user ? "DCC_USER_CONTEXT_MENU_DATA_GUARDED" : "DCC_MESSAGE_CONTEXT_MENU_DATA_GUARDED",
            options->command_name,
            handler_name,
            guards
        );
    } else {
        snprintf(
            route,
            sizeof(route),
            "        %s(\"%s\", %s, user_data)\n",
            is_user ? "DCC_USER_CONTEXT_MENU_DATA" : "DCC_MESSAGE_CONTEXT_MENU_DATA",
            options->command_name,
            handler_name
        );
    }

    char *route_marker = strstr(data, DCC_NEW_APP_CONTEXT_MENU_ROUTE_MARKER);
    if (route_marker != NULL) {
        char insertion[1700];
        snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
        int status = dcc_new_app_replace_range(path, data, len, route_marker, 0U, insertion);
        free(data);
        return status;
    }

    const char *extension = dcc_new_app_context_menu_extension_anchor(data);
    if (extension == NULL) {
        fprintf(stderr, "warning: context menu marker target not found in %s\n", path);
        free(data);
        return 0;
    }

    char block[1800];
    snprintf(
        block,
        sizeof(block),
        "    ,\n"
        "    DCC_FEATURE_CONTEXT_MENUS(\n"
        "%s"
        "%s\n"
        "    )\n",
        route,
        DCC_NEW_APP_CONTEXT_MENU_ROUTE_MARKER
    );
    int status = dcc_new_app_replace_range(path, data, len, extension, 0U, block);
    free(data);
    return status;
}

static int dcc_new_app_context_menu_append_command_json(const dcc_new_app_options_t *options) {
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

    unsigned type = dcc_new_app_context_menu_is_user(options) ? 2U : 3U;
    char duplicate_needle[240];
    snprintf(
        duplicate_needle,
        sizeof(duplicate_needle),
        "\"name\": \"%s\",\n"
        "    \"type\": %u",
        options->command_name,
        type
    );
    if (strstr(data, duplicate_needle) != NULL) {
        free(data);
        free(path);
        return 0;
    }

    char *last_bracket = strrchr(data, ']');
    if (last_bracket == NULL) {
        fprintf(stderr, "warning: commands.json is not a JSON array; %s context menu was not appended\n", options->command_name);
        free(data);
        free(path);
        return 0;
    }

    int has_existing = strchr(data, '{') != NULL;
    char command[520];
    snprintf(
        command,
        sizeof(command),
        "%s\n"
        "  {\n"
        "    \"name\": \"%s\",\n"
        "    \"type\": %u\n"
        "  }\n",
        has_existing ? "," : "",
        options->command_name,
        type
    );

    size_t prefix_len = (size_t)(last_bracket - data);
    while (prefix_len > 0U && isspace((unsigned char)data[prefix_len - 1U])) {
        --prefix_len;
    }
    size_t command_len = strlen(command);
    size_t suffix_len = len - (size_t)(last_bracket - data);
    if (command_len > SIZE_MAX - prefix_len ||
        suffix_len > SIZE_MAX - prefix_len - command_len) {
        free(data);
        free(path);
        return -1;
    }
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

int dcc_new_app_generate_context_menu(const dcc_new_app_options_t *options) {
    if (!dcc_new_app_path_exists(options->path)) {
        fprintf(stderr, "%s does not exist; create the app first\n", options->path);
        return -1;
    }

    char *path = NULL;
    if (dcc_new_app_context_menu_require_cog(&path, options) != 0) {
        return -1;
    }

    char handler_name[320];
    int status = dcc_new_app_insert_context_menu_handler(
        path,
        options,
        handler_name,
        sizeof(handler_name)
    );
    if (status == 0) {
        status = dcc_new_app_insert_context_menu_route(path, options, handler_name);
    }
    free(path);
    if (status == 0) {
        status = dcc_new_app_context_menu_append_command_json(options);
    }
    if (status != 0) {
        return -1;
    }

    printf(
        "created DCC %s context menu %s in feature %s under %s\n",
        dcc_new_app_context_menu_kind(options),
        options->command_name,
        options->cog_name,
        options->path
    );
    return 0;
}
