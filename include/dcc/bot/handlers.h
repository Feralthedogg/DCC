#ifndef DCC_BOT_HANDLERS_H
#define DCC_BOT_HANDLERS_H

#include <dcc/app/base.h>

/** Declares an internal-linkage slash-command handler returning status. */
#define DCC_SLASH_FN(name_) \
    static dcc_status_t name_(dcc_ctx_t *ctx, void *user_data)
/** Declares an internal-linkage autocomplete handler returning status. */
#define DCC_AUTOCOMPLETE_FN(name_) \
    static dcc_status_t name_(dcc_ctx_t *ctx, void *user_data)
/** Declares an internal-linkage button handler returning status. */
#define DCC_BUTTON_FN(name_) \
    static dcc_status_t name_(dcc_ctx_t *ctx, void *user_data)
/** Declares an internal-linkage select handler returning status. */
#define DCC_SELECT_FN(name_) \
    static dcc_status_t name_(dcc_ctx_t *ctx, void *user_data)
/** Declares an internal-linkage modal handler returning status. */
#define DCC_MODAL_FN(name_) \
    static dcc_status_t name_(dcc_ctx_t *ctx, void *user_data)
/** Declares an internal-linkage generic event handler returning status. */
#define DCC_EVENT_FN(name_) \
    static dcc_status_t name_(dcc_app_t *app, const dcc_event_t *event, void *user_data)
/** Declares an internal-linkage READY handler returning status. */
#define DCC_READY_FN(name_) \
    static dcc_status_t name_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data)
/** Declares an internal-linkage message event handler returning status. */
#define DCC_MESSAGE_FN(name_) \
    static dcc_status_t name_(dcc_app_t *app, const dcc_message_t *message, \
        const dcc_event_t *event, void *user_data)
/** Declares an internal-linkage scheduled-task handler returning status. */
#define DCC_TASK_FN(name_) \
    static dcc_status_t name_(dcc_app_t *app, void *user_data)

#endif
