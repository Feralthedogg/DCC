#include "internal/dcc_new_app.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_NEW_APP_SUBCOMMAND_OPTIONS_MARKER "                    /* dcc_new_app:subcommand-options */"

static const char *dcc_new_app_min_ptr(const char *lhs, const char *rhs) {
    if (lhs == NULL) {
        return rhs;
    }
    if (rhs == NULL) {
        return lhs;
    }
    return lhs < rhs ? lhs : rhs;
}

static const char *dcc_new_app_subcommand_leaf(const char *path) {
    const char *slash = path != NULL ? strrchr(path, '/') : NULL;
    return slash != NULL && slash[1] != '\0' ? slash + 1 : path;
}

static int dcc_new_app_subcommand_schema_option(
    const dcc_new_app_options_t *options,
    char *out,
    size_t out_len
) {
    const char *leaf = dcc_new_app_subcommand_leaf(options->subcommand_path);
    if (leaf == NULL || leaf[0] == '\0') {
        return -1;
    }
    char symbol_base[320];
    char args_type[360];
    char params_macro[384];
    dcc_new_app_make_subcommand_symbol_base(options, symbol_base, sizeof(symbol_base));
    snprintf(args_type, sizeof(args_type), "%s_args_t", symbol_base);
    dcc_new_app_make_macro_name(symbol_base, "_PARAMS", params_macro, sizeof(params_macro));
    int written = snprintf(
        out,
        out_len,
        "DCC_CMD_SUB_PARAMS(\n"
        "                        \"%s\",\n"
        "                        \"Generated %s subcommand\",\n"
        "                        %s,\n"
        "                        %s\n"
        "                    )",
        leaf,
        leaf,
        args_type,
        params_macro
    );
    return written >= 0 && (size_t)written < out_len ? 0 : -1;
}

static int dcc_new_app_subcommand_json_option(
    const dcc_new_app_options_t *options,
    char *out,
    size_t out_len
) {
    const char *leaf = dcc_new_app_subcommand_leaf(options->subcommand_path);
    if (leaf == NULL || leaf[0] == '\0') {
        return -1;
    }
    int written = snprintf(
        out,
        out_len,
        "      {\n"
        "        \"name\": \"%s\",\n"
        "        \"description\": \"Generated %s subcommand\",\n"
        "        \"type\": 1,\n"
        "        \"options\": [\n"
        "          {\n"
        "            \"name\": \"name\",\n"
        "            \"description\": \"Name to echo\",\n"
        "            \"type\": 3,\n"
        "            \"required\": false\n"
        "          }\n"
        "        ]\n"
        "      }",
        leaf,
        leaf
    );
    return written >= 0 && (size_t)written < out_len ? 0 : -1;
}

