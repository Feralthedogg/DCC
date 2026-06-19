#ifndef DCC_OBJECT_CLONE_HELPERS_INTERNAL_H
#define DCC_OBJECT_CLONE_HELPERS_INTERNAL_H

#include <dcc/snowflake.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

char *dcc_clone_string_or_null(const char *value);
dcc_snowflake_t *dcc_clone_snowflake_array_or_null(const dcc_snowflake_t *ids, size_t count);

#ifdef __cplusplus
}
#endif

#endif
