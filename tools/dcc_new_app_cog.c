#include "internal/dcc_new_app.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int dcc_new_app_write_cog_header(
    const dcc_new_app_options_t *options,
    const char *cog_name,
    uint8_t force
) {
    char relative_path[320];
    char guard[192];
    snprintf(relative_path, sizeof(relative_path), "src/%s.h", cog_name);
    dcc_new_app_make_guard(cog_name, guard, sizeof(guard));

    char *path = dcc_new_app_join(options->path, relative_path);
    if (path == NULL) {
        return -1;
    }
    FILE *file = dcc_new_app_open_output(path, force);
    free(path);
    if (file == NULL) {
        return -1;
    }
    fprintf(
        file,
        "#ifndef %s\n"
        "#define %s\n"
        "\n"
        "#include <dcc/sugar.h>\n"
        "\n"
        "DCC_DECLARE_FEATURE(%s);\n"
        "\n"
        "#endif\n",
        guard,
        guard,
        cog_name
    );
    return fclose(file) == 0 ? 0 : -1;
}

static int dcc_new_app_write_ping_cog_source(const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_join(options->path, "src/ping.c");
    if (path == NULL) {
        return -1;
    }
    FILE *file = dcc_new_app_open_output(path, options->force);
    free(path);
    if (file == NULL) {
        return -1;
    }
    fputs(
        "#include \"ping.h\"\n"
        "#include \"config.h\"\n"
        "\n"
        "#include <stdint.h>\n"
        "#include <stdio.h>\n"
        "\n"
        "DCC_COMMAND_IMPL(on_ping) {\n"
        "    (void)user_data;\n"
        "\n"
        "    uint64_t count = DCC_CTX_STORE_U64_OR(ctx, \"ping.count\", 0U) + 1U;\n"
        "    if (DCC_CTX_STORE(ctx) != NULL) {\n"
        "        (void)DCC_CTX_STORE_SET_U64(ctx, \"ping.count\", count);\n"
        "    } else {\n"
        "        count = 0U;\n"
        "    }\n"
        "\n"
        "    if (count != 0U) {\n"
        "        (void)DCC_CTX_REPLY_TEXT_F(ctx, \"pong #%llu\", (unsigned long long)count);\n"
        "    } else {\n"
        "        (void)DCC_CTX_REPLY_TEXT(ctx, \"pong\");\n"
        "    }\n"
        "}\n"
        "\n"
        "/* dcc_new_app:command-handlers */\n"
        "\n"
        "DCC_DEFINE_PUBLIC_FEATURE(\n"
        "    ping,\n"
        "    \"ping\",\n"
        "    /* dcc_new_app:middleware-extension */\n"
        "    DCC_FEATURE_COMMAND_ROUTES(\n"
        "        DCC_COMMAND_ROUTE_NO_OPTIONS_DATA(\"ping\", \"Reply with pong\", on_ping, user_data)\n"
        "        /* dcc_new_app:slash-routes */\n"
        "    )\n"
        "    /* dcc_new_app:subcommand-extension */\n"
        "    /* dcc_new_app:component-extension */\n"
        "    /* dcc_new_app:view-extension */\n"
        "    /* dcc_new_app:event-extension */\n"
        "    /* dcc_new_app:autocomplete-extension */\n"
        "    /* dcc_new_app:context-menu-extension */\n"
        "    /* dcc_new_app:message-command-extension */\n"
        "    /* dcc_new_app:task-extension */\n"
        ")\n",
        file
    );
    return fclose(file) == 0 ? 0 : -1;
}

