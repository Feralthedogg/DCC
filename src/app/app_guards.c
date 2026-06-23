#include "internal/app/dcc_app_internal.h"

#include <llam/runtime.h>

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DCC_APP_EXTENSION_MIDDLEWARE_MIN_SIZE \
    (offsetof(dcc_app_extension_middleware_t, user_data) + sizeof(((dcc_app_extension_middleware_t *)0)->user_data))
#define DCC_APP_CHECK_MIN_SIZE \
    (offsetof(dcc_app_check_t, check) + sizeof(((dcc_app_check_t *)0)->check))
#define DCC_APP_COOLDOWN_OPTIONS_MIN_SIZE \
    (offsetof(dcc_app_cooldown_options_t, window_ms) + sizeof(((dcc_app_cooldown_options_t *)0)->window_ms))

typedef enum dcc_app_guard_kind {
    DCC_APP_GUARD_GUILD_ONLY = 1,
    DCC_APP_GUARD_DM_ONLY = 2,
    DCC_APP_GUARD_USER_ALLOWLIST = 3,
    DCC_APP_GUARD_REQUIRED_PERMISSIONS = 4,
    DCC_APP_GUARD_NSFW_ONLY = 5,
    DCC_APP_GUARD_CHANNEL_TYPES = 6,
    DCC_APP_GUARD_REQUIRED_ROLES = 7,
    DCC_APP_GUARD_ANY_ROLE = 8
} dcc_app_guard_kind_t;

typedef struct dcc_app_guard_state {
    dcc_app_guard_kind_t kind;
    dcc_permission_t permissions;
    dcc_snowflake_t *user_ids;
    size_t user_id_count;
    dcc_snowflake_t *role_ids;
    size_t role_id_count;
    uint32_t *channel_types;
    size_t channel_type_count;
} dcc_app_guard_state_t;

typedef struct dcc_app_check_state {
    dcc_app_check_t check;
} dcc_app_check_state_t;

typedef struct dcc_app_cooldown_entry {
    dcc_snowflake_t key;
    uint64_t reset_at_ms;
    uint32_t count;
} dcc_app_cooldown_entry_t;

typedef struct dcc_app_cooldown_state {
    dcc_app_cooldown_options_t options;
    dcc_app_cooldown_entry_t *entries;
    size_t entry_count;
    size_t entry_cap;
} dcc_app_cooldown_state_t;

static void dcc_app_guard_free(void *user_data) {
    dcc_app_guard_state_t *guard = (dcc_app_guard_state_t *)user_data;
    if (guard == NULL) {
        return;
    }
    free(guard->user_ids);
    free(guard->role_ids);
    free(guard->channel_types);
    free(guard);
}

