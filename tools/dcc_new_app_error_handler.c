#include "internal/dcc_new_app.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_NEW_APP_ERROR_HANDLER_MARKER "/* dcc_new_app:error-handlers */"
#define DCC_NEW_APP_ERROR_HANDLER_OPTION_MARKER "                /* dcc_new_app:error-handler-option */"

static const char *dcc_new_app_error_handler_kind(const dcc_new_app_options_t *options) {
    if (options == NULL || options->subcommand_arg == NULL || options->subcommand_arg[0] == '\0') {
        return "friendly";
    }
    return options->subcommand_arg;
}

static int dcc_new_app_read_main(char **out_path, char **out_data, size_t *out_len, const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_join(options->path, "src/main.c");
    if (path == NULL) {
        return -1;
    }
    if (!dcc_new_app_path_exists(path)) {
        fprintf(stderr, "%s does not exist; create the app first\n", path);
        free(path);
        return -1;
    }
    size_t len = 0U;
    char *data = dcc_new_app_read_file(path, &len);
    if (data == NULL) {
        fprintf(stderr, "could not read %s for marker update\n", path);
        free(path);
        return -1;
    }
    *out_path = path;
    *out_data = data;
    *out_len = len;
    return 0;
}

static int dcc_new_app_insert_error_handler_function(
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    char *path = NULL;
    char *data = NULL;
    size_t len = 0U;
    if (dcc_new_app_read_main(&path, &data, &len, options) != 0) {
        return -1;
    }

    if (strstr(data, handler_name) != NULL) {
        free(data);
        free(path);
        return 0;
    }

    const char *kind = dcc_new_app_error_handler_kind(options);
    char insertion[1800];
    if (strcmp(kind, "verbose") == 0) {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_VERBOSE_ERROR_FN(%s)\n"
            "\n",
            handler_name
        );
    } else if (strcmp(kind, "simple") == 0) {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_SIMPLE_ERROR_FN(%s)\n"
            "\n",
            handler_name
        );
    } else {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_FRIENDLY_ERROR_FN(%s)\n"
            "\n",
            handler_name
        );
    }

    char *where = strstr(data, DCC_NEW_APP_ERROR_HANDLER_MARKER);
    if (where == NULL) {
        where = strstr(data, "int main(void)");
    }
    if (where == NULL) {
        fprintf(stderr, "warning: error handler marker target not found in %s\n", path);
        free(data);
        free(path);
        return 0;
    }

    int status = dcc_new_app_replace_range(path, data, len, where, 0U, insertion);
    free(data);
    free(path);
    return status;
}

static int dcc_new_app_insert_error_handler_option(
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    char *path = NULL;
    char *data = NULL;
    size_t len = 0U;
    if (dcc_new_app_read_main(&path, &data, &len, options) != 0) {
        return -1;
    }

    char duplicate_needle[360];
    snprintf(
        duplicate_needle,
        sizeof(duplicate_needle),
        "DCC_APP_ERROR_HANDLER(%s, &config)",
        handler_name
    );
    if (strstr(data, duplicate_needle) != NULL || strstr(data, "DCC_APP_ERROR_HANDLER(") != NULL) {
        free(data);
        free(path);
        return 0;
    }

    char *where = strstr(data, DCC_NEW_APP_ERROR_HANDLER_OPTION_MARKER);
    if (where == NULL) {
        where = strstr(data, "DCC_APP_DEFAULT_ERRORS()");
        if (where != NULL) {
            where += strlen("DCC_APP_DEFAULT_ERRORS()");
        }
    }
    if (where == NULL) {
        fprintf(stderr, "warning: error handler option marker target not found in %s\n", path);
        free(data);
        free(path);
        return 0;
    }

    int needs_leading_comma = 1;
    for (const char *scan = where; scan > data; --scan) {
        unsigned char ch = (unsigned char)scan[-1];
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
            continue;
        }
        needs_leading_comma = ch != ',';
        break;
    }

    char insertion[460];
    snprintf(
        insertion,
        sizeof(insertion),
        "%s"
        "                DCC_APP_ERROR_HANDLER(%s, &config)\n",
        needs_leading_comma ? ",\n" : "",
        handler_name
    );

    int status = dcc_new_app_replace_range(path, data, len, where, 0U, insertion);
    free(data);
    free(path);
    return status;
}

int dcc_new_app_generate_error_handler(const dcc_new_app_options_t *options) {
    if (!dcc_new_app_path_exists(options->path)) {
        fprintf(stderr, "%s does not exist; create the app first\n", options->path);
        return -1;
    }

    char handler_name[320];
    snprintf(handler_name, sizeof(handler_name), "on_%s", options->command_name);
    if (dcc_new_app_insert_error_handler_function(options, handler_name) != 0 ||
        dcc_new_app_insert_error_handler_option(options, handler_name) != 0) {
        return -1;
    }

    printf(
        "created DCC error handler %s (%s) under %s\n",
        options->command_name,
        dcc_new_app_error_handler_kind(options),
        options->path
    );
    return 0;
}
