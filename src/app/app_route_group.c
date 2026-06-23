#include "internal/app/dcc_app_internal.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DCC_APP_ROUTE_GROUP_MIN_SIZE \
    (offsetof(dcc_app_route_group_t, user_data) + sizeof(((dcc_app_route_group_t *)0)->user_data))
#define DCC_APP_VIEW_HAS_USER_DATA(view_) \
    ((view_) != NULL && \
     (view_)->size >= (offsetof(dcc_app_view_t, user_data) + sizeof(((dcc_app_view_t *)0)->user_data)))

static uint8_t dcc_app_route_group_type_is_component(dcc_event_type_t type) {
    return type == DCC_EVENT_BUTTON_CLICK ||
        type == DCC_EVENT_SELECT_CLICK ||
        type == DCC_EVENT_FORM_SUBMIT;
}

dcc_status_t dcc_app_route_group_validate_internal(const dcc_app_route_group_t *group) {
    if (group == NULL ||
        group->size < DCC_APP_ROUTE_GROUP_MIN_SIZE ||
        group->app == NULL ||
        group->custom_id_prefix == NULL ||
        group->custom_id_prefix[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

uint8_t dcc_app_route_group_matches_internal(
    const dcc_app_route_group_t *group,
    const dcc_app_route_t *route
) {
    if (dcc_app_route_group_validate_internal(group) != DCC_OK ||
        route == NULL ||
        route->key == NULL ||
        !dcc_app_route_group_type_is_component(route->type)) {
        return 0U;
    }
    size_t prefix_len = strlen(group->custom_id_prefix);
    return strncmp(route->key, group->custom_id_prefix, prefix_len) == 0 ? 1U : 0U;
}

static dcc_status_t dcc_app_route_group_key(
    const dcc_app_route_group_t *group,
    const char *action,
    char **out_key
) {
    if (out_key == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_key = NULL;

    dcc_status_t status = dcc_app_route_group_validate_internal(group);
    if (status != DCC_OK) {
        return status;
    }

    const char *suffix = action != NULL ? action : "";
    size_t prefix_len = strlen(group->custom_id_prefix);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > SIZE_MAX - prefix_len - 1U) {
        return DCC_ERR_NOMEM;
    }

    char *key = (char *)malloc(prefix_len + suffix_len + 1U);
    if (key == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(key, group->custom_id_prefix, prefix_len);
    memcpy(key + prefix_len, suffix, suffix_len);
    key[prefix_len + suffix_len] = '\0';
    *out_key = key;
    return DCC_OK;
}

static dcc_status_t dcc_app_route_group_add(
    const dcc_app_route_group_t *group,
    dcc_event_type_t type,
    const char *action,
    uint8_t prefix,
    dcc_app_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
) {
    char *key = NULL;
    dcc_status_t status = dcc_app_route_group_key(group, action, &key);
    if (status == DCC_OK) {
        status = dcc_app_add_route(group->app, type, key, prefix, handler, user_data, out_route);
    }
    free(key);
    return status;
}

dcc_status_t dcc_app_route_group_init(
    dcc_app_route_group_t *group,
    dcc_app_t *app,
    const char *custom_id_prefix,
    void *user_data
) {
    if (group == NULL || app == NULL || custom_id_prefix == NULL || custom_id_prefix[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    memset(group, 0, sizeof(*group));
    group->size = sizeof(*group);
    group->app = app;
    group->custom_id_prefix = custom_id_prefix;
    group->user_data = user_data;
    return DCC_OK;
}

dcc_status_t dcc_app_route_group_button(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_handler_fn handler
) {
    return dcc_app_route_group_button_route(group, action, handler, NULL);
}

dcc_status_t dcc_app_route_group_button_route(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_handler_fn handler,
    dcc_app_route_id_t *out_route
) {
    return dcc_app_route_group_add(
        group,
        DCC_EVENT_BUTTON_CLICK,
        action,
        0U,
        handler,
        group != NULL ? group->user_data : NULL,
        out_route
    );
}

dcc_status_t dcc_app_route_group_button_prefix(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_handler_fn handler
) {
    return dcc_app_route_group_button_prefix_route(group, action_prefix, handler, NULL);
}

dcc_status_t dcc_app_route_group_button_prefix_route(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_handler_fn handler,
    dcc_app_route_id_t *out_route
) {
    return dcc_app_route_group_add(
        group,
        DCC_EVENT_BUTTON_CLICK,
        action_prefix,
        1U,
        handler,
        group != NULL ? group->user_data : NULL,
        out_route
    );
}

dcc_status_t dcc_app_route_group_select(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_handler_fn handler
) {
    return dcc_app_route_group_select_route(group, action, handler, NULL);
}

dcc_status_t dcc_app_route_group_select_route(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_handler_fn handler,
    dcc_app_route_id_t *out_route
) {
    return dcc_app_route_group_add(
        group,
        DCC_EVENT_SELECT_CLICK,
        action,
        0U,
        handler,
        group != NULL ? group->user_data : NULL,
        out_route
    );
}

dcc_status_t dcc_app_route_group_select_prefix(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_handler_fn handler
) {
    return dcc_app_route_group_select_prefix_route(group, action_prefix, handler, NULL);
}

dcc_status_t dcc_app_route_group_select_prefix_route(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_handler_fn handler,
    dcc_app_route_id_t *out_route
) {
    return dcc_app_route_group_add(
        group,
        DCC_EVENT_SELECT_CLICK,
        action_prefix,
        1U,
        handler,
        group != NULL ? group->user_data : NULL,
        out_route
    );
}

dcc_status_t dcc_app_route_group_modal(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_handler_fn handler
) {
    return dcc_app_route_group_modal_route(group, action, handler, NULL);
}

dcc_status_t dcc_app_route_group_modal_route(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_handler_fn handler,
    dcc_app_route_id_t *out_route
) {
    return dcc_app_route_group_add(
        group,
        DCC_EVENT_FORM_SUBMIT,
        action,
        0U,
        handler,
        group != NULL ? group->user_data : NULL,
        out_route
    );
}

dcc_status_t dcc_app_route_group_modal_prefix(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_handler_fn handler
) {
    return dcc_app_route_group_modal_prefix_route(group, action_prefix, handler, NULL);
}

dcc_status_t dcc_app_route_group_modal_prefix_route(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_handler_fn handler,
    dcc_app_route_id_t *out_route
) {
    return dcc_app_route_group_add(
        group,
        DCC_EVENT_FORM_SUBMIT,
        action_prefix,
        1U,
        handler,
        group != NULL ? group->user_data : NULL,
        out_route
    );
}

static dcc_status_t dcc_app_route_group_view_register_item(
    const dcc_app_route_group_t *group,
    const dcc_app_view_item_t *item,
    void *view_user_data
) {
    if (item == NULL || item->custom_id == NULL || item->custom_id[0] == '\0' || item->handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    void *user_data = item->user_data != NULL ? item->user_data : view_user_data;
    if (user_data == NULL) {
        user_data = group->user_data;
    }
    switch (item->type) {
        case DCC_APP_VIEW_BUTTON:
            return dcc_app_route_group_add(
                group,
                DCC_EVENT_BUTTON_CLICK,
                item->custom_id,
                0U,
                item->handler,
                user_data,
                item->out_route
            );
        case DCC_APP_VIEW_BUTTON_PREFIX:
            return dcc_app_route_group_add(
                group,
                DCC_EVENT_BUTTON_CLICK,
                item->custom_id,
                1U,
                item->handler,
                user_data,
                item->out_route
            );
        case DCC_APP_VIEW_SELECT:
            return dcc_app_route_group_add(
                group,
                DCC_EVENT_SELECT_CLICK,
                item->custom_id,
                0U,
                item->handler,
                user_data,
                item->out_route
            );
        case DCC_APP_VIEW_SELECT_PREFIX:
            return dcc_app_route_group_add(
                group,
                DCC_EVENT_SELECT_CLICK,
                item->custom_id,
                1U,
                item->handler,
                user_data,
                item->out_route
            );
        case DCC_APP_VIEW_MODAL:
            return dcc_app_route_group_add(
                group,
                DCC_EVENT_FORM_SUBMIT,
                item->custom_id,
                0U,
                item->handler,
                user_data,
                item->out_route
            );
        case DCC_APP_VIEW_MODAL_PREFIX:
            return dcc_app_route_group_add(
                group,
                DCC_EVENT_FORM_SUBMIT,
                item->custom_id,
                1U,
                item->handler,
                user_data,
                item->out_route
            );
    }

    return DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_app_route_group_view_register(
    const dcc_app_route_group_t *group,
    const dcc_app_view_t *view
) {
    dcc_status_t status = dcc_app_route_group_validate_internal(group);
    if (status != DCC_OK) {
        return status;
    }
    if (view == NULL || (view->items == NULL && view->item_count != 0U)) {
        return DCC_ERR_INVALID_ARG;
    }

    void *view_user_data = DCC_APP_VIEW_HAS_USER_DATA(view) ? view->user_data : NULL;
    for (size_t i = 0; i < view->item_count; ++i) {
        status = dcc_app_route_group_view_register_item(group, &view->items[i], view_user_data);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}

dcc_status_t dcc_app_route_group_use(
    const dcc_app_route_group_t *group,
    dcc_app_middleware_fn middleware,
    void *user_data
) {
    dcc_status_t status = dcc_app_route_group_validate_internal(group);
    if (status != DCC_OK) {
        return status;
    }
    if (middleware == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t matched = 0U;
    for (size_t i = 0U; i < group->app->route_count; ++i) {
        dcc_app_route_t *route = &group->app->routes[i];
        if (!dcc_app_route_group_matches_internal(group, route)) {
            continue;
        }
        matched++;
        status = dcc_app_route_use_internal(
            group->app,
            route->id,
            middleware,
            user_data,
            NULL
        );
        if (status != DCC_OK) {
            return status;
        }
    }

    return matched != 0U ? DCC_OK : DCC_ERR_NOT_FOUND;
}