static int dcc_new_app_convert_params_command_to_builder_subcommands(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *data,
    size_t len
) {
    char anchor[320];
    int has_policy = 0;
    int has_guarded = 0;
    snprintf(
        anchor,
        sizeof(anchor),
        "DCC_COMMAND_ROUTE_DATA_POLICY(\n"
        "            \"%s\",",
        options->command_name
    );
    char *command = strstr(data, anchor);
    if (command != NULL) {
        has_policy = 1;
    }
    if (command == NULL) {
        snprintf(
            anchor,
            sizeof(anchor),
            "DCC_COMMAND_ROUTE_DATA_POLICY(\n"
            "            \"%s\",",
            options->command_name
        );
        command = strstr(data, anchor);
        if (command != NULL) {
            has_policy = 1;
        }
    }
    if (command == NULL) {
        snprintf(
            anchor,
            sizeof(anchor),
            "DCC_COMMAND_ROUTE_DATA_GUARDED(\n"
            "            \"%s\",",
            options->command_name
        );
        command = strstr(data, anchor);
        if (command != NULL) {
            has_guarded = 1;
        }
    }
    if (command == NULL) {
        snprintf(
            anchor,
            sizeof(anchor),
            "DCC_COMMAND_ROUTE_DATA_GUARDED(\n"
            "            \"%s\",",
            options->command_name
        );
        command = strstr(data, anchor);
        if (command != NULL) {
            has_guarded = 1;
        }
    }
    if (command == NULL) {
        snprintf(
            anchor,
            sizeof(anchor),
            "DCC_COMMAND_ROUTE_DATA(\n"
            "            \"%s\",",
            options->command_name
        );
        command = strstr(data, anchor);
    }
    if (command == NULL) {
        snprintf(
            anchor,
            sizeof(anchor),
            "DCC_COMMAND_ROUTE_DATA(\n"
            "            \"%s\",",
            options->command_name
        );
        command = strstr(data, anchor);
    }
    if (command == NULL) {
        return 0;
    }

    char handler_name[320];
    char params_macro[384];
    snprintf(handler_name, sizeof(handler_name), "on_%s_%s", options->cog_name, options->command_name);
    dcc_new_app_make_macro_name(handler_name, "_PARAMS", params_macro, sizeof(params_macro));

    char schema[900];
    if (dcc_new_app_subcommand_schema_option(options, schema, sizeof(schema)) != 0) {
        return -1;
    }

    const char close_needle[] = "\n        )";
    char *close = strstr(command + strlen(anchor), close_needle);
    if (close == NULL) {
        return 0;
    }
    close += strlen(close_needle);

    const char *policy_start = NULL;
    size_t policy_len = 0U;
    if (has_policy || has_guarded) {
        char *params = strstr(command, params_macro);
        char *comma = params != NULL ? strchr(params + strlen(params_macro), ',') : NULL;
        policy_start = comma != NULL ? comma + 1 : NULL;
        while (policy_start != NULL &&
               (*policy_start == ' ' || *policy_start == '\n' || *policy_start == '\t')) {
            ++policy_start;
        }
        if (policy_start == NULL || policy_start >= close) {
            return -1;
        }
        const char *policy_end = close - strlen(close_needle);
        while (policy_end > policy_start &&
               (policy_end[-1] == ' ' || policy_end[-1] == '\n' || policy_end[-1] == '\t')) {
            --policy_end;
        }
        policy_len = (size_t)(policy_end - policy_start);
    }

    char replacement[2200];
    int written = snprintf(
        replacement,
        sizeof(replacement),
        "%s(\n"
        "            &((dcc_application_command_builder_t[]){\n"
        "                DCC_SLASH_COMMAND_WITH_OPTIONS(\n"
        "                    \"%s\",\n"
        "                    \"Generated %s command\",\n"
        "                    %s\n"
        "                    /* dcc_new_app:subcommand-options */\n"
        "                )\n"
        "            })[0],\n"
        "            %s_args_t,\n"
        "            %s,\n"
        "            user_data,\n"
        "            DCC_SLASH_PARAMS_BINDINGS(%s_args_t, %s),\n"
        "            DCC_NO_VALIDATORS()%s%.*s\n"
        "        )",
        has_guarded
            ? "DCC_ON_TYPED_SLASH_BUILDER_DATA_GUARDED"
            : (has_policy ? "DCC_ON_TYPED_SLASH_BUILDER_DATA_POLICY" : "DCC_ON_TYPED_SLASH_BUILDER_DATA"),
        options->command_name,
        options->command_name,
        schema,
        handler_name,
        handler_name,
        handler_name,
        params_macro,
        (has_policy || has_guarded) ? ",\n            " : "",
        (int)policy_len,
        policy_start != NULL ? policy_start : ""
    );
    if (written < 0 || (size_t)written >= sizeof(replacement)) {
        return -1;
    }

    return dcc_new_app_replace_range(
        path,
        data,
        len,
        command,
        (size_t)(close - command),
        replacement
    ) == 0 ? 1 : -1;
}

