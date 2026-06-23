#ifndef DCC_DISCORD_GAME_INTERNAL_H
#define DCC_DISCORD_GAME_INTERNAL_H

#include <dcc/sugar.h>
#include <dcc/rest/interactions.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define GAME_COMMAND_NAME "tictactoe"
#define GAME_CUSTOM_ID_PREFIX "ttt:"
#define GAME_BOARD_LEN 9
#define GAME_CUSTOM_ID_LEN 15

char winner_for_board(const char board[GAME_BOARD_LEN + 1]);
int board_full(const char board[GAME_BOARD_LEN + 1]);
int choose_bot_move(const char board[GAME_BOARD_LEN + 1], uint64_t salt);

dcc_status_t respond_text(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_interaction_response_type_t type,
    const char *content
);
dcc_status_t respond_game(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_interaction_response_type_t type,
    const char board[GAME_BOARD_LEN + 1],
    const char *status_text,
    int game_over
);
void rest_response_log_cb(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data);
void on_game_command(dcc_ctx_t *ctx, void *user_data);
void on_game_button(dcc_ctx_t *ctx, void *user_data);

void on_ready(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data);
void on_log(dcc_app_t *app, const dcc_event_t *event, void *user_data);
void on_socket_close(dcc_app_t *app, const dcc_event_t *event, void *user_data);

#endif
