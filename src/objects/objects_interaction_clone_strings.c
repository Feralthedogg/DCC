#include "internal/objects/dcc_interaction_clone_internal.h"

int dcc_clone_interaction_string_fields(
    dcc_interaction_t *copy,
    const dcc_interaction_t *interaction
) {
    copy->token = dcc_clone_string_or_null(interaction->token);
    copy->name = dcc_clone_string_or_null(interaction->name);
    copy->custom_id = dcc_clone_string_or_null(interaction->custom_id);
    copy->locale = dcc_clone_string_or_null(interaction->locale);
    copy->guild_locale = dcc_clone_string_or_null(interaction->guild_locale);

    return (interaction->token != NULL && copy->token == NULL) ||
            (interaction->name != NULL && copy->name == NULL) ||
            (interaction->custom_id != NULL && copy->custom_id == NULL) ||
            (interaction->locale != NULL && copy->locale == NULL) ||
            (interaction->guild_locale != NULL && copy->guild_locale == NULL)
        ? -1
        : 0;
}