static dcc_status_t dcc_app_guard_copy_snowflakes(
    dcc_snowflake_t **out,
    const dcc_snowflake_t *ids,
    size_t id_count
) {
    if (out == NULL || ids == NULL || id_count == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (id_count > SIZE_MAX / sizeof(**out)) {
        return DCC_ERR_NOMEM;
    }
    dcc_snowflake_t *copy = (dcc_snowflake_t *)malloc(id_count * sizeof(*copy));
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(copy, ids, id_count * sizeof(*copy));
    *out = copy;
    return DCC_OK;
}

static dcc_status_t dcc_app_guard_copy_channel_types(
    uint32_t **out,
    const uint32_t *channel_types,
    size_t channel_type_count
) {
    if (out == NULL || channel_types == NULL || channel_type_count == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (channel_type_count > SIZE_MAX / sizeof(**out)) {
        return DCC_ERR_NOMEM;
    }
    uint32_t *copy = (uint32_t *)malloc(channel_type_count * sizeof(*copy));
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(copy, channel_types, channel_type_count * sizeof(*copy));
    *out = copy;
    return DCC_OK;
}

static int dcc_app_guard_user_allowed(const dcc_app_guard_state_t *guard, dcc_snowflake_t user_id) {
    if (guard == NULL || user_id == 0U) {
        return 0;
    }
    for (size_t i = 0U; i < guard->user_id_count; ++i) {
        if (guard->user_ids[i] == user_id) {
            return 1;
        }
    }
    return 0;
}

static int dcc_app_guard_channel_type_allowed(const dcc_app_guard_state_t *guard, dcc_ctx_t *ctx) {
    if (guard == NULL || guard->channel_types == NULL || guard->channel_type_count == 0U) {
        return 0;
    }
    uint32_t channel_type = 0U;
    if (dcc_ctx_channel_type(ctx, &channel_type) != DCC_OK) {
        return 0;
    }
    for (size_t i = 0U; i < guard->channel_type_count; ++i) {
        if (guard->channel_types[i] == channel_type) {
            return 1;
        }
    }
    return 0;
}

static dcc_status_t dcc_app_guard_reject(
    dcc_ctx_t *ctx,
    const char *title,
    const char *description
) {
    if (dcc_ctx_event_type(ctx) == DCC_EVENT_AUTOCOMPLETE) {
        dcc_autocomplete_builder_t autocomplete;
        dcc_autocomplete_builder_init(&autocomplete);
        (void)dcc_ctx_reply_autocomplete(ctx, &autocomplete, NULL, NULL);
    } else {
        (void)dcc_ctx_reply_error(ctx, title, description, NULL, NULL);
    }
    (void)title;
    (void)description;
    if (dcc_ctx_event(ctx) != NULL) {
        dcc_event_cancel(dcc_ctx_event(ctx));
    }
    return DCC_ERR_STATE;
}

static int dcc_app_check_has_field(
    const dcc_app_check_t *check,
    size_t offset,
    size_t field_size
) {
    return check != NULL &&
           check->size >= offset &&
           field_size <= check->size - offset;
}

static void dcc_app_check_free(void *user_data) {
    free(user_data);
}

static dcc_status_t dcc_app_check_new(
    const dcc_app_check_t *check,
    dcc_app_check_state_t **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (check == NULL || check->size < DCC_APP_CHECK_MIN_SIZE || check->check == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_app_check_state_t *state = (dcc_app_check_state_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->check.size = sizeof(state->check);
    state->check.check = check->check;
    if (dcc_app_check_has_field(check, offsetof(dcc_app_check_t, user_data), sizeof(check->user_data))) {
        state->check.user_data = check->user_data;
    }
    if (dcc_app_check_has_field(check, offsetof(dcc_app_check_t, title), sizeof(check->title))) {
        state->check.title = check->title;
    }
    if (dcc_app_check_has_field(check, offsetof(dcc_app_check_t, description), sizeof(check->description))) {
        state->check.description = check->description;
    }
    *out = state;
    return DCC_OK;
}

static dcc_status_t dcc_app_check_middleware(dcc_ctx_t *ctx, void *user_data) {
    dcc_app_check_state_t *state = (dcc_app_check_state_t *)user_data;
    if (ctx == NULL || state == NULL || state->check.check == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = state->check.check(ctx, state->check.user_data);
    if (status == DCC_OK) {
        return DCC_OK;
    }
    return dcc_app_guard_reject(
        ctx,
        state->check.title != NULL ? state->check.title : "Check failed",
        state->check.description != NULL
            ? state->check.description
            : "You cannot use this action right now."
    );
}

static dcc_status_t dcc_app_use_check_internal(
    dcc_app_t *app,
    const dcc_app_check_t *check
) {
    dcc_app_check_state_t *state = NULL;
    dcc_status_t status = dcc_app_check_new(check, &state);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_app_use_internal(app, dcc_app_check_middleware, state, dcc_app_check_free);
    if (status != DCC_OK) {
        dcc_app_check_free(state);
    }
    return status;
}

static dcc_status_t dcc_app_route_use_check_internal(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_app_check_t *check
) {
    dcc_app_check_state_t *state = NULL;
    dcc_status_t status = dcc_app_check_new(check, &state);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_app_route_use_internal(app, route_id, dcc_app_check_middleware, state, dcc_app_check_free);
    if (status != DCC_OK) {
        dcc_app_check_free(state);
    }
    return status;
}

static uint64_t dcc_app_cooldown_now_ms(void) {
    return llam_now_ns() / UINT64_C(1000000);
}

static uint64_t dcc_app_cooldown_saturating_add_ms(uint64_t value, uint64_t add) {
    return value > UINT64_MAX - add ? UINT64_MAX : value + add;
}

static void dcc_app_cooldown_free(void *user_data) {
    dcc_app_cooldown_state_t *state = (dcc_app_cooldown_state_t *)user_data;
    if (state == NULL) {
        return;
    }
    free(state->entries);
    free(state);
}

static int dcc_app_cooldown_options_has_field(
    const dcc_app_cooldown_options_t *options,
    size_t offset,
    size_t field_size
) {
    return options != NULL &&
           (options->size == 0U ||
            (options->size >= offset && field_size <= options->size - offset));
}

static dcc_status_t dcc_app_cooldown_normalize_options(
    const dcc_app_cooldown_options_t *options,
    dcc_app_cooldown_options_t *out
) {
    if (options == NULL || out == NULL ||
        !dcc_app_cooldown_options_has_field(options, offsetof(dcc_app_cooldown_options_t, window_ms), sizeof(options->window_ms))) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_app_cooldown_options_init(out);
    if (dcc_app_cooldown_options_has_field(
            options,
            offsetof(dcc_app_cooldown_options_t, bucket),
            sizeof(options->bucket)
        )) {
        out->bucket = options->bucket;
    }
    if (dcc_app_cooldown_options_has_field(
            options,
            offsetof(dcc_app_cooldown_options_t, limit),
            sizeof(options->limit)
        )) {
        out->limit = options->limit;
    }
    out->window_ms = options->window_ms;

    if (out->limit == 0U || out->window_ms == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    switch (out->bucket) {
        case DCC_APP_COOLDOWN_GLOBAL:
        case DCC_APP_COOLDOWN_USER:
        case DCC_APP_COOLDOWN_CHANNEL:
        case DCC_APP_COOLDOWN_GUILD:
            return DCC_OK;
    }
    return DCC_ERR_INVALID_ARG;
}

static dcc_status_t dcc_app_cooldown_new(
    const dcc_app_cooldown_options_t *options,
    dcc_app_cooldown_state_t **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    dcc_app_cooldown_options_t normalized;
    dcc_status_t status = dcc_app_cooldown_normalize_options(options, &normalized);
    if (status != DCC_OK) {
        return status;
    }

    dcc_app_cooldown_state_t *state = (dcc_app_cooldown_state_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->options = normalized;
    *out = state;
    return DCC_OK;
}

static dcc_snowflake_t dcc_app_cooldown_key(
    const dcc_app_cooldown_state_t *state,
    const dcc_ctx_t *ctx
) {
    if (state == NULL || ctx == NULL) {
        return 0U;
    }
    switch (state->options.bucket) {
        case DCC_APP_COOLDOWN_GLOBAL:
            return 1U;
        case DCC_APP_COOLDOWN_USER:
            return dcc_ctx_user_id(ctx);
        case DCC_APP_COOLDOWN_CHANNEL:
            return dcc_ctx_channel_id(ctx);
        case DCC_APP_COOLDOWN_GUILD:
            return dcc_ctx_guild_id(ctx);
    }
    return 0U;
}

static void dcc_app_cooldown_prune(dcc_app_cooldown_state_t *state, uint64_t now_ms) {
    if (state == NULL || state->entries == NULL) {
        return;
    }
    size_t out = 0U;
    for (size_t i = 0U; i < state->entry_count; ++i) {
        if (now_ms < state->entries[i].reset_at_ms) {
            if (out != i) {
                state->entries[out] = state->entries[i];
            }
            out++;
        }
    }
    state->entry_count = out;
}

static dcc_status_t dcc_app_cooldown_reserve(dcc_app_cooldown_state_t *state, size_t need) {
    if (state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (need <= state->entry_cap) {
        return DCC_OK;
    }
    size_t next_cap = state->entry_cap != 0U ? state->entry_cap : 8U;
    while (next_cap < need) {
        if (next_cap > SIZE_MAX / 2U) {
            return DCC_ERR_NOMEM;
        }
        next_cap *= 2U;
    }
    if (next_cap > SIZE_MAX / sizeof(*state->entries)) {
        return DCC_ERR_NOMEM;
    }
    dcc_app_cooldown_entry_t *next =
        (dcc_app_cooldown_entry_t *)realloc(state->entries, next_cap * sizeof(*state->entries));
    if (next == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->entries = next;
    state->entry_cap = next_cap;
    return DCC_OK;
}

static dcc_app_cooldown_entry_t *dcc_app_cooldown_find(
    dcc_app_cooldown_state_t *state,
    dcc_snowflake_t key
) {
    if (state == NULL || key == 0U) {
        return NULL;
    }
    for (size_t i = 0U; i < state->entry_count; ++i) {
        if (state->entries[i].key == key) {
            return &state->entries[i];
        }
    }
    return NULL;
}

static dcc_status_t dcc_app_cooldown_add_entry(
    dcc_app_cooldown_state_t *state,
    dcc_snowflake_t key,
    uint64_t now_ms,
    dcc_app_cooldown_entry_t **out
) {
    if (state == NULL || key == 0U || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (state->entry_count == SIZE_MAX) {
        return DCC_ERR_NOMEM;
    }
    dcc_status_t status = dcc_app_cooldown_reserve(state, state->entry_count + 1U);
    if (status != DCC_OK) {
        return status;
    }
    dcc_app_cooldown_entry_t *entry = &state->entries[state->entry_count++];
    entry->key = key;
    entry->count = 0U;
    entry->reset_at_ms = dcc_app_cooldown_saturating_add_ms(now_ms, state->options.window_ms);
    *out = entry;
    return DCC_OK;
}

static dcc_status_t dcc_app_cooldown_middleware(dcc_ctx_t *ctx, void *user_data) {
    dcc_app_cooldown_state_t *state = (dcc_app_cooldown_state_t *)user_data;
    if (ctx == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_snowflake_t key = dcc_app_cooldown_key(state, ctx);
    if (key == 0U) {
        return dcc_app_guard_reject(
            ctx,
            "Cooldown unavailable",
            "This action cannot be rate limited in the current context."
        );
    }

    uint64_t now_ms = dcc_app_cooldown_now_ms();
    dcc_app_cooldown_prune(state, now_ms);

    dcc_app_cooldown_entry_t *entry = dcc_app_cooldown_find(state, key);
    if (entry == NULL) {
        dcc_status_t status = dcc_app_cooldown_add_entry(state, key, now_ms, &entry);
        if (status != DCC_OK) {
            return status;
        }
    } else if (now_ms >= entry->reset_at_ms) {
        entry->count = 0U;
        entry->reset_at_ms = dcc_app_cooldown_saturating_add_ms(now_ms, state->options.window_ms);
    }

    if (entry->count >= state->options.limit) {
        uint64_t remaining_ms = entry->reset_at_ms > now_ms ? entry->reset_at_ms - now_ms : 0U;
        unsigned long long remaining_s = (unsigned long long)((remaining_ms + 999U) / 1000U);
        char description[128];
        snprintf(
            description,
            sizeof(description),
            "Try again in %llu second%s.",
            remaining_s,
            remaining_s == 1U ? "" : "s"
        );
        return dcc_app_guard_reject(ctx, "Slow down", description);
    }

    entry->count++;
    return DCC_OK;
}

static dcc_status_t dcc_app_use_cooldown_internal(
    dcc_app_t *app,
    const dcc_app_cooldown_options_t *options
) {
    dcc_app_cooldown_state_t *state = NULL;
    dcc_status_t status = dcc_app_cooldown_new(options, &state);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_app_use_internal(app, dcc_app_cooldown_middleware, state, dcc_app_cooldown_free);
    if (status != DCC_OK) {
        dcc_app_cooldown_free(state);
    }
    return status;
}

static dcc_status_t dcc_app_route_use_cooldown_internal(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_app_cooldown_options_t *options
) {
    dcc_app_cooldown_state_t *state = NULL;
    dcc_status_t status = dcc_app_cooldown_new(options, &state);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_app_route_use_internal(app, route_id, dcc_app_cooldown_middleware, state, dcc_app_cooldown_free);
    if (status != DCC_OK) {
        dcc_app_cooldown_free(state);
    }
    return status;
}

static dcc_status_t dcc_app_guard_middleware(dcc_ctx_t *ctx, void *user_data) {
    const dcc_app_guard_state_t *guard = (const dcc_app_guard_state_t *)user_data;
    if (ctx == NULL || guard == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    switch (guard->kind) {
        case DCC_APP_GUARD_GUILD_ONLY:
            if (dcc_ctx_guild_id(ctx) != 0U) {
                return DCC_OK;
            }
            return dcc_app_guard_reject(
                ctx,
                "Server only",
                "This action can only be used inside a Discord server."
            );

        case DCC_APP_GUARD_DM_ONLY:
            if (dcc_ctx_guild_id(ctx) == 0U) {
                return DCC_OK;
            }
            return dcc_app_guard_reject(
                ctx,
                "DM only",
                "This action can only be used in direct messages."
            );

        case DCC_APP_GUARD_NSFW_ONLY:
            if (dcc_ctx_channel_is_nsfw(ctx)) {
                return DCC_OK;
            }
            return dcc_app_guard_reject(
                ctx,
                "NSFW only",
                "This action can only be used in age-restricted channels."
            );

        case DCC_APP_GUARD_CHANNEL_TYPES:
            if (dcc_app_guard_channel_type_allowed(guard, ctx)) {
                return DCC_OK;
            }
            return dcc_app_guard_reject(
                ctx,
                "Wrong channel type",
                "This action cannot be used in this channel type."
            );

        case DCC_APP_GUARD_USER_ALLOWLIST:
            if (dcc_app_guard_user_allowed(guard, dcc_ctx_user_id(ctx))) {
                return DCC_OK;
            }
            return dcc_app_guard_reject(
                ctx,
                "Not allowed",
                "You are not allowed to use this action."
            );

        case DCC_APP_GUARD_REQUIRED_PERMISSIONS:
            if (dcc_ctx_member_has_permissions(ctx, guard->permissions)) {
                return DCC_OK;
            }
            return dcc_app_guard_reject(
                ctx,
                "Missing permissions",
                "You do not have the required server permissions for this action."
            );

        case DCC_APP_GUARD_REQUIRED_ROLES:
            if (dcc_ctx_member_has_all_roles(ctx, guard->role_ids, guard->role_id_count)) {
                return DCC_OK;
            }
            return dcc_app_guard_reject(
                ctx,
                "Missing role",
                "You do not have the required role for this action."
            );

        case DCC_APP_GUARD_ANY_ROLE:
            if (dcc_ctx_member_has_any_role(ctx, guard->role_ids, guard->role_id_count)) {
                return DCC_OK;
            }
            return dcc_app_guard_reject(
                ctx,
                "Missing role",
                "You need one of the required roles for this action."
            );
    }

    return DCC_ERR_STATE;
}

static dcc_status_t dcc_app_use_guard(dcc_app_t *app, dcc_app_guard_state_t *guard) {
    dcc_status_t status = dcc_app_use_internal(app, dcc_app_guard_middleware, guard, dcc_app_guard_free);
    if (status != DCC_OK) {
        dcc_app_guard_free(guard);
    }
    return status;
}

static dcc_status_t dcc_app_route_use_guard(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    dcc_app_guard_state_t *guard
) {
    dcc_status_t status =
        dcc_app_route_use_internal(app, route_id, dcc_app_guard_middleware, guard, dcc_app_guard_free);
    if (status != DCC_OK) {
        dcc_app_guard_free(guard);
    }
    return status;
}

static dcc_status_t dcc_app_guard_new_guild_only(dcc_app_guard_state_t **out) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    dcc_app_guard_state_t *guard = (dcc_app_guard_state_t *)calloc(1U, sizeof(*guard));
    if (guard == NULL) {
        return DCC_ERR_NOMEM;
    }
    guard->kind = DCC_APP_GUARD_GUILD_ONLY;
    *out = guard;
    return DCC_OK;
}

static dcc_status_t dcc_app_guard_new_dm_only(dcc_app_guard_state_t **out) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    dcc_app_guard_state_t *guard = (dcc_app_guard_state_t *)calloc(1U, sizeof(*guard));
    if (guard == NULL) {
        return DCC_ERR_NOMEM;
    }
    guard->kind = DCC_APP_GUARD_DM_ONLY;
    *out = guard;
    return DCC_OK;
}

static dcc_status_t dcc_app_guard_new_nsfw_only(dcc_app_guard_state_t **out) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    dcc_app_guard_state_t *guard = (dcc_app_guard_state_t *)calloc(1U, sizeof(*guard));
    if (guard == NULL) {
        return DCC_ERR_NOMEM;
    }
    guard->kind = DCC_APP_GUARD_NSFW_ONLY;
    *out = guard;
    return DCC_OK;
}

static dcc_status_t dcc_app_guard_new_channel_types(
    const uint32_t *channel_types,
    size_t channel_type_count,
    dcc_app_guard_state_t **out
) {
    if (channel_types == NULL || channel_type_count == 0U || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    dcc_app_guard_state_t *guard = (dcc_app_guard_state_t *)calloc(1U, sizeof(*guard));
    if (guard == NULL) {
        return DCC_ERR_NOMEM;
    }
    dcc_status_t status =
        dcc_app_guard_copy_channel_types(&guard->channel_types, channel_types, channel_type_count);
    if (status != DCC_OK) {
        dcc_app_guard_free(guard);
        return status;
    }
    guard->channel_type_count = channel_type_count;
    guard->kind = DCC_APP_GUARD_CHANNEL_TYPES;
    *out = guard;
    return DCC_OK;
}

static dcc_status_t dcc_app_guard_new_roles(
    dcc_app_guard_kind_t kind,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count,
    dcc_app_guard_state_t **out
) {
    if ((kind != DCC_APP_GUARD_REQUIRED_ROLES && kind != DCC_APP_GUARD_ANY_ROLE) ||
        role_ids == NULL ||
        role_id_count == 0U ||
        out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    dcc_app_guard_state_t *guard = (dcc_app_guard_state_t *)calloc(1U, sizeof(*guard));
    if (guard == NULL) {
        return DCC_ERR_NOMEM;
    }
    dcc_status_t status = dcc_app_guard_copy_snowflakes(&guard->role_ids, role_ids, role_id_count);
    if (status != DCC_OK) {
        dcc_app_guard_free(guard);
        return status;
    }
    guard->role_id_count = role_id_count;
    guard->kind = kind;
    *out = guard;
    return DCC_OK;
}

static dcc_status_t dcc_app_guard_new_user_allowlist(
    const dcc_snowflake_t *user_ids,
    size_t user_id_count,
    dcc_app_guard_state_t **out
) {
    if (user_ids == NULL || user_id_count == 0U || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    dcc_app_guard_state_t *guard = (dcc_app_guard_state_t *)calloc(1U, sizeof(*guard));
    if (guard == NULL) {
        return DCC_ERR_NOMEM;
    }
    dcc_status_t status = dcc_app_guard_copy_snowflakes(&guard->user_ids, user_ids, user_id_count);
    if (status != DCC_OK) {
        dcc_app_guard_free(guard);
        return status;
    }
    guard->user_id_count = user_id_count;
    guard->kind = DCC_APP_GUARD_USER_ALLOWLIST;
    *out = guard;
    return DCC_OK;
}

static dcc_status_t dcc_app_guard_new_required_permissions(
    dcc_permission_t permissions,
    dcc_app_guard_state_t **out
) {
    if (permissions == 0U || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    dcc_app_guard_state_t *guard = (dcc_app_guard_state_t *)calloc(1U, sizeof(*guard));
    if (guard == NULL) {
        return DCC_ERR_NOMEM;
    }
    guard->kind = DCC_APP_GUARD_REQUIRED_PERMISSIONS;
    guard->permissions = permissions;
    *out = guard;
    return DCC_OK;
}

static uint8_t dcc_app_route_policy_has_field(
    const dcc_app_route_policy_t *policy,
    size_t offset,
    size_t field_size
) {
    return policy != NULL &&
           policy->size >= offset &&
           field_size <= policy->size - offset;
}

static dcc_status_t dcc_app_route_apply_policy_middlewares(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_app_route_policy_t *policy
) {
    if (!dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, middleware_count),
            sizeof(policy->middleware_count)
        )) {
        return DCC_OK;
    }
    if (policy->middlewares == NULL && policy->middleware_count != 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    for (size_t i = 0U; i < policy->middleware_count; ++i) {
        const dcc_app_extension_middleware_t *middleware = &policy->middlewares[i];
        if (middleware->size < DCC_APP_EXTENSION_MIDDLEWARE_MIN_SIZE ||
            middleware->middleware == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        dcc_status_t status =
            dcc_app_route_use(app, route_id, middleware->middleware, middleware->user_data);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}

static dcc_status_t dcc_app_route_group_apply_policy_middlewares(
    const dcc_app_route_group_t *group,
    const dcc_app_route_policy_t *policy
) {
    if (!dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, middleware_count),
            sizeof(policy->middleware_count)
        )) {
        return DCC_OK;
    }
    if (policy->middlewares == NULL && policy->middleware_count != 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    for (size_t i = 0U; i < policy->middleware_count; ++i) {
        const dcc_app_extension_middleware_t *middleware = &policy->middlewares[i];
        if (middleware->size < DCC_APP_EXTENSION_MIDDLEWARE_MIN_SIZE ||
            middleware->middleware == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        dcc_status_t status =
            dcc_app_route_group_use(group, middleware->middleware, middleware->user_data);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}

dcc_status_t dcc_app_route_apply_policy_internal(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_app_route_policy_t *policy
) {
    if (app == NULL || route_id == DCC_APP_ROUTE_INVALID) {
        return DCC_ERR_INVALID_ARG;
    }
    if (policy == NULL || policy->size == 0U) {
        return DCC_OK;
    }
    if (policy->size < sizeof(size_t)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_app_route_apply_policy_middlewares(app, route_id, policy);
    if (status != DCC_OK) {
        return status;
    }

    uint8_t guild_only = dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, guild_only),
            sizeof(policy->guild_only)
        ) &&
        policy->guild_only != 0U;
    uint8_t dm_only = dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, dm_only),
            sizeof(policy->dm_only)
        ) &&
        policy->dm_only != 0U;
    uint8_t nsfw_only = dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, nsfw_only),
            sizeof(policy->nsfw_only)
        ) &&
        policy->nsfw_only != 0U;

    if ((guild_only && dm_only) || (dm_only && nsfw_only)) {
        return DCC_ERR_INVALID_ARG;
    }

    if (guild_only) {
        status = dcc_app_route_use_guild_only(app, route_id);
        if (status != DCC_OK) {
            return status;
        }
    }

    if (dm_only) {
        status = dcc_app_route_use_dm_only(app, route_id);
        if (status != DCC_OK) {
            return status;
        }
    }

    if (nsfw_only) {
        status = dcc_app_route_use_nsfw_only(app, route_id);
        if (status != DCC_OK) {
            return status;
        }
    }

    if (dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, channel_type_count),
            sizeof(policy->channel_type_count)
        ) &&
        policy->channel_type_count != 0U) {
        status = dcc_app_route_use_channel_types(
            app,
            route_id,
            policy->channel_types,
            policy->channel_type_count
        );
        if (status != DCC_OK) {
            return status;
        }
    }

    if (dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, owner_user_id_count),
            sizeof(policy->owner_user_id_count)
        ) &&
        policy->owner_user_id_count != 0U) {
        status = dcc_app_route_use_user_allowlist(
            app,
            route_id,
            policy->owner_user_ids,
            policy->owner_user_id_count
        );
        if (status != DCC_OK) {
            return status;
        }
    }

    if (dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, required_permissions),
            sizeof(policy->required_permissions)
        ) &&
        policy->required_permissions != 0U) {
        status = dcc_app_route_use_required_permissions(app, route_id, policy->required_permissions);
        if (status != DCC_OK) {
            return status;
        }
    }

    if (dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, required_role_id_count),
            sizeof(policy->required_role_id_count)
        ) &&
        policy->required_role_id_count != 0U) {
        status = dcc_app_route_use_required_roles(
            app,
            route_id,
            policy->required_role_ids,
            policy->required_role_id_count
        );
        if (status != DCC_OK) {
            return status;
        }
    }

    if (dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, any_role_id_count),
            sizeof(policy->any_role_id_count)
        ) &&
        policy->any_role_id_count != 0U) {
        status = dcc_app_route_use_any_role(
            app,
            route_id,
            policy->any_role_ids,
            policy->any_role_id_count
        );
        if (status != DCC_OK) {
            return status;
        }
    }

    if (dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, check_count),
            sizeof(policy->check_count)
        ) &&
        policy->check_count != 0U) {
        if (policy->checks == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        for (size_t i = 0U; i < policy->check_count; ++i) {
            status = dcc_app_route_use_check(app, route_id, &policy->checks[i]);
            if (status != DCC_OK) {
                return status;
            }
        }
    }

    if (dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, cooldown),
            sizeof(policy->cooldown)
        ) &&
        policy->cooldown.limit != 0U &&
        policy->cooldown.window_ms != 0U) {
        status = dcc_app_route_use_cooldown(app, route_id, &policy->cooldown);
        if (status != DCC_OK) {
            return status;
        }
    }

    return DCC_OK;
}

