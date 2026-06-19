#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_init_user_buffers(
    dcc_json_gateway_user_buffers_t *buffers,
    char *username,
    size_t username_cap,
    char *global_name,
    size_t global_name_cap,
    char *avatar,
    size_t avatar_cap
) {
    if (buffers == NULL || username == NULL || global_name == NULL) {
        return DCC_ERR_JSON;
    }

    *buffers = (dcc_json_gateway_user_buffers_t){
        username,
        username_cap,
        global_name,
        global_name_cap,
        avatar,
        avatar_cap
    };
    return DCC_OK;
}
