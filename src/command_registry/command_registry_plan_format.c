#include "internal/command_registry/dcc_command_registry_internal.h"

dcc_status_t dcc_command_registry_plan_format(
    const dcc_command_registry_plan_t *plan,
    char *buf,
    size_t buf_len,
    size_t *out_len
) {
    if (plan == NULL || (buf == NULL && buf_len != 0)) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t used = 0;
    dcc_status_t status = dcc_command_registry_format_append(
        buf,
        buf_len,
        &used,
        "scope=%s",
        plan->guild_id != 0 ? "guild" : "global"
    );
    if (status == DCC_OK && plan->guild_id != 0) {
        status = dcc_command_registry_format_append(buf, buf_len, &used, " guild_id=%llu", (unsigned long long)plan->guild_id);
    }
    if (status == DCC_OK) {
        status = dcc_command_registry_format_append(
            buf,
            buf_len,
            &used,
            " dry_run=%s delete_stale=%s\n",
            plan->dry_run ? "true" : "false",
            plan->delete_stale ? "true" : "false"
        );
    }
    if (status == DCC_OK && plan->guild_id == 0) {
        status = dcc_command_registry_format_append(
            buf,
            buf_len,
            &used,
            "warning=global command changes may take time to propagate across Discord\n"
        );
    }
    for (size_t i = 0; status == DCC_OK && i < plan->diff_count; ++i) {
        const dcc_command_registry_diff_t *diff = &plan->diffs[i];
        status = dcc_command_registry_format_append(
            buf,
            buf_len,
            &used,
            "%s name=%s type=%u",
            dcc_command_registry_action_string(diff->action),
            diff->name != NULL ? diff->name : "",
            diff->type
        );
        if (status == DCC_OK && diff->remote_id != 0) {
            status = dcc_command_registry_format_append(
                buf,
                buf_len,
                &used,
                " remote_id=%llu",
                (unsigned long long)diff->remote_id
            );
        }
        if (status == DCC_OK) {
            status = dcc_command_registry_format_append(buf, buf_len, &used, "\n");
        }
    }
    if (status == DCC_OK) {
        status = dcc_command_registry_format_append(
            buf,
            buf_len,
            &used,
            "summary create=%zu update=%zu delete_stale=%zu noop=%zu\n",
            plan->create_count,
            plan->update_count,
            plan->delete_stale_count,
            plan->noop_count
        );
    }

    if (buf != NULL && buf_len != 0) {
        buf[used < buf_len ? used : buf_len - 1U] = '\0';
    }
    if (out_len != NULL) {
        *out_len = used;
    }
    return status;
}