static int dcc_new_app_write_cog_source(
    const dcc_new_app_options_t *options,
    const char *cog_name,
    uint8_t force
) {
    char relative_path[320];
    snprintf(relative_path, sizeof(relative_path), "src/%s.c", cog_name);
    char *path = dcc_new_app_join(options->path, relative_path);
    if (path == NULL) {
        return -1;
    }
    FILE *file = dcc_new_app_open_output(path, force);
    free(path);
    if (file == NULL) {
        return -1;
    }
    char params_macro[192];
    dcc_new_app_make_macro_name(cog_name, "_PARAMS", params_macro, sizeof(params_macro));
    fprintf(
        file,
        "#include \"%s.h\"\n"
        "#include \"config.h\"\n"
        "\n"
        "#include <stdio.h>\n"
        "\n"
        "typedef struct %s_args {\n"
        "    const char *name;\n"
        "} %s_args_t;\n"
        "\n"
        "#define %s(PARAM, type) \\\n"
        "    PARAM(type, STRING, name, \"name\", \"Name to echo\", 0U, \"there\")\n"
        "\n"
        "DCC_COMMAND_ARGS_IMPL(on_%s, %s_args_t, command) {\n"
        "    (void)user_data;\n"
        "\n"
        "    const char *name = command != NULL && command->name != NULL ? command->name : \"there\";\n"
        "    (void)DCC_CTX_REPLY_TEXT_F(ctx, \"%s: hello %%s\", name);\n"
        "}\n"
        "\n"
        "DCC_HANDLER(on_%s_button) {\n"
        "    (void)user_data;\n"
        "\n"
        "    const char *action = DCC_CTX_ACTION(ctx, \"%s.\");\n"
        "    (void)DCC_CTX_DONEF(ctx, \"%s button: %%s\", action != NULL ? action : \"unknown\");\n"
        "}\n"
        "\n"
        "/* dcc_new_app:command-handlers */\n"
        "\n"
        "DCC_DEFINE_PUBLIC_FEATURE(\n"
        "    %s,\n"
        "    \"%s\",\n"
        "    /* dcc_new_app:middleware-extension */\n"
        "    DCC_FEATURE_COMMAND_ROUTES(\n"
        "        DCC_COMMAND_ROUTE_DATA(\n"
        "            \"%s\",\n"
        "            \"Generated %s command\",\n"
        "            %s_args_t,\n"
        "            on_%s,\n"
        "            user_data,\n"
        "            %s\n"
        "        )\n"
        "        /* dcc_new_app:slash-routes */\n"
        "    ),\n"
        "    DCC_FEATURE_COMPONENTS(\n"
        "        DCC_BUTTON_PREFIX_DATA(\"%s.\", on_%s_button, user_data)\n"
        "        /* dcc_new_app:component-routes */\n"
        "    )\n"
        "    /* dcc_new_app:subcommand-extension */\n"
        "    /* dcc_new_app:component-extension */\n"
        "    /* dcc_new_app:view-extension */\n"
        "    /* dcc_new_app:event-extension */\n"
        "    /* dcc_new_app:autocomplete-extension */\n"
        "    /* dcc_new_app:context-menu-extension */\n"
        "    /* dcc_new_app:message-command-extension */\n"
        "    /* dcc_new_app:task-extension */\n"
        ")\n",
        cog_name,
        cog_name,
        cog_name,
        params_macro,
        cog_name,
        cog_name,
        cog_name,
        cog_name,
        cog_name,
        cog_name,
        cog_name,
        cog_name,
        cog_name,
        cog_name,
        cog_name,
        cog_name,
        params_macro,
        cog_name,
        cog_name
    );
    return fclose(file) == 0 ? 0 : -1;
}

int dcc_new_app_write_default_cogs(const dcc_new_app_options_t *options) {
    return dcc_new_app_write_cog_header(options, "ping", options->force) == 0 &&
           dcc_new_app_write_ping_cog_source(options) == 0
        ? 0
        : -1;
}

