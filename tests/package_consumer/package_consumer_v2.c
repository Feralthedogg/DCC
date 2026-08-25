#include <dcc/bot.h>
#include <dcc/dcc.h>

#include <string.h>

DCC_SLASH_FN(package_ping) {
    (void)user_data;
    return DCC_CTX_REPLY_TEXT(ctx, "pong");
}

int main(void) {
    dcc_listener_t listener =
        DCC_LISTEN_SLASH("ping", "Reply with pong", package_ping);
    dcc_component_v2_builder_t text = DCC_UI_TEXT("installed package");
    dcc_component_v2_builder_t row_children[] = {
        DCC_UI_PRIMARY("OK", "package.ok"),
    };
    dcc_component_v2_builder_t row = DCC_UI_ROW_ARRAY(row_children, 1U);
    dcc_component_v2_builder_t card_children[] = {text, row};
    dcc_component_v2_builder_t card = DCC_UI_CARD_ARRAY(card_children, 2U);
    dcc_component_v2_validation_error_t error =
        DCC_COMPONENT_V2_VALIDATION_ERROR_INIT;
    if (strcmp(dcc_version_string(), "2.0.0") != 0 ||
        listener.kind != DCC_LISTENER_SLASH ||
        dcc_component_v2_validate(
            &card, 1U, DCC_COMPONENT_V2_CONTEXT_MESSAGE_V2, &error) != DCC_OK) {
        return 1;
    }
    dcc_bot_config_t config = DCC_BOT_CONFIG();
    return config.version == DCC_BOT_CONFIG_VERSION ? 0 : 1;
}
