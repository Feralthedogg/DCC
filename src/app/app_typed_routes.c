#include "internal/app/dcc_app_internal.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

typedef enum dcc_app_typed_route_kind {
    DCC_APP_TYPED_SLASH_ROUTE = 1,
    DCC_APP_TYPED_AUTOCOMPLETE_ROUTE,
    DCC_APP_TYPED_MODAL_ROUTE,
    DCC_APP_TYPED_COMPONENT_ROUTE
} dcc_app_typed_route_kind_t;

typedef struct dcc_app_typed_route_state {
    dcc_app_typed_route_kind_t kind;
    size_t args_size;
    dcc_ctx_option_field_binding_t *option_bindings;
    size_t option_binding_count;
    dcc_ctx_form_field_binding_t *form_bindings;
    size_t form_binding_count;
    dcc_ctx_component_field_binding_t *component_bindings;
    size_t component_binding_count;
    dcc_ctx_field_validator_t *validators;
    size_t validator_count;
    dcc_app_typed_handler_fn handler;
    void *user_data;
    uint8_t suppress_validation_response;
} dcc_app_typed_route_state_t;

static uint8_t dcc_app_typed_slash_has_field(
    const dcc_app_typed_slash_command_t *command,
    size_t offset,
    size_t field_size
) {
    return command != NULL &&
           command->size >= offset &&
           field_size <= command->size - offset;
}

static uint8_t dcc_app_typed_modal_has_field(
    const dcc_app_typed_modal_t *modal,
    size_t offset,
    size_t field_size
) {
    return modal != NULL &&
           modal->size >= offset &&
           field_size <= modal->size - offset;
}

static uint8_t dcc_app_typed_subcommand_has_field(
    const dcc_app_typed_subcommand_t *subcommand,
    size_t offset,
    size_t field_size
) {
    return subcommand != NULL &&
           subcommand->size >= offset &&
           field_size <= subcommand->size - offset;
}

static uint8_t dcc_app_typed_autocomplete_has_field(
    const dcc_app_typed_autocomplete_t *autocomplete,
    size_t offset,
    size_t field_size
) {
    return autocomplete != NULL &&
           autocomplete->size >= offset &&
           field_size <= autocomplete->size - offset;
}

static uint8_t dcc_app_typed_component_has_field(
    const dcc_app_typed_component_t *component,
    size_t offset,
    size_t field_size
) {
    return component != NULL &&
           component->size >= offset &&
           field_size <= component->size - offset;
}

static uint8_t dcc_app_modal_flow_has_core(const dcc_app_modal_flow_t *flow) {
    return flow != NULL &&
           flow->size >= offsetof(dcc_app_modal_flow_t, route) &&
           sizeof(flow->route) <= flow->size - offsetof(dcc_app_modal_flow_t, route);
}

