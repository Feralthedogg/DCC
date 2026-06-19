#include "internal/rest/dcc_rest_message_reactions_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_alloc_message_reaction_path(
    char **out,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    const char *suffix
) {
    char *encoded = NULL;
    dcc_status_t status = dcc_rest_escape_path_segment(reaction, &encoded);
    if (status != DCC_OK) {
        return status;
    }
    if (suffix == NULL) {
        suffix = "";
    }
    status = dcc_rest_alloc_formatted_path(
        out,
        "/channels/%llu/messages/%llu/reactions/%s%s",
        (unsigned long long)channel_id,
        (unsigned long long)message_id,
        encoded,
        suffix
    );
    free(encoded);
    return status;
}
