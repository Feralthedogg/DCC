#ifndef DCC_REST_TEMPLATE_PATHS_INTERNAL_H
#define DCC_REST_TEMPLATE_PATHS_INTERNAL_H

#include <dcc/rest/types/templates.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_alloc_template_path(char **out, const char *code);
dcc_status_t dcc_rest_alloc_guild_template_path(char **out, dcc_snowflake_t guild_id, const char *code);

#ifdef __cplusplus
}
#endif

#endif