dcc_status_t dcc_app_route_apply_policy(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_app_route_policy_t *policy
) {
    return dcc_app_route_apply_policy_internal(app, route_id, policy);
}

dcc_status_t dcc_app_use_guild_only(dcc_app_t *app) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_guard_state_t *guard = NULL;
    dcc_status_t status = dcc_app_guard_new_guild_only(&guard);
    return status == DCC_OK ? dcc_app_use_guard(app, guard) : status;
}

dcc_status_t dcc_app_use_dm_only(dcc_app_t *app) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_guard_state_t *guard = NULL;
    dcc_status_t status = dcc_app_guard_new_dm_only(&guard);
    return status == DCC_OK ? dcc_app_use_guard(app, guard) : status;
}

dcc_status_t dcc_app_use_nsfw_only(dcc_app_t *app) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_guard_state_t *guard = NULL;
    dcc_status_t status = dcc_app_guard_new_nsfw_only(&guard);
    return status == DCC_OK ? dcc_app_use_guard(app, guard) : status;
}

dcc_status_t dcc_app_use_channel_types(
    dcc_app_t *app,
    const uint32_t *channel_types,
    size_t channel_type_count
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_guard_state_t *guard = NULL;
    dcc_status_t status = dcc_app_guard_new_channel_types(channel_types, channel_type_count, &guard);
    return status == DCC_OK ? dcc_app_use_guard(app, guard) : status;
}

