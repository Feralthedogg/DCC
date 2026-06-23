#include "internal/dcc_new_app.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DCC_NEW_APP_HANDLER_MARKER "/* dcc_new_app:command-handlers */"
#define DCC_NEW_APP_COMPONENT_ROUTE_MARKER "        /* dcc_new_app:component-routes */"
#define DCC_NEW_APP_COMPONENT_EXTENSION_MARKER "    /* dcc_new_app:component-extension */"
#define DCC_NEW_APP_TYPED_BUTTON_ROUTE_MARKER "        /* dcc_new_app:typed-button-routes */"
#define DCC_NEW_APP_TYPED_BUTTON_EXTENSION_MARKER "    /* dcc_new_app:typed-button-extension */"
#define DCC_NEW_APP_TYPED_SELECT_ROUTE_MARKER "        /* dcc_new_app:typed-select-routes */"
#define DCC_NEW_APP_TYPED_SELECT_EXTENSION_MARKER "    /* dcc_new_app:typed-select-extension */"
#define DCC_NEW_APP_TYPED_MODAL_ROUTE_MARKER "        /* dcc_new_app:typed-modal-routes */"
#define DCC_NEW_APP_TYPED_MODAL_EXTENSION_MARKER "    /* dcc_new_app:typed-modal-extension */"
static char *dcc_new_app_cog_source_path(const dcc_new_app_options_t *options) {
    char relative_path[320];
    snprintf(relative_path, sizeof(relative_path), "src/%s.c", options->cog_name);
    return dcc_new_app_join(options->path, relative_path);
}
static int dcc_new_app_require_cog_source(char **out_path, const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_cog_source_path(options);
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
static int dcc_new_app_file_contains(const char *path, const char *needle) {
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
static int dcc_new_app_insert_component_route(
    const char *path,
    const char *route,
    const char *duplicate_needle
) {
    int has_route_marker = dcc_new_app_file_contains(path, DCC_NEW_APP_COMPONENT_ROUTE_MARKER);
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
static int dcc_new_app_insert_typed_route(
    const char *path,
    const char *route,
    const char *duplicate_needle,
    const char *route_marker,
    const char *extension_marker,
    const char *cog_macro
) {
    int has_route_marker = dcc_new_app_file_contains(path, route_marker);
    if (has_route_marker < 0) {
        return -1;
    }
    if (has_route_marker) {
        char insertion[1200];
        snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
        return dcc_new_app_insert_before_marker(
            path,
            route_marker,
            insertion,
            duplicate_needle
        );
    }
    const char *target_marker = extension_marker;
    int has_extension_marker = dcc_new_app_file_contains(path, target_marker);
    if (has_extension_marker < 0) {
        return -1;
    }
    if (!has_extension_marker) {
        target_marker = DCC_NEW_APP_COMPONENT_EXTENSION_MARKER;
    }

    char insertion[1400];
    snprintf(
        insertion,
        sizeof(insertion),
        "    ,\n"
        "    %s(\n"
        "%s"
        "%s\n"
        "    )\n"
        "    %s\n",
        cog_macro,
        route,
        route_marker,
        extension_marker
    );
    return dcc_new_app_insert_before_marker(path, target_marker, insertion, duplicate_needle);
}
static int dcc_new_app_insert_typed_button_route(
    const char *path,
    const char *route,
    const char *duplicate_needle
) {
    return dcc_new_app_insert_typed_route(
        path,
        route,
        duplicate_needle,
        DCC_NEW_APP_TYPED_BUTTON_ROUTE_MARKER,
        DCC_NEW_APP_TYPED_BUTTON_EXTENSION_MARKER,
        "DCC_FEATURE_BUTTON_ROUTES"
    );
}
static int dcc_new_app_insert_typed_select_route(
    const char *path,
    const char *route,
    const char *duplicate_needle
) {
    return dcc_new_app_insert_typed_route(
        path,
        route,
        duplicate_needle,
        DCC_NEW_APP_TYPED_SELECT_ROUTE_MARKER,
        DCC_NEW_APP_TYPED_SELECT_EXTENSION_MARKER,
        "DCC_FEATURE_SELECT_ROUTES"
    );
}
static int dcc_new_app_insert_typed_modal_route(
    const char *path,
    const char *route,
    const char *duplicate_needle
) {
    return dcc_new_app_insert_typed_route(
        path,
        route,
        duplicate_needle,
        DCC_NEW_APP_TYPED_MODAL_ROUTE_MARKER,
        DCC_NEW_APP_TYPED_MODAL_EXTENSION_MARKER,
        "DCC_FEATURE_MODAL_ROUTES"
    );
}
static int dcc_new_app_insert_button_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    char *handler_name,
    size_t handler_name_len
) {
    char insertion[1800];
    snprintf(handler_name, handler_name_len, "on_%s_%s_button", options->cog_name, options->command_name);
    char args_struct[360];
    char args_type[360];
    char params_macro[420];
    snprintf(args_struct, sizeof(args_struct), "%s_args", handler_name);
    snprintf(args_type, sizeof(args_type), "%s_t", args_struct);
    dcc_new_app_make_macro_name(handler_name, "_PARAMS", params_macro, sizeof(params_macro));
    snprintf(
        insertion,
        sizeof(insertion),
        "typedef struct %s {\n"
        "    const char *action;\n"
        "} %s;\n"
        "\n"
        "#define %s(PARAM, type) \\\n"
        "    PARAM(type, REQUIRED_CUSTOM_ID_SUFFIX, action, \"%s.\")\n"
        "\n"
        "DCC_BUTTON_ARGS_IMPL(%s, %s, button) {\n"
        "    (void)user_data;\n"
        "\n"
        "    (void)DCC_CTX_DONEF(\n"
        "        ctx,\n"
        "        \"%s.%s clicked: %%s\",\n"
        "        button != NULL && button->action != NULL ? button->action : \"unknown\"\n"
        "    );\n"
        "}\n"
        "\n",
        args_struct,
        args_type,
        params_macro,
        options->cog_name,
        handler_name,
        args_type,
        options->cog_name,
        options->command_name
    );
    return dcc_new_app_insert_before_marker(
        path,
        DCC_NEW_APP_HANDLER_MARKER,
        insertion,
        handler_name
    );
}
static int dcc_new_app_insert_modal_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    char *handler_name,
    size_t handler_name_len
) {
    char insertion[1800];
    snprintf(handler_name, handler_name_len, "on_%s_%s_modal", options->cog_name, options->command_name);
    char args_struct[360];
    char args_type[360];
    char params_macro[420];
    snprintf(args_struct, sizeof(args_struct), "%s_args", handler_name);
    snprintf(args_type, sizeof(args_type), "%s_t", args_struct);
    dcc_new_app_make_macro_name(handler_name, "_PARAMS", params_macro, sizeof(params_macro));
    snprintf(
        insertion,
        sizeof(insertion),
        "typedef struct %s {\n"
        "    const char *value;\n"
        "} %s;\n"
        "\n"
        "#define %s(PARAM, type) \\\n"
        "    PARAM(type, REQUIRED_STRING, value, \"value\")\n"
        "\n"
        "DCC_MODAL_ARGS_IMPL(%s, %s, modal) {\n"
        "    (void)user_data;\n"
        "\n"
        "    (void)DCC_CTX_DONEF(\n"
        "        ctx,\n"
        "        \"%s.%s submitted: %%s\",\n"
        "        modal != NULL && modal->value != NULL ? modal->value : \"\"\n"
        "    );\n"
        "}\n"
        "\n",
        args_struct,
        args_type,
        params_macro,
        handler_name,
        args_type,
        options->cog_name,
        options->command_name
    );
    return dcc_new_app_insert_before_marker(
        path,
        DCC_NEW_APP_HANDLER_MARKER,
        insertion,
        handler_name
    );
}
static int dcc_new_app_insert_select_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    char *handler_name,
    size_t handler_name_len
) {
    char insertion[1600];
    snprintf(handler_name, handler_name_len, "on_%s_%s_select", options->cog_name, options->command_name);
    char args_struct[360];
    char args_type[360];
    char params_macro[420];
    snprintf(args_struct, sizeof(args_struct), "%s_args", handler_name);
    snprintf(args_type, sizeof(args_type), "%s_t", args_struct);
    dcc_new_app_make_macro_name(handler_name, "_PARAMS", params_macro, sizeof(params_macro));
    snprintf(
        insertion,
        sizeof(insertion),
        "typedef struct %s {\n"
        "    const char *value;\n"
        "} %s;\n"
        "\n"
        "#define %s(PARAM, type) \\\n"
        "    PARAM(type, REQUIRED_FIRST_VALUE, value)\n"
        "\n"
        "DCC_SELECT_ARGS_IMPL(%s, %s, select) {\n"
        "    (void)user_data;\n"
        "\n"
        "    (void)DCC_CTX_DONEF(\n"
        "        ctx,\n"
        "        \"%s.%s selected: %%s\",\n"
        "        select != NULL && select->value != NULL ? select->value : \"unknown\"\n"
        "    );\n"
        "}\n"
        "\n",
        args_struct,
        args_type,
        params_macro,
        handler_name,
        args_type,
        options->cog_name,
        options->command_name
    );
    return dcc_new_app_insert_before_marker(
        path,
        DCC_NEW_APP_HANDLER_MARKER,
        insertion,
        handler_name
    );
}
static int dcc_new_app_insert_button_route(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    char route[1800];
    char duplicate_needle[320];
    char args_type[360];
    char params_macro[420];
    char policy[1000];
    int guarded = dcc_new_app_guard_policy_literal(options, policy, sizeof(policy));
    if (guarded < 0) { return -1; }
    snprintf(args_type, sizeof(args_type), "%s_args_t", handler_name);
    dcc_new_app_make_macro_name(handler_name, "_PARAMS", params_macro, sizeof(params_macro));
    snprintf(
        duplicate_needle,
        sizeof(duplicate_needle),
        "\"%s.%s\", %s",
        options->cog_name,
        options->command_name,
        args_type
    );
    if (guarded) {
        snprintf(
            route,
            sizeof(route),
            "        DCC_BUTTON_PARAMS_DATA_POLICY(\n"
            "            \"%s.%s\",\n"
            "            %s,\n"
            "            %s,\n"
            "            user_data,\n"
            "            %s,\n"
            "            %s\n"
            "        )\n",
            options->cog_name,
            options->command_name,
            args_type,
            handler_name,
            params_macro,
            policy
        );
    } else {
        snprintf(
            route,
            sizeof(route),
            "        DCC_BUTTON_PARAMS_DATA(\n"
            "            \"%s.%s\",\n"
            "            %s,\n"
            "            %s,\n"
            "            user_data,\n"
            "            %s\n"
            "        )\n",
            options->cog_name,
            options->command_name,
            args_type,
            handler_name,
            params_macro
        );
    }
    return dcc_new_app_insert_typed_button_route(path, route, duplicate_needle);
}
static int dcc_new_app_insert_select_route(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    char route[1800];
    char duplicate_needle[320];
    char args_type[360];
    char params_macro[420];
    char policy[1000];
    int guarded = dcc_new_app_guard_policy_literal(options, policy, sizeof(policy));
    if (guarded < 0) { return -1; }
    snprintf(args_type, sizeof(args_type), "%s_args_t", handler_name);
    dcc_new_app_make_macro_name(handler_name, "_PARAMS", params_macro, sizeof(params_macro));
    snprintf(
        duplicate_needle,
        sizeof(duplicate_needle),
        "\"%s.%s\", %s",
        options->cog_name,
        options->command_name,
        args_type
    );
    if (guarded) {
        snprintf(
            route,
            sizeof(route),
            "        DCC_SELECT_PARAMS_DATA_POLICY(\n"
            "            \"%s.%s\",\n"
            "            %s,\n"
            "            %s,\n"
            "            user_data,\n"
            "            %s,\n"
            "            %s\n"
            "        )\n",
            options->cog_name,
            options->command_name,
            args_type,
            handler_name,
            params_macro,
            policy
        );
    } else {
        snprintf(
            route,
            sizeof(route),
            "        DCC_SELECT_PARAMS_DATA(\n"
            "            \"%s.%s\",\n"
            "            %s,\n"
            "            %s,\n"
            "            user_data,\n"
            "            %s\n"
            "        )\n",
            options->cog_name,
            options->command_name,
            args_type,
            handler_name,
            params_macro
        );
    }
    return dcc_new_app_insert_typed_select_route(path, route, duplicate_needle);
}
static int dcc_new_app_insert_modal_route(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    char route[1800];
    char duplicate_needle[320];
    char args_type[360];
    char params_macro[420];
    char policy[1000];
    int guarded = dcc_new_app_guard_policy_literal(options, policy, sizeof(policy));
    if (guarded < 0) { return -1; }
    snprintf(args_type, sizeof(args_type), "%s_args_t", handler_name);
    dcc_new_app_make_macro_name(handler_name, "_PARAMS", params_macro, sizeof(params_macro));
    snprintf(
        duplicate_needle,
        sizeof(duplicate_needle),
        "\"%s.%s\", %s",
        options->cog_name,
        options->command_name,
        args_type
    );
    if (guarded) {
        snprintf(
            route,
            sizeof(route),
            "        DCC_MODAL_PARAMS_DATA_POLICY(\n"
            "            \"%s.%s\",\n"
            "            %s,\n"
            "            %s,\n"
            "            user_data,\n"
            "            %s,\n"
            "            %s\n"
            "        )\n",
            options->cog_name,
            options->command_name,
            args_type,
            handler_name,
            params_macro,
            policy
        );
    } else {
        snprintf(
            route,
            sizeof(route),
            "        DCC_MODAL_PARAMS_DATA(\n"
            "            \"%s.%s\",\n"
            "            %s,\n"
            "            %s,\n"
            "            user_data,\n"
            "            %s\n"
            "        )\n",
            options->cog_name,
            options->command_name,
            args_type,
            handler_name,
            params_macro
        );
    }
    return dcc_new_app_insert_typed_modal_route(path, route, duplicate_needle);
}
int dcc_new_app_generate_button(const dcc_new_app_options_t *options) {
    char *path = NULL;
    if (dcc_new_app_require_cog_source(&path, options) != 0) {
        return -1;
    }
    char handler_name[320];
    int status = dcc_new_app_insert_button_handler(path, options, handler_name, sizeof(handler_name));
    if (status == 0) {
        status = dcc_new_app_insert_button_route(path, options, handler_name);
    }
    free(path);
    if (status != 0) {
        return -1;
    }
    printf("created DCC button %s in feature %s under %s\n", options->command_name, options->cog_name, options->path);
    return 0;
}

int dcc_new_app_generate_select(const dcc_new_app_options_t *options) {
    char *path = NULL;
    if (dcc_new_app_require_cog_source(&path, options) != 0) {
        return -1;
    }
    char handler_name[320];
    int status = dcc_new_app_insert_select_handler(path, options, handler_name, sizeof(handler_name));
    if (status == 0) {
        status = dcc_new_app_insert_select_route(path, options, handler_name);
    }
    free(path);
    if (status != 0) {
        return -1;
    }
    printf("created DCC select %s in feature %s under %s\n", options->command_name, options->cog_name, options->path);
    return 0;
}

int dcc_new_app_generate_modal(const dcc_new_app_options_t *options) {
    char *path = NULL;
    if (dcc_new_app_require_cog_source(&path, options) != 0) {
        return -1;
    }
    char handler_name[320];
    int status = dcc_new_app_insert_modal_handler(path, options, handler_name, sizeof(handler_name));
    if (status == 0) {
        status = dcc_new_app_insert_modal_route(path, options, handler_name);
    }
    free(path);
    if (status != 0) {
        return -1;
    }
    printf("created DCC modal %s in feature %s under %s\n", options->command_name, options->cog_name, options->path);
    return 0;
}
