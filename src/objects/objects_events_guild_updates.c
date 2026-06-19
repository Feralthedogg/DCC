#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_guild_update_notice_t *dcc_guild_update_notice_clone(const dcc_guild_update_notice_t *notice) {
    if (notice == NULL) {
        return NULL;
    }

    dcc_guild_update_notice_t *copy = (dcc_guild_update_notice_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }
    *copy = *notice;
    return copy;
}

void dcc_guild_update_notice_free(dcc_guild_update_notice_t *notice) {
    free(notice);
}

dcc_scheduled_event_user_update_t *dcc_scheduled_event_user_update_clone(
    const dcc_scheduled_event_user_update_t *update
) {
    if (update == NULL) {
        return NULL;
    }

    dcc_scheduled_event_user_update_t *copy = (dcc_scheduled_event_user_update_t *)calloc(
        1,
        sizeof(*copy)
    );
    if (copy == NULL) {
        return NULL;
    }
    *copy = *update;
    return copy;
}

void dcc_scheduled_event_user_update_free(dcc_scheduled_event_user_update_t *update) {
    free(update);
}

dcc_webhooks_update_t *dcc_webhooks_update_clone(const dcc_webhooks_update_t *update) {
    if (update == NULL) {
        return NULL;
    }

    dcc_webhooks_update_t *copy = (dcc_webhooks_update_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }
    *copy = *update;
    return copy;
}

void dcc_webhooks_update_free(dcc_webhooks_update_t *update) {
    free(update);
}

dcc_guild_application_command_update_t *dcc_guild_application_command_update_clone(
    const dcc_guild_application_command_update_t *update
) {
    if (update == NULL) {
        return NULL;
    }

    dcc_guild_application_command_update_t *copy = (dcc_guild_application_command_update_t *)calloc(
        1,
        sizeof(*copy)
    );
    if (copy == NULL) {
        return NULL;
    }
    *copy = *update;
    return copy;
}

void dcc_guild_application_command_update_free(dcc_guild_application_command_update_t *update) {
    free(update);
}

dcc_embedded_activity_update_t *dcc_embedded_activity_update_clone(
    const dcc_embedded_activity_update_t *update
) {
    if (update == NULL) {
        return NULL;
    }

    dcc_embedded_activity_update_t *copy = (dcc_embedded_activity_update_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }
    *copy = *update;
    return copy;
}

void dcc_embedded_activity_update_free(dcc_embedded_activity_update_t *update) {
    free(update);
}
