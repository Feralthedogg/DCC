#ifndef DCC_EMBED_H
#define DCC_EMBED_H

#include <dcc/error.h>
#include <dcc/export.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DCC_EMBED_BUILDER_VERSION 1U

#define DCC_EMBED_BUILDER_PRESENT_TITLE UINT64_C(1)
#define DCC_EMBED_BUILDER_PRESENT_TYPE (UINT64_C(1) << 1U)
#define DCC_EMBED_BUILDER_PRESENT_DESCRIPTION (UINT64_C(1) << 2U)
#define DCC_EMBED_BUILDER_PRESENT_URL (UINT64_C(1) << 3U)
#define DCC_EMBED_BUILDER_PRESENT_TIMESTAMP (UINT64_C(1) << 4U)
#define DCC_EMBED_BUILDER_PRESENT_FOOTER (UINT64_C(1) << 5U)
#define DCC_EMBED_BUILDER_PRESENT_IMAGE (UINT64_C(1) << 6U)
#define DCC_EMBED_BUILDER_PRESENT_THUMBNAIL (UINT64_C(1) << 7U)
#define DCC_EMBED_BUILDER_PRESENT_AUTHOR (UINT64_C(1) << 8U)
#define DCC_EMBED_BUILDER_PRESENT_FIELDS (UINT64_C(1) << 9U)
#define DCC_EMBED_BUILDER_PRESENT_COLOR (UINT64_C(1) << 10U)

typedef struct dcc_embed_field {
    const char *name;
    const char *value;
    uint8_t inline_field;
    uint8_t has_inline;
} dcc_embed_field_t;

typedef struct dcc_embed_footer {
    const char *text;
    const char *icon_url;
} dcc_embed_footer_t;

typedef struct dcc_embed_media {
    const char *url;
} dcc_embed_media_t;

typedef struct dcc_embed_author {
    const char *name;
    const char *url;
    const char *icon_url;
} dcc_embed_author_t;

typedef struct dcc_embed_builder {
    size_t size;
    uint32_t version;
    uint64_t present;
    const char *title;
    const char *type;
    const char *description;
    const char *url;
    const char *timestamp;
    dcc_embed_footer_t footer;
    dcc_embed_media_t image;
    dcc_embed_media_t thumbnail;
    dcc_embed_author_t author;
    const dcc_embed_field_t *fields;
    size_t field_count;
    uint32_t color;
} dcc_embed_builder_t;

#define DCC_EMBED_BUILDER_INIT \
    { \
        sizeof(dcc_embed_builder_t), DCC_EMBED_BUILDER_VERSION, UINT64_C(0), \
        NULL, NULL, NULL, NULL, NULL, { NULL, NULL }, { NULL }, { NULL }, \
        { NULL, NULL, NULL }, NULL, 0U, 0U \
    }

DCC_API void dcc_embed_builder_init(dcc_embed_builder_t *builder);
DCC_API dcc_status_t dcc_embed_builder_set_title(dcc_embed_builder_t *builder, const char *title);
DCC_API dcc_status_t dcc_embed_builder_set_type(dcc_embed_builder_t *builder, const char *type);
DCC_API dcc_status_t dcc_embed_builder_set_description(dcc_embed_builder_t *builder, const char *description);
DCC_API dcc_status_t dcc_embed_builder_set_url(dcc_embed_builder_t *builder, const char *url);
DCC_API dcc_status_t dcc_embed_builder_set_timestamp(dcc_embed_builder_t *builder, const char *timestamp);
DCC_API dcc_status_t dcc_embed_builder_set_color(dcc_embed_builder_t *builder, uint32_t color);
DCC_API dcc_status_t dcc_embed_builder_set_footer(
    dcc_embed_builder_t *builder,
    const char *text,
    const char *icon_url
);
DCC_API dcc_status_t dcc_embed_builder_set_image(dcc_embed_builder_t *builder, const char *url);
DCC_API dcc_status_t dcc_embed_builder_set_thumbnail(dcc_embed_builder_t *builder, const char *url);
DCC_API dcc_status_t dcc_embed_builder_set_author(
    dcc_embed_builder_t *builder,
    const char *name,
    const char *url,
    const char *icon_url
);
DCC_API dcc_status_t dcc_embed_builder_set_fields(
    dcc_embed_builder_t *builder,
    const dcc_embed_field_t *fields,
    size_t field_count
);
DCC_API dcc_status_t dcc_embed_builder_build_json(const dcc_embed_builder_t *builder, char **out_json);
DCC_API dcc_status_t dcc_embed_builder_build_array_json(
    const dcc_embed_builder_t *builders,
    size_t builder_count,
    char **out_json
);
DCC_API void dcc_embed_builder_json_free(char *json);

#ifdef __cplusplus
}
#endif

#endif