dcc_status_t dcc_app_use_owner(dcc_app_t *app, dcc_snowflake_t user_id) {
    return dcc_app_use_user_allowlist(app, &user_id, 1U);
}

dcc_status_t dcc_app_use_user_allowlist(
    dcc_app_t *app,
    const dcc_snowflake_t *user_ids,
    size_t user_id_count
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_guard_state_t *guard = NULL;
    dcc_status_t status = dcc_app_guard_new_user_allowlist(user_ids, user_id_count, &guard);
    return status == DCC_OK ? dcc_app_use_guard(app, guard) : status;
}

dcc_status_t dcc_app_use_required_permissions(dcc_app_t *app, dcc_permission_t permissions) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_guard_state_t *guard = NULL;
    dcc_status_t status = dcc_app_guard_new_required_permissions(permissions, &guard);
    return status == DCC_OK ? dcc_app_use_guard(app, guard) : status;
}

dcc_status_t dcc_app_use_required_roles(
    dcc_app_t *app,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_guard_state_t *guard = NULL;
    dcc_status_t status =
        dcc_app_guard_new_roles(DCC_APP_GUARD_REQUIRED_ROLES, role_ids, role_id_count, &guard);
    return status == DCC_OK ? dcc_app_use_guard(app, guard) : status;
}

