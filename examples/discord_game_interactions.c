#include "discord_game_internal.h"

static int parse_game_custom_id(const char *custom_id, char board[GAME_BOARD_LEN + 1], int *out_pos) {
    if (custom_id == NULL || board == NULL || out_pos == NULL) {
        return 0;
    }
    if (strlen(custom_id) != GAME_CUSTOM_ID_LEN ||
        strncmp(custom_id, GAME_CUSTOM_ID_PREFIX, strlen(GAME_CUSTOM_ID_PREFIX)) != 0 ||
        custom_id[13] != ':' ||
        custom_id[14] < '0' ||
        custom_id[14] > '8') {
        return 0;
    }

    for (int i = 0; i < GAME_BOARD_LEN; ++i) {
        char cell = custom_id[4 + i];
        if (cell != '0' && cell != '1' && cell != '2') {
            return 0;
        }
        board[i] = cell;
    }
    board[GAME_BOARD_LEN] = '\0';
    *out_pos = custom_id[14] - '0';
    return 1;
}

static const char *cell_label(char cell) {
    if (cell == '1') {
        return "X";
    }
    if (cell == '2') {
        return "O";
    }
    return ".";
}

static dcc_button_style_t cell_style(char cell) {
    if (cell == '1') {
        return DCC_BUTTON_PRIMARY;
    }
    if (cell == '2') {
        return DCC_BUTTON_DANGER;
    }
    return DCC_BUTTON_SECONDARY;
}

static dcc_status_t build_game_message(
    const char board[GAME_BOARD_LEN + 1],
    const char *status_text,
    int game_over,
    dcc_message_builder_t *message,
    dcc_component_builder_t rows[3],
    dcc_component_builder_t buttons[GAME_BOARD_LEN],
    char custom_ids[GAME_BOARD_LEN][32],
    char *content,
    size_t content_len
) {
    int written = snprintf(
        content,
        content_len,
        "Tic Tac Toe\n%s\n\nYou are X. The bot is O.",
        status_text != NULL ? status_text : "Your turn."
    );
    if (written <= 0 || (size_t)written >= content_len) {
        return DCC_ERR_INVALID_ARG;
    }

    for (int i = 0; i < GAME_BOARD_LEN; ++i) {
        int id_len = snprintf(custom_ids[i], 32, GAME_CUSTOM_ID_PREFIX "%s:%d", board, i);
        if (id_len <= 0 || id_len >= 32) {
            return DCC_ERR_INVALID_ARG;
        }

        buttons[i] = DCC_BUTTON_BUILDER(cell_style(board[i]), cell_label(board[i]), custom_ids[i]);
        buttons[i].disabled = (uint8_t)(game_over || board[i] != '0');
        buttons[i].has_disabled = 1U;
    }

    for (int row = 0; row < 3; ++row) {
        rows[row] = DCC_ACTION_ROW_ARRAY(&buttons[row * 3], 3U);
    }

    *message = DCC_MESSAGE_COMPONENTS_ARRAY(rows, 3U);
    message->content = content;
    message->has_content = 1U;
    return DCC_OK;
}

void rest_response_log_cb(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data) {
    (void)client;
    const char *label = user_data != NULL ? (const char *)user_data : "request";
    if (response == NULL) {
        fprintf(stderr, "%s failed: no response\n", label);
        return;
    }
    if (response->error == DCC_OK && response->status >= 200 && response->status < 300) {
        return;
    }

    fprintf(stderr, "%s failed: HTTP %u, %s", label, (unsigned)response->status, dcc_status_string(response->error));
    if (response->body != NULL && response->body_len > 0) {
        size_t body_len = response->body_len > 512 ? 512 : response->body_len;
        fprintf(stderr, ": %.*s%s", (int)body_len, response->body, response->body_len > body_len ? "..." : "");
    }
    fputc('\n', stderr);
}

dcc_status_t respond_text(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_interaction_response_type_t type,
    const char *content
) {
    dcc_message_builder_t message = DCC_MESSAGE_TEXT(content);
    return dcc_rest_interaction_response_create_from_interaction_message_builder(
        client,
        interaction,
        type,
        &message,
        rest_response_log_cb,
        "interaction response"
    );
}

dcc_status_t respond_game(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_interaction_response_type_t type,
    const char board[GAME_BOARD_LEN + 1],
    const char *status_text,
    int game_over
) {
    dcc_message_builder_t message;
    dcc_component_builder_t rows[3];
    dcc_component_builder_t buttons[GAME_BOARD_LEN];
    char custom_ids[GAME_BOARD_LEN][32];
    char content[256];

    dcc_status_t st = build_game_message(
        board,
        status_text,
        game_over,
        &message,
        rows,
        buttons,
        custom_ids,
        content,
        sizeof(content)
    );
    if (st != DCC_OK) {
        return respond_text(client, interaction, type, "Could not build the game board.");
    }

    return dcc_rest_interaction_response_create_from_interaction_message_builder(
        client,
        interaction,
        type,
        &message,
        rest_response_log_cb,
        "interaction response"
    );
}

void on_game_command(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)user_data;
    const dcc_interaction_t *interaction = dcc_event_interaction(event);
    if (interaction == NULL) {
        return;
    }

    char board[GAME_BOARD_LEN + 1] = "000000000";
    dcc_status_t st = respond_game(
        client,
        interaction,
        DCC_INTERACTION_RESPONSE_CHANNEL_MESSAGE_WITH_SOURCE,
        board,
        "Your turn. Pick a square.",
        0
    );
    if (st != DCC_OK) {
        fprintf(stderr, "failed to respond to /%s: %s\n", GAME_COMMAND_NAME, dcc_status_string(st));
    }
}

void on_game_button(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)user_data;
    const dcc_interaction_t *interaction = dcc_event_interaction(event);
    if (interaction == NULL || interaction->custom_id == NULL) {
        return;
    }

    char board[GAME_BOARD_LEN + 1];
    int pos = 0;
    if (!parse_game_custom_id(interaction->custom_id, board, &pos)) {
        return;
    }

    const char *status = "Your turn.";
    int game_over = 0;
    if (board[pos] != '0') {
        status = "That square is already taken.";
    } else {
        board[pos] = '1';
        char winner = winner_for_board(board);
        if (winner == '1') {
            status = "You win.";
            game_over = 1;
        } else if (board_full(board)) {
            status = "Draw.";
            game_over = 1;
        } else {
            int bot_move = choose_bot_move(board, interaction->id);
            if (bot_move >= 0) {
                board[bot_move] = '2';
            }
            winner = winner_for_board(board);
            if (winner == '2') {
                status = "Bot wins.";
                game_over = 1;
            } else if (board_full(board)) {
                status = "Draw.";
                game_over = 1;
            } else {
                status = "Bot moved. Your turn.";
            }
        }
    }

    dcc_status_t st = respond_game(
        client,
        interaction,
        DCC_INTERACTION_RESPONSE_UPDATE_MESSAGE,
        board,
        status,
        game_over
    );
    if (st != DCC_OK) {
        fprintf(stderr, "failed to update game message: %s\n", dcc_status_string(st));
    }
}
