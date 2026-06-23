#include "internal/dcc_new_app.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_NEW_APP_HANDLER_MARKER "/* dcc_new_app:command-handlers */"
#define DCC_NEW_APP_PRESET_SLASH_ROUTE_MARKER "        /* dcc_new_app:preset-slash-routes */"
#define DCC_NEW_APP_TYPED_SELECT_ROUTE_MARKER "        /* dcc_new_app:typed-select-routes */"
#define DCC_NEW_APP_COMPONENT_EXTENSION_MARKER "    /* dcc_new_app:component-extension */"

static char *dcc_new_app_menu_cog_source_path(const dcc_new_app_options_t *options) {
    char relative_path[320];
    snprintf(relative_path, sizeof(relative_path), "src/%s.c", options->cog_name);
    return dcc_new_app_join(options->path, relative_path);
}

static int dcc_new_app_menu_file_contains(const char *path, const char *needle) {
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

static int dcc_new_app_menu_insert_route(
    const char *path,
    const char *route,
    const char *duplicate_needle,
    const char *route_marker,
    const char *cog_macro
) {
    int has_route_marker = dcc_new_app_menu_file_contains(path, route_marker);
    if (has_route_marker < 0) {
        return -1;
    }
    if (has_route_marker) {
        char insertion[1400];
        snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
        return dcc_new_app_insert_before_marker(path, route_marker, insertion, duplicate_needle);
    }
    char insertion[1600];
    snprintf(
        insertion,
        sizeof(insertion),
        "    ,\n"
        "    %s(\n"
        "%s"
        "%s\n"
        "    )\n",
        cog_macro,
        route,
        route_marker
    );
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_COMPONENT_EXTENSION_MARKER, insertion, duplicate_needle);
}

