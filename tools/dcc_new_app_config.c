#include "internal/dcc_new_app.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_NEW_APP_CONFIG_FIELD_MARKER "    /* dcc_new_app:config-fields */"
#define DCC_NEW_APP_CONFIG_BINDING_MARKER "            /* dcc_new_app:config-bindings */"
#define DCC_NEW_APP_CONFIG_ENV_MARKER "# dcc_new_app:config-env"

typedef struct dcc_new_app_config_type {
    const char *name;
    const char *ctype;
    const char *binding_macro;
    const char *example_value;
} dcc_new_app_config_type_t;

static const dcc_new_app_config_type_t dcc_new_app_config_types[] = {
    { "string", "const char *", "DCC_CONFIG_STRING", "replace-me" },
    { "bool", "uint8_t", "DCC_CONFIG_BOOL", "false" },
    { "boolean", "uint8_t", "DCC_CONFIG_BOOL", "false" },
    { "u64", "uint64_t", "DCC_CONFIG_U64", "0" },
    { "uint64", "uint64_t", "DCC_CONFIG_U64", "0" },
    { "i64", "int64_t", "DCC_CONFIG_I64", "0" },
    { "int64", "int64_t", "DCC_CONFIG_I64", "0" },
    { "channel", "dcc_snowflake_t", "DCC_CONFIG_CHANNEL", "123456789012345678" },
    { "role", "dcc_snowflake_t", "DCC_CONFIG_ROLE", "123456789012345678" },
    { "guild", "dcc_snowflake_t", "DCC_CONFIG_GUILD", "123456789012345678" },
    { "snowflake", "dcc_snowflake_t", "DCC_CONFIG_SNOWFLAKE", "123456789012345678" },
    { "id", "dcc_snowflake_t", "DCC_CONFIG_SNOWFLAKE", "123456789012345678" }
};

static int dcc_new_app_config_env_name_valid(const char *name) {
    if (name == NULL || name[0] == '\0') {
        return 0;
    }
    if (!(isalpha((unsigned char)name[0]) || name[0] == '_')) {
        return 0;
    }
    for (const unsigned char *p = (const unsigned char *)name; *p != '\0'; ++p) {
        if (!(isalnum(*p) || *p == '_')) {
            return 0;
        }
    }
    return 1;
}

static const dcc_new_app_config_type_t *dcc_new_app_config_find_type(const char *name) {
    size_t count = sizeof(dcc_new_app_config_types) / sizeof(dcc_new_app_config_types[0]);
    for (size_t i = 0U; i < count; ++i) {
        if (strcmp(name, dcc_new_app_config_types[i].name) == 0) {
            return &dcc_new_app_config_types[i];
        }
    }
    return NULL;
}

static int dcc_new_app_config_require_marker(const char *path, const char *marker) {
    size_t len = 0U;
    char *data = dcc_new_app_read_file(path, &len);
    (void)len;
    if (data == NULL) {
        fprintf(stderr, "could not read %s for config update\n", path);
        return -1;
    }
    int found = strstr(data, marker) != NULL;
    free(data);
    if (!found) {
        fprintf(stderr, "config marker missing in %s; recreate the app or add %s\n", path, marker);
        return -1;
    }
    return 0;
}

static int dcc_new_app_config_insert_main(
    const char *config_path,
    const char *main_path,
    const dcc_new_app_options_t *options,
    const dcc_new_app_config_type_t *type
) {
    char field[180];
    char binding[260];
    char field_duplicate[180];
    char binding_duplicate[180];

    snprintf(field, sizeof(field), "    %s %s;\n", type->ctype, options->command_name);
    snprintf(
        binding,
        sizeof(binding),
        "            %s(\"%s\", &config.%s),\n",
        type->binding_macro,
        options->config_env_arg,
        options->command_name
    );
    snprintf(field_duplicate, sizeof(field_duplicate), " %s;", options->command_name);
    snprintf(binding_duplicate, sizeof(binding_duplicate), "&config.%s", options->command_name);

    if (dcc_new_app_config_require_marker(config_path, DCC_NEW_APP_CONFIG_FIELD_MARKER) != 0 ||
        dcc_new_app_config_require_marker(main_path, DCC_NEW_APP_CONFIG_BINDING_MARKER) != 0) {
        return -1;
    }
    if (dcc_new_app_insert_before_marker(
            config_path,
            DCC_NEW_APP_CONFIG_FIELD_MARKER,
            field,
            field_duplicate
        ) != 0) {
        return -1;
    }
    return dcc_new_app_insert_before_marker(
        main_path,
        DCC_NEW_APP_CONFIG_BINDING_MARKER,
        binding,
        binding_duplicate
    );
}

static int dcc_new_app_config_insert_env(
    const char *env_path,
    const dcc_new_app_options_t *options,
    const dcc_new_app_config_type_t *type
) {
    char entry[220];
    char duplicate[180];
    snprintf(entry, sizeof(entry), "%s=%s\n", options->config_env_arg, type->example_value);
    snprintf(duplicate, sizeof(duplicate), "%s=", options->config_env_arg);

    if (dcc_new_app_config_require_marker(env_path, DCC_NEW_APP_CONFIG_ENV_MARKER) != 0) {
        return -1;
    }
    return dcc_new_app_insert_before_marker(
        env_path,
        DCC_NEW_APP_CONFIG_ENV_MARKER,
        entry,
        duplicate
    );
}

int dcc_new_app_generate_config(const dcc_new_app_options_t *options) {
    if (!dcc_new_app_config_env_name_valid(options->config_env_arg)) {
        fprintf(stderr, "invalid ENV_NAME: %s\n", options->config_env_arg);
        return -1;
    }

    const dcc_new_app_config_type_t *type = dcc_new_app_config_find_type(options->config_type_arg);
    if (type == NULL) {
        fprintf(
            stderr,
            "unknown config type %s; expected string, bool, u64, i64, channel, role, guild, or snowflake\n",
            options->config_type_arg
        );
        return -1;
    }

    char *src_dir = dcc_new_app_join(options->path, "src");
    char *main_path = src_dir != NULL ? dcc_new_app_join(src_dir, "main.c") : NULL;
    char *config_path = src_dir != NULL ? dcc_new_app_join(src_dir, "config.h") : NULL;
    char *env_path = dcc_new_app_join(options->path, ".env.example");
    free(src_dir);
    if (main_path == NULL || config_path == NULL || env_path == NULL) {
        free(main_path);
        free(config_path);
        free(env_path);
        return -1;
    }

    int status = dcc_new_app_config_insert_main(config_path, main_path, options, type);
    if (status == 0) {
        status = dcc_new_app_config_insert_env(env_path, options, type);
    }
    free(main_path);
    free(config_path);
    free(env_path);
    if (status != 0) {
        return -1;
    }
    printf(
        "created DCC config %s from %s under %s\n",
        options->command_name,
        options->config_env_arg,
        options->path
    );
    return 0;
}
