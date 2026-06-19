#ifndef DCC_REST_SCHEDULED_EVENT_BUILDERS_INTERNAL_H
#define DCC_REST_SCHEDULED_EVENT_BUILDERS_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/dcc.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_rest_scheduled_event_body_context {
    const dcc_scheduled_event_params_t *params;
    dcc_scheduled_event_privacy_level_t privacy_level;
    dcc_scheduled_event_status_t status_value;
    dcc_scheduled_event_entity_type_t entity_type;
    uint8_t include_id;
} dcc_rest_scheduled_event_body_context_t;

dcc_status_t dcc_rest_scheduled_event_prepare_body_context(
    const dcc_scheduled_event_params_t *params,
    uint8_t include_id,
    dcc_rest_scheduled_event_body_context_t *out
);

dcc_status_t dcc_rest_scheduled_event_append_body(
    dcc_rest_buffer_t *body,
    const dcc_rest_scheduled_event_body_context_t *context
);

dcc_status_t dcc_rest_build_scheduled_event_body(
    const dcc_scheduled_event_params_t *params,
    uint8_t include_id,
    char **out
);

#ifdef __cplusplus
}
#endif

#endif
