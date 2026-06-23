#include "internal/dcc_new_app.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_NEW_APP_HANDLER_MARKER "/* dcc_new_app:command-handlers */"
#define DCC_NEW_APP_SLASH_ROUTE_MARKER "        /* dcc_new_app:slash-routes */"
#define DCC_NEW_APP_VIEW_SLASH_ROUTE_MARKER "        /* dcc_new_app:view-slash-routes */"
#define DCC_NEW_APP_SUBCOMMAND_EXTENSION_MARKER "    /* dcc_new_app:subcommand-extension */"
#define DCC_NEW_APP_VIEW_ROUTE_MARKER "        /* dcc_new_app:view-routes */"
#define DCC_NEW_APP_VIEW_EXTENSION_MARKER "    /* dcc_new_app:view-extension */"
#define DCC_NEW_APP_COMPONENT_EXTENSION_MARKER "    /* dcc_new_app:component-extension */"

static char *dcc_new_app_view_cog_source_path(const dcc_new_app_options_t *options) {
    char relative_path[320];
    snprintf(relative_path, sizeof(relative_path), "src/%s.c", options->cog_name);
    return dcc_new_app_join(options->path, relative_path);
}

static int dcc_new_app_view_file_contains(const char *path, const char *needle) {
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

static char *dcc_new_app_view_last_before(char *data, const char *needle, const char *limit) {
    char *last = NULL;
    char *cursor = data;
    size_t needle_len = strlen(needle);
    while (cursor != NULL && cursor < limit) {
        cursor = strstr(cursor, needle);
        if (cursor == NULL || cursor >= limit) {
            break;
        }
        last = cursor;
        cursor += needle_len;
    }
    return last;
}

static int dcc_new_app_view_replace_old_opener_row(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *action_macro
) {
    size_t len = 0U;
    char *data = dcc_new_app_read_file(path, &len);
    if (data == NULL) {
        fprintf(stderr, "could not read %s for marker update\n", path);
        return -1;
    }

    char old_row[900];
    snprintf(
        old_row,
        sizeof(old_row),
        "DCC_UI_ROW(\n"
        "                DCC_UI_PRIMARY(\"Refresh\", \"%s.%s.refresh\"),\n"
        "                DCC_UI_SECONDARY(\"Edit\", \"%s.%s.edit\")\n"
        "            )",
        options->cog_name,
        options->command_name,
        options->cog_name,
        options->command_name
    );

    char new_row[520];
    snprintf(new_row, sizeof(new_row), "DCC_VIEW_ACTION_ROW(%s)", action_macro);
    if (strstr(data, new_row) != NULL) {
        free(data);
        return 0;
    }

    char *where = strstr(data, old_row);
    if (where == NULL) {
        free(data);
        return 0;
    }
    int status = dcc_new_app_replace_range(path, data, len, where, strlen(old_row), new_row);
    free(data);
    return status;
}

static int dcc_new_app_view_append_command_json(const dcc_new_app_options_t *options) {
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
        fprintf(
            stderr,
            "warning: commands.json is not a JSON array; %s view command was not appended\n",
            options->command_name
        );
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
        "    \"description\": \"Open %s view\",\n"
        "    \"type\": 1\n"
        "  }\n",
        has_existing ? "," : "",
        options->command_name,
        options->command_name
    );

    size_t prefix_len = (size_t)(last_bracket - data);
    while (prefix_len > 0U && (data[prefix_len - 1U] == ' ' ||
                               data[prefix_len - 1U] == '\t' ||
                               data[prefix_len - 1U] == '\n' ||
                               data[prefix_len - 1U] == '\r')) {
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

static int dcc_new_app_view_insert_opener_handler(const char *path, const dcc_new_app_options_t *options) {
    char handler[320];
    char args_type[360];
    char refresh_handler[360];
    char edit_handler[360];
    char action_macro[420];
    char duplicate_needle[400];
    char old_duplicate_needle[400];
    char action_define_needle[520];
    char support_block[1000];
    char insertion[3000];
    snprintf(handler, sizeof(handler), "on_%s_%s_view", options->cog_name, options->command_name);
    snprintf(args_type, sizeof(args_type), "%s_args_t", handler);
    snprintf(refresh_handler, sizeof(refresh_handler), "%s_refresh", handler);
    snprintf(edit_handler, sizeof(edit_handler), "%s_edit", handler);
    dcc_new_app_make_macro_name(handler, "_ACTIONS", action_macro, sizeof(action_macro));
    snprintf(duplicate_needle, sizeof(duplicate_needle), "DCC_COMMAND_ARGS_IMPL(%s", handler);
    snprintf(old_duplicate_needle, sizeof(old_duplicate_needle), "DCC_TYPED_SLASH_IMPL(%s", handler);
    snprintf(action_define_needle, sizeof(action_define_needle), "#define %s", action_macro);

    snprintf(
        support_block,
        sizeof(support_block),
        "static void %s(dcc_ctx_t *ctx, void *user_data);\n"
        "static void %s(dcc_ctx_t *ctx, void *user_data);\n"
        "\n"
        "#define %s \\\n"
        "    DCC_VIEW_ACTION_PRIMARY(\"Refresh\", \"%s.%s.refresh\", %s), \\\n"
        "    DCC_VIEW_ACTION_SECONDARY(\"Edit\", \"%s.%s.edit\", %s)\n"
        "\n",
        refresh_handler,
        edit_handler,
        action_macro,
        options->cog_name,
        options->command_name,
        refresh_handler,
        options->cog_name,
        options->command_name,
        edit_handler
    );

    int has_new_handler = dcc_new_app_view_file_contains(path, duplicate_needle);
    int has_old_handler = dcc_new_app_view_file_contains(path, old_duplicate_needle);
    if (has_new_handler < 0 || has_old_handler < 0) {
        return -1;
    }
    if (has_new_handler || has_old_handler) {
        const char *handler_needle = has_new_handler ? duplicate_needle : old_duplicate_needle;
        int has_actions = dcc_new_app_view_file_contains(path, action_define_needle);
        if (has_actions < 0) {
            return -1;
        }
        int status = has_actions
            ? 0
            : dcc_new_app_insert_before_marker(path, handler_needle, support_block, action_define_needle);
        if (status == 0) {
            status = dcc_new_app_view_replace_old_opener_row(path, options, action_macro);
        }
        return status;
    }

    snprintf(
        insertion,
        sizeof(insertion),
        "%s"
        "typedef struct %s_args {\n"
        "    int unused;\n"
        "} %s;\n"
        "\n"
        "DCC_COMMAND_ARGS_IMPL(%s, %s, command) {\n"
        "    (void)command;\n"
        "    (void)user_data;\n"
        "\n"
        "    (void)DCC_RESPOND_UI(\n"
        "        ctx,\n"
        "        DCC_UI_CARD_ACCENT(\n"
        "            DCC_COLOR_BLURPLE,\n"
        "            DCC_UI_TEXT(\"## %s\"),\n"
        "            DCC_UI_TEXT(\"Use the buttons below.\"),\n"
        "            DCC_VIEW_ACTION_ROW(%s)\n"
        "        )\n"
        "    );\n"
        "}\n"
        "\n",
        support_block,
        handler,
        args_type,
        handler,
        args_type,
        options->command_name,
        action_macro
    );
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_HANDLER_MARKER, insertion, duplicate_needle);
}

static int dcc_new_app_view_insert_route(const char *path, const char *route, const char *duplicate_needle) {
    int has_route_marker = dcc_new_app_view_file_contains(path, DCC_NEW_APP_VIEW_ROUTE_MARKER);
    if (has_route_marker < 0) {
        return -1;
    }
    if (has_route_marker) {
        char insertion[1000];
        snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
        return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_VIEW_ROUTE_MARKER, insertion, duplicate_needle);
    }

    char insertion[1400];
    snprintf(
        insertion,
        sizeof(insertion),
        "    ,\n"
        "    DCC_FEATURE_VIEWS(\n"
        "%s"
        "        /* dcc_new_app:view-routes */\n"
        "    )\n",
        route
    );
    int has_view_extension = dcc_new_app_view_file_contains(path, DCC_NEW_APP_VIEW_EXTENSION_MARKER);
    if (has_view_extension < 0) {
        return -1;
    }
    if (has_view_extension) {
        return dcc_new_app_insert_before_marker(
            path,
            DCC_NEW_APP_VIEW_EXTENSION_MARKER,
            insertion,
            duplicate_needle
        );
    }
    return dcc_new_app_insert_before_marker(
        path,
        DCC_NEW_APP_COMPONENT_EXTENSION_MARKER,
        insertion,
        duplicate_needle
    );
}

