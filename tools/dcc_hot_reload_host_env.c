#include "internal/dcc_hot_reload_host.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

const char *dcc_hot_reload_host_env_nonempty(const char *name) {
    const char *value = getenv(name);
    return value != NULL && value[0] != '\0' ? value : NULL;
}

static int dcc_hot_reload_host_setenv_if_missing(const char *name, const char *value) {
    if (name == NULL || name[0] == '\0' || value == NULL || dcc_hot_reload_host_env_nonempty(name) != NULL) {
        return 0;
    }
#if defined(_WIN32)
    return _putenv_s(name, value);
#else
    return setenv(name, value, 0);
#endif
}

int dcc_hot_reload_host_setenv(const char *name, const char *value) {
    if (name == NULL || name[0] == '\0' || value == NULL) {
        return -1;
    }
#if defined(_WIN32)
    return _putenv_s(name, value);
#else
    return setenv(name, value, 1);
#endif
}

int dcc_hot_reload_host_unsetenv(const char *name) {
    if (name == NULL || name[0] == '\0') {
        return -1;
    }
#if defined(_WIN32)
    return _putenv_s(name, "");
#else
    return unsetenv(name);
#endif
}

static char *dcc_hot_reload_host_trim_in_place(char *text) {
    char *end;
    while (*text != '\0' && isspace((unsigned char)*text)) {
        ++text;
    }
    end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1])) {
        --end;
    }
    *end = '\0';
    return text;
}

static void dcc_hot_reload_host_unquote_value(char *value) {
    size_t len = strlen(value);
    if (len >= 2U && ((value[0] == '"' && value[len - 1U] == '"') || (value[0] == '\'' && value[len - 1U] == '\''))) {
        memmove(value, value + 1, len - 2U);
        value[len - 2U] = '\0';
    }
}

void dcc_hot_reload_host_load_env_file(const char *path) {
    FILE *file = fopen(path, "r");
    char line[4096];
    if (file == NULL) {
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char *key;
        char *value;
        char *equals;
        char *hash;
        line[strcspn(line, "\r\n")] = '\0';
        key = dcc_hot_reload_host_trim_in_place(line);
        if (key[0] == '\0' || key[0] == '#') {
            continue;
        }
        if (strncmp(key, "export", 6U) == 0 && isspace((unsigned char)key[6])) {
            key = dcc_hot_reload_host_trim_in_place(key + 6);
        }
        equals = strchr(key, '=');
        if (equals == NULL) {
            continue;
        }
        *equals = '\0';
        value = dcc_hot_reload_host_trim_in_place(equals + 1);
        key = dcc_hot_reload_host_trim_in_place(key);
        hash = strchr(value, '#');
        if (hash != NULL && (hash == value || isspace((unsigned char)hash[-1]))) {
            *hash = '\0';
            value = dcc_hot_reload_host_trim_in_place(value);
        }
        dcc_hot_reload_host_unquote_value(value);
        (void)dcc_hot_reload_host_setenv_if_missing(key, value);
    }
    fclose(file);
}
