#include "internal/json/dcc_json_internal.h"

#include <string.h>

void dcc_json_gateway_payload_reset(dcc_json_gateway_payload_t *out) {
    if (out == NULL) {
        return;
    }
    /* The payload is a fixed-size owning parse arena. Clearing the complete
     * arena keeps every event projection and truncation marker from a prior
     * parse out of the next one; adding a field cannot bypass a hand-maintained
     * reset list. */
    memset(out, 0, sizeof(*out));
    out->interaction.attachment_size_limit =
        DCC_INTERACTION_DEFAULT_ATTACHMENT_SIZE_LIMIT;
}
