#include "gateway_smoke_handlers.h"

#include <string.h>

void on_guild_role_create(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_role_t *role = dcc_event_role(event);
    dcc_event_type_t type = dcc_event_type(event);
    if (data != NULL &&
        role != NULL &&
        type == DCC_EVENT_GUILD_ROLE_CREATE &&
        data->guild_id == 333 &&
        data->role_id == 555 &&
        data->name != NULL &&
        strcmp(data->name, "mods") == 0 &&
        role->id == 555 &&
        role->guild_id == 333 &&
        role->name != NULL &&
        strcmp(role->name, "mods") == 0 &&
        role->permissions == 1024 &&
        role->color == 16711680 &&
        role->position == 4 &&
        role->hoist == 1 &&
        role->managed == 0 &&
        role->mentionable == 1) {
        dcc_role_t *copy = dcc_role_clone(role);
        if (copy != NULL &&
            copy->name != NULL &&
            strcmp(copy->name, "mods") == 0 &&
            copy->permissions == 1024 &&
            copy->mentionable == 1) {
            state->guild_role_create_seen = 1;
        }
        dcc_role_free(copy);
    } else if (data != NULL &&
        role != NULL &&
        type == DCC_EVENT_GUILD_ROLE_UPDATE &&
        data->guild_id == 333 &&
        role->id == 555 &&
        role->guild_id == 333 &&
        role->name != NULL &&
        strcmp(role->name, "mods") == 0) {
        dcc_role_t *copy = dcc_role_clone(role);
        if (copy != NULL && copy->id == 555 && copy->guild_id == 333) {
            state->guild_role_update_seen = 1;
        }
        dcc_role_free(copy);
    } else if (data != NULL &&
        role != NULL &&
        type == DCC_EVENT_GUILD_ROLE_DELETE &&
        data->guild_id == 333 &&
        data->role_id == 555 &&
        role->id == 555 &&
        role->guild_id == 333) {
        dcc_role_t *copy = dcc_role_clone(role);
        if (copy != NULL && copy->id == 555 && copy->guild_id == 333) {
            state->guild_role_delete_seen = 1;
        }
        dcc_role_free(copy);
    }
}
