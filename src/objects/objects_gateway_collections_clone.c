#include "internal/objects/dcc_objects_internal.h"

int dcc_gateway_event_data_clone_collections(
    dcc_gateway_event_data_t *copy,
    const dcc_gateway_event_data_t *data
) {
    if (copy == NULL || data == NULL) {
        return -1;
    }

    if (dcc_gateway_clone_snowflake_collection(data->ids, data->ids_count, &copy->ids) != 0 ||
        dcc_gateway_clone_snowflake_collection(
            data->channel_ids,
            data->channel_ids_count,
            &copy->channel_ids
        ) != 0 ||
        dcc_gateway_clone_snowflake_collection(
            data->removed_member_ids,
            data->removed_member_ids_count,
            &copy->removed_member_ids
        ) != 0 ||
        dcc_gateway_clone_snowflake_collection(
            data->member_ids,
            data->member_ids_count,
            &copy->member_ids
        ) != 0 ||
        dcc_gateway_clone_thread_member_collection(copy, data) != 0 ||
        dcc_gateway_clone_member_collection(copy, data) != 0 ||
        dcc_gateway_clone_snowflake_collection(data->role_ids, data->role_ids_count, &copy->role_ids) != 0 ||
        dcc_gateway_clone_snowflake_collection(
            data->not_found_ids,
            data->not_found_ids_count,
            &copy->not_found_ids
        ) != 0 ||
        dcc_gateway_clone_snowflake_collection(data->sku_ids, data->sku_ids_count, &copy->sku_ids) != 0 ||
        dcc_gateway_clone_snowflake_collection(
            data->entitlement_ids,
            data->entitlement_ids_count,
            &copy->entitlement_ids
        ) != 0 ||
        dcc_gateway_clone_activity_collection(copy, data) != 0) {
        return -1;
    }

    return 0;
}
