#ifndef DCC_SUGAR_VIEW_H
#define DCC_SUGAR_VIEW_H

#include <dcc/app.h>
#include <dcc/component_v2.h>

#include <stddef.h>
#include <stdint.h>

#ifndef DCC_COMPONENT_ID
#define DCC_COMPONENT_ID(namespace_, action_) namespace_ "." action_
#endif
#ifndef DCC_COMPONENT_PREFIX
#define DCC_COMPONENT_PREFIX(namespace_) namespace_ "."
#endif
#ifndef DCC_COMPONENT_ACTION_PREFIX
#define DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_) namespace_ "." action_prefix_
#endif

#define DCC_SUGAR_VIEW_ARRAY(type_, ...) ((type_[]){ __VA_ARGS__ })
#define DCC_SUGAR_VIEW_ARRAY_LEN(type_, ...) (sizeof((type_[]){ __VA_ARGS__ }) / sizeof(type_))
#define DCC_SUGAR_VIEW_PICK_3(_1, _2, _3, NAME, ...) NAME

typedef struct dcc_sugar_view_button_action {
    const char *label;
    const char *custom_id;
    dcc_button_style_t style;
    dcc_app_handler_fn handler;
    void *user_data;
    uint8_t disabled;
} dcc_sugar_view_button_action_t;

static inline void *dcc_sugar_view_action_user_data(
    const dcc_sugar_view_button_action_t *action,
    void *default_user_data
) {
    return action != NULL && action->user_data != NULL ? action->user_data : default_user_data;
}

