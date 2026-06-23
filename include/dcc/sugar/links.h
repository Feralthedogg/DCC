#ifndef DCC_SUGAR_LINKS_H
#define DCC_SUGAR_LINKS_H

#include <dcc/message_link.h>

static inline const char *dcc_sugar_message_link(
    char *buffer,
    size_t buffer_size,
    dcc_message_link_t link
) {
    return dcc_message_link_format(buffer, buffer_size, &link) == DCC_OK ? buffer : "";
}

#define DCC_MESSAGE_LINK(guild_id_, channel_id_, message_id_) \
    dcc_sugar_message_link( \
        (char[DCC_MESSAGE_LINK_MAX]){ 0 }, \
        DCC_MESSAGE_LINK_MAX, \
        ((dcc_message_link_t){ \
            .guild_id = (guild_id_), \
            .channel_id = (channel_id_), \
            .message_id = (message_id_) \
        }) \
    )

#define DCC_DM_MESSAGE_LINK(channel_id_, message_id_) \
    dcc_sugar_message_link( \
        (char[DCC_MESSAGE_LINK_MAX]){ 0 }, \
        DCC_MESSAGE_LINK_MAX, \
        ((dcc_message_link_t){ \
            .channel_id = (channel_id_), \
            .message_id = (message_id_), \
            .is_dm = 1U \
        }) \
    )

#define DCC_PARSE_MESSAGE_LINK(text_, out_) dcc_message_link_parse((text_), (out_))

#endif
