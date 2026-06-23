#include "internal/dcc_new_app.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_NEW_APP_HANDLER_MARKER "/* dcc_new_app:command-handlers */"
#define DCC_NEW_APP_AUTOCOMPLETE_ROUTE_MARKER "        /* dcc_new_app:autocomplete-routes */"
#define DCC_NEW_APP_TYPED_AUTOCOMPLETE_ROUTE_MARKER "        /* dcc_new_app:typed-autocomplete-routes */"
#define DCC_NEW_APP_AUTOCOMPLETE_EXTENSION_MARKER "    /* dcc_new_app:autocomplete-extension */"

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

static int dcc_new_app_insert_autocomplete_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    char *handler_name,
    size_t handler_name_len,
    const char *args_type
) {
    char insertion[1400];
    snprintf(handler_name, handler_name_len, "on_%s_%s_autocomplete", options->cog_name, options->command_name);
    if (args_type != NULL && args_type[0] != '\0') {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_AUTOCOMPLETE_ARGS_IMPL(%s, %s, command) {\n"
            "    (void)user_data;\n"
            "\n"
            "    (void)command;\n"
            "    (void)DCC_CTX_AUTOCOMPLETE_MATCHING(\n"
            "        ctx,\n"
            "        DCC_AUTOCOMPLETE_STRING_CHOICE(\"Example\", \"example\"),\n"
            "        DCC_AUTOCOMPLETE_STRING_CHOICE(\"%s\", \"%s\")\n"
            "    );\n"
            "}\n"
            "\n",
            handler_name,
            args_type,
            options->command_name,
            options->command_name
        );
    } else {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_HANDLER(%s) {\n"
            "    (void)user_data;\n"
            "\n"
            "    (void)DCC_CTX_AUTOCOMPLETE_MATCHING(\n"
            "        ctx,\n"
            "        DCC_AUTOCOMPLETE_STRING_CHOICE(\"Example\", \"example\"),\n"
            "        DCC_AUTOCOMPLETE_STRING_CHOICE(\"%s\", \"%s\")\n"
            "    );\n"
            "}\n"
            "\n",
            handler_name,
            options->command_name,
            options->command_name
        );
    }
    return dcc_new_app_insert_before_marker(
        path,
        DCC_NEW_APP_HANDLER_MARKER,
        insertion,
        handler_name
    );
}

static int dcc_new_app_find_autocomplete_binding(
    const char *path,
    const dcc_new_app_options_t *options,
    char *args_type,
    size_t args_type_len,
    char *params_macro,
    size_t params_macro_len
) {
    size_t len = 0U;
    char *data = dcc_new_app_read_file(path, &len);
    if (data == NULL) {
        return -1;
    }

    char base[320];
    char macro_anchor[480];
    snprintf(base, sizeof(base), "on_%s_%s", options->cog_name, options->command_name);
    dcc_new_app_make_macro_name(base, "_PARAMS", params_macro, params_macro_len);
    snprintf(macro_anchor, sizeof(macro_anchor), "#define %s(PARAM, type)", params_macro);
    if (strstr(data, macro_anchor) != NULL) {
        snprintf(args_type, args_type_len, "%s_args_t", base);
        free(data);
        return 1;
    }

    dcc_new_app_make_macro_name(options->command_name, "_PARAMS", params_macro, params_macro_len);
    snprintf(macro_anchor, sizeof(macro_anchor), "#define %s(PARAM, type)", params_macro);
    if (strstr(data, macro_anchor) != NULL) {
        snprintf(args_type, args_type_len, "%s_args_t", options->command_name);
        free(data);
        return 1;
    }

    free(data);
    return 0;
}

static int dcc_new_app_insert_autocomplete_route_text(
    const char *path,
    const char *route,
    const char *duplicate_needle,
    int typed
) {
    const char *marker = typed
        ? DCC_NEW_APP_TYPED_AUTOCOMPLETE_ROUTE_MARKER
        : DCC_NEW_APP_AUTOCOMPLETE_ROUTE_MARKER;
    int has_route_marker = dcc_new_app_file_contains(path, marker);
    if (has_route_marker < 0) {
        return -1;
    }
    if (has_route_marker) {
        char insertion[1000];
        snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
        return dcc_new_app_insert_before_marker(
            path,
            marker,
            insertion,
            duplicate_needle
        );
    }

    char insertion[1200];
    snprintf(
        insertion,
        sizeof(insertion),
        "    ,\n"
        "    %s(\n"
        "%s"
        "%s\n"
        "    )\n",
        typed ? "DCC_FEATURE_AUTOCOMPLETE_ROUTES" : "DCC_FEATURE_AUTOCOMPLETES",
        route,
        marker
    );
    return dcc_new_app_insert_before_marker(
        path,
        DCC_NEW_APP_AUTOCOMPLETE_EXTENSION_MARKER,
        insertion,
        duplicate_needle
    );
}

