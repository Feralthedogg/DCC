#include <dcc/sugar.h>

#include <stdio.h>
#include <string.h>

static int check_text(const char *actual, const char *expected) {
    if (actual == NULL || strcmp(actual, expected) != 0) {
        fprintf(stderr, "expected '%s', got '%s'\n", expected, actual != NULL ? actual : "(null)");
        return 1;
    }
    return 0;
}

static int check_format_functions(void) {
    char buffer[DCC_FORMAT_MENTION_MAX];
    char command[DCC_FORMAT_COMMAND_MENTION_MAX];
    char emoji[DCC_FORMAT_EMOJI_MAX];
    char timestamp[DCC_FORMAT_TIMESTAMP_MAX];

    if (dcc_sugar_format_user_mention(buffer, sizeof(buffer), 123ULL) != DCC_OK ||
        check_text(buffer, "<@123>") != 0 ||
        dcc_sugar_format_member_mention(buffer, sizeof(buffer), 123ULL) != DCC_OK ||
        check_text(buffer, "<@!123>") != 0 ||
        dcc_sugar_format_role_mention(buffer, sizeof(buffer), 456ULL) != DCC_OK ||
        check_text(buffer, "<@&456>") != 0 ||
        dcc_sugar_format_channel_mention(buffer, sizeof(buffer), 789ULL) != DCC_OK ||
        check_text(buffer, "<#789>") != 0 ||
        dcc_sugar_format_command_mention(command, sizeof(command), "rank", 111ULL) != DCC_OK ||
        check_text(command, "</rank:111>") != 0 ||
        dcc_sugar_format_subcommand_mention(command, sizeof(command), "rank", "top", 222ULL) != DCC_OK ||
        check_text(command, "</rank top:222>") != 0 ||
        dcc_sugar_format_subcommand_group_mention(
            command,
            sizeof(command),
            "config",
            "birthday",
            "set",
            333ULL
        ) != DCC_OK ||
        check_text(command, "</config birthday set:333>") != 0 ||
        dcc_sugar_format_custom_emoji(emoji, sizeof(emoji), "party_blob", 987ULL, 0U) != DCC_OK ||
        check_text(emoji, "<:party_blob:987>") != 0 ||
        dcc_sugar_format_custom_emoji(emoji, sizeof(emoji), "spin", 654ULL, 1U) != DCC_OK ||
        check_text(emoji, "<a:spin:654>") != 0 ||
        dcc_sugar_format_timestamp(timestamp, sizeof(timestamp), 1700000000, 'R') != DCC_OK ||
        check_text(timestamp, "<t:1700000000:R>") != 0 ||
        dcc_sugar_format_timestamp(timestamp, sizeof(timestamp), 1700000000, '\0') != DCC_OK ||
        check_text(timestamp, "<t:1700000000>") != 0) {
        return 1;
    }

    return 0;
}

static int check_short_macros(void) {
    char sentence[256];
    int written = snprintf(
        sentence,
        sizeof(sentence),
        "%s %s %s %s %s %s %s",
        DCC_MENTION_USER(123ULL),
        DCC_MENTION_ROLE(456ULL),
        DCC_MENTION_CHANNEL(789ULL),
        DCC_MENTION_SUBCOMMAND("rank", "top", 222ULL),
        DCC_EMOJI("party_blob", 987ULL),
        DCC_ANIMATED_EMOJI("spin", 654ULL),
        DCC_TIMESTAMP_RELATIVE(1700000000)
    );

    if (written < 0 || (size_t)written >= sizeof(sentence)) {
        return 1;
    }
    return check_text(
        sentence,
        "<@123> <@&456> <#789> </rank top:222> <:party_blob:987> <a:spin:654> <t:1700000000:R>"
    );
}

static int check_custom_emoji_parse(void) {
    dcc_sugar_custom_emoji_t emoji;

    if (dcc_sugar_parse_custom_emoji("<:party_blob:987>", &emoji) != DCC_OK ||
        emoji.animated != 0U ||
        emoji.id != 987ULL ||
        emoji.name_len != strlen("party_blob") ||
        strncmp(emoji.name, "party_blob", emoji.name_len) != 0) {
        return 1;
    }

    if (dcc_sugar_parse_custom_emoji("<a:spin_2:654>", &emoji) != DCC_OK ||
        emoji.animated != 1U ||
        emoji.id != 654ULL ||
        emoji.name_len != strlen("spin_2") ||
        strncmp(emoji.name, "spin_2", emoji.name_len) != 0) {
        return 1;
    }

    return
        dcc_sugar_parse_custom_emoji("<:x:1>", &emoji) == DCC_ERR_INVALID_ARG &&
        dcc_sugar_parse_custom_emoji("<:bad-name:1>", &emoji) == DCC_ERR_INVALID_ARG &&
        dcc_sugar_parse_custom_emoji("<a:spin:0>", &emoji) == DCC_ERR_INVALID_ARG &&
        dcc_sugar_parse_custom_emoji("<:spin:18446744073709551616>", &emoji) ==
            DCC_ERR_INVALID_ARG
            ? 0
            : 1;
}