static int dcc_new_app_view_insert_slash_route(const char *path, const dcc_new_app_options_t *options) {
    size_t len = 0U;
    char *data = dcc_new_app_read_file(path, &len);
    if (data == NULL) {
        fprintf(stderr, "could not read %s for marker update\n", path);
        return -1;
    }

    char handler[320];
    char args_type[360];
    char policy[1000];
    char route[2200];
    char duplicate_needle[520];
    int guarded = dcc_new_app_guard_policy_literal(options, policy, sizeof(policy));
    if (guarded < 0) {
        free(data);
        return -1;
    }

    snprintf(handler, sizeof(handler), "on_%s_%s_view", options->cog_name, options->command_name);
    snprintf(args_type, sizeof(args_type), "%s_args_t", handler);
    snprintf(
        duplicate_needle,
        sizeof(duplicate_needle),
        "DCC_TYPED_SLASH_NO_OPTIONS_DATA%s(\n"
        "            \"%s\"",
        guarded ? "_POLICY" : "",
        options->command_name
    );
    if (strstr(data, duplicate_needle) != NULL) {
        free(data);
        return 0;
    }

    snprintf(
        route,
        sizeof(route),
        "        DCC_TYPED_SLASH_NO_OPTIONS_DATA%s(\n"
        "            \"%s\",\n"
        "            \"Open %s view\",\n"
        "            %s,\n"
        "            %s,\n"
        "            user_data,\n"
        "            DCC_NO_ARGS(),\n"
        "            DCC_NO_VALIDATORS()%s%s\n"
        "        )\n",
        guarded ? "_POLICY" : "",
        options->command_name,
        options->command_name,
        args_type,
        handler,
        guarded ? ",\n            " : "",
        guarded ? policy : ""
    );

    char *view_marker = strstr(data, DCC_NEW_APP_VIEW_SLASH_ROUTE_MARKER);
    if (view_marker != NULL) {
        char insertion[2400];
        snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
        int status = dcc_new_app_replace_range(path, data, len, view_marker, 0U, insertion);
        free(data);
        return status;
    }

    char *slash_marker = strstr(data, DCC_NEW_APP_SLASH_ROUTE_MARKER);
    if (slash_marker != NULL) {
        char *plain_slashes = dcc_new_app_view_last_before(data, "DCC_FEATURE_SLASHES(", slash_marker);
        char *plain_commands = dcc_new_app_view_last_before(data, "DCC_FEATURE_COMMANDS(", slash_marker);
        char *plain = plain_commands != NULL && (plain_slashes == NULL || plain_commands > plain_slashes)
            ? plain_commands
            : plain_slashes;
        char *typed_slashes = dcc_new_app_view_last_before(data, "DCC_FEATURE_TYPED_SLASHES(", slash_marker);
        char *typed_command_routes = dcc_new_app_view_last_before(data, "DCC_FEATURE_COMMAND_ROUTES(", slash_marker);
        char *typed_commands_old = dcc_new_app_view_last_before(data, "DCC_FEATURE_TYPED_COMMANDS(", slash_marker);
        char *typed_commands = typed_command_routes != NULL &&
            (typed_commands_old == NULL || typed_command_routes > typed_commands_old)
            ? typed_command_routes
            : typed_commands_old;
        char *typed = typed_commands != NULL && (typed_slashes == NULL || typed_commands > typed_slashes)
            ? typed_commands
            : typed_slashes;
        if (typed != NULL && (plain == NULL || typed > plain)) {
            char insertion[2400];
            snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
            int status = dcc_new_app_replace_range(path, data, len, slash_marker, 0U, insertion);
            free(data);
            return status;
        }
    }

    char *extension = strstr(data, DCC_NEW_APP_SUBCOMMAND_EXTENSION_MARKER);
    if (extension == NULL) {
        fprintf(stderr, "warning: view slash marker target not found in %s\n", path);
        free(data);
        return 0;
    }

    char block[2600];
    snprintf(
        block,
        sizeof(block),
        "    ,\n"
        "    DCC_FEATURE_COMMAND_ROUTES(\n"
        "%s"
        "%s\n"
        "    )\n",
        route,
        DCC_NEW_APP_VIEW_SLASH_ROUTE_MARKER
    );
    int status = dcc_new_app_replace_range(path, data, len, extension, 0U, block);
    free(data);
    return status;
}

