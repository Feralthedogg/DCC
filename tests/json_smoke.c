#include "support/json_smoke_support.h"

int main(void) {
    if (dcc_json_smoke_dom_core() != 0 ||
        dcc_json_smoke_gateway_foundation() != 0 ||
        dcc_json_smoke_interactions() != 0 ||
        dcc_json_smoke_gateway_extended() != 0 ||
        dcc_json_smoke_invalid_inputs() != 0) {
        return 1;
    }

    return 0;
}
