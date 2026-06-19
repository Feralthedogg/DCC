#ifndef DCC_HTTP_PERFORM_INTERNAL_H
#define DCC_HTTP_PERFORM_INTERNAL_H

#include "internal/http/dcc_http_request_internal.h"
#include "internal/http/dcc_http_response_internal.h"

#include <dcc/error.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_http_perform(const dcc_http_request_t *request, dcc_http_response_t *out);
dcc_status_t dcc_http_exchange_wire(
    const dcc_http_request_t *request,
    const dcc_http_url_parts_t *url,
    const dcc_http_buffer_t *wire_request,
    dcc_http_buffer_t *wire_response
);

#ifdef __cplusplus
}
#endif

#endif
