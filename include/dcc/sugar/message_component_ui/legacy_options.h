#ifndef DCC_SUGAR_MESSAGE_COMPONENT_UI_LEGACY_OPTIONS_H
#define DCC_SUGAR_MESSAGE_COMPONENT_UI_LEGACY_OPTIONS_H

#include <dcc/sugar/message_component_ui/messages.h>

#define DCC_COMPONENT_EMOJI_NAME(name_) \
    ((dcc_component_emoji_t){ \
        .name = (name_) \
    })
#define DCC_COMPONENT_EMOJI_ID(name_, id_, animated_) \
    ((dcc_component_emoji_t){ \
        .name = (name_), \
        .id = (id_), \
        .animated = (animated_), \
        .has_id = 1U, \
        .has_animated = 1U \
    })
#define DCC_SELECT_OPTION(label_, value_) \
    ((dcc_select_option_t){ \
        .label = (label_), \
        .value = (value_) \
    })
#define DCC_SELECT_OPTION_DESC(label_, value_, description_) \
    ((dcc_select_option_t){ \
        .label = (label_), \
        .value = (value_), \
        .description = (description_) \
    })
#define DCC_SELECT_OPTION_DEFAULT(label_, value_) \
    ((dcc_select_option_t){ \
        .label = (label_), \
        .value = (value_), \
        .is_default = 1U, \
        .has_default = 1U \
    })
#define DCC_SELECT_OPTION_EMOJI(label_, value_, emoji_) \
    ((dcc_select_option_t){ \
        .label = (label_), \
        .value = (value_), \
        .emoji = (emoji_), \
        .has_emoji = 1U \
    })


#endif /* DCC_SUGAR_MESSAGE_COMPONENT_UI_LEGACY_OPTIONS_H */