static int dcc_new_app_menu_append_command_json(const dcc_new_app_options_t *options) {
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

    size_t prefix_len = (size_t)(last_bracket - data);
    while (prefix_len > 0U && isspace((unsigned char)data[prefix_len - 1U])) {
        --prefix_len;
    }
    char command[480];
    snprintf(
        command,
        sizeof(command),
        "%s\n"
        "  {\n"
        "    \"name\": \"%s\",\n"
        "    \"description\": \"Generated menu preset\",\n"
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

static int dcc_new_app_menu_insert_handlers(const char *path, const dcc_new_app_options_t *options) {
    char handler[320];
    char select_handler[320];
    char select_args[320];
    char select_params[320];
    char insertion[5600];
    snprintf(handler, sizeof(handler), "on_%s_%s", options->cog_name, options->command_name);
    snprintf(select_handler, sizeof(select_handler), "on_%s_%s_select", options->cog_name, options->command_name);
    snprintf(select_args, sizeof(select_args), "%s_args_t", select_handler);
    dcc_new_app_make_macro_name(select_handler, "_PARAMS", select_params, sizeof(select_params));

    snprintf(
        insertion,
        sizeof(insertion),
        "typedef struct %s_args {\n"
        "    const char *choice;\n"
        "} %s;\n"
        "\n"
        "#define %s(PARAM, type) \\\n"
        "    PARAM(type, REQUIRED_FIRST_VALUE, choice)\n"
        "\n"
        "static void %s_render(dcc_ctx_t *ctx, const char *selected, uint8_t update) {\n"
        "    char text[256];\n"
        "    snprintf(\n"
        "        text,\n"
        "        sizeof(text),\n"
        "        \"## Menu\\nSelected: %%s\\nChoose a route from the select menu.\",\n"
        "        selected != NULL && selected[0] != '\\0' ? selected : \"none\"\n"
        "    );\n"
        "    dcc_message_builder_t message = DCC_MESSAGE_COMPONENTS_V2(\n"
        "        DCC_UI_CARD_ACCENT(\n"
        "            0x5865F2,\n"
        "            DCC_UI_TEXT(text),\n"
        "            DCC_UI_STRING_SELECT(\n"
        "                \"%s.%s\",\n"
        "                DCC_SELECT_OPTION_DESC(\"Overview\", \"overview\", \"Show the overview panel\"),\n"
        "                DCC_SELECT_OPTION_DESC(\"Status\", \"status\", \"Show current status\"),\n"
        "                DCC_SELECT_OPTION_DESC(\"Help\", \"help\", \"Show help actions\")\n"
        "            )\n"
        "        )\n"
        "    );\n"
        "    if (update) {\n"
        "        (void)DCC_UPDATE(ctx, message);\n"
        "    } else {\n"
        "        (void)DCC_REPLY(ctx, message);\n"
        "    }\n"
        "}\n"
        "\n"
        "DCC_HANDLER(%s) {\n"
        "    (void)user_data;\n"
        "    %s_render(ctx, \"none\", 0U);\n"
        "}\n"
        "\n"
        "DCC_SELECT_ARGS_IMPL(%s, %s, select) {\n"
        "    (void)user_data;\n"
        "\n"
        "    const char *choice = select != NULL && select->choice != NULL ? select->choice : \"unknown\";\n"
        "    if (!dcc_sugar_ctx_text_eq(choice, \"overview\") &&\n"
        "        !dcc_sugar_ctx_text_eq(choice, \"status\") &&\n"
        "        !dcc_sugar_ctx_text_eq(choice, \"help\")) {\n"
        "        (void)DCC_CTX_BAD_INPUT(ctx, \"Unknown menu choice.\");\n"
        "        return;\n"
        "    }\n"
        "    %s_render(ctx, choice, 1U);\n"
        "}\n"
        "\n",
        select_handler,
        select_args,
        select_params,
        handler,
        options->cog_name,
        options->command_name,
        handler,
        handler,
        select_handler,
        select_args,
        handler
    );
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_HANDLER_MARKER, insertion, handler);
}

static int dcc_new_app_menu_insert_routes(const char *path, const dcc_new_app_options_t *options) {
    char policy[1000];
    int guarded = dcc_new_app_guard_policy_literal(options, policy, sizeof(policy));
    if (guarded < 0) {
        return -1;
    }
    char handler[320];
    char select_handler[320];
    char select_args[320];
    char select_params[320];
    snprintf(handler, sizeof(handler), "on_%s_%s", options->cog_name, options->command_name);
    snprintf(select_handler, sizeof(select_handler), "on_%s_%s_select", options->cog_name, options->command_name);
    snprintf(select_args, sizeof(select_args), "%s_args_t", select_handler);
    dcc_new_app_make_macro_name(select_handler, "_PARAMS", select_params, sizeof(select_params));

    char slash_route[1200];
    snprintf(
        slash_route,
        sizeof(slash_route),
        "        %s(\"%s\", \"Generated menu preset\", %s, user_data%s%s%s)\n",
        guarded ? "DCC_COMMAND_DATA_POLICY" : "DCC_COMMAND_DATA",
        options->command_name,
        handler,
        guarded ? ", " : "",
        guarded ? policy : "",
        ""
    );
    char slash_needle[320];
    snprintf(slash_needle, sizeof(slash_needle), "\"%s\", \"Generated menu preset\"", options->command_name);
    int status = dcc_new_app_menu_insert_route(
        path, slash_route, slash_needle, DCC_NEW_APP_PRESET_SLASH_ROUTE_MARKER, "DCC_FEATURE_COMMANDS"
    );
    if (status != 0) {
        return status;
    }

    char select_route[1200];
    snprintf(
        select_route,
        sizeof(select_route),
        "        %s(\"%s.%s\", %s, %s, user_data, %s%s%s)\n",
        guarded ? "DCC_SELECT_PARAMS_DATA_POLICY" : "DCC_SELECT_PARAMS_DATA",
        options->cog_name,
        options->command_name,
        select_args,
        select_handler,
        select_params,
        guarded ? ", " : "",
        guarded ? policy : ""
    );
    char select_needle[320];
    snprintf(select_needle, sizeof(select_needle), "\"%s.%s\", %s", options->cog_name, options->command_name, select_args);
    return dcc_new_app_menu_insert_route(
        path, select_route, select_needle, DCC_NEW_APP_TYPED_SELECT_ROUTE_MARKER, "DCC_FEATURE_SELECT_ROUTES"
    );
}

int dcc_new_app_generate_menu_preset(const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_menu_cog_source_path(options);
    if (path == NULL) {
        return -1;
    }
    if (!dcc_new_app_path_exists(path)) {
        fprintf(stderr, "%s does not exist; add the feature first\n", path);
        free(path);
        return -1;
    }
    int status = dcc_new_app_menu_insert_handlers(path, options);
    if (status == 0) {
        status = dcc_new_app_menu_insert_routes(path, options);
    }
    free(path);
    if (status == 0) {
        status = dcc_new_app_menu_append_command_json(options);
    }
    if (status != 0) {
        return -1;
    }
    printf("created DCC menu preset %s in feature %s under %s\n", options->command_name, options->cog_name, options->path);
    return 0;
}