int dcc_new_app_update_command_builder_subcommands(
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
        int convert_status =
            dcc_new_app_convert_params_command_to_builder_subcommands(path, options, data, len);
        if (convert_status < 0) {
            free(data);
            return -1;
        }
        if (convert_status > 0) {
            free(data);
            return 0;
        }
        fprintf(
            stderr,
            "warning: command %s was not found in %s; subcommand route was added without changing command options\n",
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

    const char *leaf = dcc_new_app_subcommand_leaf(options->subcommand_path);
    char duplicate[220];
    snprintf(duplicate, sizeof(duplicate), "DCC_CMD_SUB_PARAMS(\n                        \"%s\"", leaf);
    char *existing = strstr(command, duplicate);
    if (existing == NULL) {
        snprintf(duplicate, sizeof(duplicate), "DCC_COMMAND_SUBCOMMAND(\n                        \"%s\"", leaf);
        existing = strstr(command, duplicate);
    }
    if (existing != NULL && existing < limit) {
        free(data);
        return 0;
    }

    char schema[900];
    if (dcc_new_app_subcommand_schema_option(options, schema, sizeof(schema)) != 0) {
        free(data);
        return -1;
    }

    char *marker = strstr(command, DCC_NEW_APP_SUBCOMMAND_OPTIONS_MARKER);
    if (marker != NULL && marker < limit) {
        char insertion[1100];
        snprintf(insertion, sizeof(insertion), "                    ,\n                    %s\n", schema);
        int status = dcc_new_app_replace_range(path, data, len, marker, 0U, insertion);
        free(data);
        return status;
    }

    const char plain[] = "DCC_OPT_STRING(\"name\", \"Name to echo\", 0U)";
    const char autocomplete[] = "DCC_OPT_STRING_AUTOCOMPLETE(\"name\", \"Name to echo\", 0U)";
    char *option = strstr(command, plain);
    size_t option_len = strlen(plain);
    if (option == NULL || option >= limit) {
        option = strstr(command, autocomplete);
        option_len = strlen(autocomplete);
    }
    if (option == NULL || option >= limit) {
        fprintf(
            stderr,
            "warning: generated option for %s was not found in %s; add DCC_COMMAND_SUBCOMMAND manually\n",
            options->command_name,
            path
        );
        free(data);
        return 0;
    }

    char replacement[1100];
    snprintf(
        replacement,
        sizeof(replacement),
        "%s\n"
        "%s",
        schema,
        DCC_NEW_APP_SUBCOMMAND_OPTIONS_MARKER
    );
    int status = dcc_new_app_replace_range(path, data, len, option, option_len, replacement);
    free(data);
    return status;
}

int dcc_new_app_update_command_json_subcommands(const dcc_new_app_options_t *options) {
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
    const char *leaf = dcc_new_app_subcommand_leaf(options->subcommand_path);
    char duplicate[160];
    snprintf(duplicate, sizeof(duplicate), "\"name\": \"%s\"", leaf);
    char *existing = strstr(command, duplicate);
    if (existing != NULL && existing < limit) {
        free(data);
        free(path);
        return 0;
    }

    char subcommand[900];
    if (dcc_new_app_subcommand_json_option(options, subcommand, sizeof(subcommand)) != 0) {
        free(data);
        free(path);
        return -1;
    }

    const char plain[] =
        "      {\n"
        "        \"name\": \"name\",\n"
        "        \"description\": \"Name to echo\",\n"
        "        \"type\": 3,\n"
        "        \"required\": false\n"
        "      }";
    const char autocomplete[] =
        "      {\n"
        "        \"name\": \"name\",\n"
        "        \"description\": \"Name to echo\",\n"
        "        \"type\": 3,\n"
        "        \"required\": false,\n"
        "        \"autocomplete\": true\n"
        "      }";
    char *option = strstr(command, plain);
    size_t option_len = strlen(plain);
    if (option == NULL || option >= limit) {
        option = strstr(command, autocomplete);
        option_len = strlen(autocomplete);
    }
    if (option != NULL && option < limit) {
        int status = dcc_new_app_replace_range(path, data, len, option, option_len, subcommand);
        free(data);
        free(path);
        return status;
    }

    char *array_end = strstr(command, "\n    ]");
    if (array_end == NULL || array_end >= limit) {
        free(data);
        free(path);
        return 0;
    }
    char insertion[1000];
    snprintf(insertion, sizeof(insertion), ",\n%s", subcommand);
    int status = dcc_new_app_replace_range(path, data, len, array_end, 0U, insertion);
    free(data);
    free(path);
    return status;
}
