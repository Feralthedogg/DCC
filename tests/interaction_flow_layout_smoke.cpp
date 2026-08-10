#include <dcc/interaction_flow.h>

#include <cstddef>

static_assert(
    offsetof(dcc_interaction_flow_t, response_flags) >= 56U,
    "response flags must not reuse the historical 56-byte flow object"
);
static_assert(
    offsetof(dcc_interaction_flow_t, response_flags) +
        sizeof(((dcc_interaction_flow_t *)nullptr)->response_flags) > 56U,
    "response flags must extend beyond the historical flow object"
);
static_assert(
    sizeof(dcc_interaction_flow_t) > 56U,
    "the C++ layout must honestly advertise its larger allocation"
);

int main() {
    return 0;
}
