#include "internal/objects/dcc_embed_builder_internal.h"

#include <stdio.h>

dcc_status_t dcc_embed_json_append_u32(dcc_embed_json_buffer_t *buffer, uint32_t value) {
    char tmp[16];
    int written = snprintf(tmp, sizeof(tmp), "%u", (unsigned)value);
    if (written < 0 || (size_t)written >= sizeof(tmp)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_embed_json_append(buffer, tmp, (size_t)written);
}
