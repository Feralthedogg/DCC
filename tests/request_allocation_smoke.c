/* Compile the real constructor/free code with allocator interception confined
 * to this translation unit. No test hooks or symbols enter public headers. */
#include "internal/dcc_core_internal.h"
#include "internal/rest/dcc_rest_async_request_internal.h"
#include "internal/rest/dcc_rest_request_handle_internal.h"
#include "internal/rest/dcc_rest_sensitive_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct { void *ptr; size_t size; } blocks[16];
static struct { unsigned char *ptr; size_t size; } secrets[4];
static size_t calls, live, fail_at, secret_count;
static int wipe_failed;

static void *tracked_malloc(size_t size) {
    ++calls;
    if (calls == fail_at) return NULL;
    void *p = malloc(size);
    if (p != NULL) {
        for (size_t i = 0; i < 16U; ++i) if (blocks[i].ptr == NULL) {
            blocks[i].ptr = p; blocks[i].size = size; ++live; return p;
        }
        abort();
    }
    return p;
}
static void *tracked_calloc(size_t n, size_t size) {
    if (size && n > SIZE_MAX / size) return NULL;
    void *p = tracked_malloc(n * size);
    if (p) memset(p, 0, n * size);
    return p;
}
static void tracked_free(void *p) {
    if (!p) return;
    for (size_t i = 0; i < 16U; ++i) if (blocks[i].ptr == p) {
        const char *markers[] = {"private-token", "private-reason", "/webhooks/1/secret"};
        for (size_t j = 0; j < 3U; ++j) {
            size_t length = strlen(markers[j]);
            for (size_t offset = 0; length <= blocks[i].size && offset <= blocks[i].size - length; ++offset)
                if (memcmp((char *)p + offset, markers[j], length) == 0) wipe_failed = 1;
        }
        for (size_t j = 0; j < secret_count; ++j) {
            uintptr_t at = (uintptr_t)secrets[j].ptr;
            if (at >= (uintptr_t)p && at - (uintptr_t)p < blocks[i].size) {
                for (size_t k = 0; k < secrets[j].size; ++k)
                    if (secrets[j].ptr[k] != 0U) wipe_failed = 1;
            }
        }
        blocks[i].ptr = NULL; --live; free(p); return;
    }
    abort(); /* Also catches freeing an interior metadata pointer. */
}
static char *tracked_strdup(const char *text) {
    if (!text) return NULL;
    size_t len = strlen(text) + 1U;
    char *p = tracked_malloc(len);
    if (p) memcpy(p, text, len);
    return p;
}
static void tracked_sensitive_free(char *p, size_t size) {
    dcc_endpoint_secure_zero(p, size);
    tracked_free(p);
}

#define malloc tracked_malloc
#define calloc tracked_calloc
#define free tracked_free
#define dcc_strdup tracked_strdup
#define dcc_rest_sensitive_free tracked_sensitive_free
#define dcc_rest_async_request_new subject_new
#define dcc_rest_async_request_free subject_free
#define dcc_rest_async_concurrency subject_concurrency
#define dcc_rest_priority_valid subject_priority_valid
#define dcc_rest_async_request_canceled subject_canceled
#define dcc_rest_async_request_swap_fd subject_swap_fd
#define dcc_rest_async_request_cancel subject_cancel
void subject_free(dcc_rest_async_request_t *request);
#include "rest/rest_async_request_new.c"
#include "rest/rest_async_request.c"
#undef malloc
#undef calloc
#undef free

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "allocation: %s line %d\n", #x, __LINE__); return 1; } } while (0)

static dcc_rest_async_request_t *construct(const void *body, size_t len,
                                          dcc_rest_request_t *handle) {
    return subject_new(NULL, "POST", "test", "/webhooks/1/secret", body, len,
                       "application/octet-stream", "private-reason",
                       DCC_REST_AUTH_BOT, "private-token",
                       DCC_REST_CALL_FLAG_SENSITIVE_REQUEST_BODY, 1U,
                       DCC_REST_PRIORITY_NORMAL, NULL, NULL, handle);
}

