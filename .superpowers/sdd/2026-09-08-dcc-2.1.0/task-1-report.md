# Task 1 Report: Executable ownership documentation

## Outcome

Implemented one source-of-truth, full REST request-ownership example and a
deterministic marker synchronizer. Replaced the obsolete future/response guide
with the canonical request/result contract, retained the Bot builder-array
lifetime guidance, registered the snippet check in CTest, installed the
standalone example CMake project, and added a docs-workflow installed-package
compile/link check. No public header or ABI changed; the package continues to
require LLAM 2.2.1.

The guide now states and demonstrates:

- all covered inputs are copied or serialized before successful admission
  returns;
- rejection clears the request and invokes no callback;
- successful admission guarantees exactly one terminal completion;
- completion, transport status, and HTTP success are separate;
- a timed-out wait retains request ownership;
- request destruction does not suppress callbacks;
- result views and cloned results have distinct lifetimes;
- callback affinity is unspecified; and
- shutdown owners must stop admission, drain delivery, join runtime execution,
  and keep callback user data alive through delivery.

## RED

Command:

```text
python3 -m unittest discover -s tests -p test_sync_doc_examples.py
```

Observed before `tools/sync_doc_examples.py` existed:

```text
FFFF
Ran 4 tests in 0.064s
FAILED (failures=4)
```

Each failure was the expected missing-feature failure: Python could not open
`tools/sync_doc_examples.py`. No production implementation existed yet.

## GREEN and verification

Focused behavioral tests and repository check:

```text
python3 -m unittest discover -s tests -p test_sync_doc_examples.py
....
Ran 4 tests in 0.112s
OK

python3 tools/sync_doc_examples.py --check
# exit 0, no output
```

The tests exercise real subprocess behavior in temporary repositories: exact C
region rendering, preservation of unmarked prose, drift detection without
writeback, and clear missing/duplicate marker errors.

CTest registration and relevant audits:

```text
ctest --test-dir /tmp/dcc-api-werror-tests --output-on-failure \
  -R 'dcc_doc_examples_sync|dcc_project_layout_audit|dcc_api_reference_audit'
100% tests passed, 0 tests failed out of 3
```

Fresh complete configured DCC test run:

```text
ctest --test-dir /tmp/dcc-api-werror-tests --output-on-failure
100% tests passed, 0 tests failed out of 125
Total Test time (real) = 68.54 sec
```

The remaining 24 entries in the 149-test inventory are the intentionally
disabled LLAM subdirectory tests. The DCC count increased from the supplied
124-test baseline to 125 because `dcc_doc_examples_sync` was added.

Installed-package compiler/link evidence:

```text
cmake --install /tmp/dcc-api-werror-tests --prefix /tmp/dcc-task1-install
cmake -S examples/docs -B /tmp/dcc-task1-example \
  -DCMAKE_PREFIX_PATH=/tmp/dcc-task1-install \
  -DDCC_LLAM_ROOT=/tmp/dcc-llam-v221.WXXLjm \
  -DDCC_LLAM_LIBRARY=/tmp/dcc-api-werror-tests/_deps/llam/libllam_runtime.a
cmake --build /tmp/dcc-task1-example --clean-first --verbose
```

The compile command used only installed DCC and LLAM 2.2.1 include paths and
included `-Wall -Wextra -Wpedantic -Werror`. The link command used
`/tmp/dcc-task1-install/lib/libdcc.a` and the LLAM 2.2.1 library. Result:

```text
[100%] Built target dcc_rest_ownership
```

The executable was intentionally not run, so verification did not contact
Discord. The installed package also contained both
`share/dcc/examples/docs/CMakeLists.txt` and `rest_ownership.c`.

Documentation render and hygiene:

```text
PYTHONPATH=/tmp/dcc-task1-doc-deps python3 -m mkdocs build --strict \
  --site-dir /tmp/dcc-task1-site
INFO - Documentation built in 1.94 seconds

git diff --check
python3 -m py_compile tools/sync_doc_examples.py tests/test_sync_doc_examples.py
# both exit 0
```

## Changed files

- `.github/workflows/docs.yml`
- `CMakeLists.txt`
- `docs/architecture.md`
- `docs/concepts/ownership-and-async.md`
- `examples/docs/CMakeLists.txt`
- `examples/docs/rest_ownership.c`
- `tests/test_sync_doc_examples.py`
- `tools/sync_doc_examples.py`

## Self-review and concerns

- Marker validation completes for every declared pair before any document is
  written, and replacement boundaries include only the explicit destination
  marker pair. The script never evaluates source content or executes commands.
- Expected snippets are literal in tests rather than derived through the tool.
- The example uses small Win32/POSIX runtime-thread adapters. The POSIX branch
  compiled locally with warnings as errors; the Win32 branch is covered by the
  existing Windows-oriented source style but was not compiled on this macOS
  host. The Ubuntu docs workflow compiles the POSIX branch.
- MkDocs Material emitted its upstream advisory about future MkDocs 2.0 and the
  repository's pre-existing pages omitted from nav; strict rendering still
  exited successfully.
- No live Discord call was made, by design.

## Review fix round 1

Corrected the example's non-2xx diagnostic to print
`result->transport_status` as the transport outcome while continuing to print
`result->http_status` separately. The previous expression used
`dcc_rest_result_status(result)`, whose contract maps non-2xx HTTP completion to
`DCC_ERR_DISCORD` and therefore was mislabeled as transport status. Regenerated
the ownership guide from the corrected source.

Added a behavioral CRLF fixture before changing the synchronizer. RED evidence:

```text
python3 -m unittest discover -s tests -p test_sync_doc_examples.py
...F.
Ran 5 tests in 0.141s
FAILED (failures=1)
```

The literal byte assertion showed all unmarked document CRLF bytes had been
normalized to LF by text-mode `read_text()`. The implementation now decodes
explicit UTF-8 bytes, preserves all bytes outside the declared marker range,
and emits the generated region using the destination document's newline style.

GREEN and final verification:

```text
python3 -m unittest discover -s tests -p test_sync_doc_examples.py
.....
Ran 5 tests in 0.127s
OK

python3 tools/sync_doc_examples.py --check
# exit 0, no output

cmake --build /tmp/dcc-task1-example --clean-first --verbose
[100%] Built target dcc_rest_ownership

git diff --check
# exit 0
```

The verbose installed-package build again compiled with
`-Wall -Wextra -Wpedantic -Werror` against `/tmp/dcc-task1-install/include` and
linked `/tmp/dcc-task1-install/lib/libdcc.a`. The executable was not run and no
Discord request was made.
