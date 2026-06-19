#ifndef DCC_JSON_KEY_ID_COLLECTION_ENTRIES_H
#define DCC_JSON_KEY_ID_COLLECTION_ENTRIES_H

#include "internal/json/dcc_json_key_id_collection_core_entries.h"
#include "internal/json/dcc_json_key_id_collection_text_entries.h"
#include "internal/json/dcc_json_key_id_collection_time_entries.h"
#include "internal/json/dcc_json_key_id_collection_client_entries.h"
#include "internal/json/dcc_json_key_id_collection_resource_entries.h"
#include "internal/json/dcc_json_key_id_collection_activity_entries.h"

#define DCC_JSON_KEY_ID_COLLECTION_ENTRIES(X) \
    DCC_JSON_KEY_ID_COLLECTION_CORE_ENTRIES(X) \
    DCC_JSON_KEY_ID_COLLECTION_TEXT_ENTRIES(X) \
    DCC_JSON_KEY_ID_COLLECTION_TIME_ENTRIES(X) \
    DCC_JSON_KEY_ID_COLLECTION_CLIENT_ENTRIES(X) \
    DCC_JSON_KEY_ID_COLLECTION_RESOURCE_ENTRIES(X) \
    DCC_JSON_KEY_ID_COLLECTION_ACTIVITY_ENTRIES(X)

#endif