static int dcc_new_app_insert_typed_autocomplete_route(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name,
    const char *args_type,
    const char *params_macro
) {
    char route[1100];
    char duplicate_needle[420];
    char guards[1000];
    int guarded = dcc_new_app_guard_items_literal(options, guards, sizeof(guards));
    if (guarded < 0) { return -1; }
    snprintf(
        duplicate_needle,
        sizeof(duplicate_needle),
        "\"%s\",\n            %s",
        options->command_name,
        args_type
    );
    snprintf(
        route,
        sizeof(route),
        "        %s(\n"
        "            \"%s\",\n"
        "            %s,\n"
        "            %s,\n"
        "            user_data,\n"
        "            %s%s%s\n"
        "        )\n",
        guarded ? "DCC_AUTOCOMPLETE_PARAMS_DATA_GUARDED" : "DCC_AUTOCOMPLETE_PARAMS_DATA",
        options->command_name,
        args_type,
        handler_name,
        params_macro,
        guarded ? ",\n            " : "",
        guarded ? guards : ""
    );
    return dcc_new_app_insert_autocomplete_route_text(path, route, duplicate_needle, 1);
}

static int dcc_new_app_insert_untyped_autocomplete_route(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    char route[900];
    char duplicate_needle[320];
    char guards[1000];
    int guarded = dcc_new_app_guard_items_literal(options, guards, sizeof(guards));
    if (guarded < 0) { return -1; }
    snprintf(
        duplicate_needle,
        sizeof(duplicate_needle),
        "\"%s\", %s, user_data",
        options->command_name,
        handler_name
    );
    snprintf(
        route,
        sizeof(route),
        "        %s(\"%s\", %s, user_data%s%s)\n",
        guarded ? "DCC_AUTOCOMPLETE_DATA_GUARDED" : "DCC_AUTOCOMPLETE_DATA",
        options->command_name,
        handler_name,
        guarded ? ", " : "",
        guarded ? guards : ""
    );
    return dcc_new_app_insert_autocomplete_route_text(path, route, duplicate_needle, 0);
}

static int dcc_new_app_insert_autocomplete_route(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name,
    const char *args_type,
    const char *params_macro
) {
    if (args_type != NULL && args_type[0] != '\0' && params_macro != NULL && params_macro[0] != '\0') {
        return dcc_new_app_insert_typed_autocomplete_route(
            path,
            options,
            handler_name,
            args_type,
            params_macro
        );
    }
    return dcc_new_app_insert_untyped_autocomplete_route(path, options, handler_name);
}

static const char *dcc_new_app_min_ptr(const char *lhs, const char *rhs) {
    if (lhs == NULL) {
        return rhs;
    }
    if (rhs == NULL) {
        return lhs;
    }
    return lhs < rhs ? lhs : rhs;
}

static int dcc_new_app_enable_params_option_autocomplete(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *data,
    size_t len
) {
    char handler_name[320];
    char params_macro[384];
    snprintf(handler_name, sizeof(handler_name), "on_%s_%s", options->cog_name, options->command_name);
    dcc_new_app_make_macro_name(handler_name, "_PARAMS", params_macro, sizeof(params_macro));

    char macro_anchor[480];
    snprintf(macro_anchor, sizeof(macro_anchor), "#define %s(PARAM, type)", params_macro);
    char *macro = strstr(data, macro_anchor);
    if (macro == NULL) {
        dcc_new_app_make_macro_name(options->command_name, "_PARAMS", params_macro, sizeof(params_macro));
        snprintf(macro_anchor, sizeof(macro_anchor), "#define %s(PARAM, type)", params_macro);
        macro = strstr(data, macro_anchor);
    }
    if (macro == NULL) {
        return 0;
    }

    char *limit = strstr(macro + strlen(macro_anchor), "\n\n");
    if (limit == NULL) {
        limit = (char *)data + len;
    }
    char *already = strstr(macro, "PARAM(type, STRING_AUTOCOMPLETE, name,");
    if (already != NULL && already < limit) {
        return 1;
    }
    char *param = strstr(macro, "PARAM(type, STRING, name,");
    if (param == NULL || param >= limit) {
        return 0;
    }

    const char plain[] = "PARAM(type, STRING,";
    const char replacement[] = "PARAM(type, STRING_AUTOCOMPLETE,";
    return dcc_new_app_replace_range(path, data, len, param, strlen(plain), replacement) == 0 ? 1 : -1;
}

