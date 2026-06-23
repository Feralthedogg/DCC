#include "internal/dcc_new_app.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_NEW_APP_HANDLER_MARKER "/* dcc_new_app:command-handlers */"
#define DCC_NEW_APP_PRESET_SLASH_ROUTE_MARKER "        /* dcc_new_app:preset-slash-routes */"
#define DCC_NEW_APP_COMPONENT_ROUTE_MARKER "        /* dcc_new_app:component-routes */"
#define DCC_NEW_APP_COMPONENT_EXTENSION_MARKER "    /* dcc_new_app:component-extension */"
#define DCC_NEW_APP_TYPED_MODAL_ROUTE_MARKER "        /* dcc_new_app:typed-modal-routes */"

static char *dcc_new_app_flow_cog_source_path(const dcc_new_app_options_t *options) {
    char relative_path[320];
    snprintf(relative_path, sizeof(relative_path), "src/%s.c", options->cog_name);
    return dcc_new_app_join(options->path, relative_path);
}

static int dcc_new_app_flow_file_contains(const char *path, const char *needle) {
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

static int dcc_new_app_flow_insert_route(
    const char *path,
    const char *route,
    const char *duplicate_needle,
    const char *route_marker,
    const char *cog_macro
) {
    int has_route_marker = dcc_new_app_flow_file_contains(path, route_marker);
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

static int dcc_new_app_flow_append_command_json(const dcc_new_app_options_t *options) {
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
        "    \"description\": \"Generated flow preset\",\n"
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

static int dcc_new_app_flow_insert_handlers(const char *path, const dcc_new_app_options_t *options) {
    char handler[320];
    char button_handler[320];
    char modal_handler[320];
    char modal_args[320];
    char modal_params[320];
    char insertion[7000];
    snprintf(handler, sizeof(handler), "on_%s_%s", options->cog_name, options->command_name);
    snprintf(button_handler, sizeof(button_handler), "on_%s_%s_button", options->cog_name, options->command_name);
    snprintf(modal_handler, sizeof(modal_handler), "on_%s_%s_modal", options->cog_name, options->command_name);
    snprintf(modal_args, sizeof(modal_args), "%s_args_t", modal_handler);
    dcc_new_app_make_macro_name(modal_handler, "_PARAMS", modal_params, sizeof(modal_params));

    snprintf(
        insertion,
        sizeof(insertion),
        "typedef struct %s_args {\n"
        "    const char *title;\n"
        "    const char *note;\n"
        "} %s;\n"
        "\n"
        "#define %s(PARAM, type) \\\n"
        "    PARAM(type, REQUIRED_STRING, title, \"%s.%s.title\") \\\n"
        "    PARAM(type, STRING, note, \"%s.%s.note\", \"\")\n"
        "\n"
        "DCC_HANDLER(%s) {\n"
        "    (void)user_data;\n"
        "\n"
        "    (void)DCC_SHOW_MODAL_V2(\n"
        "        ctx,\n"
        "        \"%s.%s.input\",\n"
        "        \"Generated Flow\",\n"
        "        DCC_MODAL_V2_FIELD_TEXT_PLACEHOLDER(\"%s.%s.title\", \"Title\", \"Enter a title\"),\n"
        "        DCC_MODAL_V2_FIELD_PARAGRAPH_OPTIONAL_PLACEHOLDER(\"%s.%s.note\", \"Note\", \"Optional note\")\n"
        "    );\n"
        "}\n"
        "\n"
        "DCC_MODAL_ARGS_IMPL(%s, %s, form) {\n"
        "    (void)user_data;\n"
        "\n"
        "    const char *title = form != NULL && form->title != NULL ? form->title : \"\";\n"
        "    const char *note = form != NULL && form->note != NULL ? form->note : \"\";\n"
        "    char text[384];\n"
        "    snprintf(text, sizeof(text), \"## Review flow input\\nTitle: %%s\\nNote: %%s\", title, note);\n"
        "    (void)DCC_REPLY(\n"
        "        ctx,\n"
        "        DCC_MESSAGE_COMPONENTS_V2(\n"
        "            DCC_UI_CARD_ACCENT(\n"
        "                0x5865F2,\n"
        "                DCC_UI_TEXT(text),\n"
        "                DCC_UI_ROW(\n"
        "                    DCC_UI_SUCCESS(\"Confirm\", \"%s.%s.confirm\"),\n"
        "                    DCC_UI_SECONDARY(\"Edit\", \"%s.%s.edit\"),\n"
        "                    DCC_UI_DANGER(\"Cancel\", \"%s.%s.cancel\")\n"
        "                )\n"
        "            )\n"
        "        )\n"
        "    );\n"
        "}\n"
        "\n"
        "DCC_HANDLER(%s) {\n"
        "    (void)user_data;\n"
        "\n"
        "    const char *action = DCC_CTX_ACTION(ctx, \"%s.%s.\");\n"
        "    if (dcc_sugar_ctx_text_eq(action, \"confirm\")) {\n"
        "        (void)DCC_UPDATE(ctx, DCC_MESSAGE_COMPONENTS_V2(DCC_UI_CARD_ACCENT(0x57F287, DCC_UI_TEXT(\"## Flow complete\\nConfirmed.\"))));\n"
        "        return;\n"
        "    }\n"
        "    if (dcc_sugar_ctx_text_eq(action, \"edit\")) {\n"
        "        (void)DCC_SHOW_MODAL_V2(\n"
        "            ctx,\n"
        "            \"%s.%s.input\",\n"
        "            \"Edit Flow\",\n"
        "            DCC_MODAL_V2_FIELD_TEXT_PLACEHOLDER(\"%s.%s.title\", \"Title\", \"Enter a title\"),\n"
        "            DCC_MODAL_V2_FIELD_PARAGRAPH_OPTIONAL_PLACEHOLDER(\"%s.%s.note\", \"Note\", \"Optional note\")\n"
        "        );\n"
        "        return;\n"
        "    }\n"
        "    if (dcc_sugar_ctx_text_eq(action, \"cancel\")) {\n"
        "        (void)DCC_UPDATE(ctx, DCC_MESSAGE_COMPONENTS_V2(DCC_UI_CARD_ACCENT(0xED4245, DCC_UI_TEXT(\"## Flow cancelled\"))));\n"
        "        return;\n"
        "    }\n"
        "    (void)DCC_CTX_BAD_INPUT(ctx, \"Unknown flow action.\");\n"
        "}\n"
        "\n",
        modal_handler,
        modal_args,
        modal_params,
        options->cog_name,
        options->command_name,
        options->cog_name,
        options->command_name,
        handler,
        options->cog_name,
        options->command_name,
        options->cog_name,
        options->command_name,
        options->cog_name,
        options->command_name,
        modal_handler,
        modal_args,
        options->cog_name,
        options->command_name,
        options->cog_name,
        options->command_name,
        options->cog_name,
        options->command_name,
        button_handler,
        options->cog_name,
        options->command_name,
        options->cog_name,
        options->command_name,
        options->cog_name,
        options->command_name,
        options->cog_name,
        options->command_name
    );
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_HANDLER_MARKER, insertion, handler);
}

static int dcc_new_app_flow_insert_routes(const char *path, const dcc_new_app_options_t *options) {
    char policy[1000];
    int guarded = dcc_new_app_guard_policy_literal(options, policy, sizeof(policy));
    if (guarded < 0) {
        return -1;
    }
    char handler[320];
    char button_handler[320];
    char modal_handler[320];
    char modal_args[320];
    char modal_params[320];
    snprintf(handler, sizeof(handler), "on_%s_%s", options->cog_name, options->command_name);
    snprintf(button_handler, sizeof(button_handler), "on_%s_%s_button", options->cog_name, options->command_name);
    snprintf(modal_handler, sizeof(modal_handler), "on_%s_%s_modal", options->cog_name, options->command_name);
    snprintf(modal_args, sizeof(modal_args), "%s_args_t", modal_handler);
    dcc_new_app_make_macro_name(modal_handler, "_PARAMS", modal_params, sizeof(modal_params));

    char slash_route[1200];
    snprintf(
        slash_route,
        sizeof(slash_route),
        "        %s(\"%s\", \"Generated flow preset\", %s, user_data%s%s%s)\n",
        guarded ? "DCC_COMMAND_DATA_POLICY" : "DCC_COMMAND_DATA",
        options->command_name,
        handler,
        guarded ? ", " : "",
        guarded ? policy : "",
        ""
    );
    char slash_needle[320];
    snprintf(slash_needle, sizeof(slash_needle), "\"%s\", \"Generated flow preset\"", options->command_name);
    int status = dcc_new_app_flow_insert_route(
        path, slash_route, slash_needle, DCC_NEW_APP_PRESET_SLASH_ROUTE_MARKER, "DCC_FEATURE_COMMANDS"
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
    status = dcc_new_app_flow_insert_route(
        path, button_route, button_needle, DCC_NEW_APP_COMPONENT_ROUTE_MARKER, "DCC_FEATURE_COMPONENTS"
    );
    if (status != 0) {
        return status;
    }

    char modal_route[1200];
    snprintf(
        modal_route,
        sizeof(modal_route),
        "        %s(\"%s.%s.input\", %s, %s, user_data, %s%s%s)\n",
        guarded ? "DCC_MODAL_PARAMS_DATA_POLICY" : "DCC_MODAL_PARAMS_DATA",
        options->cog_name,
        options->command_name,
        modal_args,
        modal_handler,
        modal_params,
        guarded ? ", " : "",
        guarded ? policy : ""
    );
    char modal_needle[320];
    snprintf(modal_needle, sizeof(modal_needle), "\"%s.%s.input\", %s", options->cog_name, options->command_name, modal_args);
    return dcc_new_app_flow_insert_route(
        path, modal_route, modal_needle, DCC_NEW_APP_TYPED_MODAL_ROUTE_MARKER, "DCC_FEATURE_MODAL_ROUTES"
    );
}

int dcc_new_app_generate_flow_preset(const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_flow_cog_source_path(options);
    if (path == NULL) {
        return -1;
    }
    if (!dcc_new_app_path_exists(path)) {
        fprintf(stderr, "%s does not exist; add the feature first\n", path);
        free(path);
        return -1;
    }
    int status = dcc_new_app_flow_insert_handlers(path, options);
    if (status == 0) {
        status = dcc_new_app_flow_insert_routes(path, options);
    }
    free(path);
    if (status == 0) {
        status = dcc_new_app_flow_append_command_json(options);
    }
    if (status != 0) {
        return -1;
    }
    printf("created DCC flow preset %s in feature %s under %s\n", options->command_name, options->cog_name, options->path);
    return 0;
}
