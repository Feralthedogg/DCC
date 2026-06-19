# Tools

This directory contains runtime helper tools and maintainer scripts. Audits are
wired into CTest when Python is available.

- `dcc_command_sync` reconciles local application-command JSON with Discord
  REST or a saved remote fixture. It defaults to global scope and plan output;
  pass `--guild` for guild scope and `--apply` for writes.
- `dcc_replay` validates/summarizes replay JSONL files and can feed captured
  gateway/interaction records into a hot-reload module through isolated worker
  mode.
- `dcc_hot_reload_host` and `dcc_hot_reload_worker` provide the installed
  isolated hot-reload runtime.
- `dcc_interaction_webhook` provides the installed shardless interaction
  webhook runtime.

- `audit_project_layout.py` keeps the public/internal header boundary intact and
  enforces source-size budgets for module splits.
- `audit_gateway_fast_path.py` checks Gateway parser fast-path coverage.
- `audit_gateway_payload_fields.py` checks Gateway payload scratch coverage.
- `audit_event_surface.py` checks event API coverage.
- `audit_capacity_growth.py` checks that dynamic capacity doubling has
  overflow guards.
- `audit_rest_surface.py`, `audit_rest_request_assertions.py`, and `audit_rest_field_smoke.py`
  check REST API coverage.
- `audit_source_package.py` checks that hidden local `.env` files and cache/build
  metadata are excluded without dropping deployment env templates.
- `audit_deploy_templates.py` checks systemd, env, reverse-proxy, Kubernetes,
  and hot reload deployment template contracts.
- `release_check.sh` runs the full release validation flow.
  It also dry-runs the token-backed hot reload live-soak helper without opening
  a network connection and builds the installed hot reload example Makefile
  against the installed CMake package. Source packages are checked for hidden
  `.env`, Python cache, compile database, and build metadata leakage, and
  installed packages compile a pkg-config consumer when `pkg-config`/`pkgconf`
  is available. Installed hot reload plus interaction webhook deployment
  templates are checked against `dcc_hot_reload_host --check` and
  `dcc_interaction_webhook --check` with release fixture paths.
- `package_release.sh` builds the native CPack binary/source archives used by
  the GitHub Release workflow and writes normalized files plus sha256 manifests
  under `target/dist/`.

See `../docs/release.md` for when to run these checks and
`../docs/production-playbooks.md` for operational response guidance.
