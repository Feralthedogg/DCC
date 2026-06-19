#ifndef DCC_REST_CHANNEL_FORUM_BUILDERS_INTERNAL_H
#define DCC_REST_CHANNEL_FORUM_BUILDERS_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/dcc.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int dcc_rest_channel_forum_layout_valid(dcc_channel_forum_layout_t layout);
int dcc_rest_channel_forum_tag_valid(const dcc_channel_forum_tag_params_t *tag);
dcc_status_t dcc_rest_build_channel_forum_tags_array(
    const dcc_channel_forum_tag_params_t *tags,
    size_t tag_count,
    dcc_rest_buffer_t *body
);

#ifdef __cplusplus
}
#endif

#endif
