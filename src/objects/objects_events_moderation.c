#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_auto_moderation_action_t *dcc_auto_moderation_action_clone(const dcc_auto_moderation_action_t *action) {
    if (action == NULL) {
        return NULL;
    }

    dcc_auto_moderation_action_t *copy = (dcc_auto_moderation_action_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *action;
    copy->content = dcc_clone_string_or_null(action->content);
    copy->matched_keyword = dcc_clone_string_or_null(action->matched_keyword);
    copy->matched_content = dcc_clone_string_or_null(action->matched_content);

    if ((action->content != NULL && copy->content == NULL) ||
        (action->matched_keyword != NULL && copy->matched_keyword == NULL) ||
        (action->matched_content != NULL && copy->matched_content == NULL)) {
        dcc_auto_moderation_action_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_auto_moderation_action_free(dcc_auto_moderation_action_t *action) {
    if (action == NULL) {
        return;
    }
    free((void *)action->content);
    free((void *)action->matched_keyword);
    free((void *)action->matched_content);
    free(action);
}

dcc_auto_moderation_rule_t *dcc_auto_moderation_rule_clone(const dcc_auto_moderation_rule_t *rule) {
    if (rule == NULL) {
        return NULL;
    }

    dcc_auto_moderation_rule_t *copy = (dcc_auto_moderation_rule_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *rule;
    copy->role_ids = NULL;
    copy->channel_ids = NULL;
    copy->name = dcc_clone_string_or_null(rule->name);
    if (rule->role_ids != NULL && rule->role_ids_count > 0) {
        copy->role_ids = dcc_clone_snowflake_array_or_null(rule->role_ids, rule->role_ids_count);
        if (copy->role_ids == NULL) {
            dcc_auto_moderation_rule_free(copy);
            return NULL;
        }
    }
    if (rule->channel_ids != NULL && rule->channel_ids_count > 0) {
        copy->channel_ids = dcc_clone_snowflake_array_or_null(rule->channel_ids, rule->channel_ids_count);
        if (copy->channel_ids == NULL) {
            dcc_auto_moderation_rule_free(copy);
            return NULL;
        }
    }

    if (rule->name != NULL && copy->name == NULL) {
        dcc_auto_moderation_rule_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_auto_moderation_rule_free(dcc_auto_moderation_rule_t *rule) {
    if (rule == NULL) {
        return;
    }
    free((void *)rule->role_ids);
    free((void *)rule->channel_ids);
    free((void *)rule->name);
    free(rule);
}
