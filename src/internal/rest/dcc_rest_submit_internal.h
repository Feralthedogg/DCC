#ifndef DCC_REST_SUBMIT_INTERNAL_H
#define DCC_REST_SUBMIT_INTERNAL_H

#include <dcc/rest/request.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_submit_operation(
    dcc_client_t *client,
    const dcc_rest_request_desc_t *description,
    const char *operation,
    uint8_t sensitive_path,
    dcc_rest_request_t **out_request
);

dcc_status_t dcc_rest_call_options_normalize(
    const dcc_rest_call_options_t *options,
    dcc_rest_call_options_t *out
);

#ifdef __cplusplus
}
#endif

#endif
