#ifndef DCC_INTERACTION_HEALTH_JSON_INTERNAL_H
#define DCC_INTERACTION_HEALTH_JSON_INTERNAL_H

#include <dcc/dcc.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_interaction_json_escape_string(const char *value, char *out, size_t out_size);

#ifdef __cplusplus
}
#endif

#endif
