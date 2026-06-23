#include "internal/dcc_new_app.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_NEW_APP_HANDLER_MARKER "/* dcc_new_app:command-handlers */"
#define DCC_NEW_APP_TYPED_SUBCOMMAND_ROUTE_MARKER "        /* dcc_new_app:typed-subcommand-routes */"
#define DCC_NEW_APP_SUBCOMMAND_EXTENSION_MARKER "    /* dcc_new_app:subcommand-extension */"

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

static int dcc_new_app_insert_subcommand_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    char *handler_name,
    size_t handler_name_len
) {
    char insertion[1400];
    char symbol_base[320];
    char args_type[360];
    char params_macro[384];
    dcc_new_app_make_subcommand_symbol_base(options, symbol_base, sizeof(symbol_base));
    snprintf(args_type, sizeof(args_type), "%s_args_t", symbol_base);
    dcc_new_app_make_macro_name(symbol_base, "_PARAMS", params_macro, sizeof(params_macro));
    snprintf(
        handler_name,
        handler_name_len,
        "%s_subcommand",
        symbol_base
    );
    snprintf(
        insertion,
        sizeof(insertion),
        "typedef struct %s_args {\n"
        "    const char *name;\n"
        "} %s;\n"
        "\n"
        "#define %s(PARAM, type) \\\n"
        "    PARAM(type, STRING, name, \"name\", \"Name to echo\", 0U, \"there\")\n"
        "\n"
        "DCC_SUBCOMMAND_ARGS_IMPL(%s, %s, command) {\n"
        "    (void)user_data;\n"
        "\n"
        "    const char *name = command != NULL && command->name != NULL ? command->name : \"there\";\n"
        "    (void)DCC_CTX_REPLY_TEXT_F(ctx, \"%s/%s: hello %%s\", name);\n"
        "}\n"
        "\n",
        symbol_base,
        args_type,
        params_macro,
        handler_name,
        args_type,
        options->command_name,
        options->subcommand_path
    );
    return dcc_new_app_insert_before_marker(
        path,
        DCC_NEW_APP_HANDLER_MARKER,
        insertion,
        handler_name
    );
}

static int dcc_new_app_insert_subcommand_route_text(
    const char *path,
    const char *route,
    const char *duplicate_needle
) {
    int has_route_marker = dcc_new_app_file_contains(path, DCC_NEW_APP_TYPED_SUBCOMMAND_ROUTE_MARKER);
    if (has_route_marker < 0) {
        return -1;
    }
    if (has_route_marker) {
        char insertion[1000];
        snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
        return dcc_new_app_insert_before_marker(
            path,
            DCC_NEW_APP_TYPED_SUBCOMMAND_ROUTE_MARKER,
            insertion,
            duplicate_needle
        );
    }

    char insertion[1200];
    snprintf(
        insertion,
        sizeof(insertion),
        "    ,\n"
        "    DCC_FEATURE_SUBCOMMAND_ROUTES(\n"
        "%s"
        "        /* dcc_new_app:typed-subcommand-routes */\n"
        "    )\n",
        route
    );
    return dcc_new_app_insert_before_marker(
        path,
        DCC_NEW_APP_SUBCOMMAND_EXTENSION_MARKER,
        insertion,
        duplicate_needle
    );
}

static int dcc_new_app_insert_subcommand_route(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    char route[960];
    char duplicate_needle[420];
    char symbol_base[320];
    char args_type[360];
    char params_macro[384];
    char guards[1000];
    int guarded = dcc_new_app_guard_items_literal(options, guards, sizeof(guards));
    if (guarded < 0) {
        return -1;
    }
    dcc_new_app_make_subcommand_symbol_base(options, symbol_base, sizeof(symbol_base));
    snprintf(args_type, sizeof(args_type), "%s_args_t", symbol_base);
    dcc_new_app_make_macro_name(symbol_base, "_PARAMS", params_macro, sizeof(params_macro));
    snprintf(
        duplicate_needle,
        sizeof(duplicate_needle),
        "\"%s\",\n            \"%s\"",
        options->command_name,
        options->subcommand_path
    );
    snprintf(
        route,
        sizeof(route),
	        "        %s(\n"
	        "            \"%s\",\n"
	        "            \"%s\",\n"
	        "            %s,\n"
	        "            %s,\n"
	        "            user_data,\n"
	        "            %s%s%s\n"
	        "        )\n",
	        guarded ? "DCC_SUBCOMMAND_ROUTE_DATA_GUARDED" : "DCC_SUBCOMMAND_ROUTE_DATA",
	        options->command_name,
	        options->subcommand_path,
	        args_type,
	        handler_name,
	        params_macro,
	        guarded ? ",\n            " : "",
	        guarded ? guards : ""
	    );
    return dcc_new_app_insert_subcommand_route_text(path, route, duplicate_needle);
}

int dcc_new_app_generate_subcommand(const dcc_new_app_options_t *options) {
    char *path = NULL;
    if (dcc_new_app_require_cog_source(&path, options) != 0) {
        return -1;
    }
    char handler_name[360];
    int status = dcc_new_app_insert_subcommand_handler(path, options, handler_name, sizeof(handler_name));
    if (status == 0) {
        status = dcc_new_app_insert_subcommand_route(path, options, handler_name);
    }
    if (status == 0) {
        status = dcc_new_app_update_command_builder_subcommands(path, options);
    }
    free(path);
    if (status == 0) {
        status = dcc_new_app_update_command_json_subcommands(options);
    }
    if (status != 0) {
        return -1;
    }
    printf(
        "created DCC subcommand %s for command %s in feature %s under %s\n",
        options->subcommand_path,
        options->command_name,
        options->cog_name,
        options->path
    );
    return 0;
}
