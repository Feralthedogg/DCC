#ifndef DCC_JSON_KEY_LOOKUP_H
#define DCC_JSON_KEY_LOOKUP_H

#include "internal/json/dcc_json_key_ids.h"

#include <stddef.h>

int dcc_json_key_span_eq(const char *key, size_t len, const char *literal);
dcc_json_key_id_t dcc_json_gateway_key_id(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_short(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_medium(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_medium_tail(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_long(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_extended(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_extended_tail(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_len3(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_len4(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_len5(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_len6(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_len7(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_len8(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_len9(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_len10(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_len11(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_len12(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_len13(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_len14(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_len15(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_len16(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_len17(const char *key, size_t len);
dcc_json_key_id_t dcc_json_gateway_key_id_len18(const char *key, size_t len);

#endif
