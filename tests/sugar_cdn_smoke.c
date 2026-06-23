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

static int check_default_macros(void) {
    char line[640];
    int written = snprintf(
        line,
        sizeof(line),
        "%s\n%s\n%s\n%s\n%s\n%s\n%s",
        DCC_CDN_USER_AVATAR(111ULL, "abc_hash"),
        DCC_CDN_GUILD_ICON(222ULL, "guild_icon"),
        DCC_CDN_MEMBER_AVATAR(333ULL, 444ULL, "member_avatar"),
        DCC_CDN_ROLE_ICON(555ULL, "role_icon"),
        DCC_CDN_EMOJI(666ULL),
        DCC_CDN_EMOJI_GIF(777ULL),
        DCC_CDN_DEFAULT_AVATAR(3U)
    );

    if (written < 0 || (size_t)written >= sizeof(line)) {
        return 1;
    }

    return check_text(
        line,
        "https://cdn.discordapp.com/avatars/111/abc_hash.png?size=1024\n"
        "https://cdn.discordapp.com/icons/222/guild_icon.png?size=1024\n"
        "https://cdn.discordapp.com/guilds/333/users/444/avatars/member_avatar.png?size=1024\n"
        "https://cdn.discordapp.com/role-icons/555/role_icon.png?size=1024\n"
        "https://cdn.discordapp.com/emojis/666.webp\n"
        "https://cdn.discordapp.com/emojis/777.gif\n"
        "https://cdn.discordapp.com/embed/avatars/3.png"
    );
}

static int check_checked_functions(void) {
    char url[DCC_CDN_URL_MAX];

    if (dcc_sugar_cdn_user_avatar_url(url, sizeof(url), 111ULL, "a_hash", "webp", 4096U) != DCC_OK ||
        check_text(url, "https://cdn.discordapp.com/avatars/111/a_hash.webp?size=4096") != 0 ||
        dcc_sugar_cdn_guild_icon_url(url, sizeof(url), 222ULL, "guild", "png", 0U) != DCC_OK ||
        check_text(url, "https://cdn.discordapp.com/icons/222/guild.png") != 0 ||
        dcc_sugar_cdn_emoji_url(url, sizeof(url), 333ULL, "webp", 128U) != DCC_OK ||
        check_text(url, "https://cdn.discordapp.com/emojis/333.webp?size=128") != 0 ||
        dcc_sugar_cdn_default_avatar_url(url, sizeof(url), 5U) != DCC_OK ||
        check_text(url, "https://cdn.discordapp.com/embed/avatars/5.png") != 0) {
        return 1;
    }

    return 0;
}

static int check_invalid_inputs(void) {
    char tiny[8];
    char url[DCC_CDN_URL_MAX];

    return
        dcc_sugar_cdn_user_avatar_url(NULL, 0U, 111ULL, "hash", "png", 1024U) ==
            DCC_ERR_INVALID_ARG &&
        dcc_sugar_cdn_user_avatar_url(tiny, sizeof(tiny), 111ULL, "hash", "png", 1024U) ==
            DCC_ERR_INVALID_ARG &&
        dcc_sugar_cdn_user_avatar_url(url, sizeof(url), 0ULL, "hash", "png", 1024U) ==
            DCC_ERR_INVALID_ARG &&
        dcc_sugar_cdn_user_avatar_url(url, sizeof(url), 111ULL, "bad/hash", "png", 1024U) ==
            DCC_ERR_INVALID_ARG &&
        dcc_sugar_cdn_user_avatar_url(url, sizeof(url), 111ULL, "hash", ".png", 1024U) ==
            DCC_ERR_INVALID_ARG &&
        dcc_sugar_cdn_user_avatar_url(url, sizeof(url), 111ULL, "hash", "png", 1000U) ==
            DCC_ERR_INVALID_ARG &&
        dcc_sugar_cdn_user_avatar_url(url, sizeof(url), 111ULL, "hash", "png", 8192U) ==
            DCC_ERR_INVALID_ARG &&
        dcc_sugar_cdn_member_avatar_url(url, sizeof(url), 0ULL, 111ULL, "hash", "png", 1024U) ==
            DCC_ERR_INVALID_ARG &&
        dcc_sugar_cdn_emoji_url(url, sizeof(url), 0ULL, "webp", 0U) == DCC_ERR_INVALID_ARG &&
        dcc_sugar_cdn_default_avatar_url(url, sizeof(url), 6U) == DCC_ERR_INVALID_ARG &&
        strcmp(DCC_CDN_USER_AVATAR(111ULL, "bad/hash"), "") == 0
            ? 0
            : 1;
}

int main(void) {
    if (check_default_macros() != 0) {
        return 1;
    }
    if (check_checked_functions() != 0) {
        return 1;
    }
    if (check_invalid_inputs() != 0) {
        return 1;
    }
    return 0;
}
