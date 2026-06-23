#ifndef DCC_SUGAR_CONTEXT_REQUIRE_CORE_H
#define DCC_SUGAR_CONTEXT_REQUIRE_CORE_H

#include <dcc/sugar/context/require/base.h>

#define DCC_CTX_REQUIRE(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_REPLY_EPHEMERAL((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_BAD_INPUT(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_BAD_INPUT((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_BAD_INPUT_F(ctx_, condition_, ...) \
    do { \
        if (!(condition_)) { \
            (void)DCC_CTX_BAD_INPUT_F((ctx_), __VA_ARGS__); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_DENY(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_DENY((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_DENY_F(ctx_, condition_, ...) \
    do { \
        if (!(condition_)) { \
            (void)DCC_CTX_DENY_F((ctx_), __VA_ARGS__); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_NOT_FOUND(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_NOT_FOUND((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_NOT_FOUND_F(ctx_, condition_, ...) \
    do { \
        if (!(condition_)) { \
            (void)DCC_CTX_NOT_FOUND_F((ctx_), __VA_ARGS__); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_BUSY(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_BUSY((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_BUSY_F(ctx_, condition_, ...) \
    do { \
        if (!(condition_)) { \
            (void)DCC_CTX_BUSY_F((ctx_), __VA_ARGS__); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_FAIL(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_FAIL((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_FAIL_F(ctx_, condition_, ...) \
    do { \
        if (!(condition_)) { \
            (void)DCC_CTX_FAIL_F((ctx_), __VA_ARGS__); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_INTERNAL(ctx_, condition_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_INTERNAL_ERROR((ctx_)); \
            return; \
        } \
    } while (0)

#endif /* DCC_SUGAR_CONTEXT_REQUIRE_CORE_H */