static dcc_status_t dcc_app_modal_flow_validate(const dcc_app_modal_flow_t *flow) {
    if (!dcc_app_modal_flow_has_core(flow) ||
        !flow->modal.has_custom_id ||
        flow->modal.custom_id == NULL ||
        flow->modal.custom_id[0] == '\0' ||
        flow->route.custom_id == NULL ||
        flow->route.prefix != 0U ||
        strcmp(flow->modal.custom_id, flow->route.custom_id) != 0) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

static void dcc_app_typed_route_state_free(void *user_data) {
    dcc_app_typed_route_state_t *state = (dcc_app_typed_route_state_t *)user_data;
    if (state == NULL) {
        return;
    }
    free(state->option_bindings);
    free(state->form_bindings);
    free(state->component_bindings);
    free(state->validators);
    free(state);
}

static dcc_status_t dcc_app_copy_option_field_bindings(
    dcc_app_typed_route_state_t *state,
    dcc_ctx_option_field_binding_list_t bindings
) {
    if (bindings.count == 0U) {
        return DCC_OK;
    }
    if (bindings.items == NULL || bindings.count > SIZE_MAX / sizeof(*state->option_bindings)) {
        return DCC_ERR_INVALID_ARG;
    }
    state->option_bindings =
        (dcc_ctx_option_field_binding_t *)malloc(bindings.count * sizeof(*state->option_bindings));
    if (state->option_bindings == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(
        state->option_bindings,
        bindings.items,
        bindings.count * sizeof(*state->option_bindings)
    );
    state->option_binding_count = bindings.count;
    return DCC_OK;
}

static dcc_status_t dcc_app_copy_form_field_bindings(
    dcc_app_typed_route_state_t *state,
    dcc_ctx_form_field_binding_list_t bindings
) {
    if (bindings.count == 0U) {
        return DCC_OK;
    }
    if (bindings.items == NULL || bindings.count > SIZE_MAX / sizeof(*state->form_bindings)) {
        return DCC_ERR_INVALID_ARG;
    }
    state->form_bindings =
        (dcc_ctx_form_field_binding_t *)malloc(bindings.count * sizeof(*state->form_bindings));
    if (state->form_bindings == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(
        state->form_bindings,
        bindings.items,
        bindings.count * sizeof(*state->form_bindings)
    );
    state->form_binding_count = bindings.count;
    return DCC_OK;
}

static dcc_status_t dcc_app_copy_component_field_bindings(
    dcc_app_typed_route_state_t *state,
    dcc_ctx_component_field_binding_list_t bindings
) {
    if (bindings.count == 0U) {
        return DCC_OK;
    }
    if (bindings.items == NULL || bindings.count > SIZE_MAX / sizeof(*state->component_bindings)) {
        return DCC_ERR_INVALID_ARG;
    }
    state->component_bindings =
        (dcc_ctx_component_field_binding_t *)malloc(
            bindings.count * sizeof(*state->component_bindings)
        );
    if (state->component_bindings == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(
        state->component_bindings,
        bindings.items,
        bindings.count * sizeof(*state->component_bindings)
    );
    state->component_binding_count = bindings.count;
    return DCC_OK;
}

static dcc_status_t dcc_app_copy_field_validators(
    dcc_app_typed_route_state_t *state,
    dcc_ctx_field_validator_list_t validators
) {
    if (validators.count == 0U) {
        return DCC_OK;
    }
    if (validators.items == NULL || validators.count > SIZE_MAX / sizeof(*state->validators)) {
        return DCC_ERR_INVALID_ARG;
    }
    state->validators =
        (dcc_ctx_field_validator_t *)malloc(validators.count * sizeof(*state->validators));
    if (state->validators == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(state->validators, validators.items, validators.count * sizeof(*state->validators));
    state->validator_count = validators.count;
    return DCC_OK;
}

static dcc_status_t dcc_app_typed_bind_args(
    dcc_ctx_t *ctx,
    void *args,
    const dcc_app_typed_route_state_t *state
) {
    if (state->kind == DCC_APP_TYPED_SLASH_ROUTE ||
        state->kind == DCC_APP_TYPED_AUTOCOMPLETE_ROUTE) {
        return dcc_ctx_bind_option_fields(
            ctx,
            args,
            state->option_bindings,
            state->option_binding_count
        );
    }
    if (state->kind == DCC_APP_TYPED_MODAL_ROUTE) {
        return dcc_ctx_bind_form_fields(
            ctx,
            args,
            state->form_bindings,
            state->form_binding_count
        );
    }
    return dcc_ctx_bind_component_fields(
        ctx,
        args,
        state->component_bindings,
        state->component_binding_count
    );
}

static void dcc_app_typed_validation_reply(
    dcc_ctx_t *ctx,
    dcc_status_t status,
    const dcc_ctx_validation_error_t *error,
    const dcc_app_typed_route_state_t *state
) {
    if (state->suppress_validation_response) {
        return;
    }
    if (state->kind == DCC_APP_TYPED_AUTOCOMPLETE_ROUTE) {
        dcc_autocomplete_builder_t autocomplete;
        dcc_autocomplete_builder_init(&autocomplete);
        (void)dcc_ctx_reply_autocomplete(ctx, &autocomplete, NULL, NULL);
        return;
    }
    if (error != NULL && error->status != DCC_OK) {
        (void)dcc_ctx_reply_validation_result(ctx, status, error, NULL, NULL);
    } else {
        (void)dcc_ctx_reply_validation_error(ctx, status, NULL, NULL);
    }
}

static void dcc_app_typed_route_handler(dcc_ctx_t *ctx, void *user_data) {
    dcc_app_typed_route_state_t *state = (dcc_app_typed_route_state_t *)user_data;
    if (ctx == NULL || state == NULL || state->handler == NULL || state->args_size == 0U) {
        return;
    }

    void *args = calloc(1U, state->args_size);
    if (args == NULL) {
        if (!state->suppress_validation_response) {
            (void)dcc_ctx_reply_error(
                ctx,
                "Action failed",
                "Could not allocate request state.",
                NULL,
                NULL
            );
        }
        return;
    }

    dcc_status_t status = dcc_app_typed_bind_args(ctx, args, state);
    if (status != DCC_OK) {
        dcc_app_typed_validation_reply(ctx, status, NULL, state);
        free(args);
        return;
    }

    dcc_ctx_validation_error_t error;
    status = dcc_ctx_validate_fields(
        args,
        state->validators,
        state->validator_count,
        &error
    );
    if (status != DCC_OK) {
        dcc_app_typed_validation_reply(ctx, status, &error, state);
        free(args);
        return;
    }

    void *wrapper_user_data = ctx->user_data;
    ctx->user_data = state->user_data;
    state->handler(ctx, args, state->user_data);
    ctx->user_data = wrapper_user_data;
    free(args);
}

dcc_status_t dcc_app_slash_typed(
    dcc_app_t *app,
    const dcc_app_typed_slash_command_t *command
) {
    if (app == NULL || command == NULL ||
        !dcc_app_typed_slash_has_field(command, offsetof(dcc_app_typed_slash_command_t, handler), sizeof(command->handler)) ||
        command->command == NULL ||
        command->command->name == NULL ||
        command->command->name[0] == '\0' ||
        command->args_size == 0U ||
        command->handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_app_typed_route_state_t *state = (dcc_app_typed_route_state_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->kind = DCC_APP_TYPED_SLASH_ROUTE;
    state->args_size = command->args_size;
    state->handler = command->handler;
    if (dcc_app_typed_slash_has_field(command, offsetof(dcc_app_typed_slash_command_t, user_data), sizeof(command->user_data))) {
        state->user_data = command->user_data;
    }
    if (dcc_app_typed_slash_has_field(
            command,
            offsetof(dcc_app_typed_slash_command_t, suppress_validation_response),
            sizeof(command->suppress_validation_response)
        )) {
        state->suppress_validation_response = command->suppress_validation_response != 0U;
    }

    dcc_status_t status = dcc_app_copy_option_field_bindings(state, command->bindings);
    if (status == DCC_OK) {
        status = dcc_app_copy_field_validators(state, command->validators);
    }
    if (status == DCC_OK) {
        status = dcc_command_registry_add_builder(&app->registry, command->command);
    }
    dcc_app_route_id_t *out_route = dcc_app_typed_slash_has_field(
        command,
        offsetof(dcc_app_typed_slash_command_t, out_route),
        sizeof(command->out_route)
    ) && command->out_route != NULL ? command->out_route : NULL;
    dcc_app_route_id_t local_route = DCC_APP_ROUTE_INVALID;
    if (out_route == NULL) {
        out_route = &local_route;
    }
    uint8_t route_owns_state = 0U;
    if (status == DCC_OK) {
        status = dcc_app_add_route_with_cleanup(
            app,
            DCC_EVENT_SLASH_COMMAND,
            command->command->name,
            0U,
            dcc_app_typed_route_handler,
            state,
            dcc_app_typed_route_state_free,
            out_route
        );
        if (status == DCC_OK) {
            route_owns_state = 1U;
        }
    }
    if (status == DCC_OK &&
        dcc_app_typed_slash_has_field(
            command,
            offsetof(dcc_app_typed_slash_command_t, policy),
            sizeof(command->policy)
        )) {
        status = dcc_app_route_apply_policy_internal(app, *out_route, &command->policy);
    }
    if (status != DCC_OK && !route_owns_state) {
        dcc_app_typed_route_state_free(state);
    }
    return status;
}

dcc_status_t dcc_app_subcommand_typed(
    dcc_app_t *app,
    const dcc_app_typed_subcommand_t *subcommand
) {
    if (app == NULL || subcommand == NULL ||
        !dcc_app_typed_subcommand_has_field(
            subcommand,
            offsetof(dcc_app_typed_subcommand_t, handler),
            sizeof(subcommand->handler)
        ) ||
        subcommand->handler == NULL ||
        subcommand->args_size == 0U ||
        subcommand->subcommand_path == NULL ||
        subcommand->subcommand_path[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    const char *command_name = subcommand->command_name;
    if ((command_name == NULL || command_name[0] == '\0') &&
        subcommand->command != NULL) {
        command_name = subcommand->command->name;
    }
    if (command_name == NULL || command_name[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    if (subcommand->command != NULL) {
        if (!subcommand->command->has_name ||
            subcommand->command->name == NULL ||
            subcommand->command->name[0] == '\0' ||
            strcmp(subcommand->command->name, command_name) != 0) {
            return DCC_ERR_INVALID_ARG;
        }
    }

    char route_key[256];
    dcc_status_t status =
        dcc_app_subcommand_key(command_name, subcommand->subcommand_path, route_key, sizeof(route_key));
    if (status != DCC_OK) {
        return status;
    }

    dcc_app_typed_route_state_t *state =
        (dcc_app_typed_route_state_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->kind = DCC_APP_TYPED_SLASH_ROUTE;
    state->args_size = subcommand->args_size;
    state->handler = subcommand->handler;
    if (dcc_app_typed_subcommand_has_field(
            subcommand,
            offsetof(dcc_app_typed_subcommand_t, user_data),
            sizeof(subcommand->user_data)
        )) {
        state->user_data = subcommand->user_data;
    }
    if (dcc_app_typed_subcommand_has_field(
            subcommand,
            offsetof(dcc_app_typed_subcommand_t, suppress_validation_response),
            sizeof(subcommand->suppress_validation_response)
        )) {
        state->suppress_validation_response = subcommand->suppress_validation_response != 0U;
    }

    status = dcc_app_copy_option_field_bindings(state, subcommand->bindings);
    if (status == DCC_OK) {
        status = dcc_app_copy_field_validators(state, subcommand->validators);
    }
    if (status == DCC_OK && subcommand->command != NULL) {
        status = dcc_command_registry_add_builder(&app->registry, subcommand->command);
        if (status == DCC_ERR_STATE) {
            status = DCC_OK;
        }
    }
    dcc_app_route_id_t *out_route = dcc_app_typed_subcommand_has_field(
        subcommand,
        offsetof(dcc_app_typed_subcommand_t, out_route),
        sizeof(subcommand->out_route)
    ) && subcommand->out_route != NULL ? subcommand->out_route : NULL;
    dcc_app_route_id_t local_route = DCC_APP_ROUTE_INVALID;
    if (out_route == NULL) {
        out_route = &local_route;
    }
    uint8_t route_owns_state = 0U;
    if (status == DCC_OK) {
        status = dcc_app_add_route_with_cleanup(
            app,
            DCC_EVENT_SLASH_COMMAND,
            route_key,
            0U,
            dcc_app_typed_route_handler,
            state,
            dcc_app_typed_route_state_free,
            out_route
        );
        if (status == DCC_OK) {
            route_owns_state = 1U;
        }
    }
    if (status == DCC_OK &&
        dcc_app_typed_subcommand_has_field(
            subcommand,
            offsetof(dcc_app_typed_subcommand_t, policy),
            sizeof(subcommand->policy)
        )) {
        status = dcc_app_route_apply_policy_internal(app, *out_route, &subcommand->policy);
    }
    if (status != DCC_OK && !route_owns_state) {
        dcc_app_typed_route_state_free(state);
    }
    return status;
}

dcc_status_t dcc_app_autocomplete_typed(
    dcc_app_t *app,
    const dcc_app_typed_autocomplete_t *autocomplete
) {
    if (app == NULL || autocomplete == NULL ||
        !dcc_app_typed_autocomplete_has_field(
            autocomplete,
            offsetof(dcc_app_typed_autocomplete_t, handler),
            sizeof(autocomplete->handler)
        ) ||
        autocomplete->handler == NULL ||
        autocomplete->args_size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    const char *command_name = autocomplete->command_name;
    if ((command_name == NULL || command_name[0] == '\0') &&
        autocomplete->command != NULL) {
        command_name = autocomplete->command->name;
    }
    if (command_name == NULL || command_name[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    if (autocomplete->command != NULL) {
        if (!autocomplete->command->has_name ||
            autocomplete->command->name == NULL ||
            autocomplete->command->name[0] == '\0' ||
            strcmp(autocomplete->command->name, command_name) != 0) {
            return DCC_ERR_INVALID_ARG;
        }
    }

    char route_key[256];
    const char *route_name = command_name;
    if (autocomplete->subcommand_path != NULL &&
        autocomplete->subcommand_path[0] != '\0') {
        dcc_status_t key_status = dcc_app_subcommand_key(
            command_name,
            autocomplete->subcommand_path,
            route_key,
            sizeof(route_key)
        );
        if (key_status != DCC_OK) {
            return key_status;
        }
        route_name = route_key;
    }

    dcc_app_typed_route_state_t *state =
        (dcc_app_typed_route_state_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->kind = DCC_APP_TYPED_AUTOCOMPLETE_ROUTE;
    state->args_size = autocomplete->args_size;
    state->handler = autocomplete->handler;
    if (dcc_app_typed_autocomplete_has_field(
            autocomplete,
            offsetof(dcc_app_typed_autocomplete_t, user_data),
            sizeof(autocomplete->user_data)
        )) {
        state->user_data = autocomplete->user_data;
    }
    if (dcc_app_typed_autocomplete_has_field(
            autocomplete,
            offsetof(dcc_app_typed_autocomplete_t, suppress_validation_response),
            sizeof(autocomplete->suppress_validation_response)
        )) {
        state->suppress_validation_response =
            autocomplete->suppress_validation_response != 0U;
    }

    dcc_status_t status = dcc_app_copy_option_field_bindings(state, autocomplete->bindings);
    if (status == DCC_OK) {
        status = dcc_app_copy_field_validators(state, autocomplete->validators);
    }
    if (status == DCC_OK && autocomplete->command != NULL) {
        status = dcc_command_registry_add_builder(&app->registry, autocomplete->command);
        if (status == DCC_ERR_STATE) {
            status = DCC_OK;
        }
    }
    dcc_app_route_id_t *out_route = dcc_app_typed_autocomplete_has_field(
        autocomplete,
        offsetof(dcc_app_typed_autocomplete_t, out_route),
        sizeof(autocomplete->out_route)
    ) && autocomplete->out_route != NULL ? autocomplete->out_route : NULL;
    dcc_app_route_id_t local_route = DCC_APP_ROUTE_INVALID;
    if (out_route == NULL) {
        out_route = &local_route;
    }
    uint8_t route_owns_state = 0U;
    if (status == DCC_OK) {
        status = dcc_app_add_route_with_cleanup(
            app,
            DCC_EVENT_AUTOCOMPLETE,
            route_name,
            0U,
            dcc_app_typed_route_handler,
            state,
            dcc_app_typed_route_state_free,
            out_route
        );
        if (status == DCC_OK) {
            route_owns_state = 1U;
        }
    }
    if (status == DCC_OK &&
        dcc_app_typed_autocomplete_has_field(
            autocomplete,
            offsetof(dcc_app_typed_autocomplete_t, policy),
            sizeof(autocomplete->policy)
        )) {
        status = dcc_app_route_apply_policy_internal(app, *out_route, &autocomplete->policy);
    }
    if (status != DCC_OK && !route_owns_state) {
        dcc_app_typed_route_state_free(state);
    }
    return status;
}

dcc_status_t dcc_app_modal_typed(
    dcc_app_t *app,
    const dcc_app_typed_modal_t *modal
) {
    if (app == NULL || modal == NULL ||
        !dcc_app_typed_modal_has_field(modal, offsetof(dcc_app_typed_modal_t, handler), sizeof(modal->handler)) ||
        modal->custom_id == NULL ||
        modal->custom_id[0] == '\0' ||
        modal->args_size == 0U ||
        modal->handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_app_typed_route_state_t *state = (dcc_app_typed_route_state_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->kind = DCC_APP_TYPED_MODAL_ROUTE;
    state->args_size = modal->args_size;
    state->handler = modal->handler;
    if (dcc_app_typed_modal_has_field(modal, offsetof(dcc_app_typed_modal_t, user_data), sizeof(modal->user_data))) {
        state->user_data = modal->user_data;
    }
    if (dcc_app_typed_modal_has_field(
            modal,
            offsetof(dcc_app_typed_modal_t, suppress_validation_response),
            sizeof(modal->suppress_validation_response)
        )) {
        state->suppress_validation_response = modal->suppress_validation_response != 0U;
    }

    dcc_status_t status = dcc_app_copy_form_field_bindings(state, modal->bindings);
    if (status == DCC_OK) {
        status = dcc_app_copy_field_validators(state, modal->validators);
    }
    dcc_app_route_id_t *out_route = dcc_app_typed_modal_has_field(
        modal,
        offsetof(dcc_app_typed_modal_t, out_route),
        sizeof(modal->out_route)
    ) && modal->out_route != NULL ? modal->out_route : NULL;
    dcc_app_route_id_t local_route = DCC_APP_ROUTE_INVALID;
    if (out_route == NULL) {
        out_route = &local_route;
    }
    uint8_t prefix = dcc_app_typed_modal_has_field(
        modal,
        offsetof(dcc_app_typed_modal_t, prefix),
        sizeof(modal->prefix)
    ) ? modal->prefix : 0U;
    uint8_t route_owns_state = 0U;
    if (status == DCC_OK) {
        status = dcc_app_add_route_with_cleanup(
            app,
            DCC_EVENT_FORM_SUBMIT,
            modal->custom_id,
            prefix,
            dcc_app_typed_route_handler,
            state,
            dcc_app_typed_route_state_free,
            out_route
        );
        if (status == DCC_OK) {
            route_owns_state = 1U;
        }
    }
    if (status == DCC_OK &&
        dcc_app_typed_modal_has_field(
            modal,
            offsetof(dcc_app_typed_modal_t, policy),
            sizeof(modal->policy)
        )) {
        status = dcc_app_route_apply_policy_internal(app, *out_route, &modal->policy);
    }
    if (status != DCC_OK && !route_owns_state) {
        dcc_app_typed_route_state_free(state);
    }
    return status;
}

dcc_status_t dcc_app_modal_flow_register(
    dcc_app_t *app,
    const dcc_app_modal_flow_t *flow
) {
    dcc_status_t status = dcc_app_modal_flow_validate(flow);
    return status == DCC_OK ? dcc_app_modal_typed(app, &flow->route) : status;
}

static dcc_status_t dcc_app_component_typed_route(
    dcc_app_t *app,
    const dcc_app_typed_component_t *component,
    dcc_event_type_t type
) {
    if (app == NULL || component == NULL ||
        !dcc_app_typed_component_has_field(component, offsetof(dcc_app_typed_component_t, handler), sizeof(component->handler)) ||
        component->custom_id == NULL ||
        component->custom_id[0] == '\0' ||
        component->args_size == 0U ||
        component->handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_app_typed_route_state_t *state = (dcc_app_typed_route_state_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->kind = DCC_APP_TYPED_COMPONENT_ROUTE;
    state->args_size = component->args_size;
    state->handler = component->handler;
    if (dcc_app_typed_component_has_field(component, offsetof(dcc_app_typed_component_t, user_data), sizeof(component->user_data))) {
        state->user_data = component->user_data;
    }
    if (dcc_app_typed_component_has_field(
            component,
            offsetof(dcc_app_typed_component_t, suppress_validation_response),
            sizeof(component->suppress_validation_response)
        )) {
        state->suppress_validation_response = component->suppress_validation_response != 0U;
    }

    dcc_status_t status = dcc_app_copy_component_field_bindings(state, component->bindings);
    if (status == DCC_OK) {
        status = dcc_app_copy_field_validators(state, component->validators);
    }
    dcc_app_route_id_t *out_route = dcc_app_typed_component_has_field(
        component,
        offsetof(dcc_app_typed_component_t, out_route),
        sizeof(component->out_route)
    ) && component->out_route != NULL ? component->out_route : NULL;
    dcc_app_route_id_t local_route = DCC_APP_ROUTE_INVALID;
    if (out_route == NULL) {
        out_route = &local_route;
    }
    uint8_t prefix = dcc_app_typed_component_has_field(
        component,
        offsetof(dcc_app_typed_component_t, prefix),
        sizeof(component->prefix)
    ) ? component->prefix : 0U;
    uint8_t route_owns_state = 0U;
    if (status == DCC_OK) {
        status = dcc_app_add_route_with_cleanup(
            app,
            type,
            component->custom_id,
            prefix,
            dcc_app_typed_route_handler,
            state,
            dcc_app_typed_route_state_free,
            out_route
        );
        if (status == DCC_OK) {
            route_owns_state = 1U;
        }
    }
    if (status == DCC_OK &&
        dcc_app_typed_component_has_field(
            component,
            offsetof(dcc_app_typed_component_t, policy),
            sizeof(component->policy)
        )) {
        status = dcc_app_route_apply_policy_internal(app, *out_route, &component->policy);
    }
    if (status != DCC_OK && !route_owns_state) {
        dcc_app_typed_route_state_free(state);
    }
    return status;
}

dcc_status_t dcc_app_button_typed(
    dcc_app_t *app,
    const dcc_app_typed_component_t *component
) {
    return dcc_app_component_typed_route(app, component, DCC_EVENT_BUTTON_CLICK);
}

dcc_status_t dcc_app_select_typed(
    dcc_app_t *app,
    const dcc_app_typed_component_t *component
) {
    return dcc_app_component_typed_route(app, component, DCC_EVENT_SELECT_CLICK);
}