dcc_status_t dcc_app_use_any_role(
    dcc_app_t *app,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_guard_state_t *guard = NULL;
    dcc_status_t status =
        dcc_app_guard_new_roles(DCC_APP_GUARD_ANY_ROLE, role_ids, role_id_count, &guard);
    return status == DCC_OK ? dcc_app_use_guard(app, guard) : status;
}

dcc_status_t dcc_app_use_check(
    dcc_app_t *app,
    const dcc_app_check_t *check
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_use_check_internal(app, check);
}

dcc_status_t dcc_app_use_cooldown(
    dcc_app_t *app,
    const dcc_app_cooldown_options_t *options
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_use_cooldown_internal(app, options);
}

dcc_status_t dcc_app_route_use_guild_only(dcc_app_t *app, dcc_app_route_id_t route_id) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_guard_state_t *guard = NULL;
    dcc_status_t status = dcc_app_guard_new_guild_only(&guard);
    return status == DCC_OK ? dcc_app_route_use_guard(app, route_id, guard) : status;
}

dcc_status_t dcc_app_route_use_dm_only(dcc_app_t *app, dcc_app_route_id_t route_id) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_guard_state_t *guard = NULL;
    dcc_status_t status = dcc_app_guard_new_dm_only(&guard);
    return status == DCC_OK ? dcc_app_route_use_guard(app, route_id, guard) : status;
}

