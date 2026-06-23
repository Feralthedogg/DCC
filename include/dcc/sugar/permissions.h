#ifndef DCC_SUGAR_PERMISSIONS_H
#define DCC_SUGAR_PERMISSIONS_H

#include <dcc/permissions.h>
#include <dcc/sugar/context.h>

#include <stddef.h>
#include <stdint.h>

#define DCC_PERMISSIONS(...) \
    dcc_sugar_permissions_or( \
        (dcc_permission_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_permission_t[]){ __VA_ARGS__ }) / sizeof(dcc_permission_t) \
    )

#define DCC_PERMISSIONS_ARRAY(permissions_, permission_count_) \
    dcc_sugar_permissions_or((permissions_), (permission_count_))

#define DCC_PERMISSION_TEXT_BASIC \
    (DCC_PERMISSION_VIEW_CHANNEL | \
     DCC_PERMISSION_SEND_MESSAGES | \
     DCC_PERMISSION_READ_MESSAGE_HISTORY)

#define DCC_PERMISSION_TEXT_RICH \
    (DCC_PERMISSION_TEXT_BASIC | \
     DCC_PERMISSION_EMBED_LINKS | \
     DCC_PERMISSION_ATTACH_FILES | \
     DCC_PERMISSION_ADD_REACTIONS)

#define DCC_PERMISSION_THREAD_BASIC \
    (DCC_PERMISSION_CREATE_PUBLIC_THREADS | \
     DCC_PERMISSION_CREATE_PRIVATE_THREADS | \
     DCC_PERMISSION_SEND_MESSAGES_IN_THREADS)

#define DCC_PERMISSION_VOICE_BASIC \
    (DCC_PERMISSION_VIEW_CHANNEL | \
     DCC_PERMISSION_CONNECT | \
     DCC_PERMISSION_SPEAK | \
     DCC_PERMISSION_USE_VAD)

#define DCC_PERMISSION_MODERATION_BASIC \
    (DCC_PERMISSION_KICK_MEMBERS | \
     DCC_PERMISSION_BAN_MEMBERS | \
     DCC_PERMISSION_MANAGE_MESSAGES | \
     DCC_PERMISSION_MODERATE_MEMBERS)

#define DCC_PERMISSION_ADMIN_BASIC \
    (DCC_PERMISSION_MANAGE_GUILD | \
     DCC_PERMISSION_MANAGE_CHANNELS | \
     DCC_PERMISSION_MANAGE_ROLES)

static inline dcc_permission_t dcc_sugar_permissions_or(
    const dcc_permission_t *permissions,
    size_t permission_count
) {
    dcc_permission_t result = 0U;
    if (permissions == NULL) {
        return 0U;
    }
    for (size_t i = 0U; i < permission_count; ++i) {
        result |= permissions[i];
    }
    return result;
}

static inline uint8_t dcc_sugar_ctx_can_any_permission(
    const dcc_ctx_t *ctx,
    dcc_permission_t permissions
) {
    return dcc_ctx_has_member_permissions(ctx) &&
        dcc_permissions_can_any(dcc_ctx_member_permissions(ctx), permissions)
            ? 1U
            : 0U;
}

static inline uint8_t dcc_sugar_ctx_has_raw_permissions(
    const dcc_ctx_t *ctx,
    dcc_permission_t permissions
) {
    return dcc_ctx_has_member_permissions(ctx) &&
        dcc_permissions_has(dcc_ctx_member_permissions(ctx), permissions)
            ? 1U
            : 0U;
}

static inline uint8_t dcc_sugar_ctx_has_any_raw_permission(
    const dcc_ctx_t *ctx,
    dcc_permission_t permissions
) {
    return dcc_ctx_has_member_permissions(ctx) &&
        dcc_permissions_has_any(dcc_ctx_member_permissions(ctx), permissions)
            ? 1U
            : 0U;
}

#define DCC_CTX_CAN(ctx_, permissions_) \
    DCC_CTX_HAS_PERMISSIONS((ctx_), (permissions_))
#define DCC_CTX_CAN_PERMISSION(ctx_, permissions_) \
    DCC_CTX_HAS_PERMISSIONS((ctx_), (permissions_))
#define DCC_CTX_CAN_ANY_PERMISSION(ctx_, permissions_) \
    dcc_sugar_ctx_can_any_permission((ctx_), (permissions_))
#define DCC_CTX_HAS_RAW_PERMISSIONS(ctx_, permissions_) \
    dcc_sugar_ctx_has_raw_permissions((ctx_), (permissions_))
#define DCC_CTX_HAS_ANY_RAW_PERMISSION(ctx_, permissions_) \
    dcc_sugar_ctx_has_any_raw_permission((ctx_), (permissions_))

#define DCC_CTX_REQUIRE_CAN(ctx_, permissions_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_CAN((ctx_), (permissions_)), (message_))
#define DCC_CTX_REQUIRE_CAN_ANY(ctx_, permissions_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_CAN_ANY_PERMISSION((ctx_), (permissions_)), (message_))

#define DCC_REQUIRE_CAN(ctx_, permissions_, message_) \
    DCC_CTX_REQUIRE_CAN((ctx_), (permissions_), (message_))
#define DCC_REQUIRE_CAN_ANY(ctx_, permissions_, message_) \
    DCC_CTX_REQUIRE_CAN_ANY((ctx_), (permissions_), (message_))

#endif
