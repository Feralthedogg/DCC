#ifndef DCC_MESSAGE_POLL_INTERNAL_H
#define DCC_MESSAGE_POLL_INTERNAL_H

#include "internal/objects/dcc_message_json_buffer_internal.h"

#include <dcc/message.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_message_poll_validate(const dcc_poll_builder_t *poll);
dcc_status_t dcc_message_poll_validate_emoji(const dcc_component_emoji_t *emoji);
dcc_status_t dcc_message_poll_append_media(
    const dcc_poll_media_t *media,
    dcc_message_json_buffer_t *buffer
);

#ifdef __cplusplus
}
#endif

#endif