int main(int argc, char **argv) {
    (void)argv;
    (void)tracked_strdup; /* Used only by the pre-optimization constructor. */
    const unsigned char body[] = {0x61, 0x00, 0x62, 0xff};
    calls = 0;
    dcc_rest_async_request_t *r = construct(body, sizeof(body), NULL);
    CHECK(r != NULL);
    size_t measured = calls;
    secrets[0].ptr = (unsigned char *)r->wire_path; secrets[0].size = strlen(r->wire_path) + 1U;
    secrets[1].ptr = (unsigned char *)r->audit_log_reason; secrets[1].size = strlen(r->audit_log_reason) + 1U;
    secrets[2].ptr = (unsigned char *)r->auth_token; secrets[2].size = strlen(r->auth_token) + 1U;
    secrets[3].ptr = (unsigned char *)r->body; secrets[3].size = sizeof(body);
    secret_count = 4U;
    CHECK(r->body_len == 4U && memcmp(r->body, body, 4U) == 0);
    CHECK(!subject_canceled(r));
    size_t fds = 0U;
    CHECK(subject_cancel(r, NULL, &fds) == 1 && subject_canceled(r));
    CHECK(subject_cancel(r, NULL, &fds) == 0 && fds == 0U);
    subject_free(r);
    CHECK(live == 0U && !wipe_failed);
    secret_count = 0U;

    char method[] = "POST", operation[] = "copy", path[] = "/copy";
    char type[] = "text/plain", reason[] = "audit", token[] = "token";
    unsigned char mutable_body[] = {1U, 0U, 2U};
    r = subject_new(NULL, method, operation, path, mutable_body, 3U, type,
                    reason, DCC_REST_AUTH_BOT, token, 0U, 0U,
                    DCC_REST_PRIORITY_NORMAL, NULL, NULL, NULL);
    CHECK(r != NULL);
    memset(method, 'x', 4U); memset(operation, 'x', 4U); memset(path, 'x', 5U);
    memset(type, 'x', 10U); memset(reason, 'x', 5U); memset(token, 'x', 5U);
    memset(mutable_body, 9, 3U);
    CHECK(strcmp(r->method, "POST") == 0 && strcmp(r->operation, "copy") == 0);
    CHECK(strcmp(r->wire_path, "/copy") == 0 && strcmp(r->content_type, "text/plain") == 0);
    CHECK(strcmp(r->audit_log_reason, "audit") == 0 && strcmp(r->auth_token, "token") == 0);
    CHECK(memcmp(r->body, "\1\0\2", 3U) == 0);
    subject_free(r);

    static unsigned char large[65536];
    memset(large, 'L', sizeof(large));
    large[32768] = 0U;
    r = construct(large, sizeof(large), NULL);
    CHECK(r && r->body_len == sizeof(large));
    memset(large, '?', sizeof(large));
    CHECK(r->body[0] == 'L' && r->body[32768] == 0 && r->body[65535] == 'L');
    secrets[0].ptr = (unsigned char *)r->body; secrets[0].size = sizeof(large);
    secret_count = 1U;
    subject_free(r);
    CHECK(!wipe_failed && live == 0U);
    secret_count = 0U;

    calls = 0U;
    r = construct(NULL, 0U, NULL);
    CHECK(r != NULL && r->body == NULL && r->body_len == 0U);
    size_t empty_measured = calls;
    subject_free(r);
    r = subject_new(NULL, "GET", "optional", "/", NULL, 0U, NULL, NULL,
                    DCC_REST_AUTH_BOT, NULL, 0U, 0U,
                    DCC_REST_PRIORITY_NORMAL, NULL, NULL, NULL);
    CHECK(r && !r->content_type && !r->audit_log_reason && !r->auth_token);
    subject_free(r);
    r = subject_new(NULL, "GET", "optional", "/", NULL, 0U, "", "",
                    DCC_REST_AUTH_BOT, "", 0U, 0U,
                    DCC_REST_PRIORITY_NORMAL, NULL, NULL, NULL);
    CHECK(r && r->content_type && r->audit_log_reason && r->auth_token);
    CHECK(!r->content_type[0] && !r->audit_log_reason[0] && !r->auth_token[0]);
    subject_free(r);

    const char *required[][3] = {
        {NULL, "operation", "/path"}, {"GET", NULL, "/path"}, {"GET", "operation", NULL}
    };
    for (size_t i = 0U; i < 3U; ++i) {
        dcc_rest_request_t *handle = NULL;
        CHECK(dcc_rest_request_handle_create(NULL, NULL, NULL, NULL, NULL, 1U, &handle) == DCC_OK);
        r = subject_new(NULL, required[i][0], required[i][1], required[i][2],
                        NULL, 0U, NULL, NULL, DCC_REST_AUTH_BOT, NULL, 0U, 0U,
                        DCC_REST_PRIORITY_NORMAL, NULL, NULL, handle);
        CHECK(r == NULL && live == 0U && atomic_load(&handle->references) == 1U);
        CHECK(atomic_load(&handle->async_request) == NULL);
        dcc_rest_request_handle_release(handle);
    }

    /* Every constructor allocation failure must release owned handle reference
     * exactly once, leave the caller reference valid, and reclaim all blocks. */
    for (size_t failure = 1; failure <= measured; ++failure) {
        dcc_rest_request_t *handle = NULL;
        CHECK(dcc_rest_request_handle_create(NULL, NULL, NULL, NULL, NULL, 1U, &handle) == DCC_OK);
        calls = 0; fail_at = failure;
        r = construct(body, sizeof(body), handle);
        CHECK(r == NULL && live == 0U && !wipe_failed);
        CHECK(atomic_load(&handle->references) == 1U && atomic_load(&handle->async_request) == NULL);
        dcc_rest_request_handle_release(handle);
    }
    fail_at = 0U;
    CHECK(live == 0U);
    if (argc > 1) {
        printf("{\"schema_version\":1,\"kind\":\"measured\",\"scope\":\"request constructor only; six metadata strings; excludes handle and runtime\","
               "\"allocations_per_nonempty_request\":%zu,\"allocations_per_empty_request\":%zu}\n",
               measured, empty_measured);
    } else {
        printf("request allocations measured: body=%zu empty=%zu (constructor only)\n", measured, empty_measured);
    }
    /* Explicit performance contract: metadata ownership must fit one allocation
     * and a nonempty separately copied body may use one additional allocation. */
    if (argc == 1) CHECK(measured <= 2U && empty_measured <= 1U);
    return 0;
}
