#include <dcc/sugar/app_registration.h>

#include <stdio.h>

static void focused_event_handler(dcc_app_t *app, const dcc_event_t *event, void *user_data) {
    (void)app;
    (void)event;
    (void)user_data;
}

static void focused_ready_handler(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data) {
    (void)app;
    (void)ready;
    (void)user_data;
}

static void focused_message_handler(
    dcc_app_t *app,
    const dcc_message_t *message,
    const dcc_event_t *event,
    void *user_data
) {
    (void)app;
    (void)message;
    (void)event;
    (void)user_data;
}

static void focused_message_command_handler(
    dcc_app_t *app,
    const dcc_message_t *message,
    const char *args,
    const dcc_event_t *event,
    void *user_data
) {
    (void)app;
    (void)message;
    (void)args;
    (void)event;
    (void)user_data;
}

static void focused_interaction_handler(dcc_ctx_t *ctx, void *user_data) {
    (void)ctx;
    (void)user_data;
}

static int check_null_app_aliases(void) {
    dcc_application_command_builder_t builder = {
        .name = "builder",
        .description = "Builder",
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT,
        .has_name = 1U,
        .has_description = 1U,
        .has_type = 1U,
    };

    int ok =
        DCC_APP_ON_SLASH(NULL, "ping", "Ping", focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SLASH_DATA(NULL, "ping", "Ping", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SLASH_BUILDER(NULL, &builder, focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SLASH_BUILDER_DATA(NULL, &builder, focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_COMMAND(NULL, "ping", "Ping", focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_COMMAND_DATA(NULL, "ping", "Ping", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_COMMAND_BUILDER(NULL, &builder, focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_COMMAND_BUILDER_DATA(NULL, &builder, focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SLASH_FN(NULL, focused_interaction_handler, "Ping") == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SLASH_FN_DATA(NULL, focused_interaction_handler, "Ping", NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_COMMAND_FN(NULL, focused_interaction_handler, "Ping") == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_COMMAND_FN_DATA(NULL, focused_interaction_handler, "Ping", NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SUBCOMMAND(NULL, "admin", "reload", focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SUBCOMMAND_DATA(NULL, "admin", "reload", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_AUTOCOMPLETE(NULL, "search", focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_AUTOCOMPLETE_DATA(NULL, "search", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_AUTOCOMPLETE_FN(NULL, focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_AUTOCOMPLETE_FN_DATA(NULL, focused_interaction_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_USER_CONTEXT_MENU(NULL, "Inspect user", focused_interaction_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_USER_CONTEXT_MENU_DATA(NULL, "Inspect user", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_CONTEXT_MENU(NULL, "Inspect message", focused_interaction_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_CONTEXT_MENU_DATA(NULL, "Inspect message", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_USER_CONTEXT_MENU_FN(NULL, focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_USER_CONTEXT_MENU_FN_DATA(NULL, focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_CONTEXT_MENU_FN(NULL, focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_CONTEXT_MENU_FN_DATA(NULL, focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON(NULL, "button.refresh", focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_DATA(NULL, "button.refresh", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_PREFIX(NULL, "button.", focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_PREFIX_DATA(NULL, "button.", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_FN(NULL, focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_FN_DATA(NULL, focused_interaction_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_PREFIX_FN(NULL, focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_PREFIX_FN_DATA(NULL, focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_NS(NULL, "panel", "refresh", focused_interaction_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_NS_DATA(NULL, "panel", "refresh", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_NS_PREFIX(NULL, "panel", "page.", focused_interaction_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_NS_PREFIX_DATA(NULL, "panel", "page.", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_NS_FN(NULL, "panel", focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_NS_FN_DATA(NULL, "panel", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_NS_PREFIX_FN(NULL, "panel", focused_interaction_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_NS_PREFIX_FN_DATA(NULL, "panel", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT(NULL, "select.refresh", focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_DATA(NULL, "select.refresh", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_PREFIX(NULL, "select.", focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_PREFIX_DATA(NULL, "select.", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_FN(NULL, focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_FN_DATA(NULL, focused_interaction_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_PREFIX_FN(NULL, focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_PREFIX_FN_DATA(NULL, focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_NS(NULL, "panel", "refresh", focused_interaction_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_NS_DATA(NULL, "panel", "refresh", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_NS_PREFIX(NULL, "panel", "page.", focused_interaction_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_NS_PREFIX_DATA(NULL, "panel", "page.", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_NS_FN(NULL, "panel", focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_NS_FN_DATA(NULL, "panel", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_NS_PREFIX_FN(NULL, "panel", focused_interaction_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_NS_PREFIX_FN_DATA(NULL, "panel", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL(NULL, "modal.refresh", focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_DATA(NULL, "modal.refresh", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_PREFIX(NULL, "modal.", focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_PREFIX_DATA(NULL, "modal.", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_FN(NULL, focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_FN_DATA(NULL, focused_interaction_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_PREFIX_FN(NULL, focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_PREFIX_FN_DATA(NULL, focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_NS(NULL, "panel", "refresh", focused_interaction_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_NS_DATA(NULL, "panel", "refresh", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_NS_PREFIX(NULL, "panel", "page.", focused_interaction_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_NS_PREFIX_DATA(NULL, "panel", "page.", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_NS_FN(NULL, "panel", focused_interaction_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_NS_FN_DATA(NULL, "panel", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_NS_PREFIX_FN(NULL, "panel", focused_interaction_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_NS_PREFIX_FN_DATA(NULL, "panel", focused_interaction_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_EVENT(NULL, DCC_EVENT_READY, focused_event_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_EVENT_DATA(NULL, DCC_EVENT_READY, focused_event_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_EVENT_FN(NULL, DCC_EVENT_READY, focused_event_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_EVENT_FN_DATA(NULL, DCC_EVENT_READY, focused_event_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_READY(NULL, focused_ready_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_READY_DATA(NULL, focused_ready_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_READY_FN(NULL, focused_ready_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_READY_FN_DATA(NULL, focused_ready_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_READY_ONCE(NULL, focused_ready_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_READY_ONCE_DATA(NULL, focused_ready_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_READY_ONCE_FN(NULL, focused_ready_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_READY_ONCE_FN_DATA(NULL, focused_ready_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE(NULL, focused_message_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_DATA(NULL, focused_message_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_FN(NULL, focused_message_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_FN_DATA(NULL, focused_message_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_CREATE(NULL, focused_message_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_CREATE_DATA(NULL, focused_message_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_CREATE_FN(NULL, focused_message_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_CREATE_FN_DATA(NULL, focused_message_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_UPDATE(NULL, focused_message_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_UPDATE_DATA(NULL, focused_message_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_UPDATE_FN(NULL, focused_message_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_UPDATE_FN_DATA(NULL, focused_message_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_DELETE(NULL, focused_message_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_DELETE_DATA(NULL, focused_message_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_DELETE_FN(NULL, focused_message_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_DELETE_FN_DATA(NULL, focused_message_handler, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_COMMAND(NULL, "!", "ping", focused_message_command_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_COMMAND_DATA(NULL, "!", "ping", focused_message_command_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_PREFIX_COMMAND(NULL, "!", "ping", focused_message_command_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_PREFIX_COMMAND_DATA(NULL, "!", "ping", focused_message_command_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_TEXT_COMMAND(NULL, "!", "ping", focused_message_command_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_TEXT_COMMAND_DATA(NULL, "!", "ping", focused_message_command_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_COMMAND_FN(NULL, "!", focused_message_command_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_COMMAND_FN_DATA(NULL, "!", focused_message_command_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_PREFIX_COMMAND_FN(NULL, "!", focused_message_command_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_PREFIX_COMMAND_FN_DATA(NULL, "!", focused_message_command_handler, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_TEXT_COMMAND_FN(NULL, "!", focused_message_command_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_TEXT_COMMAND_FN_DATA(NULL, "!", focused_message_command_handler, NULL) ==
            DCC_ERR_INVALID_ARG;
    return ok ? 0 : 1;
}

int main(void) {
    if (check_null_app_aliases() != 0) {
        fprintf(stderr, "focused app registration sugar aliases failed\n");
        return 1;
    }
    return 0;
}
