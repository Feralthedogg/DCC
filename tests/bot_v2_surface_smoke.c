#include <dcc/bot.h>

#include <stddef.h>

DCC_SLASH_FN(bot_v2_ping) {
    (void)user_data;
    return DCC_CTX_REPLY_TEXT(ctx, "pong");
}

DCC_AUTOCOMPLETE_FN(bot_v2_autocomplete) {
    dcc_autocomplete_builder_t choices = DCC_AUTOCOMPLETE_BUILDER_INIT;
    (void)user_data;
    return DCC_CTX_AUTOCOMPLETE(ctx, choices);
}

DCC_BUTTON_FN(bot_v2_button) {
    (void)user_data;
    return DCC_CTX_UPDATE_UI(ctx, DCC_UI_TEXT("clicked"));
}

DCC_SELECT_FN(bot_v2_select) {
    (void)user_data;
    return DCC_CTX_REPLY_EPHEMERAL_TEXT(ctx, "selected");
}

DCC_MODAL_FN(bot_v2_modal) {
    (void)user_data;
    return DCC_CTX_REPLY_TEXT(ctx, "submitted");
}

DCC_EVENT_FN(bot_v2_event) {
    (void)app;
    (void)event;
    (void)user_data;
    return DCC_OK;
}

DCC_READY_FN(bot_v2_ready) {
    (void)app;
    (void)ready;
    (void)user_data;
    return DCC_OK;
}

DCC_MESSAGE_FN(bot_v2_message) {
    (void)app;
    (void)message;
    (void)event;
    (void)user_data;
    return DCC_OK;
}

DCC_TASK_FN(bot_v2_task) {
    (void)app;
    (void)user_data;
    return DCC_OK;
}

static dcc_status_t bot_v2_typed(dcc_ctx_t *ctx, void *args, void *user_data) {
    (void)ctx;
    (void)args;
    (void)user_data;
    return DCC_OK;
}

static dcc_status_t bot_v2_message_command(
    dcc_app_t *app,
    const dcc_message_t *message,
    const char *arguments,
    void *user_data
) {
    (void)app;
    (void)message;
    (void)arguments;
    (void)user_data;
    return DCC_OK;
}

static int bot_v2_construct_surface(void) {
    dcc_listener_config_t config = DCC_LISTENER_CONFIG();
    dcc_listener_t listeners[] = {
        DCC_LISTEN_SLASH("ping", "Reply with pong", bot_v2_ping),
        DCC_LISTEN_SLASH_WITH(
            "typed", "Typed route",
            DCC_LISTENER_TYPED_HANDLER(bot_v2_typed), config),
        DCC_LISTEN_SUBCOMMAND("admin", "ban", "Ban a user", bot_v2_ping),
        DCC_LISTEN_AUTOCOMPLETE("search", "Search", bot_v2_autocomplete),
        DCC_LISTEN_SUBCOMMAND_AUTOCOMPLETE(
            "admin", "ban", "Complete a ban", bot_v2_autocomplete),
        DCC_LISTEN_USER_CONTEXT_MENU("Inspect", bot_v2_ping),
        DCC_LISTEN_MESSAGE_CONTEXT_MENU("Quote", bot_v2_ping),
        DCC_LISTEN_BUTTON("accept", bot_v2_button),
        DCC_LISTEN_BUTTON_PREFIX("page:", bot_v2_button),
        DCC_LISTEN_SELECT("role", bot_v2_select),
        DCC_LISTEN_SELECT_PREFIX("role:", bot_v2_select),
        DCC_LISTEN_MODAL("profile", bot_v2_modal),
        DCC_LISTEN_MODAL_PREFIX("profile:", bot_v2_modal),
        DCC_LISTEN_EVENT(DCC_EVENT_GUILD_CREATE, bot_v2_event),
        DCC_LISTEN_READY(bot_v2_ready),
        DCC_LISTEN_MESSAGE_CREATE(bot_v2_message),
        DCC_LISTEN_MESSAGE_UPDATE(bot_v2_message),
        DCC_LISTEN_MESSAGE_DELETE(bot_v2_message),
        DCC_LISTEN_MESSAGE_COMMAND("!", "ping", bot_v2_message_command),
        DCC_LISTEN_TASK(DCC_LISTENER_INTERVAL(1000U), bot_v2_task),
    };
    dcc_component_v2_builder_t card = DCC_UI_CARD(
        DCC_UI_TEXT("hello"),
        DCC_UI_ROW(DCC_UI_PRIMARY("OK", "ok"))
    );
    dcc_component_v2_builder_t input =
        DCC_UI_INPUT("name", DCC_TEXT_INPUT_SHORT);
    dcc_component_v2_builder_t label = DCC_UI_LABEL("Name", &input);
    dcc_modal_builder_t modal = DCC_UI_MODAL("profile", "Profile", label);
    (void)card;
    (void)modal;
    return listeners[0].kind == DCC_LISTENER_SLASH ? 0 : 1;
}

int main(void) {
    return bot_v2_construct_surface();
}
