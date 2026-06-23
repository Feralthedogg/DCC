#ifndef DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_SELECTS_H
#define DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_SELECTS_H

#include <dcc/sugar/message_component_ui/v2_media.h>

#define DCC_V2_DEFAULT_USER(id_) \
    ((dcc_component_v2_select_default_value_t){ \
        .id = (id_), \
        .type = DCC_COMPONENT_V2_SELECT_DEFAULT_USER \
    })
#define DCC_V2_DEFAULT_ROLE(id_) \
    ((dcc_component_v2_select_default_value_t){ \
        .id = (id_), \
        .type = DCC_COMPONENT_V2_SELECT_DEFAULT_ROLE \
    })
#define DCC_V2_DEFAULT_CHANNEL(id_) \
    ((dcc_component_v2_select_default_value_t){ \
        .id = (id_), \
        .type = DCC_COMPONENT_V2_SELECT_DEFAULT_CHANNEL \
    })
#define DCC_V2_DEFAULT_VALUES(...) \
    DCC_SUGAR_ARRAY(dcc_component_v2_select_default_value_t, __VA_ARGS__)
#define DCC_V2_CHANNEL_TYPES(...) \
    DCC_SUGAR_ARRAY(uint32_t, __VA_ARGS__)
#define DCC_V2_STRING_SELECT_ARRAY(custom_id_, options_, option_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_STRING_SELECT, \
        .custom_id = (custom_id_), \
        .options = (options_), \
        .options_count = (option_count_) \
    })
#define DCC_V2_STRING_SELECT(custom_id_, ...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_STRING_SELECT, \
        .custom_id = (custom_id_), \
        .options = DCC_SUGAR_ARRAY(dcc_select_option_t, __VA_ARGS__), \
        .options_count = DCC_SUGAR_ARRAY_LEN(dcc_select_option_t, __VA_ARGS__) \
    })
#define DCC_V2_STRING_SELECT_NS(namespace_, action_, ...) \
    DCC_V2_STRING_SELECT(DCC_COMPONENT_ID(namespace_, action_), __VA_ARGS__)
#define DCC_V2_USER_SELECT(custom_id_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_USER_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_V2_USER_SELECT_NS(namespace_, action_) \
    DCC_V2_USER_SELECT(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_V2_ROLE_SELECT(custom_id_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_ROLE_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_V2_ROLE_SELECT_NS(namespace_, action_) \
    DCC_V2_ROLE_SELECT(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_V2_MENTIONABLE_SELECT(custom_id_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_MENTIONABLE_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_V2_MENTIONABLE_SELECT_NS(namespace_, action_) \
    DCC_V2_MENTIONABLE_SELECT(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_V2_CHANNEL_SELECT_ARRAY(custom_id_, default_values_, default_value_count_, channel_types_, channel_type_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CHANNEL_SELECT, \
        .custom_id = (custom_id_), \
        .default_values = (default_values_), \
        .default_value_count = (default_value_count_), \
        .channel_types = (channel_types_), \
        .channel_type_count = (channel_type_count_) \
    })
#define DCC_V2_CHANNEL_SELECT(custom_id_, default_values_, channel_types_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CHANNEL_SELECT, \
        .custom_id = (custom_id_), \
        .default_values = (default_values_), \
        .default_value_count = DCC_ARRAY_LEN(default_values_), \
        .channel_types = (channel_types_), \
        .channel_type_count = DCC_ARRAY_LEN(channel_types_) \
    })
#define DCC_V2_CHANNEL_SELECT_NS(namespace_, action_, default_values_, channel_types_) \
    DCC_V2_CHANNEL_SELECT(DCC_COMPONENT_ID(namespace_, action_), (default_values_), (channel_types_))

#endif /* DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_SELECTS_H */
