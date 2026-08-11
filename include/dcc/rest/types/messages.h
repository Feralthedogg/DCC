#ifndef DCC_REST_TYPES_MESSAGES_H
#define DCC_REST_TYPES_MESSAGES_H

#include <dcc/rest/base.h>

#define DCC_REST_MESSAGE_PAYLOAD_VERSION 1U
#define DCC_REST_MESSAGE_LIST_QUERY_VERSION 1U
#define DCC_REST_ID_PAGE_VERSION 1U
#define DCC_REST_PIN_PAGE_VERSION 1U
#define DCC_REST_REACTION_QUERY_VERSION 1U

#define DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AROUND UINT64_C(1)
#define DCC_REST_MESSAGE_LIST_QUERY_PRESENT_BEFORE (UINT64_C(1) << 1U)
#define DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AFTER (UINT64_C(1) << 2U)
#define DCC_REST_MESSAGE_LIST_QUERY_PRESENT_LIMIT (UINT64_C(1) << 3U)

#define DCC_REST_ID_PAGE_PRESENT_BEFORE UINT64_C(1)
#define DCC_REST_ID_PAGE_PRESENT_AFTER (UINT64_C(1) << 1U)
#define DCC_REST_ID_PAGE_PRESENT_LIMIT (UINT64_C(1) << 2U)

#define DCC_REST_PIN_PAGE_PRESENT_BEFORE UINT64_C(1)
#define DCC_REST_PIN_PAGE_PRESENT_LIMIT (UINT64_C(1) << 1U)

#define DCC_REST_REACTION_QUERY_PRESENT_TYPE UINT64_C(1)
#define DCC_REST_REACTION_QUERY_PRESENT_AFTER (UINT64_C(1) << 1U)
#define DCC_REST_REACTION_QUERY_PRESENT_LIMIT (UINT64_C(1) << 2U)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_rest_message_payload {
    size_t size;
    uint32_t version;
    const dcc_message_builder_t *message;
    const dcc_rest_multipart_file_t *files;
    size_t file_count;
} dcc_rest_message_payload_t;

#define DCC_REST_MESSAGE_PAYLOAD_INIT \
    { sizeof(dcc_rest_message_payload_t), DCC_REST_MESSAGE_PAYLOAD_VERSION, \
      NULL, NULL, 0U }

static inline void dcc_rest_message_payload_init(
    dcc_rest_message_payload_t *payload,
    const dcc_message_builder_t *message
) {
    if (payload != NULL) {
        dcc_rest_message_payload_t value = DCC_REST_MESSAGE_PAYLOAD_INIT;
        value.message = message;
        *payload = value;
    }
}

typedef struct dcc_rest_message_list_query {
    size_t size;
    uint32_t version;
    uint64_t present;
    dcc_snowflake_t around;
    dcc_snowflake_t before;
    dcc_snowflake_t after;
    uint64_t limit;
} dcc_rest_message_list_query_t;

#define DCC_REST_MESSAGE_LIST_QUERY_INIT \
    { sizeof(dcc_rest_message_list_query_t), \
      DCC_REST_MESSAGE_LIST_QUERY_VERSION, UINT64_C(0), \
      UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0) }

static inline void dcc_rest_message_list_query_init(
    dcc_rest_message_list_query_t *query
) {
    if (query != NULL) {
        dcc_rest_message_list_query_t value = DCC_REST_MESSAGE_LIST_QUERY_INIT;
        *query = value;
    }
}

typedef struct dcc_rest_id_page {
    size_t size;
    uint32_t version;
    uint64_t present;
    dcc_snowflake_t before;
    dcc_snowflake_t after;
    uint64_t limit;
} dcc_rest_id_page_t;

#define DCC_REST_ID_PAGE_INIT \
    { sizeof(dcc_rest_id_page_t), DCC_REST_ID_PAGE_VERSION, UINT64_C(0), \
      UINT64_C(0), UINT64_C(0), UINT64_C(0) }

static inline void dcc_rest_id_page_init(dcc_rest_id_page_t *page) {
    if (page != NULL) {
        dcc_rest_id_page_t value = DCC_REST_ID_PAGE_INIT;
        *page = value;
    }
}

typedef struct dcc_rest_pin_page {
    size_t size;
    uint32_t version;
    uint64_t present;
    const char *before;
    uint64_t limit;
} dcc_rest_pin_page_t;

