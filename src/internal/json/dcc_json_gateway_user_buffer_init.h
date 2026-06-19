#ifndef DCC_JSON_GATEWAY_USER_BUFFER_INIT_H
#define DCC_JSON_GATEWAY_USER_BUFFER_INIT_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_member_buffers.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_init_user_buffers(
    dcc_json_gateway_user_buffers_t *buffers,
    char *username,
    size_t username_cap,
    char *global_name,
    size_t global_name_cap,
    char *avatar,
    size_t avatar_cap
);

#endif
