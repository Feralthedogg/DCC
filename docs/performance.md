# Offline performance measurements

DCC's benchmarks are opt-in, offline microbenchmarks. They do not measure
Discord, TLS, network latency, or production workloads. No real token is needed.

## Reproduce

Use an optimized build without sanitizers; use a separate sanitizer build for
correctness. LLAM 2.2.1 or newer is required.

```sh
cmake -S . -B /tmp/dcc-perf \
  -DCMAKE_BUILD_TYPE=Release \
  -DDCC_BUILD_BENCHMARKS=ON -DDCC_BUILD_TESTS=ON \
  -DDCC_BUILD_EXAMPLES=OFF -DDCC_BUILD_TOOLS=OFF \
  -DDCC_WITH_OPUS=OFF -DDCC_WARNINGS_AS_ERRORS=ON \
  -DDCC_LLAM_ROOT="$LLAM_ROOT" -DDCC_LLAM_USE_SUBDIRECTORY=ON
cmake --build /tmp/dcc-perf --target \
  dcc_runtime_bench dcc_operation_bench dcc_request_allocation_smoke -j 8
python3 tools/run_benchmarks.py --repeat 7 --build-dir /tmp/dcc-perf \
  --library /tmp/dcc-perf/libdcc.a --output runtime.json \
  -- /tmp/dcc-perf/dcc_runtime_bench
python3 tools/run_benchmarks.py --repeat 7 --build-dir /tmp/dcc-perf \
  --library /tmp/dcc-perf/libdcc.a --output operations.json \
  -- /tmp/dcc-perf/dcc_operation_bench
/tmp/dcc-perf/dcc_request_allocation_smoke
/tmp/dcc-perf/dcc_request_allocation_smoke --measure
```

`dcc_operation_bench` is currently POSIX-only. `dcc_runtime_bench` uses monotonic
elapsed time: `clock_gettime(CLOCK_MONOTONIC)` on POSIX and
`QueryPerformanceCounter`/`QueryPerformanceFrequency` on Windows. A clock-query
failure terminates the benchmark with an error. CPU time uses the process CPU clock. Set
`DCC_BENCH_ITERS` to change the runtime benchmark's base iteration count
(default 20,000). Dispatch, small request, and identity-admission stages run
ten times this count. All runtime stages have 128 untimed warm-up operations.
The legacy `dcc_json_gateway_bench` remains available unchanged.

Source `commit` and `dirty` are JSON `null` when Git metadata is unavailable
(for example in a source archive or without Git installed). A `null` dirty
state means unknown, not clean; Git checkouts retain their real revision and
boolean dirty state. An archive nested inside some other Git checkout does not
inherit that enclosing repository's identity. Missing metadata does not skip measurements or relax
benchmark execution and JSON validation errors.

The runner emits schema-version-1 JSON with source revision/dirty state,
fixture-source SHA256 values (including the shared clock header), machine/platform, CMake options, command, all raw
samples, stderr, and median summaries. `--library` records the linked library's
SHA256; `--library-commit` can identify a separately built baseline library.
Commit metadata describes the working source, not an automatically verified
binary provenance. Preserve the build and library metadata when comparing runs.
Allocation fields are `null` unless provided with an explicit measured or
analytical classification and scope. Null means unmeasured, not zero.

For a before/after comparison, compile exactly the same fixture source with the
same compiler/options and link it against each library. If the earlier revision
lacks a benchmark, build its pristine library separately, then relink the new
fixture object using the generated CMake link command, replacing only the DCC
library argument and output executable. Do not compare unrelated legacy/new
fixtures. Run serially, with other builds stopped, and retain raw repeated data.

## Measured request allocation change

The recorded tables isolate allocation commit
`713901bbdf076b44a256583d4cb46dfd185ad798` against pre-optimization source.
They are not measurements of all subsequent 2.1.0 changes. Final integration
benchmark smoke checks establish usability only, not a new before/after claim.

The September 8, 2026 measurement used AppleClang 21.0.0.21000101, Apple M4 arm64
macOS, Release `-O3 -DNDEBUG`, LLAM 2.2.1, Opus off, and seven interleaved pairs
of independent before/after process runs after initial baseline measurements.
The runtime fixture was identical before/after (SHA256
`531d4b9ec07096fabf91f5d583a10bf2928389077e82ede39b581642a23613f6`).

The request constructor now stores its six copied metadata strings in the tail
of the request allocation. String lengths and aggregate size are overflow
checked. The exact body span is still copied separately, including embedded NUL
bytes. All metadata is wiped before freeing the owning block; sensitive request
bodies retain their separate secure wipe. There is no pool, retained free list,
or public layout change. Private scalar fields use existing padding to keep the
request struct at its original 336 bytes on this build, preserving size-based
resource admission charges.