dcc_status_t dcc_app_route_use_nsfw_only(dcc_app_t *app, dcc_app_route_id_t route_id) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_guard_state_t *guard = NULL;
    dcc_status_t status = dcc_app_guard_new_nsfw_only(&guard);
    return status == DCC_OK ? dcc_app_route_use_guard(app, route_id, guard) : status;
}

dcc_status_t dcc_app_route_use_channel_types(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const uint32_t *channel_types,
    size_t channel_type_count
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_guard_state_t *guard = NULL;
    dcc_status_t status = dcc_app_guard_new_channel_types(channel_types, channel_type_count, &guard);
    return status == DCC_OK ? dcc_app_route_use_guard(app, route_id, guard) : status;
}

dcc_status_t dcc_app_route_use_owner(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    dcc_snowflake_t user_id
) {
    return dcc_app_route_use_user_allowlist(app, route_id, &user_id, 1U);
}

dcc_status_t dcc_app_route_use_user_allowlist(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_snowflake_t *user_ids,
    size_t user_id_count
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_guard_state_t *guard = NULL;
    dcc_status_t status = dcc_app_guard_new_user_allowlist(user_ids, user_id_count, &guard);
    return status == DCC_OK ? dcc_app_route_use_guard(app, route_id, guard) : status;
}

dcc_status_t dcc_app_route_use_required_permissions(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    dcc_permission_t permissions
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_guard_state_t *guard = NULL;
    dcc_status_t status = dcc_app_guard_new_required_permissions(permissions, &guard);
    return status == DCC_OK ? dcc_app_route_use_guard(app, route_id, guard) : status;
}

dcc_status_t dcc_app_route_use_required_roles(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_guard_state_t *guard = NULL;
    dcc_status_t status =
        dcc_app_guard_new_roles(DCC_APP_GUARD_REQUIRED_ROLES, role_ids, role_id_count, &guard);
    return status == DCC_OK ? dcc_app_route_use_guard(app, route_id, guard) : status;
}

