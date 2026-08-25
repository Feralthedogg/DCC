#ifndef DCC_REST_RESOURCE_INTERNAL_H
#define DCC_REST_RESOURCE_INTERNAL_H

#include "internal/rest/dcc_rest_async_request_internal.h"

dcc_status_t dcc_rest_resource_reserve_queued(dcc_client_t *client,
                                               size_t charge);
void dcc_rest_resource_release_unpublished(dcc_client_t *client,
                                            size_t charge);
uint8_t dcc_rest_resource_activate(dcc_rest_async_request_t *request);
void dcc_rest_resource_requeue(dcc_rest_async_request_t *request);
void dcc_rest_resource_release_request(dcc_rest_async_request_t *request);
dcc_status_t dcc_rest_resource_reserve_response(dcc_client_t *client,
                                                size_t bytes);
void dcc_rest_resource_release_response(dcc_client_t *client, size_t bytes);
void dcc_rest_resource_record_rate_limit(dcc_client_t *client);

#endif
