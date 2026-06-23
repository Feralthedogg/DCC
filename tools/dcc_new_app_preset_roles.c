#include "internal/dcc_new_app.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_NEW_APP_HANDLER_MARKER "/* dcc_new_app:command-handlers */"
#define DCC_NEW_APP_PRESET_SLASH_ROUTE_MARKER "        /* dcc_new_app:preset-slash-routes */"
#define DCC_NEW_APP_COMPONENT_ROUTE_MARKER "        /* dcc_new_app:component-routes */"
#define DCC_NEW_APP_TYPED_SELECT_ROUTE_MARKER "        /* dcc_new_app:typed-select-routes */"
#define DCC_NEW_APP_COMPONENT_EXTENSION_MARKER "    /* dcc_new_app:component-extension */"

static char *dcc_new_app_roles_cog_source_path(const dcc_new_app_options_t *options) {
    char relative_path[320];
    snprintf(relative_path, sizeof(relative_path), "src/%s.c", options->cog_name);
    return dcc_new_app_join(options->path, relative_path);
}

static int dcc_new_app_roles_file_contains(const char *path, const char *needle) {
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

static int dcc_new_app_roles_insert_route(
    const char *path,
    const char *route,
    const char *duplicate_needle,
    const char *route_marker,
    const char *cog_macro
) {
    int has_route_marker = dcc_new_app_roles_file_contains(path, route_marker);
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

static int dcc_new_app_roles_append_command_json(const dcc_new_app_options_t *options) {
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
        "    \"description\": \"Generated role menu preset\",\n"
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

static int dcc_new_app_roles_insert_handlers(const char *path, const dcc_new_app_options_t *options) {
    char handler[320];
    char select_handler[320];
    char button_handler[320];
    char select_args[320];
    char select_params[320];
    char alpha_macro[320];
    char beta_macro[320];
    char gamma_macro[320];
    char insertion[7800];
    snprintf(handler, sizeof(handler), "on_%s_%s", options->cog_name, options->command_name);
    snprintf(select_handler, sizeof(select_handler), "on_%s_%s_select", options->cog_name, options->command_name);
    snprintf(button_handler, sizeof(button_handler), "on_%s_%s_button", options->cog_name, options->command_name);
    snprintf(select_args, sizeof(select_args), "%s_args_t", select_handler);
    dcc_new_app_make_macro_name(select_handler, "_PARAMS", select_params, sizeof(select_params));
    dcc_new_app_make_macro_name(handler, "_ALPHA_ROLE_ID", alpha_macro, sizeof(alpha_macro));
    dcc_new_app_make_macro_name(handler, "_BETA_ROLE_ID", beta_macro, sizeof(beta_macro));
    dcc_new_app_make_macro_name(handler, "_GAMMA_ROLE_ID", gamma_macro, sizeof(gamma_macro));

    snprintf(
        insertion,
        sizeof(insertion),
        "#ifndef %s\n"
        "#define %s 0ULL\n"
        "#endif\n"
        "#ifndef %s\n"
        "#define %s 0ULL\n"
        "#endif\n"
        "#ifndef %s\n"
        "#define %s 0ULL\n"
        "#endif\n"
        "\n"
        "typedef struct %s_args {\n"
        "    const char *choice;\n"
        "} %s;\n"
        "\n"
        "#define %s(PARAM, type) \\\n"
        "    PARAM(type, REQUIRED_FIRST_VALUE, choice)\n"
        "\n"
        "static dcc_snowflake_t %s_role_id(const char *choice) {\n"
        "    if (dcc_sugar_ctx_text_eq(choice, \"alpha\")) {\n"
        "        return %s;\n"
        "    }\n"
        "    if (dcc_sugar_ctx_text_eq(choice, \"beta\")) {\n"
        "        return %s;\n"
        "    }\n"
        "    if (dcc_sugar_ctx_text_eq(choice, \"gamma\")) {\n"
        "        return %s;\n"
        "    }\n"
        "    return 0U;\n"
        "}\n"
        "\n"
        "static const char *%s_role_label(const char *choice) {\n"
        "    if (dcc_sugar_ctx_text_eq(choice, \"alpha\")) {\n"
        "        return \"Alpha\";\n"
        "    }\n"
        "    if (dcc_sugar_ctx_text_eq(choice, \"beta\")) {\n"
        "        return \"Beta\";\n"
        "    }\n"
        "    if (dcc_sugar_ctx_text_eq(choice, \"gamma\")) {\n"
        "        return \"Gamma\";\n"
        "    }\n"
        "    return \"Unknown\";\n"
        "}\n"
        "\n"
        "static void %s_render(dcc_ctx_t *ctx, uint8_t update) {\n"
        "    dcc_message_builder_t message = DCC_MESSAGE_COMPONENTS_V2(\n"
        "        DCC_UI_CARD_ACCENT(\n"
        "            0x5865F2,\n"
        "            DCC_UI_TEXT(\"## Role Menu\\nPick a role to add it. Use the buttons to remove roles.\"),\n"
        "            DCC_UI_STRING_SELECT(\n"
        "                \"%s.%s.pick\",\n"
        "                DCC_SELECT_OPTION_DESC(\"Alpha\", \"alpha\", \"Replace the generated Alpha role id\"),\n"
        "                DCC_SELECT_OPTION_DESC(\"Beta\", \"beta\", \"Replace the generated Beta role id\"),\n"
        "                DCC_SELECT_OPTION_DESC(\"Gamma\", \"gamma\", \"Replace the generated Gamma role id\")\n"
        "            ),\n"
        "            DCC_UI_ROW(\n"
        "                DCC_UI_SECONDARY(\"Remove Alpha\", \"%s.%s.remove_alpha\"),\n"
        "                DCC_UI_SECONDARY(\"Remove Beta\", \"%s.%s.remove_beta\"),\n"
        "                DCC_UI_SECONDARY(\"Remove Gamma\", \"%s.%s.remove_gamma\")\n"
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
        "    %s_render(ctx, 0U);\n"
        "}\n"
        "\n"
        "static void %s_apply(dcc_ctx_t *ctx, const char *choice, uint8_t remove_role) {\n"
        "    if (!DCC_CTX_IS_GUILD(ctx)) {\n"
        "        (void)DCC_CTX_BAD_INPUT(ctx, \"Role menus can only be used in a server.\");\n"
        "        return;\n"
        "    }\n"
        "    dcc_snowflake_t role_id = %s_role_id(choice);\n"
        "    if (role_id == 0U) {\n"
        "        (void)DCC_CTX_BAD_INPUT(ctx, \"Set the generated role id macros before using this role menu.\");\n"
        "        return;\n"
        "    }\n"
        "    DCC_CTX_TRY_MSG(\n"
        "        ctx,\n"
        "        remove_role ? DCC_CTX_REMOVE_AUTHOR_ROLE(ctx, role_id) : DCC_CTX_ADD_AUTHOR_ROLE(ctx, role_id),\n"
        "        remove_role ? \"Could not remove that role.\" : \"Could not add that role.\"\n"
        "    );\n"
        "\n"
        "    char text[192];\n"
        "    snprintf(text, sizeof(text), \"%%s %%s.\", remove_role ? \"Removed\" : \"Added\", %s_role_label(choice));\n"
        "    (void)DCC_CTX_EPHEMERAL_UI(ctx, DCC_UI_CARD_ACCENT(0x57F287, DCC_UI_TEXT(text)));\n"
        "}\n"
        "\n"
        "DCC_SELECT_ARGS_IMPL(%s, %s, select) {\n"
        "    (void)user_data;\n"
        "\n"
        "    const char *choice = select != NULL && select->choice != NULL ? select->choice : \"\";\n"
        "    %s_apply(ctx, choice, 0U);\n"
        "}\n"
        "\n"
        "DCC_HANDLER(%s) {\n"
        "    (void)user_data;\n"
        "\n"
        "    const char *action = DCC_CTX_ACTION(ctx, \"%s.%s.\");\n"
        "    if (dcc_sugar_ctx_text_eq(action, \"remove_alpha\")) {\n"
        "        %s_apply(ctx, \"alpha\", 1U);\n"
        "        return;\n"
        "    }\n"
        "    if (dcc_sugar_ctx_text_eq(action, \"remove_beta\")) {\n"
        "        %s_apply(ctx, \"beta\", 1U);\n"
        "        return;\n"
        "    }\n"
        "    if (dcc_sugar_ctx_text_eq(action, \"remove_gamma\")) {\n"
        "        %s_apply(ctx, \"gamma\", 1U);\n"
        "        return;\n"
        "    }\n"
        "    (void)DCC_CTX_BAD_INPUT(ctx, \"Unknown role-menu action.\");\n"
        "}\n"
        "\n",
        alpha_macro,
        alpha_macro,
        beta_macro,
        beta_macro,
        gamma_macro,
        gamma_macro,
        select_handler,
        select_args,
        select_params,
        handler,
        alpha_macro,
        beta_macro,
        gamma_macro,
        handler,
        handler,
        options->cog_name,
        options->command_name,
        options->cog_name,
        options->command_name,
        options->cog_name,
        options->command_name,
        options->cog_name,
        options->command_name,
        handler,
        handler,
        handler,
        handler,
        handler,
        select_handler,
        select_args,
        handler,
        button_handler,
        options->cog_name,
        options->command_name,
        handler,
        handler,
        handler
    );
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_HANDLER_MARKER, insertion, handler);
}

static int dcc_new_app_roles_insert_routes(const char *path, const dcc_new_app_options_t *options) {
    char policy[1000];
    int guarded = dcc_new_app_guard_policy_literal(options, policy, sizeof(policy));
    if (guarded < 0) {
        return -1;
    }
    char handler[320];
    char select_handler[320];
    char button_handler[320];
    char select_args[320];
    char select_params[320];
    snprintf(handler, sizeof(handler), "on_%s_%s", options->cog_name, options->command_name);
    snprintf(select_handler, sizeof(select_handler), "on_%s_%s_select", options->cog_name, options->command_name);
    snprintf(button_handler, sizeof(button_handler), "on_%s_%s_button", options->cog_name, options->command_name);
    snprintf(select_args, sizeof(select_args), "%s_args_t", select_handler);
    dcc_new_app_make_macro_name(select_handler, "_PARAMS", select_params, sizeof(select_params));

    char slash_route[1200];
    snprintf(
        slash_route,
        sizeof(slash_route),
        "        %s(\"%s\", \"Generated role menu preset\", %s, user_data%s%s%s)\n",
        guarded ? "DCC_COMMAND_DATA_POLICY" : "DCC_COMMAND_DATA",
        options->command_name,
        handler,
        guarded ? ", " : "",
        guarded ? policy : "",
        ""
    );
    char slash_needle[320];
    snprintf(slash_needle, sizeof(slash_needle), "\"%s\", \"Generated role menu preset\"", options->command_name);
    int status = dcc_new_app_roles_insert_route(
        path, slash_route, slash_needle, DCC_NEW_APP_PRESET_SLASH_ROUTE_MARKER, "DCC_FEATURE_COMMANDS"
    );
    if (status != 0) {
        return status;
    }

    char select_route[1200];
    snprintf(
        select_route,
        sizeof(select_route),
        "        %s(\"%s.%s.pick\", %s, %s, user_data, %s%s%s)\n",
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
    snprintf(select_needle, sizeof(select_needle), "\"%s.%s.pick\", %s", options->cog_name, options->command_name, select_args);
    status = dcc_new_app_roles_insert_route(
        path, select_route, select_needle, DCC_NEW_APP_TYPED_SELECT_ROUTE_MARKER, "DCC_FEATURE_SELECT_ROUTES"
    );
    if (status != 0) {
        return status;
    }

    char button_route[1200];
    snprintf(
        button_route,
        sizeof(button_route),
        "        %s(\"%s.%s.\", %s, user_data%s%s%s)\n",
        guarded ? "DCC_BUTTON_PREFIX_DATA_POLICY" : "DCC_BUTTON_PREFIX_DATA",
        options->cog_name,
        options->command_name,
        button_handler,
        guarded ? ", " : "",
        guarded ? policy : "",
        ""
    );
    char button_needle[320];
    snprintf(button_needle, sizeof(button_needle), "\"%s.%s.\", %s", options->cog_name, options->command_name, button_handler);
    return dcc_new_app_roles_insert_route(
        path, button_route, button_needle, DCC_NEW_APP_COMPONENT_ROUTE_MARKER, "DCC_FEATURE_COMPONENTS"
    );
}

int dcc_new_app_generate_roles_preset(const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_roles_cog_source_path(options);
    if (path == NULL) {
        return -1;
    }
    if (!dcc_new_app_path_exists(path)) {
        fprintf(stderr, "%s does not exist; add the feature first\n", path);
        free(path);
        return -1;
    }
    int status = dcc_new_app_roles_insert_handlers(path, options);
    if (status == 0) {
        status = dcc_new_app_roles_insert_routes(path, options);
    }
    free(path);
    if (status == 0) {
        status = dcc_new_app_roles_append_command_json(options);
    }
    if (status != 0) {
        return -1;
    }
    printf("created DCC role menu preset %s in feature %s under %s\n", options->command_name, options->cog_name, options->path);
    return 0;
}
