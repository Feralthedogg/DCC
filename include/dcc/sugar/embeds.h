#ifndef DCC_SUGAR_EMBEDS_H
#define DCC_SUGAR_EMBEDS_H

#include <dcc/embed.h>
#include <dcc/message.h>

#define DCC_EMBED_FIELDS(...) ((dcc_embed_field_t[]){ __VA_ARGS__ })
#define DCC_EMBED_FIELDS_COUNT(...) \
    (sizeof((dcc_embed_field_t[]){ __VA_ARGS__ }) / sizeof(dcc_embed_field_t))

static inline dcc_embed_builder_t dcc_sugar_embed_make(
    const char *title,
    const char *description,
    uint32_t color,
    uint8_t has_color,
    const char *footer_text,
    const char *footer_icon_url,
    const char *image_url,
    const char *thumbnail_url,
    const char *author_name,
    const char *author_url,
    const char *author_icon_url,
    const dcc_embed_field_t *fields,
    size_t field_count
) {
    dcc_embed_builder_t embed = DCC_EMBED_BUILDER_INIT;
    embed.title = title;
    embed.description = description;
    embed.color = color;
    embed.footer.text = footer_text;
    embed.footer.icon_url = footer_icon_url;
    embed.image.url = image_url;
    embed.thumbnail.url = thumbnail_url;
    embed.author.name = author_name;
    embed.author.url = author_url;
    embed.author.icon_url = author_icon_url;
    embed.fields = fields;
    embed.field_count = field_count;
    if (title != NULL) {
        embed.present |= DCC_EMBED_BUILDER_PRESENT_TITLE;
    }
    if (description != NULL) {
        embed.present |= DCC_EMBED_BUILDER_PRESENT_DESCRIPTION;
    }
    if (has_color) {
        embed.present |= DCC_EMBED_BUILDER_PRESENT_COLOR;
    }
    if (footer_text != NULL) {
        embed.present |= DCC_EMBED_BUILDER_PRESENT_FOOTER;
    }
    if (image_url != NULL) {
        embed.present |= DCC_EMBED_BUILDER_PRESENT_IMAGE;
    }
    if (thumbnail_url != NULL) {
        embed.present |= DCC_EMBED_BUILDER_PRESENT_THUMBNAIL;
    }
    if (author_name != NULL) {
        embed.present |= DCC_EMBED_BUILDER_PRESENT_AUTHOR;
    }
    if (field_count != 0U) {
        embed.present |= DCC_EMBED_BUILDER_PRESENT_FIELDS;
    }
    return embed;
}

static inline dcc_embed_builder_t dcc_sugar_embed_with_url_make(
    const char *title,
    const char *description,
    const char *url
) {
    dcc_embed_builder_t embed = dcc_sugar_embed_make(
        title, description, 0U, 0U, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0U
    );
    embed.url = url;
    if (url != NULL) {
        embed.present |= DCC_EMBED_BUILDER_PRESENT_URL;
    }
    return embed;
}

#define DCC_EMBED_COLOR_FIELDS(title_, description_, color_, ...) \
    dcc_sugar_embed_make( \
        (title_), (description_), (color_), 1U, NULL, NULL, NULL, NULL, NULL, NULL, NULL, \
        DCC_EMBED_FIELDS(__VA_ARGS__), DCC_EMBED_FIELDS_COUNT(__VA_ARGS__) \
    )

#define DCC_EMBED_FOOTER_TEXT(title_, description_, footer_text_, footer_icon_url_) \
    dcc_sugar_embed_make( \
        (title_), (description_), 0U, 0U, (footer_text_), (footer_icon_url_), NULL, NULL, \
        NULL, NULL, NULL, NULL, 0U \
    )

#define DCC_EMBED_COLOR_FOOTER(title_, description_, color_, footer_text_, footer_icon_url_) \
    dcc_sugar_embed_make( \
        (title_), (description_), (color_), 1U, (footer_text_), (footer_icon_url_), NULL, \
        NULL, NULL, NULL, NULL, NULL, 0U \
    )

#define DCC_EMBED_COLOR_FOOTER_FIELDS( \
    title_, description_, color_, footer_text_, footer_icon_url_, ... \
) \
    dcc_sugar_embed_make( \
        (title_), (description_), (color_), 1U, (footer_text_), (footer_icon_url_), NULL, \
        NULL, NULL, NULL, NULL, DCC_EMBED_FIELDS(__VA_ARGS__), \
        DCC_EMBED_FIELDS_COUNT(__VA_ARGS__) \
    )

