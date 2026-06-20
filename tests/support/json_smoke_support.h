#ifndef DCC_TEST_JSON_SMOKE_SUPPORT_H
#define DCC_TEST_JSON_SMOKE_SUPPORT_H

#include "internal/json/dcc_json_internal.h"
#include "internal/json/dcc_json.h"

#include <dcc/dcc.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

int dcc_json_smoke_str_eq(const char *actual, const char *expected);
dcc_json_gateway_payload_t *dcc_json_smoke_payload(void);
int dcc_json_smoke_arena_allocator_ok(void);
int dcc_json_smoke_interaction_resolved_ok(const dcc_interaction_t *interaction);
int dcc_json_smoke_interaction_metadata_ok(const dcc_interaction_t *interaction);
int dcc_json_smoke_interaction_resolved_clone_ok(
    const dcc_interaction_t *original,
    const dcc_interaction_t *copy
);

int dcc_json_smoke_dom_core(void);
int dcc_json_smoke_gateway_foundation(void);
int dcc_json_smoke_gateway_foundation_guild_create(void);
int dcc_json_smoke_interactions(void);
int dcc_json_smoke_gateway_extended(void);
int dcc_json_smoke_invalid_inputs(void);

#endif
