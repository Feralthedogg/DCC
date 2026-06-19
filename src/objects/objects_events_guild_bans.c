#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_guild_audit_log_entry_t *dcc_guild_audit_log_entry_clone(const dcc_guild_audit_log_entry_t *entry) {
    if (entry == NULL) {
        return NULL;
    }

    dcc_guild_audit_log_entry_t *copy = (dcc_guild_audit_log_entry_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }
    *copy = *entry;
    return copy;
}

void dcc_guild_audit_log_entry_free(dcc_guild_audit_log_entry_t *entry) {
    free(entry);
}

dcc_guild_ban_t *dcc_guild_ban_clone(const dcc_guild_ban_t *ban) {
    if (ban == NULL) {
        return NULL;
    }

    dcc_guild_ban_t *copy = (dcc_guild_ban_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }
    *copy = *ban;
    if (dcc_clone_user_fields(&copy->user, &ban->user) != 0) {
        dcc_guild_ban_free(copy);
        return NULL;
    }
    return copy;
}

void dcc_guild_ban_free(dcc_guild_ban_t *ban) {
    if (ban == NULL) {
        return;
    }
    free((void *)ban->user.username);
    free((void *)ban->user.global_name);
    free((void *)ban->user.avatar);
    free(ban);
}
