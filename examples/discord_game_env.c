#include "discord_game_internal.h"

void load_env(void) {
    FILE *f = fopen(".env", "r");
    if (f == NULL) {
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), f) != NULL) {
        char *eq = strchr(line, '=');
        if (eq == NULL) {
            continue;
        }
        *eq = '\0';
        char *val = eq + 1;
        val[strcspn(val, "\r\n")] = '\0';
        setenv(line, val, 1);
    }

    fclose(f);
}

dcc_snowflake_t parse_snowflake_env(const char *name) {
    const char *value = getenv(name);
    if (value == NULL || value[0] == '\0') {
        return 0;
    }

    errno = 0;
    char *end = NULL;
    unsigned long long parsed = strtoull(value, &end, 10);
    if (errno != 0 || end == value || *end != '\0') {
        fprintf(stderr, "%s must be a numeric Discord snowflake; ignoring it\n", name);
        return 0;
    }

    return (dcc_snowflake_t)parsed;
}
