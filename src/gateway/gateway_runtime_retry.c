#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_runtime_internal.h"
#include "internal/gateway/dcc_gateway_timing_internal.h"

char *dcc_gateway_attempt_url(dcc_client_t *client, bool resume) {
    const char *url = client->gateway_url != NULL && client->gateway_url[0] != '\0'
        ? client->gateway_url
        : client->gateway_cached_url;
    if (resume && client->gateway_resume_url != NULL && client->gateway_resume_url[0] != '\0') {
        url = client->gateway_resume_url;
    }
    return dcc_strdup(url != NULL ? url : "");
}

uint32_t dcc_gateway_backoff_ms(unsigned attempt) {
    uint32_t delay = 250U;
    unsigned shifts = attempt < 5U ? attempt : 5U;
    for (unsigned i = 0; i < shifts; ++i) {
        delay *= 2U;
    }
    return delay > 5000U ? 5000U : delay;
}

uint32_t dcc_gateway_backoff_jitter_ms(unsigned attempt, uint64_t salt) {
    uint32_t base = dcc_gateway_backoff_ms(attempt);
    uint32_t spread = base / 5U;
    uint32_t jitter = dcc_gateway_now_jitter(spread * 2U + 1U, salt + attempt);
    return base - spread + jitter;
}
