#include "gateway_smoke_handlers.h"

#include <stdio.h>
#include <string.h>

void on_integration_create(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_integration_t *integration = dcc_event_integration(event);
    dcc_event_type_t type = dcc_event_type(event);
    if (data != NULL &&
        integration != NULL &&
        type == DCC_EVENT_INTEGRATION_CREATE &&
        data->id == 321 &&
        data->guild_id == 333 &&
        data->name != NULL &&
        strcmp(data->name, "builds") == 0 &&
        data->type_text != NULL &&
        strcmp(data->type_text, "discord") == 0 &&
        data->enabled == 1 &&
        data->role_id == 555 &&
        integration->id == 321 &&
        integration->guild_id == 333 &&
        integration->name != NULL &&
        strcmp(integration->name, "builds") == 0 &&
        integration->type_text != NULL &&
        strcmp(integration->type_text, "discord") == 0 &&
        integration->enabled == 1 &&
        integration->role_id == 555) {
        dcc_gateway_event_data_t *copy = dcc_gateway_event_data_clone(data);
        dcc_integration_t *integration_copy = dcc_integration_clone(integration);
        if (copy != NULL &&
            copy->type_text != NULL &&
            strcmp(copy->type_text, "discord") == 0 &&
            integration_copy != NULL &&
            integration_copy->type_text != NULL &&
            strcmp(integration_copy->type_text, "discord") == 0 &&
            integration_copy->name != NULL &&
            strcmp(integration_copy->name, "builds") == 0) {
            state->integration_create_seen = 1;
        }
        dcc_integration_free(integration_copy);
        dcc_gateway_event_data_free(copy);
    } else if (data != NULL &&
        integration != NULL &&
        type == DCC_EVENT_INTEGRATION_UPDATE &&
        data->id == 321 &&
        data->guild_id == 333 &&
        data->role_id == 555 &&
        data->name != NULL &&
        strcmp(data->name, "builds") == 0 &&
        integration->id == 321 &&
        integration->guild_id == 333 &&
        integration->role_id == 555 &&
        integration->name != NULL &&
        strcmp(integration->name, "builds") == 0) {
        dcc_integration_t *copy = dcc_integration_clone(integration);
        if (copy != NULL && copy->id == 321 && copy->role_id == 555) {
            state->integration_update_seen = 1;
        }
        dcc_integration_free(copy);
    } else if (data != NULL &&
        integration != NULL &&
        type == DCC_EVENT_INTEGRATION_DELETE &&
        data->id == 321 &&
        data->guild_id == 333 &&
        data->application_id == 666 &&
        integration->id == 321 &&
        integration->guild_id == 333 &&
        integration->application_id == 666) {
        dcc_integration_t *copy = dcc_integration_clone(integration);
        if (copy != NULL && copy->id == 321 && copy->application_id == 666) {
            state->integration_delete_seen = 1;
        }
        dcc_integration_free(copy);
    }
}

void on_application_command_permissions_update(
    dcc_client_t *client,
    const dcc_event_t *event,
    void *user_data
) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_application_command_permissions_update_t *update =
        dcc_event_application_command_permissions_update(event);
    if (update != NULL &&
        update->id == 777 &&
        update->application_id == 666 &&
        update->guild_id == 333 &&
        update->permissions != NULL &&
        update->permissions_count == 2 &&
        update->permissions[0].id == 555 &&
        update->permissions[0].type == 1 &&
        update->permissions[0].permission == 1 &&
        update->permissions[1].id == 446 &&
        update->permissions[1].type == 2 &&
        update->permissions[1].permission == 0) {
        dcc_application_command_permissions_update_t *copy =
            dcc_application_command_permissions_update_clone(update);
        dcc_application_command_permission_t *permission_copy =
            dcc_application_command_permission_clone(&update->permissions[0]);
        if (copy != NULL &&
            copy->id == 777 &&
            copy->application_id == 666 &&
            copy->guild_id == 333 &&
            copy->permissions != NULL &&
            copy->permissions_count == 2 &&
            copy->permissions[0].id == 555 &&
            copy->permissions[1].permission == 0 &&
            permission_copy != NULL &&
            permission_copy->id == 555 &&
            permission_copy->type == 1 &&
            permission_copy->permission == 1) {
            state->application_command_permissions_update_seen = 1;
        }
        dcc_application_command_permission_free(permission_copy);
        dcc_application_command_permissions_update_free(copy);
    }
}

void on_guild_audit_log_entry_create(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_guild_audit_log_entry_t *entry = dcc_event_guild_audit_log_entry(event);
    if (entry != NULL &&
        entry->id == 901 &&
        entry->guild_id == 333 &&
        entry->user_id == 446 &&
        entry->target_id == 111 &&
        entry->action_type == 1) {
        dcc_guild_audit_log_entry_t *copy = dcc_guild_audit_log_entry_clone(entry);
        if (copy != NULL && copy->id == 901 && copy->target_id == 111 && copy->action_type == 1) {
            state->audit_log_entry_seen = 1;
        }
        dcc_guild_audit_log_entry_free(copy);
    }
}

