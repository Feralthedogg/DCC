#include <dcc/sugar.h>
#include <dcc/sugar/permissions.h>

#include <stdio.h>

static int expect_permissions(
    const char *label,
    dcc_permission_t actual,
    dcc_permission_t expected
) {
    if (actual != expected) {
        fprintf(
            stderr,
            "%s mismatch: actual=%llu expected=%llu\n",
            label,
            (unsigned long long)actual,
            (unsigned long long)expected
        );
        return 1;
    }
    return 0;
}

static int check_permission_sets(void) {
    dcc_permission_t moderation =
        DCC_PERMISSIONS(
            DCC_PERMISSION_KICK_MEMBERS,
            DCC_PERMISSION_BAN_MEMBERS,
            DCC_PERMISSION_MANAGE_MESSAGES
        );
    dcc_permission_t voice_array[] = {
        DCC_PERMISSION_CONNECT,
        DCC_PERMISSION_SPEAK,
        DCC_PERMISSION_USE_VAD
    };

    return
        expect_permissions(
            "moderation",
            moderation,
            DCC_PERMISSION_KICK_MEMBERS |
                DCC_PERMISSION_BAN_MEMBERS |
                DCC_PERMISSION_MANAGE_MESSAGES
        ) ||
        expect_permissions(
            "voice array",
            DCC_PERMISSIONS_ARRAY(voice_array, 3U),
            DCC_PERMISSION_CONNECT |
                DCC_PERMISSION_SPEAK |
                DCC_PERMISSION_USE_VAD
        ) ||
        !dcc_permissions_has(DCC_PERMISSION_TEXT_RICH, DCC_PERMISSION_TEXT_BASIC) ||
        !dcc_permissions_has(DCC_PERMISSION_VOICE_BASIC, DCC_PERMISSION_CONNECT) ||
        !dcc_permissions_has(DCC_PERMISSION_MODERATION_BASIC, DCC_PERMISSION_MODERATE_MEMBERS) ||
        !dcc_permissions_has(DCC_PERMISSION_ADMIN_BASIC, DCC_PERMISSION_MANAGE_ROLES);
}

static int check_context_helpers_are_null_safe(void) {
    return
        DCC_CTX_CAN(NULL, DCC_PERMISSION_MANAGE_MESSAGES) != 0U ||
        DCC_CTX_CAN_PERMISSION(NULL, DCC_PERMISSION_MANAGE_MESSAGES) != 0U ||
        DCC_CTX_CAN_ANY_PERMISSION(NULL, DCC_PERMISSION_MANAGE_MESSAGES) != 0U ||
        DCC_CTX_HAS_RAW_PERMISSIONS(NULL, DCC_PERMISSION_MANAGE_MESSAGES) != 0U ||
        DCC_CTX_HAS_ANY_RAW_PERMISSION(NULL, DCC_PERMISSION_MANAGE_MESSAGES) != 0U;
}

int main(void) {
    if (check_permission_sets() != 0) {
        return 1;
    }
    if (check_context_helpers_are_null_safe() != 0) {
        return 1;
    }
    return 0;
}
