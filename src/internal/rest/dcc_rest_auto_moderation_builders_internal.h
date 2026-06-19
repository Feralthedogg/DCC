#ifndef DCC_REST_AUTO_MODERATION_BUILDERS_INTERNAL_H
#define DCC_REST_AUTO_MODERATION_BUILDERS_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/dcc.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_build_auto_moderation_rule_body(
    const dcc_auto_moderation_rule_params_t *params,
    uint8_t include_id,
    char **out
);
dcc_status_t dcc_rest_build_auto_moderation_trigger_metadata(
    const dcc_auto_moderation_metadata_params_t *metadata,
    dcc_rest_buffer_t *body
);
dcc_status_t dcc_rest_build_auto_moderation_action(
    const dcc_auto_moderation_action_params_t *action,
    dcc_rest_buffer_t *body
);
dcc_status_t dcc_rest_build_auto_moderation_actions(
    const dcc_auto_moderation_action_params_t *actions,
    size_t action_count,
    dcc_rest_buffer_t *body
);

#ifdef __cplusplus
}
#endif

#endif
