#ifndef DCC_JSON_DOM_API_H
#define DCC_JSON_DOM_API_H

#include <dcc/error.h>

#include <stddef.h>
#include <stdint.h>

#include "internal/json/dcc_json_types.h"

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_json_parse(const char *input, size_t len, dcc_json_t **out);
void dcc_json_free(dcc_json_t *json);

dcc_json_type_t dcc_json_typeof(const dcc_json_t *json);
const dcc_json_t *dcc_json_object_get(const dcc_json_t *json, const char *key);
const dcc_json_t *dcc_json_array_get(const dcc_json_t *json, size_t index);
size_t dcc_json_array_size(const dcc_json_t *json);

const char *dcc_json_string(const dcc_json_t *json);
int dcc_json_bool(const dcc_json_t *json, int *out);
int dcc_json_i64(const dcc_json_t *json, int64_t *out);
int dcc_json_u64(const dcc_json_t *json, uint64_t *out);

#ifdef __cplusplus
}
#endif

#endif
