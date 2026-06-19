#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

char *dcc_clone_string_or_null(const char *value) {
    return value != NULL ? dcc_strdup(value) : NULL;
}

dcc_snowflake_t *dcc_clone_snowflake_array_or_null(const dcc_snowflake_t *ids, size_t count) {
    if (ids == NULL || count == 0) {
        return NULL;
    }
    if (count > (size_t)-1 / sizeof(dcc_snowflake_t)) {
        return NULL;
    }

    dcc_snowflake_t *copy = (dcc_snowflake_t *)malloc(count * sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, ids, count * sizeof(*copy));
    return copy;
}

int dcc_clone_user_fields(dcc_user_t *copy, const dcc_user_t *user) {
    if (copy == NULL || user == NULL) {
        return -1;
    }

    *copy = *user;
    copy->username = dcc_clone_string_or_null(user->username);
    copy->global_name = dcc_clone_string_or_null(user->global_name);
    copy->avatar = dcc_clone_string_or_null(user->avatar);
    if ((user->username != NULL && copy->username == NULL) ||
        (user->global_name != NULL && copy->global_name == NULL) ||
        (user->avatar != NULL && copy->avatar == NULL)) {
        free((void *)copy->username);
        free((void *)copy->global_name);
        free((void *)copy->avatar);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    return 0;
}

void dcc_free_user_fields(dcc_user_t *user) {
    if (user == NULL) {
        return;
    }
    free((void *)user->username);
    free((void *)user->global_name);
    free((void *)user->avatar);
    user->username = NULL;
    user->global_name = NULL;
    user->avatar = NULL;
}

dcc_user_t *dcc_user_clone(const dcc_user_t *user) {
    if (user == NULL) {
        return NULL;
    }

    dcc_user_t *copy = (dcc_user_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }
    if (dcc_clone_user_fields(copy, user) != 0) {
        free(copy);
        return NULL;
    }

    return copy;
}

void dcc_user_free(dcc_user_t *user) {
    if (user == NULL) {
        return;
    }
    dcc_free_user_fields(user);
    free(user);
}
