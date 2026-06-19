#include "internal/rest/dcc_rest_builders_internal.h"

#include <string.h>

static int dcc_rest_scheduled_event_privacy_valid(dcc_scheduled_event_privacy_level_t privacy_level) {
    return privacy_level == DCC_SCHEDULED_EVENT_PRIVACY_GUILD_ONLY;
}

static int dcc_rest_scheduled_event_status_valid(dcc_scheduled_event_status_t status) {
    return status == DCC_SCHEDULED_EVENT_STATUS_SCHEDULED ||
           status == DCC_SCHEDULED_EVENT_STATUS_ACTIVE ||
           status == DCC_SCHEDULED_EVENT_STATUS_COMPLETED ||
           status == DCC_SCHEDULED_EVENT_STATUS_CANCELLED;
}

static int dcc_rest_scheduled_event_entity_type_valid(dcc_scheduled_event_entity_type_t entity_type) {
    return entity_type == DCC_SCHEDULED_EVENT_ENTITY_STAGE_INSTANCE ||
           entity_type == DCC_SCHEDULED_EVENT_ENTITY_VOICE ||
           entity_type == DCC_SCHEDULED_EVENT_ENTITY_EXTERNAL;
}

dcc_status_t dcc_rest_scheduled_event_prepare_body_context(
    const dcc_scheduled_event_params_t *params,
    uint8_t include_id,
    dcc_rest_scheduled_event_body_context_t *out
) {
    if (out == NULL ||
        params == NULL ||
        params->size < sizeof(*params) ||
        params->guild_id == 0 ||
        (include_id != 0 && params->event_id == 0) ||
        params->name == NULL ||
        params->name[0] == '\0' ||
        strlen(params->name) > 100U ||
        (params->description != NULL && params->description[0] != '\0' && strlen(params->description) > 1000U) ||
        (params->location != NULL && params->location[0] != '\0' && strlen(params->location) > 100U)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_scheduled_event_privacy_level_t privacy_level = params->privacy_level;
    if (privacy_level == 0) {
        privacy_level = DCC_SCHEDULED_EVENT_PRIVACY_GUILD_ONLY;
    }
    dcc_scheduled_event_status_t status_value = params->status;
    if (status_value == 0) {
        status_value = DCC_SCHEDULED_EVENT_STATUS_SCHEDULED;
    }
    dcc_scheduled_event_entity_type_t entity_type = params->entity_type;
    if (entity_type == 0) {
        entity_type = DCC_SCHEDULED_EVENT_ENTITY_EXTERNAL;
    }
    if (!dcc_rest_scheduled_event_privacy_valid(privacy_level) ||
        !dcc_rest_scheduled_event_status_valid(status_value) ||
        !dcc_rest_scheduled_event_entity_type_valid(entity_type)) {
        return DCC_ERR_INVALID_ARG;
    }

    out->params = params;
    out->privacy_level = privacy_level;
    out->status_value = status_value;
    out->entity_type = entity_type;
    out->include_id = include_id != 0 ? 1U : 0U;
    return DCC_OK;
}
