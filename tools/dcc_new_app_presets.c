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
#define DCC_NEW_APP_TYPED_MODAL_EXTENSION_MARKER "    /* dcc_new_app:typed-modal-extension */"

static char *dcc_new_app_preset_cog_source_path(const dcc_new_app_options_t *options) {
    char relative_path[320];
    snprintf(relative_path, sizeof(relative_path), "src/%s.c", options->cog_name);
    return dcc_new_app_join(options->path, relative_path);
}

static int dcc_new_app_preset_file_contains(const char *path, const char *needle) {
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

static int dcc_new_app_preset_require_cog_source(char **out_path, const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_preset_cog_source_path(options);
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

static const char *dcc_new_app_preset_kind(const dcc_new_app_options_t *options) {
    if (options == NULL || options->command_arg == NULL) {
        return NULL;
    }
    if (strcmp(options->command_arg, "confirm") == 0 ||
        strcmp(options->command_arg, "confirmation") == 0) {
        return "confirm";
    }
    if (strcmp(options->command_arg, "paginator") == 0 ||
        strcmp(options->command_arg, "pagination") == 0 ||
        strcmp(options->command_arg, "pages") == 0) {
        return "paginator";
    }
    if (strcmp(options->command_arg, "form") == 0 ||
        strcmp(options->command_arg, "modal-form") == 0 ||
        strcmp(options->command_arg, "modal_form") == 0) {
        return "modal_form";
    }
    return NULL;
}

static int dcc_new_app_preset_append_command_json(const dcc_new_app_options_t *options, const char *description) {
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
    char command[480];
    snprintf(
        command,
        sizeof(command),
        "%s\n"
        "  {\n"
        "    \"name\": \"%s\",\n"
        "    \"description\": \"%s\",\n"
        "    \"type\": 1\n"
        "  }\n",
        has_existing ? "," : "",
        options->command_name,
        description
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

static int dcc_new_app_preset_insert_component_route(const char *path, const char *route, const char *duplicate_needle) {
    int has_route_marker = dcc_new_app_preset_file_contains(path, DCC_NEW_APP_COMPONENT_ROUTE_MARKER);
    if (has_route_marker < 0) {
        return -1;
    }
    if (has_route_marker) {
        char insertion[1200];
        snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
        return dcc_new_app_insert_before_marker(
            path,
            DCC_NEW_APP_COMPONENT_ROUTE_MARKER,
            insertion,
            duplicate_needle
        );
    }

    char insertion[1400];
    snprintf(
        insertion,
        sizeof(insertion),
        "    ,\n"
        "    DCC_FEATURE_COMPONENTS(\n"
        "%s"
        "        /* dcc_new_app:component-routes */\n"
        "    )\n",
        route
    );
    return dcc_new_app_insert_before_marker(
        path,
        DCC_NEW_APP_COMPONENT_EXTENSION_MARKER,
        insertion,
        duplicate_needle
    );
}

static int dcc_new_app_preset_insert_slash_route(const char *path, const char *route, const char *duplicate_needle) {
    int has_route_marker = dcc_new_app_preset_file_contains(path, DCC_NEW_APP_PRESET_SLASH_ROUTE_MARKER);
    if (has_route_marker < 0) {
        return -1;
    }
    if (has_route_marker) {
        char insertion[1200];
        snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
        return dcc_new_app_insert_before_marker(
            path,
            DCC_NEW_APP_PRESET_SLASH_ROUTE_MARKER,
            insertion,
            duplicate_needle
        );
    }

    char insertion[1400];
    snprintf(
        insertion,
        sizeof(insertion),
        "    ,\n"
        "    DCC_FEATURE_COMMANDS(\n"
        "%s"
        "        /* dcc_new_app:preset-slash-routes */\n"
        "    )\n",
        route
    );
    return dcc_new_app_insert_before_marker(
        path,
        DCC_NEW_APP_COMPONENT_EXTENSION_MARKER,
        insertion,
        duplicate_needle
    );
}

static int dcc_new_app_preset_insert_typed_modal_route(const char *path, const char *route, const char *duplicate_needle) {
    int has_route_marker = dcc_new_app_preset_file_contains(path, DCC_NEW_APP_TYPED_MODAL_ROUTE_MARKER);
    if (has_route_marker < 0) {
        return -1;
    }
    if (has_route_marker) {
        char insertion[1200];
        snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
        return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_TYPED_MODAL_ROUTE_MARKER, insertion, duplicate_needle);
    }

    char insertion[1400];
    snprintf(
        insertion,
        sizeof(insertion),
        "    ,\n"
        "    DCC_FEATURE_MODAL_ROUTES(\n"
        "%s"
        "        /* dcc_new_app:typed-modal-routes */\n"
        "    )\n"
        "    /* dcc_new_app:typed-modal-extension */\n",
        route
    );
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_COMPONENT_EXTENSION_MARKER, insertion, duplicate_needle);
}

static int dcc_new_app_preset_insert_handlers(const char *path, const dcc_new_app_options_t *options, const char *kind) {
    char command_handler[320];
    char button_handler[320];
    char modal_handler[320];
    char modal_args_type[320];
    char modal_params_macro[320];
    char insertion[3600];
    snprintf(command_handler, sizeof(command_handler), "on_%s_%s", options->cog_name, options->command_name);
    snprintf(button_handler, sizeof(button_handler), "on_%s_%s_button", options->cog_name, options->command_name);
    snprintf(modal_handler, sizeof(modal_handler), "on_%s_%s_modal", options->cog_name, options->command_name);
    snprintf(modal_args_type, sizeof(modal_args_type), "%s_args_t", modal_handler);
    dcc_new_app_make_macro_name(modal_handler, "_PARAMS", modal_params_macro, sizeof(modal_params_macro));

    if (strcmp(kind, "confirm") == 0) {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_HANDLER(%s) {\n"
            "    (void)user_data;\n"
            "\n"
            "    (void)DCC_REPLY(\n"
            "        ctx,\n"
            "        DCC_CONFIRM_MESSAGE(\"Run this action?\", \"%s.%s.yes\", \"%s.%s.no\")\n"
            "    );\n"
            "}\n"
            "\n"
            "DCC_HANDLER(%s) {\n"
            "    (void)user_data;\n"
            "\n"
            "    const char *action = DCC_CTX_ACTION(ctx, \"%s.%s.\");\n"
            "    if (dcc_sugar_ctx_text_eq(action, \"yes\")) {\n"
            "        (void)DCC_UPDATE(ctx, DCC_MESSAGE_TEXT(\"Confirmed.\"));\n"
            "        return;\n"
            "    }\n"
            "    if (dcc_sugar_ctx_text_eq(action, \"no\")) {\n"
            "        (void)DCC_UPDATE(ctx, DCC_MESSAGE_TEXT(\"Cancelled.\"));\n"
            "        return;\n"
            "    }\n"
            "    (void)DCC_CTX_BAD_INPUT(ctx, \"Unknown confirmation action.\");\n"
            "}\n"
            "\n",
            command_handler,
            options->cog_name,
            options->command_name,
            options->cog_name,
            options->command_name,
            button_handler,
            options->cog_name,
            options->command_name
        );
    } else if (strcmp(kind, "paginator") == 0) {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_HANDLER(%s) {\n"
            "    (void)user_data;\n"
            "\n"
            "    (void)DCC_REPLY(\n"
            "        ctx,\n"
            "        DCC_PAGINATOR_MESSAGE(\"Page 1/3\", \"%s.%s.prev\", \"%s.%s.next\", 1U, 0U)\n"
            "    );\n"
            "}\n"
            "\n"
            "DCC_HANDLER(%s) {\n"
            "    (void)user_data;\n"
            "\n"
            "    const char *action = DCC_CTX_ACTION(ctx, \"%s.%s.\");\n"
            "    uint8_t at_start = dcc_sugar_ctx_text_eq(action, \"prev\");\n"
            "    uint8_t at_end = dcc_sugar_ctx_text_eq(action, \"next\");\n"
            "    (void)DCC_UPDATE(\n"
            "        ctx,\n"
            "        DCC_PAGINATOR_MESSAGE(\"Generated page\", \"%s.%s.prev\", \"%s.%s.next\", at_start, at_end)\n"
            "    );\n"
            "}\n"
            "\n",
            command_handler,
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
            options->command_name
        );
    } else {
        snprintf(
            insertion,
            sizeof(insertion),
            "typedef struct %s_args {\n"
            "    const char *name;\n"
            "    const char *note;\n"
            "} %s;\n"
            "\n"
            "#define %s(PARAM, type) \\\n"
            "    PARAM(type, REQUIRED_STRING, name, \"%s.%s.name\") \\\n"
            "    PARAM(type, STRING, note, \"%s.%s.note\", \"\")\n"
            "\n"
            "DCC_HANDLER(%s) {\n"
            "    (void)user_data;\n"
            "\n"
            "    (void)DCC_SHOW_MODAL_V2(\n"
            "        ctx,\n"
            "        \"%s.%s\",\n"
            "        \"Generated Form\",\n"
            "        DCC_MODAL_V2_FIELD_TEXT_PLACEHOLDER(\"%s.%s.name\", \"Name\", \"Enter a value\"),\n"
            "        DCC_MODAL_V2_FIELD_PARAGRAPH_OPTIONAL_PLACEHOLDER(\"%s.%s.note\", \"Note\", \"Optional note\")\n"
            "    );\n"
            "}\n"
            "\n"
            "DCC_MODAL_ARGS_IMPL(%s, %s, form) {\n"
            "    (void)user_data;\n"
            "\n"
            "    const char *name = form != NULL && form->name != NULL ? form->name : \"\";\n"
            "    (void)DCC_CTX_DONEF(ctx, \"Submitted form for %%s\", name);\n"
            "}\n"
            "\n",
            modal_handler,
            modal_args_type,
            modal_params_macro,
            options->cog_name,
            options->command_name,
            options->cog_name,
            options->command_name,
            command_handler,
            options->cog_name,
            options->command_name,
            options->cog_name,
            options->command_name,
            options->cog_name,
            options->command_name,
            modal_handler,
            modal_args_type
        );
    }

    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_HANDLER_MARKER, insertion, command_handler);
}

static int dcc_new_app_preset_insert_routes(
    const char *path, const dcc_new_app_options_t *options, const char *kind, const char *description
) {
    char policy[1000];
    int guarded = dcc_new_app_guard_policy_literal(options, policy, sizeof(policy));
    if (guarded < 0) {
        return -1;
    }

    char command_handler[320];
    char button_handler[320];
    snprintf(command_handler, sizeof(command_handler), "on_%s_%s", options->cog_name, options->command_name);
    snprintf(button_handler, sizeof(button_handler), "on_%s_%s_button", options->cog_name, options->command_name);

    char slash_route[1200];
    snprintf(
        slash_route,
        sizeof(slash_route),
        "        %s(\"%s\", \"%s\", %s, user_data%s%s%s)\n",
        guarded ? "DCC_COMMAND_DATA_POLICY" : "DCC_COMMAND_DATA",
        options->command_name,
        description,
        command_handler,
        guarded ? ", " : "",
        guarded ? policy : "",
        ""
    );
    char slash_needle[320];
    snprintf(slash_needle, sizeof(slash_needle), "\"%s\", \"%s\"", options->command_name, description);
    int status = dcc_new_app_preset_insert_slash_route(path, slash_route, slash_needle);
    if (status != 0) {
        return status;
    }
    if (strcmp(kind, "modal_form") == 0) {
        char modal_handler[320];
        char modal_args_type[320];
        char modal_params_macro[320];
        snprintf(modal_handler, sizeof(modal_handler), "on_%s_%s_modal", options->cog_name, options->command_name);
        snprintf(modal_args_type, sizeof(modal_args_type), "%s_args_t", modal_handler);
        dcc_new_app_make_macro_name(modal_handler, "_PARAMS", modal_params_macro, sizeof(modal_params_macro));

        char modal_route[1200];
        snprintf(
            modal_route,
            sizeof(modal_route),
            "        %s(\"%s.%s\", %s, %s, user_data, %s%s%s)\n",
            guarded ? "DCC_MODAL_PARAMS_DATA_POLICY" : "DCC_MODAL_PARAMS_DATA",
            options->cog_name,
            options->command_name,
            modal_args_type,
            modal_handler,
            modal_params_macro,
            guarded ? ", " : "",
            guarded ? policy : ""
        );
        char modal_needle[320];
        snprintf(modal_needle, sizeof(modal_needle), "\"%s.%s\", %s", options->cog_name, options->command_name, modal_args_type);
        return dcc_new_app_preset_insert_typed_modal_route(path, modal_route, modal_needle);
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
    snprintf(
        button_needle,
        sizeof(button_needle),
        "\"%s.%s.\", %s",
        options->cog_name,
        options->command_name,
        button_handler
    );
    return dcc_new_app_preset_insert_component_route(path, button_route, button_needle);
}

int dcc_new_app_generate_preset(const dcc_new_app_options_t *options) {
    const char *kind = dcc_new_app_preset_kind(options);
    if (kind == NULL) {
        fprintf(stderr, "unknown preset %s; expected confirm, paginator, or form\n", options->command_arg);
        return -1;
    }

    char *path = NULL;
    if (dcc_new_app_preset_require_cog_source(&path, options) != 0) {
        return -1;
    }
    const char *description = strcmp(kind, "confirm") == 0
        ? "Generated confirm preset"
        : strcmp(kind, "paginator") == 0
            ? "Generated paginator preset"
            : "Generated form preset";
    int status = dcc_new_app_preset_insert_handlers(path, options, kind);
    if (status == 0) {
        status = dcc_new_app_preset_insert_routes(path, options, kind, description);
    }
    free(path);
    if (status == 0) {
        status = dcc_new_app_preset_append_command_json(options, description);
    }
    if (status != 0) {
        return -1;
    }
    printf(
        "created DCC %s preset %s in feature %s under %s\n",
        strcmp(kind, "modal_form") == 0 ? "form" : kind,
        options->command_name,
        options->cog_name,
        options->path
    );
    return 0;
}
