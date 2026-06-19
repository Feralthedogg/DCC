#ifndef DCC_JSON_KEY_ID_CORE_ENTRIES_H
#define DCC_JSON_KEY_ID_CORE_ENTRIES_H

#include "internal/json/dcc_json_key_id_core_gateway_entries.h"
#include "internal/json/dcc_json_key_id_core_message_entries.h"
#include "internal/json/dcc_json_key_id_core_user_resource_entries.h"

#define DCC_JSON_KEY_ID_CORE_ENTRIES(X) \
    DCC_JSON_KEY_ID_CORE_GATEWAY_ENTRIES(X) \
    DCC_JSON_KEY_ID_CORE_MESSAGE_ENTRIES(X) \
    DCC_JSON_KEY_ID_CORE_USER_RESOURCE_ENTRIES(X)

#endif
