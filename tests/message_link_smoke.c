#include <dcc/message_link.h>
#include <dcc/sugar/links.h>

#include <stdio.h>
#include <string.h>

static int expect_parse(
    const char *text,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    uint8_t is_dm
) {
    dcc_message_link_t link = { 0 };
    dcc_status_t status = dcc_message_link_parse(text, &link);
    if (status != DCC_OK ||
        link.guild_id != guild_id ||
        link.channel_id != channel_id ||
        link.message_id != message_id ||
        link.is_dm != is_dm) {
        fprintf(stderr, "message link parse failed: %s status=%d\n", text, (int)status);
        return 1;
    }
    return 0;
}

static int expect_reject(const char *text) {
    dcc_message_link_t link = {
        .guild_id = 1U,
        .channel_id = 2U,
        .message_id = 3U
    };
    if (dcc_message_link_parse(text, &link) == DCC_OK) {
        fprintf(stderr, "message link should have failed: %s\n", text ? text : "(null)");
        return 1;
    }
    return 0;
}

static int check_format(void) {
    char buffer[DCC_MESSAGE_LINK_MAX];
    dcc_message_link_t link = {
        .guild_id = 111U,
        .channel_id = 222U,
        .message_id = 333U
    };
    dcc_message_link_t dm_link = {
        .channel_id = 444U,
        .message_id = 555U,
        .is_dm = 1U
    };

    return
        dcc_message_link_format(buffer, sizeof(buffer), &link) != DCC_OK ||
        strcmp(buffer, "https://discord.com/channels/111/222/333") != 0 ||
        strcmp(DCC_MESSAGE_LINK(111U, 222U, 333U), buffer) != 0 ||
        dcc_message_link_format(buffer, sizeof(buffer), &dm_link) != DCC_OK ||
        strcmp(buffer, "https://discord.com/channels/@me/444/555") != 0 ||
        strcmp(DCC_DM_MESSAGE_LINK(444U, 555U), buffer) != 0;
}

static int check_parse(void) {
    dcc_message_link_t link = { 0 };
    return
        expect_parse("https://discord.com/channels/111/222/333", 111U, 222U, 333U, 0U) ||
        expect_parse("http://discord.com/channels/111/222/333", 111U, 222U, 333U, 0U) ||
        expect_parse("https://canary.discord.com/channels/111/222/333", 111U, 222U, 333U, 0U) ||
        expect_parse("https://ptb.discord.com/channels/111/222/333", 111U, 222U, 333U, 0U) ||
        expect_parse("/channels/111/222/333", 111U, 222U, 333U, 0U) ||
        expect_parse("channels/@me/444/555", 0U, 444U, 555U, 1U) ||
        DCC_PARSE_MESSAGE_LINK("https://discord.com/channels/111/222/333", &link) != DCC_OK ||
        link.guild_id != 111U ||
        link.channel_id != 222U ||
        link.message_id != 333U;
}

static int check_rejections(void) {
    dcc_message_link_t link = {
        .channel_id = 1U,
        .message_id = 2U,
        .is_dm = 1U,
        .guild_id = 3U
    };
    char small[8];

    return
        dcc_message_link_format(small, sizeof(small), &link) == DCC_OK ||
        dcc_message_link_format(small, sizeof(small), NULL) == DCC_OK ||
        expect_reject(NULL) ||
        expect_reject("https://discord.com/channels/111/222") ||
        expect_reject("https://discord.com/channels/111/222/333/444") ||
        expect_reject("https://discord.com/channels/0/222/333") ||
        expect_reject("https://discord.com/channels/111/0/333") ||
        expect_reject("https://discord.com/channels/111/222/0") ||
        expect_reject("https://discord.com/channels/@everyone/222/333") ||
        expect_reject("https://discord.com/channels/18446744073709551616/222/333") ||
        expect_reject("https://example.com/channels/111/222/333");
}

int main(void) {
    if (check_format() != 0) {
        return 1;
    }
    if (check_parse() != 0) {
        return 1;
    }
    if (check_rejections() != 0) {
        return 1;
    }
    return 0;
}
