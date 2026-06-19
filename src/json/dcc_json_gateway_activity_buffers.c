#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_init_activity_buffers(
    dcc_json_gateway_activity_buffers_t *buffers,
    char *name,
    size_t name_cap,
    char *state,
    size_t state_cap,
    char *details,
    size_t details_cap,
    char *url,
    size_t url_cap,
    char *emoji_name,
    size_t emoji_name_cap
) {
    if (buffers == NULL || name == NULL || state == NULL ||
        details == NULL || url == NULL || emoji_name == NULL) {
        return DCC_ERR_JSON;
    }

    *buffers = (dcc_json_gateway_activity_buffers_t){
        name,
        name_cap,
        state,
        state_cap,
        details,
        details_cap,
        url,
        url_cap,
        emoji_name,
        emoji_name_cap
    };
    return DCC_OK;
}
