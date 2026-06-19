#ifndef DCC_REST_TYPES_TEMPLATES_H
#define DCC_REST_TYPES_TEMPLATES_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_template_params {
    size_t size;
    dcc_snowflake_t guild_id;
    const char *code;
    const char *name;
    const char *description;
} dcc_template_params_t;

#ifdef __cplusplus
}
#endif

#endif
