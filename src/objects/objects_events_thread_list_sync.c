#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_thread_list_sync_t *dcc_thread_list_sync_clone(const dcc_thread_list_sync_t *sync) {
    if (sync == NULL) {
        return NULL;
    }

    dcc_thread_list_sync_t *copy = (dcc_thread_list_sync_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }
    *copy = *sync;
    copy->channel_ids = NULL;
    copy->thread_ids = NULL;
    copy->member_ids = NULL;
    copy->threads = NULL;
    copy->members = NULL;
    if (sync->channel_ids != NULL && sync->channel_ids_count > 0) {
        copy->channel_ids = dcc_clone_snowflake_array_or_null(sync->channel_ids, sync->channel_ids_count);
        if (copy->channel_ids == NULL) {
            dcc_thread_list_sync_free(copy);
            return NULL;
        }
    }
    if (sync->thread_ids != NULL && sync->thread_ids_count > 0) {
        copy->thread_ids = dcc_clone_snowflake_array_or_null(sync->thread_ids, sync->thread_ids_count);
        if (copy->thread_ids == NULL) {
            dcc_thread_list_sync_free(copy);
            return NULL;
        }
    }
    if (sync->member_ids != NULL && sync->member_ids_count > 0) {
        copy->member_ids = dcc_clone_snowflake_array_or_null(sync->member_ids, sync->member_ids_count);
        if (copy->member_ids == NULL) {
            dcc_thread_list_sync_free(copy);
            return NULL;
        }
    }
    if (sync->threads != NULL && sync->threads_count > 0) {
        dcc_channel_t *threads = (dcc_channel_t *)calloc(sync->threads_count, sizeof(*threads));
        if (threads == NULL) {
            dcc_thread_list_sync_free(copy);
            return NULL;
        }
        copy->threads = threads;
        for (size_t i = 0; i < sync->threads_count; i++) {
            dcc_channel_t *item = dcc_channel_clone(&sync->threads[i]);
            if (item == NULL) {
                dcc_thread_list_sync_free(copy);
                return NULL;
            }
            threads[i] = *item;
            free(item);
        }
    }
    if (sync->members != NULL && sync->members_count > 0) {
        dcc_thread_member_t *members = (dcc_thread_member_t *)calloc(sync->members_count, sizeof(*members));
        if (members == NULL) {
            dcc_thread_list_sync_free(copy);
            return NULL;
        }
        copy->members = members;
        for (size_t i = 0; i < sync->members_count; i++) {
            dcc_thread_member_t *item = dcc_thread_member_clone(&sync->members[i]);
            if (item == NULL) {
                dcc_thread_list_sync_free(copy);
                return NULL;
            }
            members[i] = *item;
            free(item);
        }
    }
    return copy;
}

void dcc_thread_list_sync_free(dcc_thread_list_sync_t *sync) {
    if (sync == NULL) {
        return;
    }
    free((void *)sync->channel_ids);
    free((void *)sync->thread_ids);
    free((void *)sync->member_ids);
    if (sync->threads != NULL) {
        dcc_channel_t *threads = (dcc_channel_t *)sync->threads;
        for (size_t i = 0; i < sync->threads_count; i++) {
            dcc_free_channel_fields(&threads[i]);
        }
        free(threads);
    }
    if (sync->members != NULL) {
        dcc_thread_member_t *members = (dcc_thread_member_t *)sync->members;
        for (size_t i = 0; i < sync->members_count; i++) {
            dcc_free_thread_member_fields(&members[i]);
        }
        free(members);
    }
    free(sync);
}
