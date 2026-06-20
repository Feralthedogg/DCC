#ifndef DCC_OBJECTS_BASE_H
#define DCC_OBJECTS_BASE_H

/* This header is part of the DCC public object model. */

#include <dcc/component_v2.h>
#include <dcc/export.h>
#include <dcc/permissions.h>
#include <dcc/snowflake.h>

#include <stddef.h>
#include <stdint.h>

typedef struct dcc_user {
    dcc_snowflake_t id;
    const char *username;
    const char *global_name;
    uint16_t discriminator;
    uint8_t bot;
    uint32_t flags;
    const char *avatar;
} dcc_user_t;

typedef struct dcc_attachment {
    dcc_snowflake_t id;
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint32_t flags;
    uint8_t ephemeral;
    double duration_secs;
    const char *filename;
    const char *description;
    const char *url;
    const char *proxy_url;
    const char *content_type;
    const char *waveform;
} dcc_attachment_t;

typedef struct dcc_message {
    dcc_snowflake_t id;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t guild_id;
    dcc_user_t author;
    const char *content;
    uint64_t flags;
    const char *timestamp;
    const char *edited_timestamp;
    const char *components_json;
    const dcc_component_v2_t *components;
    size_t components_count;
    dcc_snowflake_t webhook_id;
    uint32_t type;
    uint8_t component_root_indices[DCC_COMPONENT_V2_MAX_COMPONENTS];
    size_t component_root_count;
    uint8_t tts;
    uint8_t mention_everyone;
    uint8_t pinned;
    uint8_t components_json_truncated;
    uint8_t components_truncated;
} dcc_message_t;

#endif
