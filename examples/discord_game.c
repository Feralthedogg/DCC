#include "discord_game_internal.h"

DCC_DEV_BOT_MAIN(
    DCC_LISTEN_SLASH(
        GAME_COMMAND_NAME, "Start a Tic Tac Toe game", on_game_command),
    DCC_LISTEN_BUTTON_PREFIX(GAME_CUSTOM_ID_PREFIX, on_game_button),
    DCC_LISTEN_READY(on_ready),
    DCC_LISTEN_EVENT(DCC_EVENT_LOG, on_log),
    DCC_LISTEN_EVENT(DCC_EVENT_SOCKET_CLOSE, on_socket_close)
)
