#ifndef DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_ID_COLLECTION_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_ID_COLLECTION_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_GATEWAY_ID_COLLECTION_FIELDS \
    dcc_snowflake_t gateway_ids[DCC_JSON_GATEWAY_ID_LIST_CAP]; \
    dcc_snowflake_t gateway_channel_ids[DCC_JSON_GATEWAY_ID_LIST_CAP]; \
    dcc_snowflake_t gateway_removed_member_ids[DCC_JSON_GATEWAY_ID_LIST_CAP]; \
    dcc_snowflake_t gateway_member_ids[DCC_JSON_GATEWAY_ID_LIST_CAP]; \
    dcc_snowflake_t gateway_role_ids[DCC_JSON_GATEWAY_ID_LIST_CAP]; \
    dcc_snowflake_t gateway_not_found_ids[DCC_JSON_GATEWAY_ID_LIST_CAP]; \
    dcc_snowflake_t gateway_sku_ids[DCC_JSON_GATEWAY_ID_LIST_CAP]; \
    dcc_snowflake_t gateway_entitlement_ids[DCC_JSON_GATEWAY_ID_LIST_CAP]

#endif
