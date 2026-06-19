#ifndef DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_OPTION_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_OPTION_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_INTERACTION_OPTION_FIELDS \
    size_t interaction_option_list_count; \
    dcc_interaction_option_t interaction_option_lists[DCC_JSON_GATEWAY_INTERACTION_OPTION_LIST_CAP] \
                                                    [DCC_JSON_GATEWAY_INTERACTION_OPTION_CAP]; \
    char interaction_option_names[DCC_JSON_GATEWAY_INTERACTION_OPTION_LIST_CAP] \
                                 [DCC_JSON_GATEWAY_INTERACTION_OPTION_CAP] \
                                 [DCC_JSON_GATEWAY_INTERACTION_OPTION_NAME_CAP]; \
    char interaction_option_strings[DCC_JSON_GATEWAY_INTERACTION_OPTION_LIST_CAP] \
                                   [DCC_JSON_GATEWAY_INTERACTION_OPTION_CAP] \
                                   [DCC_JSON_GATEWAY_INTERACTION_OPTION_STRING_CAP]

#endif
