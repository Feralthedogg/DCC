#ifndef DCC_CLIENT_STATE_INTERNAL_H
#define DCC_CLIENT_STATE_INTERNAL_H

#include "internal/client/dcc_client_config_state_fields_internal.h"
#include "internal/client/dcc_client_dm_cache_state_fields_internal.h"
#include "internal/client/dcc_client_gateway_state_fields_internal.h"
#include "internal/client/dcc_client_lifecycle_state_fields_internal.h"
#include "internal/client/dcc_client_rest_state_fields_internal.h"
#include "internal/client/dcc_client_runtime_state_fields_internal.h"
#include "internal/client/dcc_client_voice_registry_state_fields_internal.h"
#include "internal/voice/dcc_voice_owner_registry_internal.h"

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

struct dcc_client {
    DCC_CLIENT_GATEWAY_STATE_FIELDS;
    DCC_CLIENT_CONFIG_STATE_FIELDS;
    DCC_CLIENT_REST_STATE_FIELDS
    DCC_CLIENT_DM_CACHE_STATE_FIELDS;
    DCC_CLIENT_VOICE_REGISTRY_STATE_FIELDS;

    DCC_CLIENT_RUNTIME_STATE_FIELDS;

    DCC_CLIENT_LIFECYCLE_STATE_FIELDS;
};

#ifdef __cplusplus
}
#endif

#endif
