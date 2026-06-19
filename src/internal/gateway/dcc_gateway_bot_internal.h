#ifndef DCC_GATEWAY_BOT_INTERNAL_H
#define DCC_GATEWAY_BOT_INTERNAL_H

#include "internal/json/dcc_json.h"

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_gateway_build_url(const char *base, const char *path, char **out);
dcc_status_t dcc_gateway_url_with_json_query(const char *raw, char **out);
dcc_status_t dcc_gateway_apply_bot_info(dcc_client_t *client, const dcc_json_t *root);

#ifdef __cplusplus
}
#endif

#endif
