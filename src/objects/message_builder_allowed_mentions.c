#include "internal/objects/dcc_message_builder_serialize_internal.h"
#include "internal/objects/dcc_message_json_members_internal.h"

static dcc_status_t dcc_allowed_mentions_append_parse(
    const dcc_allowed_mentions_builder_t *builder,
    dcc_message_json_buffer_t *buffer,
    int *first
) {
    dcc_status_t status = dcc_message_json_member_prefix(buffer, first, "parse");
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_message_json_append_cstr(buffer, "[");
    int parse_first = 1;
    if (status == DCC_OK && builder->parse_roles) {
        status = dcc_message_json_append_escaped_string(buffer, "roles");
        parse_first = 0;
    }
    if (status == DCC_OK && builder->parse_users) {
        if (!parse_first) {
            status = dcc_message_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_message_json_append_escaped_string(buffer, "users");
        }
        parse_first = 0;
    }
    if (status == DCC_OK && builder->parse_everyone) {
        if (!parse_first) {
            status = dcc_message_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_message_json_append_escaped_string(buffer, "everyone");
        }
    }
    return status == DCC_OK ? dcc_message_json_append_cstr(buffer, "]") : status;
}

static dcc_status_t dcc_allowed_mentions_append_snowflakes(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const char *name,
    const dcc_snowflake_t *items,
    size_t item_count
) {
    if (item_count == 0U) {
        return DCC_OK;
    }
    if (items == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_message_json_member_prefix(buffer, first, name);
    if (status == DCC_OK) {
        status = dcc_message_json_append_cstr(buffer, "[");
    }
    for (size_t i = 0U; status == DCC_OK && i < item_count; ++i) {
        if (i != 0U) {
            status = dcc_message_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_message_json_append_snowflake_string(buffer, items[i]);
        }
    }
    return status == DCC_OK ? dcc_message_json_append_cstr(buffer, "]") : status;
}

dcc_status_t dcc_allowed_mentions_validate_for_json(const dcc_allowed_mentions_builder_t *builder) {
    if (builder == NULL ||
        (builder->user_count != 0U && builder->users == NULL) ||
        (builder->role_count != 0U && builder->roles == NULL) ||
        (builder->parse_users && builder->user_count != 0U) ||
        (builder->parse_roles && builder->role_count != 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_allowed_mentions_append_json(
    const dcc_allowed_mentions_builder_t *builder,
    dcc_message_json_buffer_t *buffer
) {
    dcc_status_t status = dcc_allowed_mentions_validate_for_json(builder);
    if (status != DCC_OK || buffer == NULL) {
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    }

    status = dcc_message_json_append_cstr(buffer, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_allowed_mentions_append_parse(builder, buffer, &first);
    }
    if (status == DCC_OK) {
        status = dcc_allowed_mentions_append_snowflakes(
            buffer,
            &first,
            "users",
            builder->users,
            builder->user_count
        );
    }
    if (status == DCC_OK) {
        status = dcc_allowed_mentions_append_snowflakes(
            buffer,
            &first,
            "roles",
            builder->roles,
            builder->role_count
        );
    }
    if (status == DCC_OK && builder->has_replied_user) {
        status = dcc_message_json_append_bool_member(buffer, &first, "replied_user", builder->replied_user);
    }
    return status == DCC_OK ? dcc_message_json_append_cstr(buffer, "}") : status;
}
