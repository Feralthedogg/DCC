#ifndef DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_RESOLVED_MEMBER_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_RESOLVED_MEMBER_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_RESOLVED_MEMBER_FIELDS \
    dcc_member_t interaction_resolved_members[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP]; \
    dcc_snowflake_t interaction_resolved_member_role_ids[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP] \
                                                      [DCC_JSON_GATEWAY_ID_LIST_CAP]; \
    char interaction_resolved_member_usernames[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][128]; \
    char interaction_resolved_member_user_global_names[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][128]; \
    char interaction_resolved_member_user_avatars[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][128]; \
    char interaction_resolved_member_nicks[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][128]; \
    char interaction_resolved_member_avatars[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][128]; \
    char interaction_resolved_member_joined_ats[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][64]; \
    char interaction_resolved_member_premium_sinces[DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][64]; \
    char interaction_resolved_member_communication_disabled_untils \
        [DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP][64]; \
    dcc_interaction_resolved_permission_t interaction_resolved_member_permissions \
        [DCC_JSON_GATEWAY_INTERACTION_RESOLVED_CAP]; \
    uint64_t interaction_member_permissions; \
    uint8_t interaction_member_permissions_present

#endif
