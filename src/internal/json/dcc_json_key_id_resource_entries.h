#ifndef DCC_JSON_KEY_ID_RESOURCE_ENTRIES_H
#define DCC_JSON_KEY_ID_RESOURCE_ENTRIES_H

#include "internal/json/dcc_json_key_id_resource_media_entries.h"
#include "internal/json/dcc_json_key_id_resource_voice_entries.h"
#include "internal/json/dcc_json_key_id_resource_reference_entries.h"
#include "internal/json/dcc_json_key_id_resource_scalar_entries.h"
#include "internal/json/dcc_json_key_id_resource_text_time_entries.h"

#define DCC_JSON_KEY_ID_RESOURCE_ENTRIES(X) \
    DCC_JSON_KEY_ID_RESOURCE_MEDIA_ENTRIES(X) \
    DCC_JSON_KEY_ID_RESOURCE_VOICE_ENTRIES(X) \
    DCC_JSON_KEY_ID_RESOURCE_REFERENCE_ENTRIES(X) \
    DCC_JSON_KEY_ID_RESOURCE_SCALAR_ENTRIES(X) \
    DCC_JSON_KEY_ID_RESOURCE_TEXT_TIME_ENTRIES(X)

#endif