#define DCC_EMBED_COLOR_IMAGE(title_, description_, color_, image_url_) \
    dcc_sugar_embed_make( \
        (title_), (description_), (color_), 1U, NULL, NULL, (image_url_), NULL, NULL, NULL, \
        NULL, NULL, 0U \
    )

#define DCC_EMBED_COLOR_THUMBNAIL(title_, description_, color_, thumbnail_url_) \
    dcc_sugar_embed_make( \
        (title_), (description_), (color_), 1U, NULL, NULL, NULL, (thumbnail_url_), NULL, \
        NULL, NULL, NULL, 0U \
    )

#define DCC_EMBED_COLOR_AUTHOR( \
    title_, description_, color_, author_name_, author_url_, author_icon_url_ \
) \
    dcc_sugar_embed_make( \
        (title_), (description_), (color_), 1U, NULL, NULL, NULL, NULL, (author_name_), \
        (author_url_), (author_icon_url_), NULL, 0U \
    )

#define DCC_EMBED_RICH( \
    title_, description_, color_, footer_text_, footer_icon_url_, image_url_, thumbnail_url_, \
    author_name_, author_url_, author_icon_url_ \
) \
    dcc_sugar_embed_make( \
        (title_), (description_), (color_), 1U, (footer_text_), (footer_icon_url_), \
        (image_url_), (thumbnail_url_), (author_name_), (author_url_), (author_icon_url_), \
        NULL, 0U \
    )

#define DCC_EMBED_RICH_FIELDS( \
    title_, description_, color_, footer_text_, footer_icon_url_, image_url_, thumbnail_url_, \
    author_name_, author_url_, author_icon_url_, ... \
) \
    dcc_sugar_embed_make( \
        (title_), (description_), (color_), 1U, (footer_text_), (footer_icon_url_), \
        (image_url_), (thumbnail_url_), (author_name_), (author_url_), (author_icon_url_), \
        DCC_EMBED_FIELDS(__VA_ARGS__), DCC_EMBED_FIELDS_COUNT(__VA_ARGS__) \
    )

#define DCC_MESSAGE_EMBED(embed_) \
    ((dcc_message_builder_t){ \
        .size = sizeof(dcc_message_builder_t), \
        .version = DCC_MESSAGE_BUILDER_VERSION, \
        .present = DCC_MESSAGE_BUILDER_PRESENT_EMBEDS, \
        .embeds = (dcc_embed_builder_t[]){ (embed_) }, \
        .embeds_count = 1U \
    })

#define DCC_MESSAGE_TEXT_EMBED(content_, embed_) \
    ((dcc_message_builder_t){ \
        .size = sizeof(dcc_message_builder_t), \
        .version = DCC_MESSAGE_BUILDER_VERSION, \
        .present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT | DCC_MESSAGE_BUILDER_PRESENT_EMBEDS, \
        .content = (content_), \
        .embeds = (dcc_embed_builder_t[]){ (embed_) }, \
        .embeds_count = 1U \
    })

#define DCC_MESSAGE_EPHEMERAL_EMBED(embed_) \
    ((dcc_message_builder_t){ \
        .size = sizeof(dcc_message_builder_t), \
        .version = DCC_MESSAGE_BUILDER_VERSION, \
        .present = DCC_MESSAGE_BUILDER_PRESENT_EMBEDS | DCC_MESSAGE_BUILDER_PRESENT_FLAGS, \
        .embeds = (dcc_embed_builder_t[]){ (embed_) }, \
        .embeds_count = 1U, \
        .flags = DCC_MESSAGE_FLAG_EPHEMERAL \
    })

#define DCC_MESSAGE_EPHEMERAL_TEXT_EMBED(content_, embed_) \
    ((dcc_message_builder_t){ \
        .size = sizeof(dcc_message_builder_t), \
        .version = DCC_MESSAGE_BUILDER_VERSION, \
        .present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT | DCC_MESSAGE_BUILDER_PRESENT_EMBEDS | \
            DCC_MESSAGE_BUILDER_PRESENT_FLAGS, \
        .content = (content_), \
        .embeds = (dcc_embed_builder_t[]){ (embed_) }, \
        .embeds_count = 1U, \
        .flags = DCC_MESSAGE_FLAG_EPHEMERAL \
    })

#endif
