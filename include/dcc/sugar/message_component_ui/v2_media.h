#ifndef DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_MEDIA_H
#define DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_MEDIA_H

#include <dcc/sugar/message_component_ui/v2_core.h>

#define DCC_V2_MEDIA(url_, description_) \
    ((dcc_component_v2_media_t){ \
        .url = (url_), \
        .description = (description_) \
    })
#define DCC_V2_MEDIA_SPOILER(url_, description_) \
    ((dcc_component_v2_media_t){ \
        .url = (url_), \
        .description = (description_), \
        .spoiler = 1U, \
        .has_spoiler = 1U \
    })
#define DCC_V2_MEDIA_GALLERY_ARRAY(media_, media_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_MEDIA_GALLERY, \
        .media = (media_), \
        .media_count = (media_count_) \
    })
#define DCC_V2_MEDIA_GALLERY(...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_MEDIA_GALLERY, \
        .media = DCC_SUGAR_ARRAY(dcc_component_v2_media_t, __VA_ARGS__), \
        .media_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_media_t, __VA_ARGS__) \
    })
#define DCC_V2_THUMBNAIL_ARRAY(media_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_THUMBNAIL, \
        .media = (media_), \
        .media_count = 1U \
    })
#define DCC_V2_THUMBNAIL(media_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_THUMBNAIL, \
        .media = DCC_SUGAR_PTR(dcc_component_v2_media_t, media_), \
        .media_count = 1U \
    })
#define DCC_V2_FILE_ARRAY(media_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_FILE, \
        .media = (media_), \
        .media_count = 1U \
    })
#define DCC_V2_FILE(media_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_FILE, \
        .media = DCC_SUGAR_PTR(dcc_component_v2_media_t, media_), \
        .media_count = 1U \
    })

#endif /* DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_MEDIA_H */
