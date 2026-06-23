#include <dcc/app.h>

#include <stddef.h>

#define DCC_APP_VIEW_HAS_USER_DATA(view_) \
    ((view_) != NULL && \
     (view_)->size >= (offsetof(dcc_app_view_t, user_data) + sizeof(((dcc_app_view_t *)0)->user_data)))

static void *dcc_app_view_default_user_data(const dcc_app_view_t *view) {
    return DCC_APP_VIEW_HAS_USER_DATA(view) ? view->user_data : NULL;
}

static dcc_status_t dcc_app_view_register_item(
    dcc_app_t *app,
    const dcc_app_view_item_t *item,
    void *view_user_data
) {
    if (app == NULL || item == NULL || item->custom_id == NULL ||
        item->custom_id[0] == '\0' || item->handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    void *user_data = item->user_data != NULL ? item->user_data : view_user_data;
    switch (item->type) {
        case DCC_APP_VIEW_BUTTON:
            return dcc_app_button_route(
                app,
                item->custom_id,
                item->handler,
                user_data,
                item->out_route
            );
        case DCC_APP_VIEW_BUTTON_PREFIX:
            return dcc_app_button_prefix_route(
                app,
                item->custom_id,
                item->handler,
                user_data,
                item->out_route
            );
        case DCC_APP_VIEW_SELECT:
            return dcc_app_select_route(
                app,
                item->custom_id,
                item->handler,
                user_data,
                item->out_route
            );
        case DCC_APP_VIEW_SELECT_PREFIX:
            return dcc_app_select_prefix_route(
                app,
                item->custom_id,
                item->handler,
                user_data,
                item->out_route
            );
        case DCC_APP_VIEW_MODAL:
            return dcc_app_modal_route(
                app,
                item->custom_id,
                item->handler,
                user_data,
                item->out_route
            );
        case DCC_APP_VIEW_MODAL_PREFIX:
            return dcc_app_modal_prefix_route(
                app,
                item->custom_id,
                item->handler,
                user_data,
                item->out_route
            );
    }

    return DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_app_view_register(dcc_app_t *app, const dcc_app_view_t *view) {
    if (app == NULL || view == NULL ||
        (view->items == NULL && view->item_count != 0U)) {
        return DCC_ERR_INVALID_ARG;
    }

    for (size_t i = 0; i < view->item_count; ++i) {
        dcc_status_t status =
            dcc_app_view_register_item(app, &view->items[i], dcc_app_view_default_user_data(view));
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}