void on_guild_ban(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_guild_ban_t *ban = dcc_event_guild_ban(event);
    if (ban != NULL &&
        ban->guild_id == 333 &&
        ban->user.id == 446 &&
        ban->user.username != NULL &&
        strcmp(ban->user.username, "member") == 0) {
        dcc_guild_ban_t *copy = dcc_guild_ban_clone(ban);
        if (copy != NULL &&
            copy->user.username != NULL &&
            strcmp(copy->user.username, "member") == 0) {
            if (dcc_event_type(event) == DCC_EVENT_GUILD_BAN_ADD) {
                state->guild_ban_add_seen = 1;
            } else {
                state->guild_ban_remove_seen = 1;
            }
        }
        dcc_guild_ban_free(copy);
    }
}

void on_guild_integrations_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_guild_update_notice_t *notice = dcc_event_guild_update_notice(event);
    if (notice != NULL && notice->guild_id == 333) {
        dcc_guild_update_notice_t *copy = dcc_guild_update_notice_clone(notice);
        if (copy != NULL && copy->guild_id == 333) {
            state->guild_integrations_update_seen = 1;
        }
        dcc_guild_update_notice_free(copy);
    }
}

void on_scheduled_event_user_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_scheduled_event_user_update_t *update = dcc_event_scheduled_event_user_update(event);
    if (update != NULL &&
        update->guild_id == 333 &&
        update->scheduled_event_id == 888 &&
        update->user_id == 446) {
        dcc_scheduled_event_user_update_t *copy = dcc_scheduled_event_user_update_clone(update);
        if (copy != NULL && copy->scheduled_event_id == 888 && copy->user_id == 446) {
            if (dcc_event_type(event) == DCC_EVENT_GUILD_SCHEDULED_EVENT_USER_ADD) {
                state->scheduled_event_user_add_seen = 1;
            } else {
                state->scheduled_event_user_remove_seen = 1;
            }
        }
        dcc_scheduled_event_user_update_free(copy);
    }
}

void on_webhooks_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_webhooks_update_t *update = dcc_event_webhooks_update(event);
    if (update != NULL && update->guild_id == 333 && update->channel_id == 222) {
        dcc_webhooks_update_t *copy = dcc_webhooks_update_clone(update);
        if (copy != NULL && copy->guild_id == 333 && copy->channel_id == 222) {
            state->webhooks_update_seen = 1;
        }
        dcc_webhooks_update_free(copy);
    }
}

void on_guild_join_request_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_guild_join_request_update_t *update = dcc_event_guild_join_request_update(event);
    int status_ok = dcc_event_type(event) == DCC_EVENT_GUILD_JOIN_REQUEST_DELETE
        ? update != NULL && update->status == NULL
        : update != NULL && update->status != NULL && strcmp(update->status, "pending") == 0;
    if (update != NULL && update->guild_id == 333 && update->user_id == 446 && status_ok) {
        dcc_guild_join_request_update_t *copy = dcc_guild_join_request_update_clone(update);
        int copy_ok = dcc_event_type(event) == DCC_EVENT_GUILD_JOIN_REQUEST_DELETE
            ? copy != NULL && copy->status == NULL
            : copy != NULL && copy->status != NULL && strcmp(copy->status, "pending") == 0;
        if (copy_ok && copy->guild_id == 333 && copy->user_id == 446) {
            if (dcc_event_type(event) == DCC_EVENT_GUILD_JOIN_REQUEST_DELETE) {
                state->guild_join_request_delete_seen = 1;
            } else {
                state->guild_join_request_update_seen = 1;
            }
        }
        dcc_guild_join_request_update_free(copy);
    }
}

void on_guild_application_command_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_guild_application_command_update_t *update = dcc_event_guild_application_command_update(event);
    if (update != NULL && update->guild_id == 333 && update->application_id == 666) {
        dcc_guild_application_command_update_t *copy = dcc_guild_application_command_update_clone(update);
        if (copy != NULL && copy->guild_id == 333 && copy->application_id == 666) {
            if (dcc_event_type(event) == DCC_EVENT_GUILD_APPLICATION_COMMAND_COUNTS_UPDATE) {
                state->guild_application_command_counts_seen = 1;
            } else {
                state->guild_application_command_index_seen = 1;
            }
        }
        dcc_guild_application_command_update_free(copy);
    }
}

void on_embedded_activity_update(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_embedded_activity_update_t *update = dcc_event_embedded_activity_update(event);
    if (update != NULL &&
        update->guild_id == 333 &&
        update->channel_id == 222 &&
        update->application_id == 666) {
        dcc_embedded_activity_update_t *copy = dcc_embedded_activity_update_clone(update);
        if (copy != NULL && copy->channel_id == 222 && copy->application_id == 666) {
            state->embedded_activity_update_seen = 1;
        }
        dcc_embedded_activity_update_free(copy);
    }
}

void on_log(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_log_event_t *log = dcc_event_log(event);
    if (log != NULL &&
        dcc_event_type(event) == DCC_EVENT_LOG &&
        dcc_event_data(event) == NULL &&
        log->message != NULL &&
        log->message[0] != '\0') {
        state->log_seen = 1;
        snprintf(state->last_log, sizeof(state->last_log), "%s", log->message);
    }
}

void on_socket_close(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_socket_close_event_t *close_event = dcc_event_socket_close(event);
    if (close_event != NULL &&
        dcc_event_type(event) == DCC_EVENT_SOCKET_CLOSE &&
        dcc_event_data(event) == NULL &&
        close_event->fd >= 0 &&
        close_event->code == 4000 &&
        close_event->reason != NULL &&
        strcmp(close_event->reason, "gateway smoke close") == 0) {
        state->socket_close_seen = 1;
        (void)dcc_client_stop(client);
    }
}
