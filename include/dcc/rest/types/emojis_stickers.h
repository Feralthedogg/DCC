#ifndef DCC_REST_TYPES_EMOJIS_STICKERS_H
#define DCC_REST_TYPES_EMOJIS_STICKERS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  DCC_EMOJI_PARAMS_VERSION = 1U,
  DCC_GUILD_STICKER_PARAMS_VERSION = 1U,
  DCC_EMOJI_PARAMS_PRESENT_NAME = UINT64_C(1),
  DCC_EMOJI_PARAMS_PRESENT_IMAGE = UINT64_C(1) << 1U,
  DCC_EMOJI_PARAMS_PRESENT_ROLES = UINT64_C(1) << 2U,
  DCC_GUILD_STICKER_PARAMS_PRESENT_NAME = UINT64_C(1),
  DCC_GUILD_STICKER_PARAMS_PRESENT_DESCRIPTION = UINT64_C(1) << 1U,
  DCC_GUILD_STICKER_PARAMS_PRESENT_TAGS = UINT64_C(1) << 2U,
  DCC_GUILD_STICKER_PARAMS_PRESENT_FILE = UINT64_C(1) << 3U
};

typedef struct dcc_guild_sticker_params {
    size_t size;
    uint32_t version;
    uint64_t present;
    uint64_t nulls;
    const char *name;
    const char *description;
    const char *tags;
    dcc_rest_multipart_file_t file;
    dcc_snowflake_t guild_id;
    const char *filename;
    const char *content_type;
    const void *data;
    size_t data_len;
} dcc_guild_sticker_params_t;

typedef struct dcc_emoji_params {
    size_t size;
    uint32_t version;
    uint64_t present;
    uint64_t nulls;
    const char *name;
    const char *image;
    const dcc_snowflake_t *roles;
    size_t role_count;
    dcc_snowflake_t emoji_id;
} dcc_emoji_params_t;

#define DCC_GUILD_STICKER_PARAMS_INIT                                          \
  {sizeof(dcc_guild_sticker_params_t), DCC_GUILD_STICKER_PARAMS_VERSION,       \
   UINT64_C(0), UINT64_C(0), NULL, NULL, NULL, \
   {NULL, NULL, NULL, NULL, 0U}, UINT64_C(0), NULL, NULL, NULL, UINT64_C(0)}
#define DCC_EMOJI_PARAMS_INIT                                                  \
  {sizeof(dcc_emoji_params_t), DCC_EMOJI_PARAMS_VERSION, UINT64_C(0),          \
   UINT64_C(0), NULL, NULL, NULL, 0U, UINT64_C(0)}

static inline void dcc_guild_sticker_params_init(
    dcc_guild_sticker_params_t *params) {
  if (params != NULL) {
    dcc_guild_sticker_params_t value = DCC_GUILD_STICKER_PARAMS_INIT;
    *params = value;
  }
}

static inline void dcc_emoji_params_init(dcc_emoji_params_t *params) {
  if (params != NULL) {
    dcc_emoji_params_t value = DCC_EMOJI_PARAMS_INIT;
    *params = value;
  }
}

#ifdef __cplusplus
}
#endif

#endif
