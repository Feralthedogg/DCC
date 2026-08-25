#ifndef DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_MEDIA_H
#define DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_MEDIA_H

#include <dcc/sugar/message_component_ui/v2_core.h>

#define DCC_V2_MEDIA(url_, description_) \
    ((dcc_component_v2_media_gallery_item_t){ \
        .media = {(url_)}, \
        .description = (description_) \
    })
#define DCC_V2_MEDIA_SPOILER(url_, description_) \
    ((dcc_component_v2_media_gallery_item_t){ \
        .media = {(url_)}, \
        .description = (description_), \
        .spoiler = 1U, \
        .has_spoiler = 1U \
    })
#define DCC_V2_MEDIA_GALLERY_ARRAY(media_, media_count_) \
    dcc_component_v2_media_gallery((media_), (media_count_))
#define DCC_V2_MEDIA_GALLERY(...) \
    dcc_component_v2_media_gallery( \
        DCC_SUGAR_ARRAY(dcc_component_v2_media_gallery_item_t, __VA_ARGS__), \
        DCC_SUGAR_ARRAY_LEN(dcc_component_v2_media_gallery_item_t, __VA_ARGS__))
#define DCC_V2_THUMBNAIL_ARRAY(media_) \
    ((dcc_component_v2_builder_t){.size=sizeof(dcc_component_v2_builder_t), .version=DCC_COMPONENT_V2_BUILDER_VERSION, .present=(UINT64_C(1)<<DCC_COMPONENT_V2_FIELD_MEDIA), .type=DCC_COMPONENT_V2_THUMBNAIL, .as.media.thumbnail={(media_)->media, (media_)->description, (media_)->spoiler}})
#define DCC_V2_THUMBNAIL(media_) \
    ((dcc_component_v2_builder_t){.size=sizeof(dcc_component_v2_builder_t), .version=DCC_COMPONENT_V2_BUILDER_VERSION, .present=(UINT64_C(1)<<DCC_COMPONENT_V2_FIELD_MEDIA) | ((media_).description != NULL ? (UINT64_C(1)<<DCC_COMPONENT_V2_FIELD_DESCRIPTION) : 0U) | ((media_).has_spoiler ? (UINT64_C(1)<<DCC_COMPONENT_V2_FIELD_SPOILER) : 0U), .type=DCC_COMPONENT_V2_THUMBNAIL, .as.media.thumbnail={(media_).media, (media_).description, (media_).spoiler}})
#define DCC_V2_FILE_ARRAY(media_) \
    ((dcc_component_v2_builder_t){.size=sizeof(dcc_component_v2_builder_t), .version=DCC_COMPONENT_V2_BUILDER_VERSION, .present=(UINT64_C(1)<<DCC_COMPONENT_V2_FIELD_MEDIA), .type=DCC_COMPONENT_V2_FILE, .as.media.file={(media_)->media, (media_)->spoiler}})
#define DCC_V2_FILE(media_) \
    ((dcc_component_v2_builder_t){.size=sizeof(dcc_component_v2_builder_t), .version=DCC_COMPONENT_V2_BUILDER_VERSION, .present=(UINT64_C(1)<<DCC_COMPONENT_V2_FIELD_MEDIA) | ((media_).has_spoiler ? (UINT64_C(1)<<DCC_COMPONENT_V2_FIELD_SPOILER) : 0U), .type=DCC_COMPONENT_V2_FILE, .as.media.file={(media_).media, (media_).spoiler}})

#endif /* DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_MEDIA_H */
