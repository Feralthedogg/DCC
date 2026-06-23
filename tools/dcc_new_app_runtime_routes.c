#include "internal/dcc_new_app.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_NEW_APP_HANDLER_MARKER "/* dcc_new_app:command-handlers */"
#define DCC_NEW_APP_MESSAGE_ROUTE_MARKER "        /* dcc_new_app:message-command-routes */"
#define DCC_NEW_APP_MESSAGE_EXTENSION_MARKER "    /* dcc_new_app:message-command-extension */"
#define DCC_NEW_APP_TASK_ROUTE_MARKER "        /* dcc_new_app:task-routes */"
#define DCC_NEW_APP_TASK_EXTENSION_MARKER "    /* dcc_new_app:task-extension */"

static char *dcc_new_app_runtime_cog_source_path(const dcc_new_app_options_t *options) {
    char relative_path[320];
    snprintf(relative_path, sizeof(relative_path), "src/%s.c", options->cog_name);
    return dcc_new_app_join(options->path, relative_path);
}

static int dcc_new_app_runtime_require_cog_source(char **out_path, const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_runtime_cog_source_path(options);
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

static int dcc_new_app_runtime_file_contains(const char *path, const char *needle) {
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

static int dcc_new_app_runtime_insert_route_block(
    const char *path,
    const char *route_marker,
    const char *extension_marker,
    const char *extension_macro,
    const char *route,
    const char *duplicate_needle
) {
    int has_route_marker = dcc_new_app_runtime_file_contains(path, route_marker);
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
        extension_macro,
        route,
        route_marker
    );
    return dcc_new_app_insert_before_marker(path, extension_marker, insertion, duplicate_needle);
}

static int dcc_new_app_runtime_c_string_safe(const char *value) {
    if (value == NULL || value[0] == '\0') {
        return 0;
    }
    for (const unsigned char *p = (const unsigned char *)value; *p != '\0'; ++p) {
        if (*p < 0x20U || *p == '"' || *p == '\\') {
            return 0;
        }
    }
    return 1;
}

static int dcc_new_app_runtime_uint_literal_safe(const char *value) {
    if (value == NULL || value[0] == '\0') {
        return 0;
    }
    int saw_nonzero = 0;
    for (const unsigned char *p = (const unsigned char *)value; *p != '\0'; ++p) {
        if (!isdigit(*p)) {
            return 0;
        }
        if (*p != '0') {
            saw_nonzero = 1;
        }
    }
    return saw_nonzero;
}

static int dcc_new_app_runtime_hhmm_safe(const char *value) {
    if (value == NULL ||
        !isdigit((unsigned char)value[0]) ||
        !isdigit((unsigned char)value[1]) ||
        value[2] != ':' ||
        !isdigit((unsigned char)value[3]) ||
        !isdigit((unsigned char)value[4]) ||
        value[5] != '\0') {
        return 0;
    }
    unsigned hour = (unsigned)(value[0] - '0') * 10U + (unsigned)(value[1] - '0');
    unsigned minute = (unsigned)(value[3] - '0') * 10U + (unsigned)(value[4] - '0');
    return hour < 24U && minute < 60U;
}

static int dcc_new_app_runtime_insert_message_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    char *handler_name,
    size_t handler_name_len
) {
    char insertion[1800];
    snprintf(handler_name, handler_name_len, "on_%s_%s_message_command", options->cog_name, options->command_name);
    snprintf(
        insertion,
        sizeof(insertion),
        "DCC_MESSAGE_COMMAND_FN(%s) {\n"
        "    (void)event;\n"
        "    (void)user_data;\n"
        "\n"
        "    char reply[192];\n"
        "    snprintf(\n"
        "        reply,\n"
        "        sizeof(reply),\n"
        "        \"%s command received%%s%%s\",\n"
        "        args != NULL && args[0] != '\\0' ? \": \" : \"\",\n"
        "        args != NULL ? args : \"\"\n"
        "    );\n"
        "    if (app != NULL && message != NULL) {\n"
        "        (void)DCC_APP_SEND_UI(app, message->channel_id, DCC_UI_TEXT(reply));\n"
        "    }\n"
        "}\n"
        "\n",
        handler_name,
        options->command_name
    );
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_HANDLER_MARKER, insertion, handler_name);
}

static int dcc_new_app_runtime_insert_message_route(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    char route[760];
    char duplicate_needle[360];
    snprintf(
        duplicate_needle,
        sizeof(duplicate_needle),
        "DCC_MESSAGE_COMMAND_DATA(\"%s\", \"%s\"",
        options->prefix_arg,
        options->command_name
    );
    snprintf(
        route,
        sizeof(route),
        "        DCC_MESSAGE_COMMAND_DATA(\"%s\", \"%s\", %s, user_data)\n",
        options->prefix_arg,
        options->command_name,
        handler_name
    );
    return dcc_new_app_runtime_insert_route_block(
        path,
        DCC_NEW_APP_MESSAGE_ROUTE_MARKER,
        DCC_NEW_APP_MESSAGE_EXTENSION_MARKER,
        "DCC_FEATURE_MESSAGE_COMMANDS",
        route,
        duplicate_needle
    );
}

