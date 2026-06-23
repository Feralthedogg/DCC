#ifndef DCC_SUGAR_CDN_H
#define DCC_SUGAR_CDN_H

#include <dcc/error.h>
#include <dcc/snowflake.h>

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define DCC_CDN_BASE_URL "https://cdn.discordapp.com"
#define DCC_CDN_URL_MAX 256U
#define DCC_CDN_DEFAULT_SIZE 1024U

static inline dcc_status_t dcc_sugar_cdn_done(char *buffer, size_t buffer_size, int written) {
    if (buffer == NULL || buffer_size == 0U || written < 0 || (size_t)written >= buffer_size) {
        if (buffer != NULL && buffer_size != 0U) {
            buffer[0] = '\0';
        }
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

static inline uint8_t dcc_sugar_cdn_size_valid(uint32_t image_size) {
    return image_size == 0U ||
        (image_size >= 16U &&
         image_size <= 4096U &&
         (image_size & (image_size - 1U)) == 0U)
            ? 1U
            : 0U;
}

static inline uint8_t dcc_sugar_cdn_hash_valid(const char *hash) {
    if (hash == NULL || hash[0] == '\0') {
        return 0U;
    }

    for (size_t i = 0U; hash[i] != '\0'; ++i) {
        unsigned char ch = (unsigned char)hash[i];
        if (i >= 128U ||
            !((ch >= 'A' && ch <= 'Z') ||
              (ch >= 'a' && ch <= 'z') ||
              (ch >= '0' && ch <= '9') ||
              ch == '_' ||
              ch == '-')) {
            return 0U;
        }
    }
    return 1U;
}

static inline uint8_t dcc_sugar_cdn_extension_valid(const char *extension) {
    return extension != NULL &&
        (strcmp(extension, "png") == 0 ||
         strcmp(extension, "jpg") == 0 ||
         strcmp(extension, "jpeg") == 0 ||
         strcmp(extension, "webp") == 0 ||
         strcmp(extension, "gif") == 0)
            ? 1U
            : 0U;
}

static inline dcc_status_t dcc_sugar_cdn_format_hashed_id_url(
    char *buffer,
    size_t buffer_size,
    const char *kind,
    dcc_snowflake_t id,
    const char *hash,
    const char *extension,
    uint32_t image_size
) {
    if (buffer == NULL ||
        buffer_size == 0U ||
        kind == NULL ||
        id == 0U ||
        !dcc_sugar_cdn_hash_valid(hash) ||
        !dcc_sugar_cdn_extension_valid(extension) ||
        !dcc_sugar_cdn_size_valid(image_size)) {
        return dcc_sugar_cdn_done(buffer, buffer_size, -1);
    }

    if (image_size == 0U) {
        return dcc_sugar_cdn_done(
            buffer,
            buffer_size,
            snprintf(
                buffer,
                buffer_size,
                DCC_CDN_BASE_URL "/%s/%" PRIu64 "/%s.%s",
                kind,
                (uint64_t)id,
                hash,
                extension
            )
        );
    }

    return dcc_sugar_cdn_done(
        buffer,
        buffer_size,
        snprintf(
            buffer,
            buffer_size,
            DCC_CDN_BASE_URL "/%s/%" PRIu64 "/%s.%s?size=%" PRIu32,
            kind,
            (uint64_t)id,
            hash,
            extension,
            image_size
        )
    );
}

static inline dcc_status_t dcc_sugar_cdn_user_avatar_url(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t user_id,
    const char *avatar_hash,
    const char *extension,
    uint32_t image_size
) {
    return dcc_sugar_cdn_format_hashed_id_url(
        buffer,
        buffer_size,
        "avatars",
        user_id,
        avatar_hash,
        extension,
        image_size
    );
}

static inline dcc_status_t dcc_sugar_cdn_guild_icon_url(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t guild_id,
    const char *icon_hash,
    const char *extension,
    uint32_t image_size
) {
    return dcc_sugar_cdn_format_hashed_id_url(
        buffer,
        buffer_size,
        "icons",
        guild_id,
        icon_hash,
        extension,
        image_size
    );
}

static inline dcc_status_t dcc_sugar_cdn_role_icon_url(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t role_id,
    const char *icon_hash,
    const char *extension,
    uint32_t image_size
) {
    return dcc_sugar_cdn_format_hashed_id_url(
        buffer,
        buffer_size,
        "role-icons",
        role_id,
        icon_hash,
        extension,
        image_size
    );
}

static inline dcc_status_t dcc_sugar_cdn_member_avatar_url(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    const char *avatar_hash,
    const char *extension,
    uint32_t image_size
) {
    if (buffer == NULL ||
        buffer_size == 0U ||
        guild_id == 0U ||
        user_id == 0U ||
        !dcc_sugar_cdn_hash_valid(avatar_hash) ||
        !dcc_sugar_cdn_extension_valid(extension) ||
        !dcc_sugar_cdn_size_valid(image_size)) {
        return dcc_sugar_cdn_done(buffer, buffer_size, -1);
    }

    if (image_size == 0U) {
        return dcc_sugar_cdn_done(
            buffer,
            buffer_size,
            snprintf(
                buffer,
                buffer_size,
                DCC_CDN_BASE_URL "/guilds/%" PRIu64 "/users/%" PRIu64 "/avatars/%s.%s",
                (uint64_t)guild_id,
                (uint64_t)user_id,
                avatar_hash,
                extension
            )
        );
    }

    return dcc_sugar_cdn_done(
        buffer,
        buffer_size,
        snprintf(
            buffer,
            buffer_size,
            DCC_CDN_BASE_URL "/guilds/%" PRIu64 "/users/%" PRIu64 "/avatars/%s.%s?size=%" PRIu32,
            (uint64_t)guild_id,
            (uint64_t)user_id,
            avatar_hash,
            extension,
            image_size
        )
    );
}

static inline dcc_status_t dcc_sugar_cdn_emoji_url(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t emoji_id,
    const char *extension,
    uint32_t image_size
) {
    if (buffer == NULL ||
        buffer_size == 0U ||
        emoji_id == 0U ||
        !dcc_sugar_cdn_extension_valid(extension) ||
        !dcc_sugar_cdn_size_valid(image_size)) {
        return dcc_sugar_cdn_done(buffer, buffer_size, -1);
    }

    if (image_size == 0U) {
        return dcc_sugar_cdn_done(
            buffer,
            buffer_size,
            snprintf(
                buffer,
                buffer_size,
                DCC_CDN_BASE_URL "/emojis/%" PRIu64 ".%s",
                (uint64_t)emoji_id,
                extension
            )
        );
    }

    return dcc_sugar_cdn_done(
        buffer,
        buffer_size,
        snprintf(
            buffer,
            buffer_size,
            DCC_CDN_BASE_URL "/emojis/%" PRIu64 ".%s?size=%" PRIu32,
            (uint64_t)emoji_id,
            extension,
            image_size
        )
    );
}

static inline dcc_status_t dcc_sugar_cdn_default_avatar_url(
    char *buffer,
    size_t buffer_size,
    uint8_t index
) {
    if (buffer == NULL || buffer_size == 0U || index > 5U) {
        return dcc_sugar_cdn_done(buffer, buffer_size, -1);
    }
    return dcc_sugar_cdn_done(
        buffer,
        buffer_size,
        snprintf(buffer, buffer_size, DCC_CDN_BASE_URL "/embed/avatars/%u.png", (unsigned)index)
    );
}

static inline const char *dcc_sugar_cdn_user_avatar(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t user_id,
    const char *avatar_hash,
    const char *extension,
    uint32_t image_size
) {
    return dcc_sugar_cdn_user_avatar_url(
        buffer,
        buffer_size,
        user_id,
        avatar_hash,
        extension,
        image_size
    ) == DCC_OK
        ? buffer
        : "";
}

static inline const char *dcc_sugar_cdn_guild_icon(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t guild_id,
    const char *icon_hash,
    const char *extension,
    uint32_t image_size
) {
    return dcc_sugar_cdn_guild_icon_url(
        buffer,
        buffer_size,
        guild_id,
        icon_hash,
        extension,
        image_size
    ) == DCC_OK
        ? buffer
        : "";
}

static inline const char *dcc_sugar_cdn_member_avatar(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    const char *avatar_hash,
    const char *extension,
    uint32_t image_size
) {
    return dcc_sugar_cdn_member_avatar_url(
        buffer,
        buffer_size,
        guild_id,
        user_id,
        avatar_hash,
        extension,
        image_size
    ) == DCC_OK
        ? buffer
        : "";
}

static inline const char *dcc_sugar_cdn_role_icon(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t role_id,
    const char *icon_hash,
    const char *extension,
    uint32_t image_size
) {
    return dcc_sugar_cdn_role_icon_url(
        buffer,
        buffer_size,
        role_id,
        icon_hash,
        extension,
        image_size
    ) == DCC_OK
        ? buffer
        : "";
}

static inline const char *dcc_sugar_cdn_emoji(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t emoji_id,
    const char *extension,
    uint32_t image_size
) {
    return dcc_sugar_cdn_emoji_url(buffer, buffer_size, emoji_id, extension, image_size) == DCC_OK
        ? buffer
        : "";
}

static inline const char *dcc_sugar_cdn_default_avatar(
    char *buffer,
    size_t buffer_size,
    uint8_t index
) {
    return dcc_sugar_cdn_default_avatar_url(buffer, buffer_size, index) == DCC_OK ? buffer : "";
}

#define DCC_CDN_USER_AVATAR(user_id_, avatar_hash_) \
    dcc_sugar_cdn_user_avatar( \
        (char[DCC_CDN_URL_MAX]){ 0 }, \
        DCC_CDN_URL_MAX, \
        (user_id_), \
        (avatar_hash_), \
        "png", \
        DCC_CDN_DEFAULT_SIZE \
    )

#define DCC_CDN_USER_AVATAR_EXT(user_id_, avatar_hash_, extension_, image_size_) \
    dcc_sugar_cdn_user_avatar( \
        (char[DCC_CDN_URL_MAX]){ 0 }, \
        DCC_CDN_URL_MAX, \
        (user_id_), \
        (avatar_hash_), \
        (extension_), \
        (image_size_) \
    )

#define DCC_CDN_GUILD_ICON(guild_id_, icon_hash_) \
    dcc_sugar_cdn_guild_icon( \
        (char[DCC_CDN_URL_MAX]){ 0 }, \
        DCC_CDN_URL_MAX, \
        (guild_id_), \
        (icon_hash_), \
        "png", \
        DCC_CDN_DEFAULT_SIZE \
    )

#define DCC_CDN_GUILD_ICON_EXT(guild_id_, icon_hash_, extension_, image_size_) \
    dcc_sugar_cdn_guild_icon( \
        (char[DCC_CDN_URL_MAX]){ 0 }, \
        DCC_CDN_URL_MAX, \
        (guild_id_), \
        (icon_hash_), \
        (extension_), \
        (image_size_) \
    )

#define DCC_CDN_MEMBER_AVATAR(guild_id_, user_id_, avatar_hash_) \
    dcc_sugar_cdn_member_avatar( \
        (char[DCC_CDN_URL_MAX]){ 0 }, \
        DCC_CDN_URL_MAX, \
        (guild_id_), \
        (user_id_), \
        (avatar_hash_), \
        "png", \
        DCC_CDN_DEFAULT_SIZE \
    )

#define DCC_CDN_MEMBER_AVATAR_EXT(guild_id_, user_id_, avatar_hash_, extension_, image_size_) \
    dcc_sugar_cdn_member_avatar( \
        (char[DCC_CDN_URL_MAX]){ 0 }, \
        DCC_CDN_URL_MAX, \
        (guild_id_), \
        (user_id_), \
        (avatar_hash_), \
        (extension_), \
        (image_size_) \
    )

#define DCC_CDN_ROLE_ICON(role_id_, icon_hash_) \
    dcc_sugar_cdn_role_icon( \
        (char[DCC_CDN_URL_MAX]){ 0 }, \
        DCC_CDN_URL_MAX, \
        (role_id_), \
        (icon_hash_), \
        "png", \
        DCC_CDN_DEFAULT_SIZE \
    )

#define DCC_CDN_ROLE_ICON_EXT(role_id_, icon_hash_, extension_, image_size_) \
    dcc_sugar_cdn_role_icon( \
        (char[DCC_CDN_URL_MAX]){ 0 }, \
        DCC_CDN_URL_MAX, \
        (role_id_), \
        (icon_hash_), \
        (extension_), \
        (image_size_) \
    )

#define DCC_CDN_EMOJI(emoji_id_) \
    dcc_sugar_cdn_emoji((char[DCC_CDN_URL_MAX]){ 0 }, DCC_CDN_URL_MAX, (emoji_id_), "webp", 0U)

#define DCC_CDN_EMOJI_EXT(emoji_id_, extension_, image_size_) \
    dcc_sugar_cdn_emoji( \
        (char[DCC_CDN_URL_MAX]){ 0 }, \
        DCC_CDN_URL_MAX, \
        (emoji_id_), \
        (extension_), \
        (image_size_) \
    )

#define DCC_CDN_EMOJI_GIF(emoji_id_) \
    dcc_sugar_cdn_emoji((char[DCC_CDN_URL_MAX]){ 0 }, DCC_CDN_URL_MAX, (emoji_id_), "gif", 0U)

#define DCC_CDN_DEFAULT_AVATAR(index_) \
    dcc_sugar_cdn_default_avatar((char[DCC_CDN_URL_MAX]){ 0 }, DCC_CDN_URL_MAX, (index_))

#endif
