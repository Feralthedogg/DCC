#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_init_scheduled_event_buffers(
    dcc_json_gateway_scheduled_event_buffers_t *buffers,
    char *name,
    size_t name_cap,
    char *description,
    size_t description_cap,
    char *image,
    size_t image_cap,
    char *location,
    size_t location_cap,
    char *start_time,
    size_t start_time_cap,
    char *end_time,
    size_t end_time_cap,
    char *creator_username,
    size_t creator_username_cap,
    char *creator_global_name,
    size_t creator_global_name_cap,
    char *creator_avatar,
    size_t creator_avatar_cap
) {
    if (buffers == NULL || name == NULL || description == NULL ||
        image == NULL || location == NULL || start_time == NULL ||
        end_time == NULL || creator_username == NULL ||
        creator_global_name == NULL || creator_avatar == NULL) {
        return DCC_ERR_JSON;
    }

    *buffers = (dcc_json_gateway_scheduled_event_buffers_t){
        name,
        name_cap,
        description,
        description_cap,
        image,
        image_cap,
        location,
        location_cap,
        start_time,
        start_time_cap,
        end_time,
        end_time_cap,
        creator_username,
        creator_username_cap,
        creator_global_name,
        creator_global_name_cap,
        creator_avatar,
        creator_avatar_cap
    };
    return DCC_OK;
}
