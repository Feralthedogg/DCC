#include "discord_game_internal.h"

static const int win_lines[8][3] = {
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8},
    {0, 3, 6},
    {1, 4, 7},
    {2, 5, 8},
    {0, 4, 8},
    {2, 4, 6},
};

char winner_for_board(const char board[GAME_BOARD_LEN + 1]) {
    for (size_t i = 0; i < sizeof(win_lines) / sizeof(win_lines[0]); ++i) {
        int a = win_lines[i][0];
        int b = win_lines[i][1];
        int c = win_lines[i][2];
        if (board[a] != '0' && board[a] == board[b] && board[b] == board[c]) {
            return board[a];
        }
    }
    return '0';
}

int board_full(const char board[GAME_BOARD_LEN + 1]) {
    for (int i = 0; i < GAME_BOARD_LEN; ++i) {
        if (board[i] == '0') {
            return 0;
        }
    }
    return 1;
}

static int find_line_move(const char board[GAME_BOARD_LEN + 1], char mark) {
    for (size_t i = 0; i < sizeof(win_lines) / sizeof(win_lines[0]); ++i) {
        int empty = -1;
        int marks = 0;
        for (int j = 0; j < 3; ++j) {
            int cell = win_lines[i][j];
            if (board[cell] == mark) {
                marks += 1;
            } else if (board[cell] == '0') {
                empty = cell;
            }
        }
        if (marks == 2 && empty >= 0) {
            return empty;
        }
    }
    return -1;
}

static uint32_t ttt_board_seed(const char board[GAME_BOARD_LEN + 1], uint64_t salt) {
    uint32_t hash = 2166136261U;
    for (int i = 0; i < GAME_BOARD_LEN; ++i) {
        hash ^= (uint8_t)board[i];
        hash *= 16777619U;
    }
    hash ^= (uint32_t)salt;
    hash *= 16777619U;
    hash ^= (uint32_t)(salt >> 32);
    return hash != 0 ? hash : 0x9e3779b9U;
}

static uint32_t ttt_rng_next(uint32_t *state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x != 0 ? x : 0x6d2b79f5U;
    return *state;
}

static double ttt_rng_unit(uint32_t *state) {
    return (double)(ttt_rng_next(state) >> 8) / 16777216.0;
}

static double ttt_relu(double value) {
    return value > 0.0 ? value : 0.0;
}

static int count_open_twos(const char board[GAME_BOARD_LEN + 1], char mark) {
    int count = 0;
    for (size_t i = 0; i < sizeof(win_lines) / sizeof(win_lines[0]); ++i) {
        int marks = 0;
        int empty = 0;
        for (int j = 0; j < 3; ++j) {
            char cell = board[win_lines[i][j]];
            if (cell == mark) {
                marks += 1;
            } else if (cell == '0') {
                empty += 1;
            }
        }
        if (marks == 2 && empty == 1) {
            count += 1;
        }
    }
    return count;
}

static int find_fork_move(const char board[GAME_BOARD_LEN + 1], char mark) {
    int best_move = -1;
    int best_score = 1;

    for (int cell = 0; cell < GAME_BOARD_LEN; ++cell) {
        if (board[cell] != '0') {
            continue;
        }

        char next[GAME_BOARD_LEN + 1];
        memcpy(next, board, GAME_BOARD_LEN + 1);
        next[cell] = mark;

        int score = count_open_twos(next, mark);
        if (score > best_score) {
            best_score = score;
            best_move = cell;
        }
    }

    return best_move;
}

