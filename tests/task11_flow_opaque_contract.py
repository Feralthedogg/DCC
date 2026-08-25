#!/usr/bin/env python3
from __future__ import annotations

import argparse
from pathlib import Path
import subprocess
import tempfile


def compile_unit(compiler: Path, include_dirs: list[Path], source: str) -> subprocess.CompletedProcess[str]:
    with tempfile.TemporaryDirectory(prefix="dcc-task11-flow-") as directory:
        unit = Path(directory) / "contract.c"
        unit.write_text(source, encoding="utf-8")
        command = [str(compiler), "-std=c11", "-fsyntax-only",
                   "-Werror=implicit-function-declaration"]
        command.extend(f"-I{path}" for path in include_dirs)
        command.append(str(unit))
        return subprocess.run(command, capture_output=True, text=True, check=False)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--compiler", type=Path, required=True)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--binary", type=Path, required=True)
    parser.add_argument("--llam-include", type=Path, required=True)
    args = parser.parse_args()
    includes = [args.source / "include", args.binary / "generated/include",
                args.source / "src", args.llam_include]

    canonical = compile_unit(args.compiler, includes, r'''
#include <dcc/interaction_flow.h>
#include <dcc/app/context.h>
#include <dcc/interaction_helpers.h>
#include <dcc/rest/request.h>
#include <dcc/client.h>
#include <dcc/rest.h>
typedef dcc_status_t (*create_fn)(dcc_client_t *, const dcc_interaction_t *,
                                  dcc_interaction_flow_t **);
typedef void (*destroy_fn)(dcc_interaction_flow_t *);
typedef dcc_status_t (*flow_reply_fn)(dcc_interaction_flow_t *,
    const dcc_message_builder_t *, dcc_rest_result_fn, void *);
typedef dcc_status_t (*ctx_reply_fn)(dcc_ctx_t *,
    const dcc_message_builder_t *, dcc_rest_result_fn, void *);
typedef dcc_status_t (*helper_reply_fn)(dcc_client_t *,
    const dcc_interaction_t *, const dcc_message_builder_t *,
    dcc_rest_result_fn, void *);
_Static_assert(_Generic(&dcc_flow_create, create_fn: 1, default: 0), "create");
_Static_assert(_Generic(&dcc_flow_destroy, destroy_fn: 1, default: 0), "destroy");
_Static_assert(_Generic(&dcc_flow_reply, flow_reply_fn: 1, default: 0), "flow result callback");
_Static_assert(_Generic(&dcc_ctx_reply, ctx_reply_fn: 1, default: 0), "context result callback");
_Static_assert(_Generic(&dcc_interaction_reply_message, helper_reply_fn: 1, default: 0), "helper result callback");

typedef dcc_status_t (*runtime_stats_fn)(dcc_client_t *, dcc_rest_runtime_stats_t *);
_Static_assert(_Generic(&dcc_rest_runtime_stats, runtime_stats_fn: 1, default: 0), "runtime stats");
static dcc_client_options_t limits = {
    .size = sizeof(dcc_client_options_t),
    .rest_max_queued_requests = 1U,
    .rest_max_request_bytes = 2U,
    .rest_max_queued_bytes = 3U,
    .rest_max_active_bytes = 4U,
    .rest_max_response_bytes = 5U,
    .interaction_max_live_queues = 6U,
    .interaction_max_tombstones = 7U,
    .interaction_tombstone_ttl_ms = 8U,
    .interaction_max_actions_per_queue = 9U,
    .interaction_max_reserved_bytes_per_queue = 10U,
    .interaction_max_reserved_bytes_total = 11U,
};
static dcc_rest_runtime_stats_t stats = DCC_REST_RUNTIME_STATS_INIT;
_Static_assert(DCC_ERR_RESOURCE_LIMIT > DCC_ERR_CANCELED, "status append");
''')
    exposed = compile_unit(args.compiler, includes, r'''
#include <dcc/interaction_flow.h>
void old_surface(dcc_interaction_flow_t *flow, dcc_client_t *client,
                 const dcc_interaction_t *interaction) {
    (void)sizeof(dcc_interaction_flow_t);
    dcc_flow_init(flow, client, interaction);
}
''')
    if canonical.returncode != 0:
        print("Task 11 opaque flow create/destroy contract is missing")
        print((canonical.stdout + canonical.stderr)[:12000])
        return 1
    if exposed.returncode == 0:
        print("Task 11 flow layout/init remain publicly exposed")
        return 1
    print("Task 11 opaque flow compile contract confirmed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
