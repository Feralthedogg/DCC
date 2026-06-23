#ifndef DCC_SUGAR_MESSAGE_COMPONENT_UI_MESSAGES_H
#define DCC_SUGAR_MESSAGE_COMPONENT_UI_MESSAGES_H

#include <dcc/sugar/message_component_ui/base.h>

#define DCC_MESSAGE_ALLOWED_MENTIONS_JSON(content_, allowed_mentions_json_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .allowed_mentions_json = (allowed_mentions_json_), \
        .has_content = 1U \
    })
#define DCC_MESSAGE_STICKERS_ARRAY(sticker_ids_, sticker_count_) \
    ((dcc_message_builder_t){ \
        .sticker_ids = (sticker_ids_), \
        .sticker_ids_count = (sticker_count_) \
    })
#define DCC_MESSAGE_STICKERS(...) \
    ((dcc_message_builder_t){ \
        .sticker_ids = DCC_SUGAR_ARRAY(dcc_snowflake_t, __VA_ARGS__), \
        .sticker_ids_count = DCC_SUGAR_ARRAY_LEN(dcc_snowflake_t, __VA_ARGS__) \
    })

#endif /* DCC_SUGAR_MESSAGE_COMPONENT_UI_MESSAGES_H */