static int dcc_new_app_runtime_insert_task_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    char *handler_name,
    size_t handler_name_len
) {
    char insertion[760];
    snprintf(handler_name, handler_name_len, "on_%s_%s_task", options->cog_name, options->command_name);
    snprintf(
        insertion,
        sizeof(insertion),
        "DCC_TASK_FN(%s) {\n"
        "    (void)app;\n"
        "    (void)user_data;\n"
        "}\n"
        "\n",
        handler_name
    );
    return dcc_new_app_insert_before_marker(path, DCC_NEW_APP_HANDLER_MARKER, insertion, handler_name);
}

static const char *dcc_new_app_runtime_task_macro(dcc_new_app_task_schedule_t schedule) {
    switch (schedule) {
        case DCC_NEW_APP_TASK_MS:
            return "DCC_TASK_LOOP_MS_DATA";
        case DCC_NEW_APP_TASK_MINUTES:
            return "DCC_TASK_LOOP_MINUTES_DATA";
        case DCC_NEW_APP_TASK_HOURS:
            return "DCC_TASK_LOOP_HOURS_DATA";
        case DCC_NEW_APP_TASK_DAILY_KST:
            return "DCC_TASK_LOOP_KST_DATA";
        case DCC_NEW_APP_TASK_SECONDS:
        default:
            return "DCC_TASK_LOOP_SECONDS_DATA";
    }
}

static int dcc_new_app_runtime_insert_task_route(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    char route[760];
    char duplicate_needle[360];
    const char *macro = dcc_new_app_runtime_task_macro(options->task_schedule);
    if (options->task_schedule == DCC_NEW_APP_TASK_DAILY_KST) {
        snprintf(duplicate_needle, sizeof(duplicate_needle), "%s(\"%s\", %s", macro, options->task_interval_arg, handler_name);
        snprintf(
            route,
            sizeof(route),
            "        %s(\"%s\", %s, user_data)\n",
            macro,
            options->task_interval_arg,
            handler_name
        );
    } else {
        snprintf(duplicate_needle, sizeof(duplicate_needle), "%s(%sU, %s", macro, options->task_interval_arg, handler_name);
        snprintf(
            route,
            sizeof(route),
            "        %s(%sU, %s, user_data)\n",
            macro,
            options->task_interval_arg,
            handler_name
        );
    }
    return dcc_new_app_runtime_insert_route_block(
        path,
        DCC_NEW_APP_TASK_ROUTE_MARKER,
        DCC_NEW_APP_TASK_EXTENSION_MARKER,
        "DCC_FEATURE_TASKS",
        route,
        duplicate_needle
    );
}

int dcc_new_app_generate_message_command(const dcc_new_app_options_t *options) {
    if (!dcc_new_app_runtime_c_string_safe(options->prefix_arg)) {
        fprintf(stderr, "--prefix must be a non-empty C-string-safe value\n");
        return -1;
    }
    char *path = NULL;
    if (dcc_new_app_runtime_require_cog_source(&path, options) != 0) {
        return -1;
    }
    char handler_name[320];
    int status = dcc_new_app_runtime_insert_message_handler(path, options, handler_name, sizeof(handler_name));
    if (status == 0) {
        status = dcc_new_app_runtime_insert_message_route(path, options, handler_name);
    }
    free(path);
    if (status != 0) {
        return -1;
    }
    printf("created DCC message command %s in feature %s under %s\n", options->command_name, options->cog_name, options->path);
    return 0;
}

int dcc_new_app_generate_task(const dcc_new_app_options_t *options) {
    if (options->task_schedule == DCC_NEW_APP_TASK_DAILY_KST) {
        if (!dcc_new_app_runtime_hhmm_safe(options->task_interval_arg)) {
            fprintf(stderr, "--daily-kst must use HH:MM with a valid KST time\n");
            return -1;
        }
    } else if (!dcc_new_app_runtime_uint_literal_safe(options->task_interval_arg)) {
        fprintf(stderr, "task interval must be a positive integer literal\n");
        return -1;
    }
    char *path = NULL;
    if (dcc_new_app_runtime_require_cog_source(&path, options) != 0) {
        return -1;
    }
    char handler_name[320];
    int status = dcc_new_app_runtime_insert_task_handler(path, options, handler_name, sizeof(handler_name));
    if (status == 0) {
        status = dcc_new_app_runtime_insert_task_route(path, options, handler_name);
    }
    free(path);
    if (status != 0) {
        return -1;
    }
    printf("created DCC task %s in feature %s under %s\n", options->command_name, options->cog_name, options->path);
    return 0;
}