static inline dcc_status_t dcc_sugar_view_register_button_actions(
    dcc_app_t *app,
    const dcc_sugar_view_button_action_t *actions,
    size_t action_count,
    void *default_user_data
) {
    if (app == NULL || (actions == NULL && action_count != 0U)) {
        return DCC_ERR_INVALID_ARG;
    }

    for (size_t i = 0U; i < action_count; ++i) {
        const dcc_sugar_view_button_action_t *action = &actions[i];
        if (action->custom_id == NULL || action->handler == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        dcc_status_t status =
            dcc_app_button(
                app,
                action->custom_id,
                action->handler,
                dcc_sugar_view_action_user_data(action, default_user_data)
            );
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}

static inline dcc_component_v2_builder_t dcc_sugar_view_button_action_row(
    const dcc_sugar_view_button_action_t *actions,
    size_t action_count,
    dcc_component_v2_builder_t *button_buffer,
    size_t button_buffer_count
) {
    size_t count = action_count;
    if (count > button_buffer_count) {
        count = button_buffer_count;
    }
    if (count > DCC_COMPONENT_V2_MAX_ACTION_ROW_CHILDREN) {
        count = DCC_COMPONENT_V2_MAX_ACTION_ROW_CHILDREN;
    }
    if (actions == NULL || button_buffer == NULL) {
        count = 0U;
    }

    for (size_t i = 0U; i < count; ++i) {
        button_buffer[i] = (dcc_component_v2_builder_t){
            .type = DCC_COMPONENT_V2_BUTTON,
            .button_style = actions[i].style,
            .custom_id = actions[i].custom_id,
            .label = actions[i].label,
            .disabled = actions[i].disabled,
            .has_button_style = 1U,
            .has_disabled = actions[i].disabled ? 1U : 0U
        };
    }

    return (dcc_component_v2_builder_t){
        .type = DCC_COMPONENT_V2_ACTION_ROW,
        .children = button_buffer,
        .children_count = count
    };
}

static inline dcc_app_view_t dcc_sugar_view_button_action_routes(
    const dcc_sugar_view_button_action_t *actions,
    size_t action_count,
    dcc_app_view_item_t *item_buffer,
    size_t item_buffer_count,
    void *default_user_data
) {
    size_t count = action_count;
    if (count > item_buffer_count) {
        count = item_buffer_count;
    }
    if (actions == NULL || item_buffer == NULL) {
        count = 0U;
    }

    for (size_t i = 0U; i < count; ++i) {
        item_buffer[i] = (dcc_app_view_item_t){
            .size = sizeof(dcc_app_view_item_t),
            .type = DCC_APP_VIEW_BUTTON,
            .custom_id = actions[i].custom_id,
            .handler = actions[i].handler,
            .user_data = actions[i].user_data
        };
    }

    return (dcc_app_view_t){
        .size = sizeof(dcc_app_view_t),
        .items = item_buffer,
        .item_count = count,
        .user_data = default_user_data
    };
}

#define DCC_VIEW_ACTION(style_, label_, custom_id_, handler_) \
    DCC_VIEW_ACTION_DATA((style_), (label_), (custom_id_), (handler_), NULL)
#define DCC_VIEW_ACTION_DATA(style_, label_, custom_id_, handler_, user_data_) \
    ((dcc_sugar_view_button_action_t){ \
        .label = (label_), \
        .custom_id = (custom_id_), \
        .style = (style_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })
#define DCC_VIEW_ACTION_DISABLED(style_, label_, custom_id_, handler_, disabled_) \
    ((dcc_sugar_view_button_action_t){ \
        .label = (label_), \
        .custom_id = (custom_id_), \
        .style = (style_), \
        .handler = (handler_), \
        .disabled = (disabled_), \
    })
#define DCC_VIEW_ACTION_DISABLED_DATA(style_, label_, custom_id_, handler_, user_data_, disabled_) \
    ((dcc_sugar_view_button_action_t){ \
        .label = (label_), \
        .custom_id = (custom_id_), \
        .style = (style_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .disabled = (disabled_), \
    })

#define DCC_VIEW_ACTION_PRIMARY(label_, custom_id_, handler_) \
    DCC_VIEW_ACTION(DCC_BUTTON_PRIMARY, (label_), (custom_id_), (handler_))
#define DCC_VIEW_ACTION_SECONDARY(label_, custom_id_, handler_) \
    DCC_VIEW_ACTION(DCC_BUTTON_SECONDARY, (label_), (custom_id_), (handler_))
#define DCC_VIEW_ACTION_SUCCESS(label_, custom_id_, handler_) \
    DCC_VIEW_ACTION(DCC_BUTTON_SUCCESS, (label_), (custom_id_), (handler_))
#define DCC_VIEW_ACTION_DANGER(label_, custom_id_, handler_) \
    DCC_VIEW_ACTION(DCC_BUTTON_DANGER, (label_), (custom_id_), (handler_))

#define DCC_VIEW_ACTION_PRIMARY_DATA(label_, custom_id_, handler_, user_data_) \
    DCC_VIEW_ACTION_DATA(DCC_BUTTON_PRIMARY, (label_), (custom_id_), (handler_), (user_data_))
#define DCC_VIEW_ACTION_SECONDARY_DATA(label_, custom_id_, handler_, user_data_) \
    DCC_VIEW_ACTION_DATA(DCC_BUTTON_SECONDARY, (label_), (custom_id_), (handler_), (user_data_))
#define DCC_VIEW_ACTION_SUCCESS_DATA(label_, custom_id_, handler_, user_data_) \
    DCC_VIEW_ACTION_DATA(DCC_BUTTON_SUCCESS, (label_), (custom_id_), (handler_), (user_data_))
#define DCC_VIEW_ACTION_DANGER_DATA(label_, custom_id_, handler_, user_data_) \
    DCC_VIEW_ACTION_DATA(DCC_BUTTON_DANGER, (label_), (custom_id_), (handler_), (user_data_))

#define DCC_VIEW_ACTION_NS(style_, label_, namespace_, action_, handler_) \
    DCC_VIEW_ACTION((style_), (label_), DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_VIEW_ACTION_NS_DATA(style_, label_, namespace_, action_, handler_, user_data_) \
    DCC_VIEW_ACTION_DATA((style_), (label_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))
#define DCC_VIEW_ACTION_NS_DISABLED(style_, label_, namespace_, action_, handler_, disabled_) \
    DCC_VIEW_ACTION_DISABLED((style_), (label_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (disabled_))
#define DCC_VIEW_ACTION_NS_DISABLED_DATA(style_, label_, namespace_, action_, handler_, user_data_, disabled_) \
    DCC_VIEW_ACTION_DISABLED_DATA( \
        (style_), (label_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_), (disabled_) \
    )

#define DCC_VIEW_ACTION_PRIMARY_NS(label_, namespace_, action_, handler_) \
    DCC_VIEW_ACTION_NS(DCC_BUTTON_PRIMARY, (label_), namespace_, action_, (handler_))
#define DCC_VIEW_ACTION_SECONDARY_NS(label_, namespace_, action_, handler_) \
    DCC_VIEW_ACTION_NS(DCC_BUTTON_SECONDARY, (label_), namespace_, action_, (handler_))
#define DCC_VIEW_ACTION_SUCCESS_NS(label_, namespace_, action_, handler_) \
    DCC_VIEW_ACTION_NS(DCC_BUTTON_SUCCESS, (label_), namespace_, action_, (handler_))
#define DCC_VIEW_ACTION_DANGER_NS(label_, namespace_, action_, handler_) \
    DCC_VIEW_ACTION_NS(DCC_BUTTON_DANGER, (label_), namespace_, action_, (handler_))

#define DCC_VIEW_ACTION_PRIMARY_NS_DATA(label_, namespace_, action_, handler_, user_data_) \
    DCC_VIEW_ACTION_NS_DATA(DCC_BUTTON_PRIMARY, (label_), namespace_, action_, (handler_), (user_data_))
#define DCC_VIEW_ACTION_SECONDARY_NS_DATA(label_, namespace_, action_, handler_, user_data_) \
    DCC_VIEW_ACTION_NS_DATA(DCC_BUTTON_SECONDARY, (label_), namespace_, action_, (handler_), (user_data_))
#define DCC_VIEW_ACTION_SUCCESS_NS_DATA(label_, namespace_, action_, handler_, user_data_) \
    DCC_VIEW_ACTION_NS_DATA(DCC_BUTTON_SUCCESS, (label_), namespace_, action_, (handler_), (user_data_))
#define DCC_VIEW_ACTION_DANGER_NS_DATA(label_, namespace_, action_, handler_, user_data_) \
    DCC_VIEW_ACTION_NS_DATA(DCC_BUTTON_DANGER, (label_), namespace_, action_, (handler_), (user_data_))

#define DCC_VIEW_ACTION_PRIMARY_NS_DISABLED(label_, namespace_, action_, handler_, disabled_) \
    DCC_VIEW_ACTION_NS_DISABLED(DCC_BUTTON_PRIMARY, (label_), namespace_, action_, (handler_), (disabled_))
#define DCC_VIEW_ACTION_SECONDARY_NS_DISABLED(label_, namespace_, action_, handler_, disabled_) \
    DCC_VIEW_ACTION_NS_DISABLED(DCC_BUTTON_SECONDARY, (label_), namespace_, action_, (handler_), (disabled_))
#define DCC_VIEW_ACTION_SUCCESS_NS_DISABLED(label_, namespace_, action_, handler_, disabled_) \
    DCC_VIEW_ACTION_NS_DISABLED(DCC_BUTTON_SUCCESS, (label_), namespace_, action_, (handler_), (disabled_))
#define DCC_VIEW_ACTION_DANGER_NS_DISABLED(label_, namespace_, action_, handler_, disabled_) \
    DCC_VIEW_ACTION_NS_DISABLED(DCC_BUTTON_DANGER, (label_), namespace_, action_, (handler_), (disabled_))

#define DCC_VIEW_ACTION_ROW(...) \
    dcc_sugar_view_button_action_row( \
        DCC_SUGAR_VIEW_ARRAY(dcc_sugar_view_button_action_t, __VA_ARGS__), \
        DCC_SUGAR_VIEW_ARRAY_LEN(dcc_sugar_view_button_action_t, __VA_ARGS__), \
        (dcc_component_v2_builder_t[DCC_SUGAR_VIEW_ARRAY_LEN(dcc_sugar_view_button_action_t, __VA_ARGS__)]){0}, \
        DCC_SUGAR_VIEW_ARRAY_LEN(dcc_sugar_view_button_action_t, __VA_ARGS__) \
    )

#define DCC_APP_USE_VIEW_ACTIONS(app_, user_data_, ...) \
    dcc_sugar_view_register_button_actions( \
        (app_), \
        DCC_SUGAR_VIEW_ARRAY(dcc_sugar_view_button_action_t, __VA_ARGS__), \
        DCC_SUGAR_VIEW_ARRAY_LEN(dcc_sugar_view_button_action_t, __VA_ARGS__), \
        (user_data_) \
    )

#define DCC_VIEW_ACTION_ROUTES_DATA(user_data_, ...) \
    dcc_sugar_view_button_action_routes( \
        DCC_SUGAR_VIEW_ARRAY(dcc_sugar_view_button_action_t, __VA_ARGS__), \
        DCC_SUGAR_VIEW_ARRAY_LEN(dcc_sugar_view_button_action_t, __VA_ARGS__), \
        (dcc_app_view_item_t[DCC_SUGAR_VIEW_ARRAY_LEN(dcc_sugar_view_button_action_t, __VA_ARGS__)]){0}, \
        DCC_SUGAR_VIEW_ARRAY_LEN(dcc_sugar_view_button_action_t, __VA_ARGS__), \
        (user_data_) \
    )

#define DCC_VIEW_ACTION_ROUTES(...) \
    DCC_VIEW_ACTION_ROUTES_DATA(NULL, __VA_ARGS__)

#define DCC_VIEW_BUTTON_SIMPLE(custom_id_, handler_) \
    DCC_VIEW_BUTTON_DATA((custom_id_), (handler_), NULL)
#define DCC_VIEW_BUTTON_DATA(custom_id_, handler_, user_data_) \
    ((dcc_app_view_item_t){ \
        .size = sizeof(dcc_app_view_item_t), \
        .type = DCC_APP_VIEW_BUTTON, \
        .custom_id = (custom_id_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })
#define DCC_VIEW_BUTTON(...) \
    DCC_SUGAR_VIEW_PICK_3(__VA_ARGS__, DCC_VIEW_BUTTON_DATA, DCC_VIEW_BUTTON_SIMPLE, DCC_VIEW_BUTTON_SIMPLE)(__VA_ARGS__)

#define DCC_VIEW_BUTTON_NS(namespace_, action_, handler_) \
    DCC_VIEW_BUTTON(DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_VIEW_BUTTON_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_VIEW_BUTTON_DATA(DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))

#define DCC_VIEW_BUTTON_PREFIX_SIMPLE(custom_id_prefix_, handler_) \
    DCC_VIEW_BUTTON_PREFIX_DATA((custom_id_prefix_), (handler_), NULL)
#define DCC_VIEW_BUTTON_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    ((dcc_app_view_item_t){ \
        .size = sizeof(dcc_app_view_item_t), \
        .type = DCC_APP_VIEW_BUTTON_PREFIX, \
        .custom_id = (custom_id_prefix_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })
#define DCC_VIEW_BUTTON_PREFIX(...) \
    DCC_SUGAR_VIEW_PICK_3( \
        __VA_ARGS__, \
        DCC_VIEW_BUTTON_PREFIX_DATA, \
        DCC_VIEW_BUTTON_PREFIX_SIMPLE, \
        DCC_VIEW_BUTTON_PREFIX_SIMPLE \
    )(__VA_ARGS__)

#define DCC_VIEW_BUTTON_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_VIEW_BUTTON_PREFIX(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_VIEW_BUTTON_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_VIEW_BUTTON_PREFIX_DATA(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_), (user_data_))

#define DCC_VIEW_SELECT_SIMPLE(custom_id_, handler_) \
    DCC_VIEW_SELECT_DATA((custom_id_), (handler_), NULL)
#define DCC_VIEW_SELECT_DATA(custom_id_, handler_, user_data_) \
    ((dcc_app_view_item_t){ \
        .size = sizeof(dcc_app_view_item_t), \
        .type = DCC_APP_VIEW_SELECT, \
        .custom_id = (custom_id_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })
#define DCC_VIEW_SELECT(...) \
    DCC_SUGAR_VIEW_PICK_3(__VA_ARGS__, DCC_VIEW_SELECT_DATA, DCC_VIEW_SELECT_SIMPLE, DCC_VIEW_SELECT_SIMPLE)(__VA_ARGS__)

#define DCC_VIEW_SELECT_NS(namespace_, action_, handler_) \
    DCC_VIEW_SELECT(DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_VIEW_SELECT_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_VIEW_SELECT_DATA(DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))

#define DCC_VIEW_SELECT_PREFIX_SIMPLE(custom_id_prefix_, handler_) \
    DCC_VIEW_SELECT_PREFIX_DATA((custom_id_prefix_), (handler_), NULL)
#define DCC_VIEW_SELECT_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    ((dcc_app_view_item_t){ \
        .size = sizeof(dcc_app_view_item_t), \
        .type = DCC_APP_VIEW_SELECT_PREFIX, \
        .custom_id = (custom_id_prefix_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })
#define DCC_VIEW_SELECT_PREFIX(...) \
    DCC_SUGAR_VIEW_PICK_3( \
        __VA_ARGS__, \
        DCC_VIEW_SELECT_PREFIX_DATA, \
        DCC_VIEW_SELECT_PREFIX_SIMPLE, \
        DCC_VIEW_SELECT_PREFIX_SIMPLE \
    )(__VA_ARGS__)

#define DCC_VIEW_SELECT_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_VIEW_SELECT_PREFIX(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_VIEW_SELECT_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_VIEW_SELECT_PREFIX_DATA(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_), (user_data_))

#define DCC_VIEW_MODAL_SIMPLE(custom_id_, handler_) \
    DCC_VIEW_MODAL_DATA((custom_id_), (handler_), NULL)
#define DCC_VIEW_MODAL_DATA(custom_id_, handler_, user_data_) \
    ((dcc_app_view_item_t){ \
        .size = sizeof(dcc_app_view_item_t), \
        .type = DCC_APP_VIEW_MODAL, \
        .custom_id = (custom_id_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })
#define DCC_VIEW_MODAL(...) \
    DCC_SUGAR_VIEW_PICK_3(__VA_ARGS__, DCC_VIEW_MODAL_DATA, DCC_VIEW_MODAL_SIMPLE, DCC_VIEW_MODAL_SIMPLE)(__VA_ARGS__)

#define DCC_VIEW_MODAL_NS(namespace_, action_, handler_) \
    DCC_VIEW_MODAL(DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_VIEW_MODAL_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_VIEW_MODAL_DATA(DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))

#define DCC_VIEW_MODAL_PREFIX_SIMPLE(custom_id_prefix_, handler_) \
    DCC_VIEW_MODAL_PREFIX_DATA((custom_id_prefix_), (handler_), NULL)
#define DCC_VIEW_MODAL_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    ((dcc_app_view_item_t){ \
        .size = sizeof(dcc_app_view_item_t), \
        .type = DCC_APP_VIEW_MODAL_PREFIX, \
        .custom_id = (custom_id_prefix_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })
#define DCC_VIEW_MODAL_PREFIX(...) \
    DCC_SUGAR_VIEW_PICK_3( \
        __VA_ARGS__, \
        DCC_VIEW_MODAL_PREFIX_DATA, \
        DCC_VIEW_MODAL_PREFIX_SIMPLE, \
        DCC_VIEW_MODAL_PREFIX_SIMPLE \
    )(__VA_ARGS__)

#define DCC_VIEW_MODAL_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_VIEW_MODAL_PREFIX(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_VIEW_MODAL_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_VIEW_MODAL_PREFIX_DATA(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_), (user_data_))

#define DCC_APP_VIEW(...) \
    ((dcc_app_view_t){ \
        .size = sizeof(dcc_app_view_t), \
        .items = DCC_SUGAR_VIEW_ARRAY(dcc_app_view_item_t, __VA_ARGS__), \
        .item_count = DCC_SUGAR_VIEW_ARRAY_LEN(dcc_app_view_item_t, __VA_ARGS__) \
    })
#define DCC_VIEW(...) DCC_APP_VIEW(__VA_ARGS__)
#define DCC_VIEW_DATA(user_data_, ...) \
    ((dcc_app_view_t){ \
        .size = sizeof(dcc_app_view_t), \
        .items = DCC_SUGAR_VIEW_ARRAY(dcc_app_view_item_t, __VA_ARGS__), \
        .item_count = DCC_SUGAR_VIEW_ARRAY_LEN(dcc_app_view_item_t, __VA_ARGS__), \
        .user_data = (user_data_) \
    })

#define DCC_PERSISTENT_VIEW(...) DCC_VIEW(__VA_ARGS__)
#define DCC_PERSISTENT_VIEW_DATA(user_data_, ...) DCC_VIEW_DATA((user_data_), __VA_ARGS__)

#define DCC_APP_USE_VIEW(app_, ...) \
    dcc_app_view_register((app_), &((dcc_app_view_t[]){ DCC_VIEW(__VA_ARGS__) })[0])
#define DCC_APP_USE_VIEW_DATA(app_, user_data_, ...) \
    dcc_app_view_register((app_), &((dcc_app_view_t[]){ DCC_VIEW_DATA((user_data_), __VA_ARGS__) })[0])

#define DCC_GROUP_USE_VIEW(group_, ...) \
    dcc_app_route_group_view_register((group_), &((dcc_app_view_t[]){ DCC_VIEW(__VA_ARGS__) })[0])
#define DCC_GROUP_USE_VIEW_DATA(group_, user_data_, ...) \
    dcc_app_route_group_view_register((group_), &((dcc_app_view_t[]){ DCC_VIEW_DATA((user_data_), __VA_ARGS__) })[0])

#endif