dcc_status_t dcc_app_route_use_any_role(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_guard_state_t *guard = NULL;
    dcc_status_t status =
        dcc_app_guard_new_roles(DCC_APP_GUARD_ANY_ROLE, role_ids, role_id_count, &guard);
    return status == DCC_OK ? dcc_app_route_use_guard(app, route_id, guard) : status;
}

dcc_status_t dcc_app_route_use_check(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_app_check_t *check
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_route_use_check_internal(app, route_id, check);
}

dcc_status_t dcc_app_route_use_cooldown(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_app_cooldown_options_t *options
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_route_use_cooldown_internal(app, route_id, options);
}

static dcc_status_t dcc_app_route_group_use_guard_kind(
    const dcc_app_route_group_t *group,
    dcc_app_guard_kind_t kind,
    const dcc_snowflake_t *user_ids,
    size_t user_id_count,
    dcc_permission_t permissions
) {
    dcc_status_t status = dcc_app_route_group_validate_internal(group);
    if (status != DCC_OK) {
        return status;
    }

    size_t matched = 0U;
    for (size_t i = 0U; i < group->app->route_count; ++i) {
        const dcc_app_route_t *route = &group->app->routes[i];
        if (!dcc_app_route_group_matches_internal(group, route)) {
            continue;
        }

        dcc_app_guard_state_t *guard = NULL;
        switch (kind) {
            case DCC_APP_GUARD_GUILD_ONLY:
                status = dcc_app_guard_new_guild_only(&guard);
                break;
            case DCC_APP_GUARD_DM_ONLY:
                status = dcc_app_guard_new_dm_only(&guard);
                break;
            case DCC_APP_GUARD_NSFW_ONLY:
                status = dcc_app_guard_new_nsfw_only(&guard);
                break;
            case DCC_APP_GUARD_USER_ALLOWLIST:
                status = dcc_app_guard_new_user_allowlist(user_ids, user_id_count, &guard);
                break;
            case DCC_APP_GUARD_REQUIRED_PERMISSIONS:
                status = dcc_app_guard_new_required_permissions(permissions, &guard);
                break;
            case DCC_APP_GUARD_CHANNEL_TYPES:
            case DCC_APP_GUARD_REQUIRED_ROLES:
            case DCC_APP_GUARD_ANY_ROLE:
                status = DCC_ERR_INVALID_ARG;
                break;
        }
        if (status != DCC_OK) {
            return status;
        }

        matched++;
        status = dcc_app_route_use_guard(group->app, route->id, guard);
        if (status != DCC_OK) {
            return status;
        }
    }

    return matched != 0U ? DCC_OK : DCC_ERR_NOT_FOUND;
}

dcc_status_t dcc_app_route_group_use_guild_only(const dcc_app_route_group_t *group) {
    return dcc_app_route_group_use_guard_kind(
        group,
        DCC_APP_GUARD_GUILD_ONLY,
        NULL,
        0U,
        0U
    );
}

dcc_status_t dcc_app_route_group_use_dm_only(const dcc_app_route_group_t *group) {
    return dcc_app_route_group_use_guard_kind(
        group,
        DCC_APP_GUARD_DM_ONLY,
        NULL,
        0U,
        0U
    );
}

dcc_status_t dcc_app_route_group_use_nsfw_only(const dcc_app_route_group_t *group) {
    return dcc_app_route_group_use_guard_kind(
        group,
        DCC_APP_GUARD_NSFW_ONLY,
        NULL,
        0U,
        0U
    );
}

dcc_status_t dcc_app_route_group_use_channel_types(
    const dcc_app_route_group_t *group,
    const uint32_t *channel_types,
    size_t channel_type_count
) {
    dcc_status_t status = dcc_app_route_group_validate_internal(group);
    if (status != DCC_OK) {
        return status;
    }

    size_t matched = 0U;
    for (size_t i = 0U; i < group->app->route_count; ++i) {
        const dcc_app_route_t *route = &group->app->routes[i];
        if (!dcc_app_route_group_matches_internal(group, route)) {
            continue;
        }
        matched++;
        status = dcc_app_route_use_channel_types(
            group->app,
            route->id,
            channel_types,
            channel_type_count
        );
        if (status != DCC_OK) {
            return status;
        }
    }

    return matched != 0U ? DCC_OK : DCC_ERR_NOT_FOUND;
}

dcc_status_t dcc_app_route_group_use_owner(
    const dcc_app_route_group_t *group,
    dcc_snowflake_t user_id
) {
    return dcc_app_route_group_use_user_allowlist(group, &user_id, 1U);
}

dcc_status_t dcc_app_route_group_use_user_allowlist(
    const dcc_app_route_group_t *group,
    const dcc_snowflake_t *user_ids,
    size_t user_id_count
) {
    return dcc_app_route_group_use_guard_kind(
        group,
        DCC_APP_GUARD_USER_ALLOWLIST,
        user_ids,
        user_id_count,
        0U
    );
}

dcc_status_t dcc_app_route_group_use_required_permissions(
    const dcc_app_route_group_t *group,
    dcc_permission_t permissions
) {
    return dcc_app_route_group_use_guard_kind(
        group,
        DCC_APP_GUARD_REQUIRED_PERMISSIONS,
        NULL,
        0U,
        permissions
    );
}

dcc_status_t dcc_app_route_group_use_required_roles(
    const dcc_app_route_group_t *group,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
) {
    dcc_status_t status = dcc_app_route_group_validate_internal(group);
    if (status != DCC_OK) {
        return status;
    }

    size_t matched = 0U;
    for (size_t i = 0U; i < group->app->route_count; ++i) {
        const dcc_app_route_t *route = &group->app->routes[i];
        if (!dcc_app_route_group_matches_internal(group, route)) {
            continue;
        }
        matched++;
        status = dcc_app_route_use_required_roles(group->app, route->id, role_ids, role_id_count);
        if (status != DCC_OK) {
            return status;
        }
    }

    return matched != 0U ? DCC_OK : DCC_ERR_NOT_FOUND;
}

