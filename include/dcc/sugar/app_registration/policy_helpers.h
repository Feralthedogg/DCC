#ifndef DCC_SUGAR_APP_REGISTRATION_POLICY_HELPERS_H
#define DCC_SUGAR_APP_REGISTRATION_POLICY_HELPERS_H

#include <dcc/sugar/app_registration/base.h>

static inline dcc_status_t dcc_sugar_app_apply_route_policy(
    dcc_app_t *app,
    dcc_status_t route_status,
    dcc_app_route_id_t route_id,
    const dcc_app_route_policy_t *policy
) {
    return route_status == DCC_OK
        ? dcc_app_route_apply_policy(app, route_id, policy)
        : route_status;
}

static inline dcc_status_t dcc_sugar_app_on_slash_policy(
    dcc_app_t *app,
    const char *name,
    const char *description,
    dcc_app_handler_fn handler,
    void *user_data,
    const dcc_app_route_policy_t *policy
) {
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_status_t status =
        dcc_app_slash_route(app, name, description, handler, user_data, &route_id);
    return dcc_sugar_app_apply_route_policy(app, status, route_id, policy);
}

static inline dcc_status_t dcc_sugar_app_on_slash_builder_policy(
    dcc_app_t *app,
    const dcc_application_command_builder_t *builder,
    dcc_app_handler_fn handler,
    void *user_data,
    const dcc_app_route_policy_t *policy
) {
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_status_t status =
        dcc_app_slash_builder_route(app, builder, handler, user_data, &route_id);
    return dcc_sugar_app_apply_route_policy(app, status, route_id, policy);
}

static inline dcc_status_t dcc_sugar_app_on_subcommand_policy(
    dcc_app_t *app,
    const char *command_name,
    const char *subcommand_path,
    dcc_app_handler_fn handler,
    void *user_data,
    const dcc_app_route_policy_t *policy
) {
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_status_t status = dcc_app_subcommand_route(
        app,
        command_name,
        subcommand_path,
        handler,
        user_data,
        &route_id
    );
    return dcc_sugar_app_apply_route_policy(app, status, route_id, policy);
}

static inline dcc_status_t dcc_sugar_app_on_autocomplete_policy(
    dcc_app_t *app,
    const char *name,
    dcc_app_handler_fn handler,
    void *user_data,
    const dcc_app_route_policy_t *policy
) {
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_status_t status =
        dcc_app_autocomplete_route(app, name, handler, user_data, &route_id);
    return dcc_sugar_app_apply_route_policy(app, status, route_id, policy);
}

static inline dcc_status_t dcc_sugar_app_on_user_context_menu_policy(
    dcc_app_t *app,
    const char *name,
    dcc_app_handler_fn handler,
    void *user_data,
    const dcc_app_route_policy_t *policy
) {
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_status_t status =
        dcc_app_user_context_menu_route(app, name, handler, user_data, &route_id);
    return dcc_sugar_app_apply_route_policy(app, status, route_id, policy);
}

static inline dcc_status_t dcc_sugar_app_on_message_context_menu_policy(
    dcc_app_t *app,
    const char *name,
    dcc_app_handler_fn handler,
    void *user_data,
    const dcc_app_route_policy_t *policy
) {
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_status_t status =
        dcc_app_message_context_menu_route(app, name, handler, user_data, &route_id);
    return dcc_sugar_app_apply_route_policy(app, status, route_id, policy);
}

static inline dcc_status_t dcc_sugar_app_on_button_policy(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_handler_fn handler,
    void *user_data,
    const dcc_app_route_policy_t *policy
) {
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_status_t status = dcc_app_button_route(app, custom_id, handler, user_data, &route_id);
    return dcc_sugar_app_apply_route_policy(app, status, route_id, policy);
}

static inline dcc_status_t dcc_sugar_app_on_button_prefix_policy(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_handler_fn handler,
    void *user_data,
    const dcc_app_route_policy_t *policy
) {
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_status_t status =
        dcc_app_button_prefix_route(app, custom_id_prefix, handler, user_data, &route_id);
    return dcc_sugar_app_apply_route_policy(app, status, route_id, policy);
}

static inline dcc_status_t dcc_sugar_app_on_select_policy(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_handler_fn handler,
    void *user_data,
    const dcc_app_route_policy_t *policy
) {
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_status_t status = dcc_app_select_route(app, custom_id, handler, user_data, &route_id);
    return dcc_sugar_app_apply_route_policy(app, status, route_id, policy);
}

static inline dcc_status_t dcc_sugar_app_on_select_prefix_policy(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_handler_fn handler,
    void *user_data,
    const dcc_app_route_policy_t *policy
) {
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_status_t status =
        dcc_app_select_prefix_route(app, custom_id_prefix, handler, user_data, &route_id);
    return dcc_sugar_app_apply_route_policy(app, status, route_id, policy);
}

static inline dcc_status_t dcc_sugar_app_on_modal_policy(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_handler_fn handler,
    void *user_data,
    const dcc_app_route_policy_t *policy
) {
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_status_t status = dcc_app_modal_route(app, custom_id, handler, user_data, &route_id);
    return dcc_sugar_app_apply_route_policy(app, status, route_id, policy);
}

static inline dcc_status_t dcc_sugar_app_on_modal_prefix_policy(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_handler_fn handler,
    void *user_data,
    const dcc_app_route_policy_t *policy
) {
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_status_t status =
        dcc_app_modal_prefix_route(app, custom_id_prefix, handler, user_data, &route_id);
    return dcc_sugar_app_apply_route_policy(app, status, route_id, policy);
}

#endif /* DCC_SUGAR_APP_REGISTRATION_POLICY_HELPERS_H */
