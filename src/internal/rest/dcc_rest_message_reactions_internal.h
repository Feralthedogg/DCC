#ifndef DCC_REST_MESSAGE_REACTIONS_INTERNAL_H
#define DCC_REST_MESSAGE_REACTIONS_INTERNAL_H

#include <dcc/rest.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_alloc_message_reaction_path(
    char **out,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    const char *suffix
);

#ifdef __cplusplus
}
#endif

#endif
