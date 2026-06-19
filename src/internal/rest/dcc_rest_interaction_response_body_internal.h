#ifndef DCC_REST_INTERACTION_RESPONSE_BODY_INTERNAL_H
#define DCC_REST_INTERACTION_RESPONSE_BODY_INTERNAL_H

#include <dcc/message.h>
#include <dcc/rest/types/interactions.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_build_interaction_response_message_body(
    dcc_interaction_response_type_t type,
    const dcc_message_builder_t *message,
    char **out
);
dcc_status_t dcc_rest_build_interaction_response_type_body(
    dcc_interaction_response_type_t type,
    char **out
);

#ifdef __cplusplus
}
#endif

#endif