static int dcc_new_app_wire_cog(const dcc_new_app_options_t *options, const char *cog_name) {
    char insertion[384];
    char needle[192];

    char *cmake_path = dcc_new_app_join(options->path, "CMakeLists.txt");
    char *main_path = dcc_new_app_join(options->path, "src/main.c");
    if (cmake_path == NULL || main_path == NULL) {
        free(cmake_path);
        free(main_path);
        return -1;
    }

    snprintf(insertion, sizeof(insertion), "    src/%s.c\n", cog_name);
    snprintf(needle, sizeof(needle), "src/%s.c", cog_name);
    int status = dcc_new_app_insert_before_marker_compat(
        cmake_path,
        "    # dcc_new_app:feature-sources",
        "    # dcc_new_app:cog-sources",
        insertion,
        needle
    );
    if (status == 0) {
        snprintf(insertion, sizeof(insertion), "#include \"%s.h\"\n", cog_name);
        snprintf(needle, sizeof(needle), "\"%s.h\"", cog_name);
        status = dcc_new_app_insert_before_marker_compat(
            main_path,
            "/* dcc_new_app:feature-includes */",
            "/* dcc_new_app:cog-includes */",
            insertion,
            needle
        );
    }
    if (status == 0) {
        snprintf(insertion, sizeof(insertion), "                    DCC_USE_FEATURE(%s, &config),\n", cog_name);
        snprintf(needle, sizeof(needle), "DCC_USE_FEATURE(%s,", cog_name);
        status = dcc_new_app_insert_before_marker_compat(
            main_path,
            "                    /* dcc_new_app:feature-modules */",
            "                    /* dcc_new_app:cog-modules */",
            insertion,
            needle
        );
    }

    free(cmake_path);
    free(main_path);
    return status;
}

