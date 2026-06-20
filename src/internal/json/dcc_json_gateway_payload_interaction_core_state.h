#ifndef DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_CORE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_CORE_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_CORE_FIELDS \
    int has_interaction; \
    dcc_interaction_t interaction; \
    char interaction_token[1024]; \
    char interaction_name[128]; \
    char interaction_custom_id[128]; \
    char interaction_locale[32]; \
    char interaction_guild_locale[32]; \
    const char *interaction_values[DCC_JSON_GATEWAY_INTERACTION_VALUE_CAP]; \
    char interaction_value_storage[DCC_JSON_GATEWAY_INTERACTION_VALUE_CAP][128]; \
    dcc_interaction_form_field_t interaction_form_fields[DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_CAP]; \
    char interaction_form_field_custom_ids[DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_CAP] \
        [DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_CUSTOM_ID_CAP]; \
    char interaction_form_field_values[DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_CAP] \
        [DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_VALUE_CAP]; \
    const char *interaction_form_field_value_lists[DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_CAP] \
        [DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_VALUE_LIST_CAP]; \
    char interaction_form_field_value_storage[DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_CAP] \
        [DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_VALUE_LIST_CAP] \
        [DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_VALUE_LIST_ITEM_CAP]; \
    dcc_interaction_authorizing_integration_owner_t interaction_authorizing_integration_owners \
        [DCC_JSON_GATEWAY_INTERACTION_AUTH_OWNER_CAP]; \
    dcc_entitlement_t interaction_entitlements[DCC_JSON_GATEWAY_INTERACTION_ENTITLEMENT_CAP]; \
    char interaction_entitlement_start_times[DCC_JSON_GATEWAY_INTERACTION_ENTITLEMENT_CAP][64]; \
    char interaction_entitlement_end_times[DCC_JSON_GATEWAY_INTERACTION_ENTITLEMENT_CAP][64]

#endif