static int dcc_new_app_enable_generated_option_autocomplete(
    const char *path,
    const dcc_new_app_options_t *options
) {
    size_t len = 0U;
    char *data = dcc_new_app_read_file(path, &len);
    if (data == NULL) {
        return -1;
    }

    char anchor[320];
    snprintf(
        anchor,
        sizeof(anchor),
        "DCC_SLASH_COMMAND_WITH_OPTIONS(\n"
        "                    \"%s\",",
        options->command_name
    );
    char *command = strstr(data, anchor);
    if (command == NULL) {
        int params_status = dcc_new_app_enable_params_option_autocomplete(path, options, data, len);
        if (params_status != 0) {
            free(data);
            return params_status > 0 ? 0 : -1;
        }
        fprintf(
            stderr,
            "warning: command %s was not found in %s; autocomplete route was added without changing command options\n",
            options->command_name,
            path
        );
        free(data);
        return 0;
    }

    const char *next_command = strstr(command + strlen(anchor), "DCC_COMMAND_BUILDER_DATA(");
    const char *next_typed_command = strstr(command + strlen(anchor), "DCC_TYPED_SLASH(");
    const char *next_on_typed_command = strstr(command + strlen(anchor), "DCC_ON_TYPED_SLASH");
    const char *slash_marker = strstr(command + strlen(anchor), "        /* dcc_new_app:slash-routes */");
    const char *limit = dcc_new_app_min_ptr(next_command, next_typed_command);
    limit = dcc_new_app_min_ptr(limit, next_on_typed_command);
    limit = dcc_new_app_min_ptr(limit, slash_marker);
    if (limit == NULL) {
        limit = data + len;
    }

    const char plain[] = "DCC_OPT_STRING(\"name\", \"Name to echo\", 0U)";
    const char autocomplete[] = "DCC_OPT_STRING_AUTOCOMPLETE(\"name\", \"Name to echo\", 0U)";
    char *already = strstr(command, autocomplete);
    if (already != NULL && already < limit) {
        free(data);
        return 0;
    }
    char *option = strstr(command, plain);
    if (option == NULL || option >= limit) {
        fprintf(
            stderr,
            "warning: generated string option for %s was not found in %s; enable DCC_OPT_STRING_AUTOCOMPLETE manually\n",
            options->command_name,
            path
        );
        free(data);
        return 0;
    }

    int status = dcc_new_app_replace_range(path, data, len, option, strlen(plain), autocomplete);
    free(data);
    return status;
}

static int dcc_new_app_mark_command_json_autocomplete(const dcc_new_app_options_t *options) {
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

    char anchor[240];
    snprintf(
        anchor,
        sizeof(anchor),
        "    \"name\": \"%s\",\n"
        "    \"description\":",
        options->command_name
    );
    char *command = strstr(data, anchor);
    if (command == NULL) {
        free(data);
        free(path);
        return 0;
    }

    const char *next_command = strstr(command + strlen(anchor), "\n  {\n");
    const char *limit = next_command != NULL ? next_command : data + len;
    char *already = strstr(command, "\"autocomplete\": true");
    if (already != NULL && already < limit) {
        free(data);
        free(path);
        return 0;
    }

    const char required[] = "        \"required\": false";
    const char replacement[] = "        \"required\": false,\n        \"autocomplete\": true";
    char *field = strstr(command, required);
    if (field == NULL || field >= limit) {
        free(data);
        free(path);
        return 0;
    }

    int status = dcc_new_app_replace_range(path, data, len, field, strlen(required), replacement);
    free(data);
    free(path);
    return status;
}

int dcc_new_app_generate_autocomplete(const dcc_new_app_options_t *options) {
    char *path = NULL;
    if (dcc_new_app_require_cog_source(&path, options) != 0) {
        return -1;
    }
    char handler_name[320];
    char args_type[360] = {0};
    char params_macro[384] = {0};
    int binding_status = dcc_new_app_find_autocomplete_binding(
        path,
        options,
        args_type,
        sizeof(args_type),
        params_macro,
        sizeof(params_macro)
    );
    if (binding_status < 0) {
        free(path);
        return -1;
    }
    int status = dcc_new_app_insert_autocomplete_handler(
        path,
        options,
        handler_name,
        sizeof(handler_name),
        binding_status > 0 ? args_type : NULL
    );
    if (status == 0) {
        status = dcc_new_app_insert_autocomplete_route(
            path,
            options,
            handler_name,
            binding_status > 0 ? args_type : NULL,
            binding_status > 0 ? params_macro : NULL
        );
    }
    if (status == 0) {
        status = dcc_new_app_enable_generated_option_autocomplete(path, options);
    }
    free(path);
    if (status == 0) {
        status = dcc_new_app_mark_command_json_autocomplete(options);
    }
    if (status != 0) {
        return -1;
    }
    printf(
        "created DCC autocomplete %s in feature %s under %s\n",
        options->command_name,
        options->cog_name,
        options->path
    );
    return 0;
}