The focused test recompiles the real constructor and destructor with allocator
wrappers confined to the test translation unit. For six present strings it
**measured 8 → 2 allocations with a body and 7 → 1 without a body**. These counts
exclude request handles, queues, builders, responses, and runtime work. Every
constructor allocation failure is injected individually; tests check cleanup,
handle-reference release, copied strings/bytes, cancellation and secret wiping.
`--measure` emits the measured counts as a separate JSON allocation report;
the timing reports leave their uninstrumented allocation fields null.

Median time per operation in nanoseconds:

| Fixture / measured stage | CPU before | CPU after | Elapsed before | Elapsed after |
| --- | ---: | ---: | ---: | ---: |
| Message / stage1 validation | 594.60 | 602.30 | 594.65 | 602.25 |
| Message / gateway parse | 19,822.85 | 19,770.00 | 20,122.35 | 19,853.50 |
| Message / validation + parse | 20,565.95 | 20,134.45 | 20,882.65 | 20,227.85 |
| 0 listeners / dispatch | 8.69 | 8.71 | 8.68 | 8.71 |
| 1 listener / dispatch | 9.62 | 9.44 | 9.72 | 9.45 |
| 8 listeners / dispatch | 15.84 | 21.31 | 15.85 | 21.36 |
| 9 listeners / dispatch | 33.20 | 32.45 | 33.56 | 32.57 |
| 32 listeners / dispatch | 58.91 | 59.80 | 59.13 | 60.08 |
| Empty body / request construct + free | 585.34 | 476.00 | 586.25 | 476.80 |
| 32-byte sensitive body / construct + free | 597.06 | 525.06 | 598.32 | 526.98 |
| 64-KiB sensitive body / construct + free | 18,559.10 | 18,696.20 | 18,764.80 | 18,866.05 |
| Interaction identity / queue admit + release | 30.29 | 29.86 | 30.40 | 29.92 |

This supports fewer allocations and approximately 12–19% lower small-request
constructor/free CPU time in this fixture. It does **not** establish a large-body,
parser, dispatch, or end-to-end latency improvement. Secure body wiping is inside
the timed request stage and dominates the large-body case. The unchanged
eight-listener stage measured about 5.5 ns slower in the final paired runs;
the cause is not established, and this result is not hidden as a dispatch gain.

## Offline operation completion

A separate fixture exercises public REST submission and interaction reply using
the existing synchronous REST interceptor, an active runtime thread, successful
204 responses, callbacks, and `dcc_rest_async_wait`. Each stage submits and
fully drains 500 operations serially. Client setup, message setup and interaction
identity registration are outside the timed stage. Flow creation is not a proxy
for reply admission; the identity microbenchmark above measures only queue
reservation/release, not serialization or operation completion.

The same compiled operation-fixture object was linked against the original
`b89bbea208d84d23d7f6488556db87ec30c33c31` library and the optimized library,
using the same LLAM and OpenSSL libraries. This baseline has the same production
request code as the later documentation-only commit used for the runtime baseline.

| Stage | Mean elapsed before/after (ns) | p95 before/after (ns) | p99 before/after (ns) |
| --- | ---: | ---: | ---: |
| REST submit → callback + drain | 5,098 / 5,108 | 6,000 / 6,000 | 9,000 / 10,000 |
| Interaction reply → callback + drain | 6,472 / 6,826 | 7,000 / 9,000 | 10,000 / 12,000 |

Each number is the median of seven runs' statistic. Percentiles are actual
per-operation elapsed samples, using nearest rank (475th and 495th of 500),
not percentiles of batch means. Process CPU means were 6,110 → 6,148 ns for REST
and 7,390 → 7,542 ns for replies. The final paired reply mean/tails were higher,
while earlier serial runs differed; scheduling and clock granularity limit
interpretation. No completion-latency improvement is claimed. There is no
network, real Discord rate limiting, or concurrent workload in this fixture.

## Optimizations deliberately not adopted

- **JSON partial reset:** the full payload `memset` remains. Reuse tests cover a
  larger message followed by a smaller one, failure followed by success, event
  type changes, and clearing truncated-list state and optional data. Preserving
  these guarantees is more important than an unsupported reset-time claim.
- **Dispatch snapshot redesign:** source analysis gives zero snapshot allocations
  through eight listeners, and one allocation/free above eight. These are
  analytical counts, not instrumented measurements. The measured 8-to-9 listener
  step is about 11–18 ns in this fixture, including the additional callback. It
  does not justify introducing shared snapshot ownership, caching or reclamation
  complexity into mutation/reentrant/concurrent listener semantics.
- **Pooling / zero-copy request admission:** not added. Caller buffers remain
  reusable immediately after submission; sensitive bytes are not retained in a
  pool. Large-body and broader throughput improvements require separate evidence.

These results are observations on one machine, not portable performance
guarantees or confidence intervals. The volatile callback sink, allocator,
cache state, OS scheduling, security wiping and microsecond clock resolution
all affect the results. Allocation counts are more reproducible than nanosecond
timing differences; raw JSON should accompany any further comparison.
