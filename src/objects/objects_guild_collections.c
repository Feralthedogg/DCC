#include "internal/objects/dcc_objects_internal.h"

int dcc_clone_guild_collection_fields(dcc_guild_t *copy, const dcc_guild_t *guild) {
    return dcc_clone_guild_core_collection_fields(copy, guild) == 0 &&
           dcc_clone_guild_resource_collection_fields(copy, guild) == 0
        ? 0
        : -1;
}