#define DCC_REST_PIN_PAGE_INIT \
    { sizeof(dcc_rest_pin_page_t), DCC_REST_PIN_PAGE_VERSION, UINT64_C(0), \
      NULL, UINT64_C(0) }

static inline void dcc_rest_pin_page_init(dcc_rest_pin_page_t *page) {
    if (page != NULL) {
        dcc_rest_pin_page_t value = DCC_REST_PIN_PAGE_INIT;
        *page = value;
    }
}

typedef enum dcc_rest_reaction_type {
    DCC_REST_REACTION_NORMAL = 0,
    DCC_REST_REACTION_BURST = 1
} dcc_rest_reaction_type_t;

typedef struct dcc_rest_reaction_query {
    size_t size;
    uint32_t version;
    uint64_t present;
    dcc_rest_reaction_type_t type;
    dcc_snowflake_t after;
    uint64_t limit;
} dcc_rest_reaction_query_t;

#define DCC_REST_REACTION_QUERY_INIT \
    { sizeof(dcc_rest_reaction_query_t), DCC_REST_REACTION_QUERY_VERSION, \
      UINT64_C(0), DCC_REST_REACTION_NORMAL, UINT64_C(0), UINT64_C(0) }

static inline void dcc_rest_reaction_query_init(
    dcc_rest_reaction_query_t *query
) {
    if (query != NULL) {
        dcc_rest_reaction_query_t value = DCC_REST_REACTION_QUERY_INIT;
        *query = value;
    }
}

typedef enum dcc_message_search_has {
    DCC_MESSAGE_SEARCH_HAS_IMAGE = 0,
    DCC_MESSAGE_SEARCH_HAS_SOUND,
    DCC_MESSAGE_SEARCH_HAS_VIDEO,
    DCC_MESSAGE_SEARCH_HAS_FILE,
    DCC_MESSAGE_SEARCH_HAS_STICKER,
    DCC_MESSAGE_SEARCH_HAS_EMBED,
    DCC_MESSAGE_SEARCH_HAS_LINK,
    DCC_MESSAGE_SEARCH_HAS_POLL,
    DCC_MESSAGE_SEARCH_HAS_SNAPSHOT
} dcc_message_search_has_t;

typedef enum dcc_message_search_sort {
    DCC_MESSAGE_SEARCH_SORT_TIMESTAMP = 0,
    DCC_MESSAGE_SEARCH_SORT_RELEVANCE
} dcc_message_search_sort_t;

typedef enum dcc_message_search_order {
    DCC_MESSAGE_SEARCH_ORDER_DESC = 0,
    DCC_MESSAGE_SEARCH_ORDER_ASC
} dcc_message_search_order_t;

typedef struct dcc_message_search_params {
    size_t size;
    const char *content;
    const dcc_snowflake_t *author_ids;
    size_t author_id_count;
    const dcc_snowflake_t *mentions;
    size_t mention_count;
    const dcc_snowflake_t *channel_ids;
    size_t channel_id_count;
    const dcc_snowflake_t *mention_role_ids;
    size_t mention_role_id_count;
    const dcc_snowflake_t *replied_to_user_ids;
    size_t replied_to_user_id_count;
    const dcc_snowflake_t *replied_to_message_ids;
    size_t replied_to_message_id_count;
    const char * const *author_types;
    size_t author_type_count;
    const dcc_message_search_has_t *has;
    size_t has_count;
    const char * const *embed_types;
    size_t embed_type_count;
    const char * const *embed_providers;
    size_t embed_provider_count;
    const char * const *link_hostnames;
    size_t link_hostname_count;
    const char * const *attachment_filenames;
    size_t attachment_filename_count;
    const char * const *attachment_extensions;
    size_t attachment_extension_count;
    dcc_snowflake_t max_id;
    dcc_snowflake_t min_id;
    uint8_t has_pinned;
    uint8_t pinned;
    uint8_t has_mention_everyone;
    uint8_t mention_everyone;
    uint8_t has_include_nsfw;
    uint8_t include_nsfw;
    uint8_t has_limit;
    uint32_t limit;
    uint8_t has_offset;
    uint32_t offset;
    uint8_t has_slop;
    uint32_t slop;
    uint8_t has_sort_by;
    dcc_message_search_sort_t sort_by;
    uint8_t has_sort_order;
    dcc_message_search_order_t sort_order;
} dcc_message_search_params_t;

#ifdef __cplusplus
}
#endif

#endif