dcc_status_t dcc_app_route_group_use_any_role(
    const dcc_app_route_group_t *group,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
) {
    dcc_status_t status = dcc_app_route_group_validate_internal(group);
    if (status != DCC_OK) {
        return status;
    }

    size_t matched = 0U;
    for (size_t i = 0U; i < group->app->route_count; ++i) {
        const dcc_app_route_t *route = &group->app->routes[i];
        if (!dcc_app_route_group_matches_internal(group, route)) {
            continue;
        }
        matched++;
        status = dcc_app_route_use_any_role(group->app, route->id, role_ids, role_id_count);
        if (status != DCC_OK) {
            return status;
        }
    }

    return matched != 0U ? DCC_OK : DCC_ERR_NOT_FOUND;
}

dcc_status_t dcc_app_route_group_use_check(
    const dcc_app_route_group_t *group,
    const dcc_app_check_t *check
) {
    dcc_status_t status = dcc_app_route_group_validate_internal(group);
    if (status != DCC_OK) {
        return status;
    }

    size_t matched = 0U;
    for (size_t i = 0U; i < group->app->route_count; ++i) {
        const dcc_app_route_t *route = &group->app->routes[i];
        if (!dcc_app_route_group_matches_internal(group, route)) {
            continue;
        }
        matched++;
        status = dcc_app_route_use_check(group->app, route->id, check);
        if (status != DCC_OK) {
            return status;
        }
    }

    return matched != 0U ? DCC_OK : DCC_ERR_NOT_FOUND;
}

dcc_status_t dcc_app_route_group_use_cooldown(
    const dcc_app_route_group_t *group,
    const dcc_app_cooldown_options_t *options
) {
    dcc_status_t status = dcc_app_route_group_validate_internal(group);
    if (status != DCC_OK) {
        return status;
    }

    size_t matched = 0U;
    for (size_t i = 0U; i < group->app->route_count; ++i) {
        const dcc_app_route_t *route = &group->app->routes[i];
        if (!dcc_app_route_group_matches_internal(group, route)) {
            continue;
        }
        matched++;
        status = dcc_app_route_use_cooldown(group->app, route->id, options);
        if (status != DCC_OK) {
            return status;
        }
    }

    return matched != 0U ? DCC_OK : DCC_ERR_NOT_FOUND;
}

dcc_status_t dcc_app_route_group_apply_policy(
    const dcc_app_route_group_t *group,
    const dcc_app_route_policy_t *policy
) {
    dcc_status_t status = dcc_app_route_group_validate_internal(group);
    if (status != DCC_OK) {
        return status;
    }
    if (policy == NULL || policy->size == 0U) {
        return DCC_OK;
    }
    if (policy->size < sizeof(size_t)) {
        return DCC_ERR_INVALID_ARG;
    }

    status = dcc_app_route_group_apply_policy_middlewares(group, policy);
    if (status != DCC_OK) {
        return status;
    }

    uint8_t guild_only = dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, guild_only),
            sizeof(policy->guild_only)
        ) &&
        policy->guild_only != 0U;
    uint8_t dm_only = dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, dm_only),
            sizeof(policy->dm_only)
        ) &&
        policy->dm_only != 0U;
    uint8_t nsfw_only = dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, nsfw_only),
            sizeof(policy->nsfw_only)
        ) &&
        policy->nsfw_only != 0U;

    if ((guild_only && dm_only) || (dm_only && nsfw_only)) {
        return DCC_ERR_INVALID_ARG;
    }

    if (guild_only) {
        status = dcc_app_route_group_use_guild_only(group);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (dm_only) {
        status = dcc_app_route_group_use_dm_only(group);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (nsfw_only) {
        status = dcc_app_route_group_use_nsfw_only(group);
        if (status != DCC_OK) {
            return status;
        }
    }

    if (dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, channel_type_count),
            sizeof(policy->channel_type_count)
        ) &&
        policy->channel_type_count != 0U) {
        status = dcc_app_route_group_use_channel_types(
            group,
            policy->channel_types,
            policy->channel_type_count
        );
        if (status != DCC_OK) {
            return status;
        }
    }

    if (dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, owner_user_id_count),
            sizeof(policy->owner_user_id_count)
        ) &&
        policy->owner_user_id_count != 0U) {
        status = dcc_app_route_group_use_user_allowlist(
            group,
            policy->owner_user_ids,
            policy->owner_user_id_count
        );
        if (status != DCC_OK) {
            return status;
        }
    }

    if (dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, required_permissions),
            sizeof(policy->required_permissions)
        ) &&
        policy->required_permissions != 0U) {
        status = dcc_app_route_group_use_required_permissions(group, policy->required_permissions);
        if (status != DCC_OK) {
            return status;
        }
    }

    if (dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, required_role_id_count),
            sizeof(policy->required_role_id_count)
        ) &&
        policy->required_role_id_count != 0U) {
        status = dcc_app_route_group_use_required_roles(
            group,
            policy->required_role_ids,
            policy->required_role_id_count
        );
        if (status != DCC_OK) {
            return status;
        }
    }

    if (dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, any_role_id_count),
            sizeof(policy->any_role_id_count)
        ) &&
        policy->any_role_id_count != 0U) {
        status = dcc_app_route_group_use_any_role(
            group,
            policy->any_role_ids,
            policy->any_role_id_count
        );
        if (status != DCC_OK) {
            return status;
        }
    }

    if (dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, check_count),
            sizeof(policy->check_count)
        ) &&
        policy->check_count != 0U) {
        if (policy->checks == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        for (size_t i = 0U; i < policy->check_count; ++i) {
            status = dcc_app_route_group_use_check(group, &policy->checks[i]);
            if (status != DCC_OK) {
                return status;
            }
        }
    }

    if (dcc_app_route_policy_has_field(
            policy,
            offsetof(dcc_app_route_policy_t, cooldown),
            sizeof(policy->cooldown)
        ) &&
        policy->cooldown.limit != 0U &&
        policy->cooldown.window_ms != 0U) {
        status = dcc_app_route_group_use_cooldown(group, &policy->cooldown);
        if (status != DCC_OK) {
            return status;
        }
    }

    return DCC_OK;
}
