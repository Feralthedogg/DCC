#ifndef DCC_JSON_KEY_IDS_H
#define DCC_JSON_KEY_IDS_H

#include "internal/json/dcc_json_key_id_collection_entries.h"
#include "internal/json/dcc_json_key_id_core_entries.h"
#include "internal/json/dcc_json_key_id_guild_entries.h"
#include "internal/json/dcc_json_key_id_interaction_entries.h"
#include "internal/json/dcc_json_key_id_object_entries.h"
#include "internal/json/dcc_json_key_id_resource_entries.h"

#define DCC_JSON_KEY_ENUM(name) DCC_JSON_KEY_##name,

typedef enum dcc_json_key_id {
    DCC_JSON_KEY_UNKNOWN = 0,
    DCC_JSON_KEY_ID_CORE_ENTRIES(DCC_JSON_KEY_ENUM)
    DCC_JSON_KEY_ID_INTERACTION_ENTRIES(DCC_JSON_KEY_ENUM)
    DCC_JSON_KEY_ID_RESOURCE_ENTRIES(DCC_JSON_KEY_ENUM)
    DCC_JSON_KEY_ID_GUILD_ENTRIES(DCC_JSON_KEY_ENUM)
    DCC_JSON_KEY_ID_COLLECTION_ENTRIES(DCC_JSON_KEY_ENUM)
    DCC_JSON_KEY_ID_OBJECT_ENTRIES(DCC_JSON_KEY_ENUM)
} dcc_json_key_id_t;

#undef DCC_JSON_KEY_ENUM

#endif