static int dcc_new_app_append_command_json(const dcc_new_app_options_t *options, const char *command_name) {
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
    snprintf(name_needle, sizeof(name_needle), "\"name\": \"%s\"", command_name);
    if (strstr(data, name_needle) != NULL) {
        free(data);
        free(path);
        return 0;
    }

    char *last_bracket = strrchr(data, ']');
    if (last_bracket == NULL) {
        fprintf(stderr, "warning: commands.json is not a JSON array; %s command was not appended\n", command_name);
        free(data);
        free(path);
        return 0;
    }
    int has_existing = strchr(data, '{') != NULL;
    char command[640];
    snprintf(
        command,
        sizeof(command),
        "%s\n"
        "  {\n"
        "    \"name\": \"%s\",\n"
        "    \"description\": \"Generated %s command\",\n"
        "    \"type\": 1,\n"
        "    \"options\": [\n"
        "      {\n"
        "        \"name\": \"name\",\n"
        "        \"description\": \"Name to echo\",\n"
        "        \"type\": 3,\n"
        "        \"required\": false\n"
        "      }\n"
        "    ]\n"
        "  }\n",
        has_existing ? "," : "",
        command_name,
        command_name
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

static int dcc_new_app_insert_command_handler(const dcc_new_app_options_t *options) {
    char relative_path[320];
    snprintf(relative_path, sizeof(relative_path), "src/%s.c", options->cog_name);
    char *path = dcc_new_app_join(options->path, relative_path);
    if (path == NULL) {
        return -1;
    }
    if (!dcc_new_app_path_exists(path)) {
        fprintf(stderr, "%s does not exist; add the feature first\n", path);
        free(path);
        return -1;
    }

    char handler_name[320];
    char params_macro[384];
    char insertion[1200];
    snprintf(handler_name, sizeof(handler_name), "on_%s_%s", options->cog_name, options->command_name);
    dcc_new_app_make_macro_name(handler_name, "_PARAMS", params_macro, sizeof(params_macro));
    snprintf(
        insertion,
        sizeof(insertion),
        "typedef struct %s_args {\n"
        "    const char *name;\n"
        "} %s_args_t;\n"
        "\n"
        "#define %s(PARAM, type) \\\n"
        "    PARAM(type, STRING, name, \"name\", \"Name to echo\", 0U, \"there\")\n"
        "\n"
        "DCC_COMMAND_ARGS_IMPL(%s, %s_args_t, command) {\n"
        "    (void)user_data;\n"
        "\n"
        "    const char *name = command != NULL && command->name != NULL ? command->name : \"there\";\n"
        "    (void)DCC_CTX_REPLY_TEXT_F(ctx, \"%s: hello %%s\", name);\n"
        "}\n"
        "\n",
        handler_name,
        handler_name,
        params_macro,
        handler_name,
        handler_name,
        options->command_name
    );
    int status = dcc_new_app_insert_before_marker(
        path,
        "/* dcc_new_app:command-handlers */",
        insertion,
        handler_name
    );
    free(path);
    return status;
}

static int dcc_new_app_insert_command_route(const dcc_new_app_options_t *options) {
    char relative_path[320];
    snprintf(relative_path, sizeof(relative_path), "src/%s.c", options->cog_name);
    char *path = dcc_new_app_join(options->path, relative_path);
    if (path == NULL) {
        return -1;
    }

    char handler_name[320];
    char params_macro[384];
    char route_needle[320];
    char insertion[1600];
    char guards[1000];
    int guarded = dcc_new_app_guard_items_literal(options, guards, sizeof(guards));
    if (guarded < 0) {
        free(path);
        return -1;
    }
    snprintf(handler_name, sizeof(handler_name), "on_%s_%s", options->cog_name, options->command_name);
    dcc_new_app_make_macro_name(handler_name, "_PARAMS", params_macro, sizeof(params_macro));
    snprintf(route_needle, sizeof(route_needle), "Generated %s command", options->command_name);
    snprintf(
        insertion,
        sizeof(insertion),
        "        ,\n"
	        "        %s(\n"
	        "            \"%s\",\n"
	        "            \"Generated %s command\",\n"
	        "            %s_args_t,\n"
	        "            %s,\n"
	        "            user_data,\n"
	        "            %s%s%s\n"
	        "        )\n",
	        guarded ? "DCC_COMMAND_ROUTE_DATA_GUARDED" : "DCC_COMMAND_ROUTE_DATA",
	        options->command_name,
	        options->command_name,
	        handler_name,
	        handler_name,
	        params_macro,
	        guarded ? ",\n            " : "",
	        guarded ? guards : ""
	    );
    int status = dcc_new_app_insert_before_marker(
        path,
        "        /* dcc_new_app:slash-routes */",
        insertion,
        route_needle
    );
    free(path);
    return status;
}

int dcc_new_app_generate_cog(const dcc_new_app_options_t *options) {
    if (!dcc_new_app_path_exists(options->path)) {
        fprintf(stderr, "%s does not exist; create the app first\n", options->path);
        return -1;
    }
    char *src_dir = dcc_new_app_join(options->path, "src");
    if (src_dir == NULL) {
        return -1;
    }
    int status = dcc_new_app_mkdirs(src_dir);
    free(src_dir);
    if (status != 0) {
        return -1;
    }
    if (dcc_new_app_write_cog_header(options, options->cog_name, options->force) != 0 ||
        dcc_new_app_write_cog_source(options, options->cog_name, options->force) != 0 ||
        dcc_new_app_wire_cog(options, options->cog_name) != 0 ||
        dcc_new_app_append_command_json(options, options->cog_name) != 0) {
        return -1;
    }
    printf("created DCC feature %s in %s\n", options->cog_name, options->path);
    return 0;
}

int dcc_new_app_generate_command(const dcc_new_app_options_t *options) {
    if (!dcc_new_app_path_exists(options->path)) {
        fprintf(stderr, "%s does not exist; create the app first\n", options->path);
        return -1;
    }
    if (dcc_new_app_insert_command_handler(options) != 0 ||
        dcc_new_app_insert_command_route(options) != 0 ||
        dcc_new_app_append_command_json(options, options->command_name) != 0) {
        return -1;
    }
    printf(
        "created DCC command %s in feature %s under %s\n",
        options->command_name,
        options->cog_name,
        options->path
    );
    return 0;
}
