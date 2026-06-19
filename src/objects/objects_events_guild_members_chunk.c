#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_guild_members_chunk_t *dcc_guild_members_chunk_clone(const dcc_guild_members_chunk_t *chunk) {
    if (chunk == NULL) {
        return NULL;
    }

    dcc_guild_members_chunk_t *copy = (dcc_guild_members_chunk_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *chunk;
    copy->not_found_ids = NULL;
    copy->member_ids = NULL;
    copy->members = NULL;
    copy->nonce = dcc_clone_string_or_null(chunk->nonce);
    if (chunk->not_found_ids != NULL && chunk->not_found_ids_count > 0) {
        copy->not_found_ids = dcc_clone_snowflake_array_or_null(
            chunk->not_found_ids,
            chunk->not_found_ids_count
        );
        if (copy->not_found_ids == NULL) {
            dcc_guild_members_chunk_free(copy);
            return NULL;
        }
    }
    if (chunk->member_ids != NULL && chunk->member_ids_count > 0) {
        copy->member_ids = dcc_clone_snowflake_array_or_null(chunk->member_ids, chunk->member_ids_count);
        if (copy->member_ids == NULL) {
            dcc_guild_members_chunk_free(copy);
            return NULL;
        }
    }
    if (chunk->members != NULL && chunk->members_count > 0) {
        dcc_member_t *members = (dcc_member_t *)calloc(chunk->members_count, sizeof(*members));
        if (members == NULL) {
            dcc_guild_members_chunk_free(copy);
            return NULL;
        }
        copy->members = members;
        for (size_t i = 0; i < chunk->members_count; i++) {
            if (dcc_clone_member_fields(&members[i], &chunk->members[i]) != 0) {
                dcc_guild_members_chunk_free(copy);
                return NULL;
            }
        }
    }

    if (chunk->nonce != NULL && copy->nonce == NULL) {
        dcc_guild_members_chunk_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_guild_members_chunk_free(dcc_guild_members_chunk_t *chunk) {
    if (chunk == NULL) {
        return;
    }
    free((void *)chunk->not_found_ids);
    free((void *)chunk->member_ids);
    if (chunk->members != NULL) {
        dcc_member_t *members = (dcc_member_t *)chunk->members;
        for (size_t i = 0; i < chunk->members_count; i++) {
            dcc_free_member_fields(&members[i]);
        }
        free(members);
    }
    free((void *)chunk->nonce);
    free(chunk);
}