static int check_alloc_format(void) {
    char *text = NULL;
    if (DCC_FORMAT_ALLOC(&text, "rank #%d: %s", 7, "feral") != DCC_OK) {
        return 1;
    }
    int failed = check_text(text, "rank #7: feral");
    DCC_FORMAT_FREE(text);
    if (failed != 0) {
        return 1;
    }

    text = (char *)1;
    return
        DCC_FORMAT_ALLOC(NULL, "x") == DCC_ERR_INVALID_ARG &&
        DCC_FORMAT_ALLOC(&text, NULL) == DCC_ERR_INVALID_ARG &&
        text == (char *)1
            ? 0
            : 1;
}

static int check_formatted_reply_nulls(void) {
    return
        DCC_REPLYF(NULL, "rank #%d", 7) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_PRIVATEF(NULL, "secret %s", "value") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_SILENTF(NULL, "quiet %u", 1U) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_NO_EMBEDSF(NULL, "plain %s", "link") == DCC_ERR_INVALID_ARG &&
        DCC_PRIVATEF(NULL, "private %d", 1) == DCC_ERR_INVALID_ARG &&
        DCC_RESPONDF(NULL, "respond %d", 2) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_PRIVATEF(NULL, "private respond %d", 3) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_SILENTF(NULL, "silent respond %d", 4) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_NO_EMBEDSF(NULL, "plain respond %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUPF(NULL, "followup %d", 2) == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_PRIVATEF(NULL, "private followup %d", 3) == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_SILENTF(NULL, "silent followup %d", 4) == DCC_ERR_INVALID_ARG &&
        DCC_SENDF(NULL, "send %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_UPDATEF(NULL, "update %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_UPDATE_MESSAGEF(NULL, "update message %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_EDITF(NULL, "edit %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_EDIT_ORIGINALF(NULL, "edit original %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_DONEF(NULL, "done %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_ACKF(NULL, "ack %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_SUCCESSF(NULL, "Saved", "success %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_SUCCESSF((dcc_ctx_t *)1, NULL, "success %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_FAILF(NULL, "fail %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_ERRORF(NULL, "Error", "error %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_ERRORF((dcc_ctx_t *)1, NULL, "error %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_DENYF(NULL, "deny %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_BAD_INPUTF(NULL, "bad input %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_NOT_FOUNDF(NULL, "not found %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_BUSYF(NULL, "busy %d", 5) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_REPLYF(NULL, "ctx %d", 6) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPONDF(NULL, "ctx respond %d", 7) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_PRIVATEF(NULL, "ctx private %d", 7) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_PRIVATEF(NULL, "ctx private respond %d", 8) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SILENTF(NULL, "ctx silent %d", 8) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_SILENTF(NULL, "ctx silent respond %d", 9) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_NO_EMBEDSF(NULL, "ctx plain %d", 9) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_NO_EMBEDSF(NULL, "ctx plain respond %d", 10) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUPF(NULL, "ctx followup %d", 10) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP_PRIVATEF(NULL, "ctx private followup %d", 11) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP_SILENTF(NULL, "ctx silent followup %d", 12) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SENDF(NULL, "ctx send %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_UPDATEF(NULL, "ctx update %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_UPDATE_MESSAGEF(NULL, "ctx update message %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_EDITF(NULL, "ctx edit %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_EDIT_ORIGINALF(NULL, "ctx edit original %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_DONEF(NULL, "ctx done %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_ACKF(NULL, "ctx ack %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SUCCESSF(NULL, "Saved", "ctx success %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FAILF(NULL, "ctx fail %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_ERRORF(NULL, "Error", "ctx error %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_DENYF(NULL, "ctx deny %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_BAD_INPUTF(NULL, "ctx bad input %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_NOT_FOUNDF(NULL, "ctx not found %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_REPLY_F(NULL, "ctx reply alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_REPLY_TEXT_F(NULL, "ctx reply text alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_F(NULL, "ctx respond alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_TEXT_F(NULL, "ctx respond text alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_PRIVATE_F(NULL, "ctx private alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_EPHEMERAL_F(NULL, "ctx ephemeral alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_PRIVATE_F(NULL, "ctx respond private alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_EPHEMERAL_F(NULL, "ctx respond ephemeral alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SILENT_F(NULL, "ctx silent alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_SILENT_F(NULL, "ctx respond silent alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_NO_EMBEDS_F(NULL, "ctx no embeds alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_NO_EMBEDS_F(NULL, "ctx respond no embeds alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP_F(NULL, "ctx followup alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP_TEXT_F(NULL, "ctx followup text alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP_PRIVATE_F(NULL, "ctx followup private alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP_EPHEMERAL_F(NULL, "ctx followup ephemeral alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP_SILENT_F(NULL, "ctx followup silent alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_F(NULL, "ctx send alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_TEXT_F(NULL, "ctx send text alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_UPDATE_F(NULL, "ctx update alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_UPDATE_TEXT_F(NULL, "ctx update text alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_UPDATE_MESSAGE_F(NULL, "ctx update message alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_EDIT_F(NULL, "ctx edit alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_EDIT_TEXT_F(NULL, "ctx edit text alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_EDIT_ORIGINAL_F(NULL, "ctx edit original alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_DONE_F(NULL, "ctx done alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_ACK_F(NULL, "ctx ack alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SUCCESS_F(NULL, "Saved", "ctx success alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FAIL_F(NULL, "ctx fail alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_ERROR_F(NULL, "Error", "ctx error alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_DENY_F(NULL, "ctx deny alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_BAD_INPUT_F(NULL, "ctx bad input alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_NOT_FOUND_F(NULL, "ctx not found alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_BUSY_F(NULL, "ctx busy alias %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_BUSYF(NULL, "ctx busy %d", 13) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SENDF(NULL, 1ULL, "app %d", 14) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_F(NULL, 1ULL, "app alias %d", 14) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_TEXT_F(NULL, 1ULL, "app text alias %d", 14) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SENDF(NULL, 1ULL, "channel %d", 14) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_TEXT_F(NULL, 1ULL, "channel text %d", 14) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SENDF((dcc_app_t *)1, 0ULL, "app %d", 15) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDITF(NULL, 1ULL, 2ULL, "app edit %d", 16) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_F(NULL, 1ULL, 2ULL, "app edit alias %d", 16) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_TEXT_F(NULL, 1ULL, 2ULL, "app edit text alias %d", 16) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_EDITF(NULL, 1ULL, 2ULL, "message edit %d", 16) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_EDIT_TEXT_F(NULL, 1ULL, 2ULL, "message edit text %d", 16) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDITF((dcc_app_t *)1, 0ULL, 2ULL, "app edit %d", 17) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDITF((dcc_app_t *)1, 1ULL, 0ULL, "app edit %d", 18) == DCC_ERR_INVALID_ARG
            ? 0
            : 1;
}

static int check_invalid_inputs(void) {
    char tiny[4];
    char buffer[DCC_FORMAT_TIMESTAMP_MAX];

    return
        dcc_sugar_format_user_mention(NULL, 0U, 123ULL) == DCC_ERR_INVALID_ARG &&
        dcc_sugar_format_user_mention(tiny, sizeof(tiny), 1234567890123456789ULL) == DCC_ERR_INVALID_ARG &&
        dcc_sugar_format_user_mention(tiny, sizeof(tiny), 0ULL) == DCC_ERR_INVALID_ARG &&
        dcc_sugar_format_command_mention(buffer, sizeof(buffer), "bad name", 123ULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_sugar_format_command_mention(buffer, sizeof(buffer), "bad:name", 123ULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_sugar_format_command_mention(buffer, sizeof(buffer), "rank", 0ULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_sugar_format_custom_emoji(tiny, sizeof(tiny), "party", 123ULL, 0U) == DCC_ERR_INVALID_ARG &&
        dcc_sugar_format_custom_emoji(buffer, sizeof(buffer), "bad-name", 123ULL, 0U) ==
            DCC_ERR_INVALID_ARG &&
        dcc_sugar_format_timestamp(buffer, sizeof(buffer), 1700000000, 'x') == DCC_ERR_INVALID_ARG &&
        strcmp(DCC_MENTION_COMMAND("bad name", 123ULL), "") == 0 &&
        strcmp(DCC_EMOJI("bad-name", 123ULL), "") == 0 &&
        strcmp(DCC_MENTION_USER(0ULL), "") == 0
            ? 0
            : 1;
}

int main(void) {
    if (check_format_functions() != 0) {
        return 1;
    }
    if (check_short_macros() != 0) {
        return 1;
    }
    if (check_custom_emoji_parse() != 0) {
        return 1;
    }
    if (check_alloc_format() != 0) {
        return 1;
    }
    if (check_formatted_reply_nulls() != 0) {
        return 1;
    }
    if (check_invalid_inputs() != 0) {
        return 1;
    }
    return 0;
}
