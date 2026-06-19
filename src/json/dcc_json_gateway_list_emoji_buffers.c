#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_init_emoji_list_buffers(
    dcc_json_gateway_emoji_buffers_t *buffers,
    char *name,
    size_t name_cap,
    char *username,
    size_t username_cap,
    char *global_name,
    size_t global_name_cap,
    char *avatar,
    size_t avatar_cap
) {
    if (buffers == NULL || name == NULL || username == NULL ||
        global_name == NULL || avatar == NULL) {
        return DCC_ERR_JSON;
    }

    *buffers = (dcc_json_gateway_emoji_buffers_t){
        name,
        name_cap,
        username,
        username_cap,
        global_name,
        global_name_cap,
        avatar,
        avatar_cap
    };
    return DCC_OK;
}
