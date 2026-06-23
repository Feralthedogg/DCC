#include "internal/dcc_new_app.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_NEW_APP_HANDLER_MARKER "/* dcc_new_app:command-handlers */"
#define DCC_NEW_APP_MIDDLEWARE_ROUTE_MARKER "        /* dcc_new_app:middleware-routes */"
#define DCC_NEW_APP_MIDDLEWARE_EXTENSION_MARKER "    /* dcc_new_app:middleware-extension */"

static char *dcc_new_app_middleware_cog_source_path(const dcc_new_app_options_t *options) {
    char relative_path[320];
    snprintf(relative_path, sizeof(relative_path), "src/%s.c", options->cog_name);
    return dcc_new_app_join(options->path, relative_path);
}

static const char *dcc_new_app_middleware_kind(const dcc_new_app_options_t *options) {
    if (options == NULL || options->subcommand_arg == NULL || options->subcommand_arg[0] == '\0') {
        return "pass";
    }
    if (strcmp(options->subcommand_arg, "noop") == 0) {
        return "pass";
    }
    if (strcmp(options->subcommand_arg, "state") == 0) {
        return "config";
    }
    if (strcmp(options->subcommand_arg, "guild-only") == 0) {
        return "guild";
    }
    if (strcmp(options->subcommand_arg, "dm-only") == 0) {
        return "dm";
    }
    return options->subcommand_arg;
}

static int dcc_new_app_middleware_require_cog(char **out_path, const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_middleware_cog_source_path(options);
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

static int dcc_new_app_insert_middleware_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    char *handler_name,
    size_t handler_name_len
) {
    const char *kind = dcc_new_app_middleware_kind(options);
    snprintf(
        handler_name,
        handler_name_len,
        "on_%s_%s_middleware",
        options->cog_name,
        options->command_name
    );

    char insertion[1400];
    if (strcmp(kind, "config") == 0) {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_MIDDLEWARE_FN(%s) {\n"
            "    (void)ctx;\n"
            "\n"
            "    bot_config_t *config = BOT_CONFIG(user_data);\n"
            "    return config != NULL ? DCC_OK : DCC_ERR_STATE;\n"
            "}\n"
            "\n",
            handler_name
        );
    } else if (strcmp(kind, "guild") == 0) {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_MIDDLEWARE_FN(%s) {\n"
            "    (void)user_data;\n"
            "\n"
            "    return DCC_CTX_IS_GUILD(ctx) ? DCC_OK : DCC_ERR_STATE;\n"
            "}\n"
            "\n",
            handler_name
        );
    } else if (strcmp(kind, "dm") == 0) {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_MIDDLEWARE_FN(%s) {\n"
            "    (void)user_data;\n"
            "\n"
            "    return DCC_CTX_IS_DM(ctx) ? DCC_OK : DCC_ERR_STATE;\n"
            "}\n"
            "\n",
            handler_name
        );
    } else {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_MIDDLEWARE_FN(%s) {\n"
            "    (void)ctx;\n"
            "    (void)user_data;\n"
            "\n"
            "    return DCC_OK;\n"
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

static int dcc_new_app_insert_middleware_route(
    const char *path,
    const char *handler_name
) {
    size_t len = 0U;
    char *data = dcc_new_app_read_file(path, &len);
    if (data == NULL) {
        fprintf(stderr, "could not read %s for marker update\n", path);
        return -1;
    }

    char duplicate_needle[420];
    snprintf(
        duplicate_needle,
        sizeof(duplicate_needle),
        "DCC_MIDDLEWARE_DATA(%s, user_data)",
        handler_name
    );
    if (strstr(data, duplicate_needle) != NULL) {
        free(data);
        return 0;
    }

    char route[620];
    snprintf(
        route,
        sizeof(route),
        "        DCC_MIDDLEWARE_DATA(%s, user_data)\n",
        handler_name
    );

    char *route_marker = strstr(data, DCC_NEW_APP_MIDDLEWARE_ROUTE_MARKER);
    if (route_marker != NULL) {
        char insertion[760];
        snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
        int status = dcc_new_app_replace_range(path, data, len, route_marker, 0U, insertion);
        free(data);
        return status;
    }

    char *extension = strstr(data, DCC_NEW_APP_MIDDLEWARE_EXTENSION_MARKER);
    if (extension == NULL) {
        fprintf(stderr, "warning: middleware marker target not found in %s\n", path);
        free(data);
        return 0;
    }

    char block[900];
    snprintf(
        block,
        sizeof(block),
        "    DCC_FEATURE_MIDDLEWARE(\n"
        "%s"
        "%s\n"
        "    ),\n",
        route,
        DCC_NEW_APP_MIDDLEWARE_ROUTE_MARKER
    );
    int status = dcc_new_app_replace_range(path, data, len, extension, 0U, block);
    free(data);
    return status;
}

int dcc_new_app_generate_middleware(const dcc_new_app_options_t *options) {
    if (!dcc_new_app_path_exists(options->path)) {
        fprintf(stderr, "%s does not exist; create the app first\n", options->path);
        return -1;
    }

    char *path = NULL;
    if (dcc_new_app_middleware_require_cog(&path, options) != 0) {
        return -1;
    }

    char handler_name[320];
    int status = dcc_new_app_insert_middleware_handler(
        path,
        options,
        handler_name,
        sizeof(handler_name)
    );
    if (status == 0) {
        status = dcc_new_app_insert_middleware_route(path, handler_name);
    }
    free(path);
    if (status != 0) {
        return -1;
    }

    printf(
        "created DCC middleware %s (%s) in feature %s under %s\n",
        options->command_name,
        dcc_new_app_middleware_kind(options),
        options->cog_name,
        options->path
    );
    return 0;
}