static void ttt_cnn_policy_logits(const char board[GAME_BOARD_LEN + 1], double logits[GAME_BOARD_LEN]) {
    static const double conv_bias[4] = {0.02, 0.03, -0.01, 0.00};
    static const double conv_filter[4][2][3][3] = {
        {
            {{-0.10, -0.22, -0.10}, {-0.22, 0.00, -0.22}, {-0.10, -0.22, -0.10}},
            {{0.18, 0.42, 0.18}, {0.42, 0.00, 0.42}, {0.18, 0.42, 0.18}},
        },
        {
            {{0.20, 0.45, 0.20}, {0.45, 0.00, 0.45}, {0.20, 0.45, 0.20}},
            {{0.06, -0.18, 0.06}, {-0.18, 0.00, -0.18}, {0.06, -0.18, 0.06}},
        },
        {
            {{0.40, -0.08, 0.40}, {-0.08, 0.05, -0.08}, {0.40, -0.08, 0.40}},
            {{0.26, -0.04, 0.26}, {-0.04, -0.04, -0.04}, {0.26, -0.04, 0.26}},
        },
        {
            {{-0.08, 0.22, -0.08}, {0.22, 0.00, 0.22}, {-0.08, 0.22, -0.08}},
            {{0.12, 0.10, 0.12}, {0.10, 0.00, 0.10}, {0.12, 0.10, 0.12}},
        },
    };
    static const double head_weight[4] = {0.70, 0.55, 0.34, 0.28};
    static const double position_bias[GAME_BOARD_LEN] = {0.25, -0.10, 0.25, -0.10, 0.44, -0.10, 0.25, -0.10, 0.25};

    double planes[2][GAME_BOARD_LEN] = {{0}};
    double features[4][GAME_BOARD_LEN] = {{0}};

    for (int i = 0; i < GAME_BOARD_LEN; ++i) {
        planes[0][i] = board[i] == '1' ? 1.0 : 0.0;
        planes[1][i] = board[i] == '2' ? 1.0 : 0.0;
    }

    for (int filter = 0; filter < 4; ++filter) {
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                double sum = conv_bias[filter];
                for (int channel = 0; channel < 2; ++channel) {
                    for (int ky = 0; ky < 3; ++ky) {
                        for (int kx = 0; kx < 3; ++kx) {
                            int yy = y + ky - 1;
                            int xx = x + kx - 1;
                            if (yy >= 0 && yy < 3 && xx >= 0 && xx < 3) {
                                sum += planes[channel][yy * 3 + xx] * conv_filter[filter][channel][ky][kx];
                            }
                        }
                    }
                }
                features[filter][y * 3 + x] = ttt_relu(sum);
            }
        }
    }

    for (int cell = 0; cell < GAME_BOARD_LEN; ++cell) {
        if (board[cell] != '0') {
            logits[cell] = -1000.0;
            continue;
        }

        double logit = position_bias[cell];
        for (int filter = 0; filter < 4; ++filter) {
            logit += head_weight[filter] * features[filter][cell];
        }

        char next[GAME_BOARD_LEN + 1];
        memcpy(next, board, GAME_BOARD_LEN + 1);
        next[cell] = '2';

        int bot_twos = count_open_twos(next, '2');
        int human_twos = count_open_twos(next, '1');
        if (winner_for_board(next) == '2') {
            logit += 0.95;
        }
        logit += 0.34 * (double)bot_twos;
        logit -= 0.18 * (double)human_twos;
        if (bot_twos >= 2) {
            logit += 0.45;
        }
        if (human_twos >= 2) {
            logit -= 0.35;
        }
        logits[cell] = logit;
    }
}

static int sample_policy_move(const char board[GAME_BOARD_LEN + 1], const double logits[GAME_BOARD_LEN], uint32_t *rng) {
    double scores[GAME_BOARD_LEN];
    double min_score = 1000.0;
    int first_empty = -1;

    for (int i = 0; i < GAME_BOARD_LEN; ++i) {
        if (board[i] != '0') {
            scores[i] = -1000.0;
            continue;
        }
        if (first_empty < 0) {
            first_empty = i;
        }
        scores[i] = logits[i] + (ttt_rng_unit(rng) - 0.5) * 0.62;
        if (scores[i] < min_score) {
            min_score = scores[i];
        }
    }

    if (first_empty < 0) {
        return -1;
    }

    double total = 0.0;
    double weights[GAME_BOARD_LEN] = {0};
    for (int i = 0; i < GAME_BOARD_LEN; ++i) {
        if (board[i] != '0') {
            continue;
        }
        double shifted = scores[i] - min_score + 0.30;
        weights[i] = shifted * shifted + 0.05;
        total += weights[i];
    }

    double pick = ttt_rng_unit(rng) * total;
    for (int i = 0; i < GAME_BOARD_LEN; ++i) {
        if (board[i] != '0') {
            continue;
        }
        if (pick <= weights[i]) {
            return i;
        }
        pick -= weights[i];
    }

    return first_empty;
}

int choose_bot_move(const char board[GAME_BOARD_LEN + 1], uint64_t salt) {
    uint32_t rng = ttt_board_seed(board, salt);
    int win_move = find_line_move(board, '2');
    int block_move = find_line_move(board, '1');
    int fork_move = find_fork_move(board, '2');
    int block_fork_move = find_fork_move(board, '1');

    if (win_move >= 0 && ttt_rng_unit(&rng) < 0.90) {
        return win_move;
    }
    if (block_move >= 0 && ttt_rng_unit(&rng) < 0.70) {
        return block_move;
    }
    if (fork_move >= 0 && ttt_rng_unit(&rng) < 0.62) {
        return fork_move;
    }
    if (block_fork_move >= 0 && ttt_rng_unit(&rng) < 0.52) {
        return block_fork_move;
    }

    double logits[GAME_BOARD_LEN] = {0};
    ttt_cnn_policy_logits(board, logits);
    if (win_move >= 0) {
        logits[win_move] += 1.20;
    }
    if (block_move >= 0) {
        logits[block_move] += 0.75;
    }
    if (fork_move >= 0) {
        logits[fork_move] += 0.55;
    }
    if (block_fork_move >= 0) {
        logits[block_fork_move] += 0.42;
    }

    return sample_policy_move(board, logits, &rng);
}
