#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#define DAVE_TEST_API __declspec(dllexport)
#else
#define DAVE_TEST_API __attribute__((visibility("default")))
#endif

typedef struct fake_handle { int value; } *fake_handle_t;
typedef void (*failure_cb_t)(const char *, const char *, void *);

static fake_handle_t make_handle(void) {
    fake_handle_t handle = malloc(sizeof(*handle));
    if (handle != NULL) handle->value = 1;
    return handle;
}

DAVE_TEST_API uint16_t daveMaxSupportedProtocolVersion(void) { return 1U; }
DAVE_TEST_API void daveFree(void *ptr) { free(ptr); }
DAVE_TEST_API void *daveSessionCreate(void *context, const char *auth, failure_cb_t cb, void *data) {
    (void)context; (void)auth; (void)cb; (void)data; return make_handle();
}
DAVE_TEST_API void daveSessionDestroy(void *handle) { free(handle); }
DAVE_TEST_API void daveSessionInit(void *s, uint16_t v, uint64_t g, const char *u) {
    (void)s; (void)v; (void)g; (void)u;
}
DAVE_TEST_API void daveSessionReset(void *s) { (void)s; }
DAVE_TEST_API void daveSessionSetProtocolVersion(void *s, uint16_t v) { (void)s; (void)v; }
DAVE_TEST_API void daveSessionSetExternalSender(void *s, const uint8_t *p, size_t n) {
    (void)s; (void)p; (void)n;
}
DAVE_TEST_API void daveSessionProcessProposals(
    void *s, const uint8_t *p, size_t n, const char **users, size_t user_count,
    uint8_t **out, size_t *out_size
) {
    (void)s; (void)users; (void)user_count;
    *out = malloc(n != 0U ? n : 1U); *out_size = n != 0U ? n : 1U;
    if (*out != NULL) memcpy(*out, n != 0U ? p : (const uint8_t *)"x", *out_size);
}
DAVE_TEST_API void *daveSessionProcessCommit(void *s, const uint8_t *p, size_t n) {
    (void)s; (void)p; (void)n; return make_handle();
}
DAVE_TEST_API void *daveSessionProcessWelcome(
    void *s, const uint8_t *p, size_t n, const char **users, size_t user_count
) {
    (void)s; (void)p; (void)n; (void)users; (void)user_count; return make_handle();
}
DAVE_TEST_API void daveSessionGetMarshalledKeyPackage(void *s, uint8_t **out, size_t *n) {
    (void)s; *out = malloc(2U); *n = 2U;
    if (*out != NULL) { (*out)[0] = 0x4b; (*out)[1] = 0x50; }
}
DAVE_TEST_API void *daveSessionGetKeyRatchet(void *s, const char *user) {
    (void)s; (void)user; return make_handle();
}
DAVE_TEST_API bool daveCommitResultIsFailed(void *r) { return r == NULL; }
DAVE_TEST_API bool daveCommitResultIsIgnored(void *r) { (void)r; return false; }
DAVE_TEST_API void daveCommitResultDestroy(void *r) { free(r); }
DAVE_TEST_API void daveWelcomeResultDestroy(void *r) { free(r); }
DAVE_TEST_API void daveKeyRatchetDestroy(void *r) { free(r); }
DAVE_TEST_API void *daveEncryptorCreate(void) { return make_handle(); }
DAVE_TEST_API void daveEncryptorDestroy(void *e) { free(e); }
DAVE_TEST_API void daveEncryptorSetKeyRatchet(void *e, void *r) { (void)e; (void)r; }
DAVE_TEST_API void daveEncryptorSetPassthroughMode(void *e, bool p) { (void)e; (void)p; }
DAVE_TEST_API void daveEncryptorAssignSsrcToCodec(void *e, uint32_t s, int c) { (void)e; (void)s; (void)c; }
DAVE_TEST_API size_t daveEncryptorGetMaxCiphertextByteSize(void *e, int m, size_t n) {
    (void)e; (void)m; return n + 1U;
}
DAVE_TEST_API int daveEncryptorEncrypt(
    void *e, int m, uint32_t s, const uint8_t *in, size_t n,
    uint8_t *out, size_t cap, size_t *written
) {
    (void)e; (void)m; (void)s;
    if (cap < n + 1U) return 1;
    out[0] = 0xda; memcpy(out + 1U, in, n); *written = n + 1U; return 0;
}
DAVE_TEST_API void *daveDecryptorCreate(void) { return make_handle(); }
DAVE_TEST_API void daveDecryptorDestroy(void *d) { free(d); }
DAVE_TEST_API void daveDecryptorTransitionToKeyRatchet(void *d, void *r) { (void)d; (void)r; }
DAVE_TEST_API void daveDecryptorTransitionToPassthroughMode(void *d, bool p) { (void)d; (void)p; }
DAVE_TEST_API size_t daveDecryptorGetMaxPlaintextByteSize(void *d, int m, size_t n) {
    (void)d; (void)m; return n;
}
DAVE_TEST_API int daveDecryptorDecrypt(
    void *d, int m, const uint8_t *in, size_t n,
    uint8_t *out, size_t cap, size_t *written
) {
    (void)d; (void)m;
    if (n < 1U || in[0] != 0xda || cap < n - 1U) return 1;
    memcpy(out, in + 1U, n - 1U); *written = n - 1U; return 0;
}
