#ifndef DCC_SUGAR_APP_REGISTRATION_H
#define DCC_SUGAR_APP_REGISTRATION_H

#include <dcc/app.h>
#include <dcc/sugar/route_policy.h>

#ifndef DCC_COMPONENT_ID
#define DCC_COMPONENT_ID(namespace_, action_) namespace_ "." action_
#endif
#ifndef DCC_COMPONENT_PREFIX
#define DCC_COMPONENT_PREFIX(namespace_) namespace_ "."
#endif
#ifndef DCC_COMPONENT_ACTION_PREFIX
#define DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_) namespace_ "." action_prefix_
#endif

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

#define DCC_APP_ON_SLASH_POLICY(app_, name_, description_, handler_, policy_) \
    dcc_sugar_app_on_slash_policy( \
        (app_), \
        (name_), \
        (description_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SLASH_DATA_POLICY(app_, name_, description_, handler_, user_data_, policy_) \
    dcc_sugar_app_on_slash_policy( \
        (app_), \
        (name_), \
        (description_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SLASH_BUILDER_POLICY(app_, builder_, handler_, policy_) \
    dcc_sugar_app_on_slash_builder_policy( \
        (app_), \
        (builder_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SLASH_BUILDER_DATA_POLICY(app_, builder_, handler_, user_data_, policy_) \
    dcc_sugar_app_on_slash_builder_policy( \
        (app_), \
        (builder_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SUBCOMMAND_POLICY(app_, command_name_, subcommand_path_, handler_, policy_) \
    dcc_sugar_app_on_subcommand_policy( \
        (app_), \
        (command_name_), \
        (subcommand_path_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SUBCOMMAND_DATA_POLICY( \
    app_, \
    command_name_, \
    subcommand_path_, \
    handler_, \
    user_data_, \
    policy_ \
) \
    dcc_sugar_app_on_subcommand_policy( \
        (app_), \
        (command_name_), \
        (subcommand_path_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_AUTOCOMPLETE_POLICY(app_, name_, handler_, policy_) \
    dcc_sugar_app_on_autocomplete_policy( \
        (app_), \
        (name_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_AUTOCOMPLETE_DATA_POLICY(app_, name_, handler_, user_data_, policy_) \
    dcc_sugar_app_on_autocomplete_policy( \
        (app_), \
        (name_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_USER_CONTEXT_MENU_POLICY(app_, name_, handler_, policy_) \
    dcc_sugar_app_on_user_context_menu_policy( \
        (app_), \
        (name_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_USER_CONTEXT_MENU_DATA_POLICY(app_, name_, handler_, user_data_, policy_) \
    dcc_sugar_app_on_user_context_menu_policy( \
        (app_), \
        (name_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_POLICY(app_, name_, handler_, policy_) \
    dcc_sugar_app_on_message_context_menu_policy( \
        (app_), \
        (name_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_DATA_POLICY(app_, name_, handler_, user_data_, policy_) \
    dcc_sugar_app_on_message_context_menu_policy( \
        (app_), \
        (name_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_BUTTON_POLICY(app_, custom_id_, handler_, policy_) \
    dcc_sugar_app_on_button_policy( \
        (app_), \
        (custom_id_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_BUTTON_DATA_POLICY(app_, custom_id_, handler_, user_data_, policy_) \
    dcc_sugar_app_on_button_policy( \
        (app_), \
        (custom_id_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_BUTTON_PREFIX_POLICY(app_, custom_id_prefix_, handler_, policy_) \
    dcc_sugar_app_on_button_prefix_policy( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_BUTTON_PREFIX_DATA_POLICY( \
    app_, \
    custom_id_prefix_, \
    handler_, \
    user_data_, \
    policy_ \
) \
    dcc_sugar_app_on_button_prefix_policy( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SELECT_POLICY(app_, custom_id_, handler_, policy_) \
    dcc_sugar_app_on_select_policy( \
        (app_), \
        (custom_id_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SELECT_DATA_POLICY(app_, custom_id_, handler_, user_data_, policy_) \
    dcc_sugar_app_on_select_policy( \
        (app_), \
        (custom_id_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SELECT_PREFIX_POLICY(app_, custom_id_prefix_, handler_, policy_) \
    dcc_sugar_app_on_select_prefix_policy( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SELECT_PREFIX_DATA_POLICY( \
    app_, \
    custom_id_prefix_, \
    handler_, \
    user_data_, \
    policy_ \
) \
    dcc_sugar_app_on_select_prefix_policy( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_MODAL_POLICY(app_, custom_id_, handler_, policy_) \
    dcc_sugar_app_on_modal_policy( \
        (app_), \
        (custom_id_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_MODAL_DATA_POLICY(app_, custom_id_, handler_, user_data_, policy_) \
    dcc_sugar_app_on_modal_policy( \
        (app_), \
        (custom_id_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_MODAL_PREFIX_POLICY(app_, custom_id_prefix_, handler_, policy_) \
    dcc_sugar_app_on_modal_prefix_policy( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_MODAL_PREFIX_DATA_POLICY( \
    app_, \
    custom_id_prefix_, \
    handler_, \
    user_data_, \
    policy_ \
) \
    dcc_sugar_app_on_modal_prefix_policy( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )

#define DCC_APP_ON_BUTTON_NS_POLICY(app_, namespace_, action_, handler_, policy_) \
    DCC_APP_ON_BUTTON_POLICY((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (policy_))
#define DCC_APP_ON_BUTTON_NS_DATA_POLICY(app_, namespace_, action_, handler_, user_data_, policy_) \
    DCC_APP_ON_BUTTON_DATA_POLICY( \
        (app_), \
        DCC_COMPONENT_ID(namespace_, action_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )
#define DCC_APP_ON_BUTTON_NS_PREFIX_POLICY(app_, namespace_, action_prefix_, handler_, policy_) \
    DCC_APP_ON_BUTTON_PREFIX_POLICY( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (policy_) \
    )
#define DCC_APP_ON_BUTTON_NS_PREFIX_DATA_POLICY(app_, namespace_, action_prefix_, handler_, user_data_, policy_) \
    DCC_APP_ON_BUTTON_PREFIX_DATA_POLICY( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )

#define DCC_APP_ON_SELECT_NS_POLICY(app_, namespace_, action_, handler_, policy_) \
    DCC_APP_ON_SELECT_POLICY((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (policy_))
#define DCC_APP_ON_SELECT_NS_DATA_POLICY(app_, namespace_, action_, handler_, user_data_, policy_) \
    DCC_APP_ON_SELECT_DATA_POLICY( \
        (app_), \
        DCC_COMPONENT_ID(namespace_, action_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )
#define DCC_APP_ON_SELECT_NS_PREFIX_POLICY(app_, namespace_, action_prefix_, handler_, policy_) \
    DCC_APP_ON_SELECT_PREFIX_POLICY( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (policy_) \
    )
#define DCC_APP_ON_SELECT_NS_PREFIX_DATA_POLICY(app_, namespace_, action_prefix_, handler_, user_data_, policy_) \
    DCC_APP_ON_SELECT_PREFIX_DATA_POLICY( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )

#define DCC_APP_ON_MODAL_NS_POLICY(app_, namespace_, action_, handler_, policy_) \
    DCC_APP_ON_MODAL_POLICY((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (policy_))
#define DCC_APP_ON_MODAL_NS_DATA_POLICY(app_, namespace_, action_, handler_, user_data_, policy_) \
    DCC_APP_ON_MODAL_DATA_POLICY( \
        (app_), \
        DCC_COMPONENT_ID(namespace_, action_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )
#define DCC_APP_ON_MODAL_NS_PREFIX_POLICY(app_, namespace_, action_prefix_, handler_, policy_) \
    DCC_APP_ON_MODAL_PREFIX_POLICY( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (policy_) \
    )
#define DCC_APP_ON_MODAL_NS_PREFIX_DATA_POLICY(app_, namespace_, action_prefix_, handler_, user_data_, policy_) \
    DCC_APP_ON_MODAL_PREFIX_DATA_POLICY( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )

#define DCC_APP_ON_COMMAND_POLICY(app_, name_, description_, handler_, policy_) \
    DCC_APP_ON_SLASH_POLICY((app_), (name_), (description_), (handler_), (policy_))

#define DCC_APP_ON_COMMAND_DATA_POLICY(app_, name_, description_, handler_, user_data_, policy_) \
    DCC_APP_ON_SLASH_DATA_POLICY((app_), (name_), (description_), (handler_), (user_data_), (policy_))

#define DCC_APP_ON_COMMAND_BUILDER_POLICY(app_, builder_, handler_, policy_) \
    DCC_APP_ON_SLASH_BUILDER_POLICY((app_), (builder_), (handler_), (policy_))

#define DCC_APP_ON_COMMAND_BUILDER_DATA_POLICY(app_, builder_, handler_, user_data_, policy_) \
    DCC_APP_ON_SLASH_BUILDER_DATA_POLICY((app_), (builder_), (handler_), (user_data_), (policy_))

#define DCC_APP_ON_SLASH_GUARDED(app_, name_, description_, handler_, ...) \
    DCC_APP_ON_SLASH_POLICY((app_), (name_), (description_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_SLASH_DATA_GUARDED(app_, name_, description_, handler_, user_data_, ...) \
    DCC_APP_ON_SLASH_DATA_POLICY( \
        (app_), \
        (name_), \
        (description_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_SLASH_BUILDER_GUARDED(app_, builder_, handler_, ...) \
    DCC_APP_ON_SLASH_BUILDER_POLICY((app_), (builder_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_SLASH_BUILDER_DATA_GUARDED(app_, builder_, handler_, user_data_, ...) \
    DCC_APP_ON_SLASH_BUILDER_DATA_POLICY( \
        (app_), \
        (builder_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_COMMAND_GUARDED(app_, name_, description_, handler_, ...) \
    DCC_APP_ON_SLASH_GUARDED((app_), (name_), (description_), (handler_), __VA_ARGS__)

#define DCC_APP_ON_COMMAND_DATA_GUARDED(app_, name_, description_, handler_, user_data_, ...) \
    DCC_APP_ON_SLASH_DATA_GUARDED((app_), (name_), (description_), (handler_), (user_data_), __VA_ARGS__)

#define DCC_APP_ON_COMMAND_BUILDER_GUARDED(app_, builder_, handler_, ...) \
    DCC_APP_ON_SLASH_BUILDER_GUARDED((app_), (builder_), (handler_), __VA_ARGS__)

#define DCC_APP_ON_COMMAND_BUILDER_DATA_GUARDED(app_, builder_, handler_, user_data_, ...) \
    DCC_APP_ON_SLASH_BUILDER_DATA_GUARDED((app_), (builder_), (handler_), (user_data_), __VA_ARGS__)

#define DCC_APP_ON_SUBCOMMAND_GUARDED(app_, command_name_, subcommand_path_, handler_, ...) \
    DCC_APP_ON_SUBCOMMAND_POLICY( \
        (app_), \
        (command_name_), \
        (subcommand_path_), \
        (handler_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_SUBCOMMAND_DATA_GUARDED(app_, command_name_, subcommand_path_, handler_, user_data_, ...) \
    DCC_APP_ON_SUBCOMMAND_DATA_POLICY( \
        (app_), \
        (command_name_), \
        (subcommand_path_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_AUTOCOMPLETE_GUARDED(app_, name_, handler_, ...) \
    DCC_APP_ON_AUTOCOMPLETE_POLICY((app_), (name_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_AUTOCOMPLETE_DATA_GUARDED(app_, name_, handler_, user_data_, ...) \
    DCC_APP_ON_AUTOCOMPLETE_DATA_POLICY((app_), (name_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_USER_CONTEXT_MENU_GUARDED(app_, name_, handler_, ...) \
    DCC_APP_ON_USER_CONTEXT_MENU_POLICY((app_), (name_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_USER_CONTEXT_MENU_DATA_GUARDED(app_, name_, handler_, user_data_, ...) \
    DCC_APP_ON_USER_CONTEXT_MENU_DATA_POLICY((app_), (name_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_GUARDED(app_, name_, handler_, ...) \
    DCC_APP_ON_MESSAGE_CONTEXT_MENU_POLICY((app_), (name_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_DATA_GUARDED(app_, name_, handler_, user_data_, ...) \
    DCC_APP_ON_MESSAGE_CONTEXT_MENU_DATA_POLICY((app_), (name_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_BUTTON_GUARDED(app_, custom_id_, handler_, ...) \
    DCC_APP_ON_BUTTON_POLICY((app_), (custom_id_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_BUTTON_DATA_GUARDED(app_, custom_id_, handler_, user_data_, ...) \
    DCC_APP_ON_BUTTON_DATA_POLICY((app_), (custom_id_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_BUTTON_PREFIX_GUARDED(app_, custom_id_prefix_, handler_, ...) \
    DCC_APP_ON_BUTTON_PREFIX_POLICY((app_), (custom_id_prefix_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_BUTTON_PREFIX_DATA_GUARDED(app_, custom_id_prefix_, handler_, user_data_, ...) \
    DCC_APP_ON_BUTTON_PREFIX_DATA_POLICY( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_BUTTON_NS_GUARDED(app_, namespace_, action_, handler_, ...) \
    DCC_APP_ON_BUTTON_NS_POLICY((app_), namespace_, action_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_BUTTON_NS_DATA_GUARDED(app_, namespace_, action_, handler_, user_data_, ...) \
    DCC_APP_ON_BUTTON_NS_DATA_POLICY((app_), namespace_, action_, (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_BUTTON_NS_PREFIX_GUARDED(app_, namespace_, action_prefix_, handler_, ...) \
    DCC_APP_ON_BUTTON_NS_PREFIX_POLICY((app_), namespace_, action_prefix_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_BUTTON_NS_PREFIX_DATA_GUARDED(app_, namespace_, action_prefix_, handler_, user_data_, ...) \
    DCC_APP_ON_BUTTON_NS_PREFIX_DATA_POLICY( \
        (app_), \
        namespace_, \
        action_prefix_, \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_SELECT_GUARDED(app_, custom_id_, handler_, ...) \
    DCC_APP_ON_SELECT_POLICY((app_), (custom_id_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_SELECT_DATA_GUARDED(app_, custom_id_, handler_, user_data_, ...) \
    DCC_APP_ON_SELECT_DATA_POLICY((app_), (custom_id_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_SELECT_PREFIX_GUARDED(app_, custom_id_prefix_, handler_, ...) \
    DCC_APP_ON_SELECT_PREFIX_POLICY((app_), (custom_id_prefix_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_SELECT_PREFIX_DATA_GUARDED(app_, custom_id_prefix_, handler_, user_data_, ...) \
    DCC_APP_ON_SELECT_PREFIX_DATA_POLICY( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_SELECT_NS_GUARDED(app_, namespace_, action_, handler_, ...) \
    DCC_APP_ON_SELECT_NS_POLICY((app_), namespace_, action_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_SELECT_NS_DATA_GUARDED(app_, namespace_, action_, handler_, user_data_, ...) \
    DCC_APP_ON_SELECT_NS_DATA_POLICY((app_), namespace_, action_, (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_SELECT_NS_PREFIX_GUARDED(app_, namespace_, action_prefix_, handler_, ...) \
    DCC_APP_ON_SELECT_NS_PREFIX_POLICY((app_), namespace_, action_prefix_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_SELECT_NS_PREFIX_DATA_GUARDED(app_, namespace_, action_prefix_, handler_, user_data_, ...) \
    DCC_APP_ON_SELECT_NS_PREFIX_DATA_POLICY( \
        (app_), \
        namespace_, \
        action_prefix_, \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_MODAL_GUARDED(app_, custom_id_, handler_, ...) \
    DCC_APP_ON_MODAL_POLICY((app_), (custom_id_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_MODAL_DATA_GUARDED(app_, custom_id_, handler_, user_data_, ...) \
    DCC_APP_ON_MODAL_DATA_POLICY((app_), (custom_id_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_MODAL_PREFIX_GUARDED(app_, custom_id_prefix_, handler_, ...) \
    DCC_APP_ON_MODAL_PREFIX_POLICY((app_), (custom_id_prefix_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_MODAL_PREFIX_DATA_GUARDED(app_, custom_id_prefix_, handler_, user_data_, ...) \
    DCC_APP_ON_MODAL_PREFIX_DATA_POLICY( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_MODAL_NS_GUARDED(app_, namespace_, action_, handler_, ...) \
    DCC_APP_ON_MODAL_NS_POLICY((app_), namespace_, action_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_MODAL_NS_DATA_GUARDED(app_, namespace_, action_, handler_, user_data_, ...) \
    DCC_APP_ON_MODAL_NS_DATA_POLICY((app_), namespace_, action_, (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_MODAL_NS_PREFIX_GUARDED(app_, namespace_, action_prefix_, handler_, ...) \
    DCC_APP_ON_MODAL_NS_PREFIX_POLICY((app_), namespace_, action_prefix_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_MODAL_NS_PREFIX_DATA_GUARDED(app_, namespace_, action_prefix_, handler_, user_data_, ...) \
    DCC_APP_ON_MODAL_NS_PREFIX_DATA_POLICY( \
        (app_), \
        namespace_, \
        action_prefix_, \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_SLASH(app_, name_, description_, handler_) \
    dcc_app_slash((app_), (name_), (description_), (handler_), NULL)
#define DCC_APP_ON_SLASH_DATA(app_, name_, description_, handler_, user_data_) \
    dcc_app_slash((app_), (name_), (description_), (handler_), (user_data_))
#define DCC_APP_ON_SLASH_BUILDER(app_, builder_, handler_) \
    dcc_app_slash_builder((app_), (builder_), (handler_), NULL)
#define DCC_APP_ON_SLASH_BUILDER_DATA(app_, builder_, handler_, user_data_) \
    dcc_app_slash_builder((app_), (builder_), (handler_), (user_data_))
#define DCC_APP_ON_COMMAND(app_, name_, description_, handler_) \
    DCC_APP_ON_SLASH((app_), (name_), (description_), (handler_))
#define DCC_APP_ON_COMMAND_DATA(app_, name_, description_, handler_, user_data_) \
    DCC_APP_ON_SLASH_DATA((app_), (name_), (description_), (handler_), (user_data_))
#define DCC_APP_ON_COMMAND_BUILDER(app_, builder_, handler_) \
    DCC_APP_ON_SLASH_BUILDER((app_), (builder_), (handler_))
#define DCC_APP_ON_COMMAND_BUILDER_DATA(app_, builder_, handler_, user_data_) \
    DCC_APP_ON_SLASH_BUILDER_DATA((app_), (builder_), (handler_), (user_data_))
#define DCC_APP_ON_SLASH_FN(app_, handler_, description_) \
    DCC_APP_ON_SLASH((app_), #handler_, (description_), (handler_))
#define DCC_APP_ON_SLASH_FN_DATA(app_, handler_, description_, user_data_) \
    DCC_APP_ON_SLASH_DATA((app_), #handler_, (description_), (handler_), (user_data_))
#define DCC_APP_ON_COMMAND_FN(app_, handler_, description_) \
    DCC_APP_ON_SLASH_FN((app_), handler_, (description_))
#define DCC_APP_ON_COMMAND_FN_DATA(app_, handler_, description_, user_data_) \
    DCC_APP_ON_SLASH_FN_DATA((app_), handler_, (description_), (user_data_))
#define DCC_APP_ON_SUBCOMMAND(app_, command_name_, subcommand_path_, handler_) \
    dcc_app_subcommand((app_), (command_name_), (subcommand_path_), (handler_), NULL)
#define DCC_APP_ON_SUBCOMMAND_DATA(app_, command_name_, subcommand_path_, handler_, user_data_) \
    dcc_app_subcommand((app_), (command_name_), (subcommand_path_), (handler_), (user_data_))
#define DCC_APP_ON_AUTOCOMPLETE(app_, name_, handler_) \
    dcc_app_autocomplete((app_), (name_), (handler_), NULL)
#define DCC_APP_ON_AUTOCOMPLETE_DATA(app_, name_, handler_, user_data_) \
    dcc_app_autocomplete((app_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_AUTOCOMPLETE_FN(app_, handler_) \
    DCC_APP_ON_AUTOCOMPLETE((app_), #handler_, (handler_))
#define DCC_APP_ON_AUTOCOMPLETE_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_AUTOCOMPLETE_DATA((app_), #handler_, (handler_), (user_data_))

#define DCC_APP_ON_USER_CONTEXT_MENU(app_, name_, handler_) \
    dcc_app_user_context_menu((app_), (name_), (handler_), NULL)
#define DCC_APP_ON_USER_CONTEXT_MENU_DATA(app_, name_, handler_, user_data_) \
    dcc_app_user_context_menu((app_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU(app_, name_, handler_) \
    dcc_app_message_context_menu((app_), (name_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_DATA(app_, name_, handler_, user_data_) \
    dcc_app_message_context_menu((app_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_USER_CONTEXT_MENU_FN(app_, handler_) \
    DCC_APP_ON_USER_CONTEXT_MENU((app_), #handler_, (handler_))
#define DCC_APP_ON_USER_CONTEXT_MENU_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_USER_CONTEXT_MENU_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_FN(app_, handler_) \
    DCC_APP_ON_MESSAGE_CONTEXT_MENU((app_), #handler_, (handler_))
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_CONTEXT_MENU_DATA((app_), #handler_, (handler_), (user_data_))

#define DCC_APP_ON_BUTTON(app_, custom_id_, handler_) \
    dcc_app_button((app_), (custom_id_), (handler_), NULL)
#define DCC_APP_ON_BUTTON_DATA(app_, custom_id_, handler_, user_data_) \
    dcc_app_button((app_), (custom_id_), (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_PREFIX(app_, custom_id_prefix_, handler_) \
    dcc_app_button_prefix((app_), (custom_id_prefix_), (handler_), NULL)
#define DCC_APP_ON_BUTTON_PREFIX_DATA(app_, custom_id_prefix_, handler_, user_data_) \
    dcc_app_button_prefix((app_), (custom_id_prefix_), (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_FN(app_, handler_) \
    DCC_APP_ON_BUTTON((app_), #handler_, (handler_))
#define DCC_APP_ON_BUTTON_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_PREFIX_FN(app_, handler_) \
    DCC_APP_ON_BUTTON_PREFIX((app_), #handler_, (handler_))
#define DCC_APP_ON_BUTTON_PREFIX_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_PREFIX_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_NS(app_, namespace_, action_, handler_) \
    DCC_APP_ON_BUTTON((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_APP_ON_BUTTON_NS_DATA(app_, namespace_, action_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_DATA((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_NS_PREFIX(app_, namespace_, action_prefix_, handler_) \
    DCC_APP_ON_BUTTON_PREFIX((app_), DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_APP_ON_BUTTON_NS_PREFIX_DATA(app_, namespace_, action_prefix_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_PREFIX_DATA( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_) \
    )
#define DCC_APP_ON_BUTTON_NS_FN(app_, namespace_, handler_) \
    DCC_APP_ON_BUTTON_NS((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_BUTTON_NS_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_NS_DATA((app_), namespace_, #handler_, (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_NS_PREFIX_FN(app_, namespace_, handler_) \
    DCC_APP_ON_BUTTON_NS_PREFIX((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_BUTTON_NS_PREFIX_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_NS_PREFIX_DATA((app_), namespace_, #handler_, (handler_), (user_data_))

#define DCC_APP_ON_SELECT(app_, custom_id_, handler_) \
    dcc_app_select((app_), (custom_id_), (handler_), NULL)
#define DCC_APP_ON_SELECT_DATA(app_, custom_id_, handler_, user_data_) \
    dcc_app_select((app_), (custom_id_), (handler_), (user_data_))
#define DCC_APP_ON_SELECT_PREFIX(app_, custom_id_prefix_, handler_) \
    dcc_app_select_prefix((app_), (custom_id_prefix_), (handler_), NULL)
#define DCC_APP_ON_SELECT_PREFIX_DATA(app_, custom_id_prefix_, handler_, user_data_) \
    dcc_app_select_prefix((app_), (custom_id_prefix_), (handler_), (user_data_))
#define DCC_APP_ON_SELECT_FN(app_, handler_) \
    DCC_APP_ON_SELECT((app_), #handler_, (handler_))
#define DCC_APP_ON_SELECT_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_SELECT_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_SELECT_PREFIX_FN(app_, handler_) \
    DCC_APP_ON_SELECT_PREFIX((app_), #handler_, (handler_))
#define DCC_APP_ON_SELECT_PREFIX_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_SELECT_PREFIX_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_SELECT_NS(app_, namespace_, action_, handler_) \
    DCC_APP_ON_SELECT((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_APP_ON_SELECT_NS_DATA(app_, namespace_, action_, handler_, user_data_) \
    DCC_APP_ON_SELECT_DATA((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))
#define DCC_APP_ON_SELECT_NS_PREFIX(app_, namespace_, action_prefix_, handler_) \
    DCC_APP_ON_SELECT_PREFIX((app_), DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_APP_ON_SELECT_NS_PREFIX_DATA(app_, namespace_, action_prefix_, handler_, user_data_) \
    DCC_APP_ON_SELECT_PREFIX_DATA( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_) \
    )
#define DCC_APP_ON_SELECT_NS_FN(app_, namespace_, handler_) \
    DCC_APP_ON_SELECT_NS((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_SELECT_NS_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_SELECT_NS_DATA((app_), namespace_, #handler_, (handler_), (user_data_))
#define DCC_APP_ON_SELECT_NS_PREFIX_FN(app_, namespace_, handler_) \
    DCC_APP_ON_SELECT_NS_PREFIX((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_SELECT_NS_PREFIX_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_SELECT_NS_PREFIX_DATA((app_), namespace_, #handler_, (handler_), (user_data_))

#define DCC_APP_ON_MODAL(app_, custom_id_, handler_) \
    dcc_app_modal((app_), (custom_id_), (handler_), NULL)
#define DCC_APP_ON_MODAL_DATA(app_, custom_id_, handler_, user_data_) \
    dcc_app_modal((app_), (custom_id_), (handler_), (user_data_))
#define DCC_APP_ON_MODAL_PREFIX(app_, custom_id_prefix_, handler_) \
    dcc_app_modal_prefix((app_), (custom_id_prefix_), (handler_), NULL)
#define DCC_APP_ON_MODAL_PREFIX_DATA(app_, custom_id_prefix_, handler_, user_data_) \
    dcc_app_modal_prefix((app_), (custom_id_prefix_), (handler_), (user_data_))
#define DCC_APP_ON_MODAL_FN(app_, handler_) \
    DCC_APP_ON_MODAL((app_), #handler_, (handler_))
#define DCC_APP_ON_MODAL_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MODAL_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_MODAL_PREFIX_FN(app_, handler_) \
    DCC_APP_ON_MODAL_PREFIX((app_), #handler_, (handler_))
#define DCC_APP_ON_MODAL_PREFIX_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MODAL_PREFIX_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_MODAL_NS(app_, namespace_, action_, handler_) \
    DCC_APP_ON_MODAL((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_APP_ON_MODAL_NS_DATA(app_, namespace_, action_, handler_, user_data_) \
    DCC_APP_ON_MODAL_DATA((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))
#define DCC_APP_ON_MODAL_NS_PREFIX(app_, namespace_, action_prefix_, handler_) \
    DCC_APP_ON_MODAL_PREFIX((app_), DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_APP_ON_MODAL_NS_PREFIX_DATA(app_, namespace_, action_prefix_, handler_, user_data_) \
    DCC_APP_ON_MODAL_PREFIX_DATA( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_) \
    )
#define DCC_APP_ON_MODAL_NS_FN(app_, namespace_, handler_) \
    DCC_APP_ON_MODAL_NS((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_MODAL_NS_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_MODAL_NS_DATA((app_), namespace_, #handler_, (handler_), (user_data_))
#define DCC_APP_ON_MODAL_NS_PREFIX_FN(app_, namespace_, handler_) \
    DCC_APP_ON_MODAL_NS_PREFIX((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_MODAL_NS_PREFIX_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_MODAL_NS_PREFIX_DATA((app_), namespace_, #handler_, (handler_), (user_data_))

#define DCC_APP_ON_EVENT(app_, type_, handler_) dcc_app_on((app_), (type_), (handler_), NULL)
#define DCC_APP_ON_EVENT_DATA(app_, type_, handler_, user_data_) \
    dcc_app_on((app_), (type_), (handler_), (user_data_))
#define DCC_APP_ON_EVENT_FN(app_, type_, handler_) \
    DCC_APP_ON_EVENT((app_), (type_), (handler_))
#define DCC_APP_ON_EVENT_FN_DATA(app_, type_, handler_, user_data_) \
    DCC_APP_ON_EVENT_DATA((app_), (type_), (handler_), (user_data_))

#define DCC_APP_ON_MESSAGE(app_, handler_) dcc_app_on_message_create((app_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_DATA(app_, handler_, user_data_) \
    dcc_app_on_message_create((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_FN(app_, handler_) \
    DCC_APP_ON_MESSAGE((app_), (handler_))
#define DCC_APP_ON_MESSAGE_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_DATA((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_CREATE(app_, handler_) \
    dcc_app_on_message_create((app_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_CREATE_DATA(app_, handler_, user_data_) \
    dcc_app_on_message_create((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_CREATE_FN(app_, handler_) \
    DCC_APP_ON_MESSAGE_CREATE((app_), (handler_))
#define DCC_APP_ON_MESSAGE_CREATE_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_CREATE_DATA((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_UPDATE(app_, handler_) \
    dcc_app_on_message_update((app_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_UPDATE_DATA(app_, handler_, user_data_) \
    dcc_app_on_message_update((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_UPDATE_FN(app_, handler_) \
    DCC_APP_ON_MESSAGE_UPDATE((app_), (handler_))
#define DCC_APP_ON_MESSAGE_UPDATE_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_UPDATE_DATA((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_DELETE(app_, handler_) \
    dcc_app_on_message_delete((app_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_DELETE_DATA(app_, handler_, user_data_) \
    dcc_app_on_message_delete((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_DELETE_FN(app_, handler_) \
    DCC_APP_ON_MESSAGE_DELETE((app_), (handler_))
#define DCC_APP_ON_MESSAGE_DELETE_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_DELETE_DATA((app_), (handler_), (user_data_))

#define DCC_APP_ON_MESSAGE_COMMAND(app_, prefix_, name_, handler_) \
    dcc_app_on_message_command((app_), (prefix_), (name_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_COMMAND_DATA(app_, prefix_, name_, handler_, user_data_) \
    dcc_app_on_message_command((app_), (prefix_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_PREFIX_COMMAND(app_, prefix_, name_, handler_) \
    DCC_APP_ON_MESSAGE_COMMAND((app_), (prefix_), (name_), (handler_))
#define DCC_APP_ON_PREFIX_COMMAND_DATA(app_, prefix_, name_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_COMMAND_DATA((app_), (prefix_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_TEXT_COMMAND(app_, prefix_, name_, handler_) \
    DCC_APP_ON_PREFIX_COMMAND((app_), (prefix_), (name_), (handler_))
#define DCC_APP_ON_TEXT_COMMAND_DATA(app_, prefix_, name_, handler_, user_data_) \
    DCC_APP_ON_PREFIX_COMMAND_DATA((app_), (prefix_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_COMMAND_FN(app_, prefix_, handler_) \
    DCC_APP_ON_MESSAGE_COMMAND((app_), (prefix_), #handler_, (handler_))
#define DCC_APP_ON_MESSAGE_COMMAND_FN_DATA(app_, prefix_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_COMMAND_DATA((app_), (prefix_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_PREFIX_COMMAND_FN(app_, prefix_, handler_) \
    DCC_APP_ON_MESSAGE_COMMAND_FN((app_), (prefix_), handler_)
#define DCC_APP_ON_PREFIX_COMMAND_FN_DATA(app_, prefix_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_COMMAND_FN_DATA((app_), (prefix_), handler_, (user_data_))
#define DCC_APP_ON_TEXT_COMMAND_FN(app_, prefix_, handler_) \
    DCC_APP_ON_PREFIX_COMMAND_FN((app_), (prefix_), handler_)
#define DCC_APP_ON_TEXT_COMMAND_FN_DATA(app_, prefix_, handler_, user_data_) \
    DCC_APP_ON_PREFIX_COMMAND_FN_DATA((app_), (prefix_), handler_, (user_data_))

#define DCC_APP_ON_READY(app_, handler_) \
    dcc_app_on_ready((app_), (handler_), NULL)

#define DCC_APP_ON_READY_DATA(app_, handler_, user_data_) \
    dcc_app_on_ready((app_), (handler_), (user_data_))

#define DCC_APP_ON_READY_FN(app_, handler_) \
    DCC_APP_ON_READY((app_), (handler_))

#define DCC_APP_ON_READY_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_READY_DATA((app_), (handler_), (user_data_))

#define DCC_APP_ON_READY_ONCE(app_, handler_) \
    dcc_app_on_ready_once((app_), (handler_), NULL)

#define DCC_APP_ON_READY_ONCE_DATA(app_, handler_, user_data_) \
    dcc_app_on_ready_once((app_), (handler_), (user_data_))

#define DCC_APP_ON_READY_ONCE_FN(app_, handler_) \
    DCC_APP_ON_READY_ONCE((app_), (handler_))

#define DCC_APP_ON_READY_ONCE_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_READY_ONCE_DATA((app_), (handler_), (user_data_))

#endif
