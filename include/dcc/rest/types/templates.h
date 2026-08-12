#ifndef DCC_REST_TYPES_TEMPLATES_H
#define DCC_REST_TYPES_TEMPLATES_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_template_params {
    size_t size;
    uint32_t version;
    uint64_t present;
    uint64_t nulls;
    const char *name;
    const char *description;
    dcc_snowflake_t guild_id;
    const char *code;
} dcc_template_params_t;

enum {
  DCC_TEMPLATE_PARAMS_VERSION = 1U,
  DCC_TEMPLATE_PARAMS_PRESENT_NAME = UINT64_C(1),
  DCC_TEMPLATE_PARAMS_PRESENT_DESCRIPTION = UINT64_C(1) << 1U
};

#define DCC_TEMPLATE_PARAMS_INIT                                               \
  {sizeof(dcc_template_params_t), DCC_TEMPLATE_PARAMS_VERSION, UINT64_C(0),    \
   UINT64_C(0), NULL, NULL, UINT64_C(0), NULL}

static inline void dcc_template_params_init(dcc_template_params_t *params) {
  if (params != NULL) {
    dcc_template_params_t value = DCC_TEMPLATE_PARAMS_INIT;
    *params = value;
  }
}

#ifdef __cplusplus
}
#endif

#endif
