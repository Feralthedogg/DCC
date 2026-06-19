#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_template_paths_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_alloc_template_path(char **out, const char *code) {
    char *encoded = NULL;
    dcc_status_t status = dcc_rest_escape_path_segment(code, &encoded);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_alloc_formatted_path(out, "/guilds/templates/%s", encoded);
    free(encoded);
    return status;
}

dcc_status_t dcc_rest_alloc_guild_template_path(char **out, dcc_snowflake_t guild_id, const char *code) {
    char *encoded = NULL;
    dcc_status_t status = dcc_rest_escape_path_segment(code, &encoded);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_alloc_formatted_path(out, "/guilds/%llu/templates/%s", (unsigned long long)guild_id, encoded);
    free(encoded);
    return status;
}