static int dcc_new_app_view_insert_handlers(const char *path, const dcc_new_app_options_t *options) {
    char refresh_handler[360];
    char edit_handler[360];
    char modal_handler[320];
    char refresh_definition[420];
    char modal_definition[380];
    char insertion[3600];
    snprintf(refresh_handler, sizeof(refresh_handler), "on_%s_%s_view_refresh", options->cog_name, options->command_name);
    snprintf(edit_handler, sizeof(edit_handler), "on_%s_%s_view_edit", options->cog_name, options->command_name);
    snprintf(modal_handler, sizeof(modal_handler), "on_%s_%s_view_modal", options->cog_name, options->command_name);
    snprintf(refresh_definition, sizeof(refresh_definition), "DCC_HANDLER(%s)", refresh_handler);
    snprintf(modal_definition, sizeof(modal_definition), "DCC_HANDLER(%s)", modal_handler);

    int has_refresh = dcc_new_app_view_file_contains(path, refresh_definition);
    if (has_refresh < 0) {
        return -1;
    }
    if (has_refresh) {
        return 0;
    }
    int has_modal = dcc_new_app_view_file_contains(path, modal_definition);
    if (has_modal < 0) {
        return -1;
    }

    if (has_modal) {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_HANDLER(%s) {\n"
            "    (void)user_data;\n"
            "\n"
            "    (void)DCC_CTX_OK(ctx, \"View refreshed.\");\n"
            "}\n"
            "\n"
            "DCC_HANDLER(%s) {\n"
            "    (void)user_data;\n"
            "\n"
            "    (void)DCC_SHOW_MODAL_V2(\n"
            "        ctx,\n"
            "        \"%s.%s.edit\",\n"
            "        \"Edit View\",\n"
            "        DCC_MODAL_V2_FIELD_TEXT_PLACEHOLDER(\"%s.%s.name\", \"Name\", \"Enter a value\")\n"
            "    );\n"
            "}\n"
            "\n",
            refresh_handler,
            edit_handler,
            options->cog_name,
            options->command_name,
            options->cog_name,
            options->command_name
        );
    } else {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_HANDLER(%s) {\n"
            "    (void)user_data;\n"
            "\n"
            "    (void)DCC_CTX_OK(ctx, \"View refreshed.\");\n"
            "}\n"
            "\n"
            "DCC_HANDLER(%s) {\n"
            "    (void)user_data;\n"
            "\n"
            "    (void)DCC_SHOW_MODAL_V2(\n"
            "        ctx,\n"
            "        \"%s.%s.edit\",\n"
            "        \"Edit View\",\n"
            "        DCC_MODAL_V2_FIELD_TEXT_PLACEHOLDER(\"%s.%s.name\", \"Name\", \"Enter a value\")\n"
            "    );\n"
            "}\n"
            "\n"
            "DCC_HANDLER(%s) {\n"
            "    (void)user_data;\n"
            "\n"
            "    (void)DCC_CTX_OK(ctx, \"View modal submitted.\");\n"
            "}\n"
            "\n",
            refresh_handler,
            edit_handler,
            options->cog_name,
            options->command_name,
            options->cog_name,
            options->command_name,
            modal_handler
        );
    }
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_HANDLER_MARKER, insertion, refresh_definition);
}

