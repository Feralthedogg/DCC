#ifndef DCC_SUGAR_EMBEDS_H
#define DCC_SUGAR_EMBEDS_H

#include <dcc/embed.h>
#include <dcc/message.h>

#define DCC_EMBED_FIELDS(...) ((dcc_embed_field_t[]){ __VA_ARGS__ })
#define DCC_EMBED_FIELDS_COUNT(...) (sizeof((dcc_embed_field_t[]){ __VA_ARGS__ }) / sizeof(dcc_embed_field_t))

#define DCC_EMBED_COLOR_FIELDS(title_, description_, color_, ...) \
    ((dcc_embed_builder_t){ \
        .title = (title_), \
        .description = (description_), \
        .color = (color_), \
        .has_color = 1U, \
        .fields = DCC_EMBED_FIELDS(__VA_ARGS__), \
        .field_count = DCC_EMBED_FIELDS_COUNT(__VA_ARGS__) \
    })

#define DCC_EMBED_FOOTER_TEXT(title_, description_, footer_text_, footer_icon_url_) \
    ((dcc_embed_builder_t){ \
        .title = (title_), \
        .description = (description_), \
        .footer = { .text = (footer_text_), .icon_url = (footer_icon_url_) }, \
        .has_footer = (footer_text_) != NULL \
    })

#define DCC_EMBED_COLOR_FOOTER(title_, description_, color_, footer_text_, footer_icon_url_) \
    ((dcc_embed_builder_t){ \
        .title = (title_), \
        .description = (description_), \
        .color = (color_), \
        .has_color = 1U, \
        .footer = { .text = (footer_text_), .icon_url = (footer_icon_url_) }, \
        .has_footer = (footer_text_) != NULL \
    })

#define DCC_EMBED_COLOR_FOOTER_FIELDS(title_, description_, color_, footer_text_, footer_icon_url_, ...) \
    ((dcc_embed_builder_t){ \
        .title = (title_), \
        .description = (description_), \
        .color = (color_), \
        .has_color = 1U, \
        .footer = { .text = (footer_text_), .icon_url = (footer_icon_url_) }, \
        .has_footer = (footer_text_) != NULL, \
        .fields = DCC_EMBED_FIELDS(__VA_ARGS__), \
        .field_count = DCC_EMBED_FIELDS_COUNT(__VA_ARGS__) \
    })

#define DCC_EMBED_COLOR_IMAGE(title_, description_, color_, image_url_) \
    ((dcc_embed_builder_t){ \
        .title = (title_), \
        .description = (description_), \
        .color = (color_), \
        .has_color = 1U, \
        .image = { .url = (image_url_) }, \
        .has_image = (image_url_) != NULL \
    })

#define DCC_EMBED_COLOR_THUMBNAIL(title_, description_, color_, thumbnail_url_) \
    ((dcc_embed_builder_t){ \
        .title = (title_), \
        .description = (description_), \
        .color = (color_), \
        .has_color = 1U, \
        .thumbnail = { .url = (thumbnail_url_) }, \
        .has_thumbnail = (thumbnail_url_) != NULL \
    })

#define DCC_EMBED_COLOR_AUTHOR(title_, description_, color_, author_name_, author_url_, author_icon_url_) \
    ((dcc_embed_builder_t){ \
        .title = (title_), \
        .description = (description_), \
        .color = (color_), \
        .has_color = 1U, \
        .author = { .name = (author_name_), .url = (author_url_), .icon_url = (author_icon_url_) }, \
        .has_author = (author_name_) != NULL \
    })

#define DCC_EMBED_RICH( \
    title_, \
    description_, \
    color_, \
    footer_text_, \
    footer_icon_url_, \
    image_url_, \
    thumbnail_url_, \
    author_name_, \
    author_url_, \
    author_icon_url_ \
) \
    ((dcc_embed_builder_t){ \
        .title = (title_), \
        .description = (description_), \
        .color = (color_), \
        .has_color = 1U, \
        .footer = { .text = (footer_text_), .icon_url = (footer_icon_url_) }, \
        .image = { .url = (image_url_) }, \
        .thumbnail = { .url = (thumbnail_url_) }, \
        .author = { .name = (author_name_), .url = (author_url_), .icon_url = (author_icon_url_) }, \
        .has_footer = (footer_text_) != NULL, \
        .has_image = (image_url_) != NULL, \
        .has_thumbnail = (thumbnail_url_) != NULL, \
        .has_author = (author_name_) != NULL \
    })

#define DCC_EMBED_RICH_FIELDS( \
    title_, \
    description_, \
    color_, \
    footer_text_, \
    footer_icon_url_, \
    image_url_, \
    thumbnail_url_, \
    author_name_, \
    author_url_, \
    author_icon_url_, \
    ... \
) \
    ((dcc_embed_builder_t){ \
        .title = (title_), \
        .description = (description_), \
        .color = (color_), \
        .has_color = 1U, \
        .footer = { .text = (footer_text_), .icon_url = (footer_icon_url_) }, \
        .image = { .url = (image_url_) }, \
        .thumbnail = { .url = (thumbnail_url_) }, \
        .author = { .name = (author_name_), .url = (author_url_), .icon_url = (author_icon_url_) }, \
        .has_footer = (footer_text_) != NULL, \
        .has_image = (image_url_) != NULL, \
        .has_thumbnail = (thumbnail_url_) != NULL, \
        .has_author = (author_name_) != NULL, \
        .fields = DCC_EMBED_FIELDS(__VA_ARGS__), \
        .field_count = DCC_EMBED_FIELDS_COUNT(__VA_ARGS__) \
    })

#define DCC_MESSAGE_EMBED(embed_) \
    ((dcc_message_builder_t){ \
        .embeds = (dcc_embed_builder_t[]){ (embed_) }, \
        .embeds_count = 1U \
    })

#define DCC_MESSAGE_TEXT_EMBED(content_, embed_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .embeds = (dcc_embed_builder_t[]){ (embed_) }, \
        .embeds_count = 1U, \
        .has_content = 1U \
    })

#define DCC_MESSAGE_EPHEMERAL_EMBED(embed_) \
    ((dcc_message_builder_t){ \
        .embeds = (dcc_embed_builder_t[]){ (embed_) }, \
        .embeds_count = 1U, \
        .flags = DCC_MESSAGE_FLAG_EPHEMERAL, \
        .has_flags = 1U \
    })

#define DCC_MESSAGE_EPHEMERAL_TEXT_EMBED(content_, embed_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .embeds = (dcc_embed_builder_t[]){ (embed_) }, \
        .embeds_count = 1U, \
        .flags = DCC_MESSAGE_FLAG_EPHEMERAL, \
        .has_content = 1U, \
        .has_flags = 1U \
    })

#endif
