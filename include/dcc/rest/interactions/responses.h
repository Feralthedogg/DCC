#ifndef DCC_REST_INTERACTIONS_RESPONSES_H
#define DCC_REST_INTERACTIONS_RESPONSES_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_interaction_response_create(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    const dcc_rest_interaction_response_t *response,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