static int dcc_new_app_view_insert_routes(const char *path, const dcc_new_app_options_t *options) {
    char modal_handler[320];
    char handler[320];
    char action_macro[420];
    char route[1400];
    char duplicate_needle[420];
    char modal_route_needle[360];
    snprintf(handler, sizeof(handler), "on_%s_%s_view", options->cog_name, options->command_name);
    dcc_new_app_make_macro_name(handler, "_ACTIONS", action_macro, sizeof(action_macro));
    snprintf(modal_handler, sizeof(modal_handler), "on_%s_%s_view_modal", options->cog_name, options->command_name);
    snprintf(
        modal_route_needle,
        sizeof(modal_route_needle),
        "DCC_VIEW_MODAL(\"%s.%s.edit\"",
        options->cog_name,
        options->command_name
    );
    int has_modal_route = dcc_new_app_view_file_contains(path, modal_route_needle);
    if (has_modal_route < 0) {
        return -1;
    }

    snprintf(
        route,
        sizeof(route),
        "        DCC_VIEW_ACTION_ROUTES_DATA(\n"
        "            user_data,\n"
        "            %s\n"
        "        )%s",
        action_macro,
        has_modal_route ? "\n" : ",\n"
    );
    if (!has_modal_route) {
        size_t used = strlen(route);
        snprintf(
            route + used,
            sizeof(route) - used,
            "        DCC_VIEW_DATA(\n"
            "            user_data,\n"
            "            DCC_VIEW_MODAL(\"%s.%s.edit\", %s)\n"
            "        )\n",
            options->cog_name,
            options->command_name,
            modal_handler
        );
    }
    snprintf(
        duplicate_needle,
        sizeof(duplicate_needle),
        "DCC_VIEW_ACTION_ROUTES_DATA(\n"
        "            user_data,\n"
        "            %s",
        action_macro
    );
    return dcc_new_app_view_insert_route(path, route, duplicate_needle);
}

int dcc_new_app_generate_view(const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_view_cog_source_path(options);
    if (path == NULL) {
        return -1;
    }
    if (!dcc_new_app_path_exists(path)) {
        fprintf(stderr, "%s does not exist; add the feature first\n", path);
        free(path);
        return -1;
    }
    int status = dcc_new_app_view_insert_opener_handler(path, options);
    if (status == 0) {
        status = dcc_new_app_view_insert_handlers(path, options);
    }
    if (status == 0) {
        status = dcc_new_app_view_insert_routes(path, options);
    }
    if (status == 0) {
        status = dcc_new_app_view_insert_slash_route(path, options);
    }
    if (status == 0) {
        status = dcc_new_app_view_append_command_json(options);
    }
    free(path);
    if (status != 0) {
        return -1;
    }
    printf("created DCC view %s in feature %s under %s\n", options->command_name, options->cog_name, options->path);
    return 0;
}
