# DCC Release Checklist

Run the release check from the repository root:

```sh
tools/release_check.sh
```

Build distributable CPack archives locally:

```sh
tools/package_release.sh
```

This writes normalized archives and checksum files to `target/dist/`, for
example `dcc-0.1.0-macos-aarch64.tar.gz`.

GitHub Actions publishes releases automatically from version tags:

```sh
git tag v0.1.0
git push origin v0.1.0
```

The `Release` workflow checks out `DCC` and `LLAM` side by side, builds LLAM
through `DCC_LLAM_USE_SUBDIRECTORY=ON`, runs the DCC test suite, creates CPack
binary/source archives, uploads artifacts, builds `SHA256SUMS`, and creates the
GitHub Release. The tag version must match `project(dcc VERSION ...)` unless
`DCC_ALLOW_VERSION_MISMATCH=1` is set for an explicit development package.

The release path expects:

- Public headers only under `include/dcc/`.
- Private headers only under `src/internal/`.
- Source files within the layout audit's module-size budgets, with any temporary
  oversized file kept under its capped split budget.
- Build trees named `build*`, which are ignored by source packaging.
- Hidden local `.env` files, Python bytecode caches, compile databases, and
  build metadata excluded from source packages while deployment env templates
  remain included.
- Examples that compile using public headers.
- Package consumer tests that verify installed CMake metadata and, when
  `pkg-config` or `pkgconf` is available, compile a `dcc.pc` consumer.
- Installed hot reload host/worker preflight through `dcc_hot_reload_host
  --check`, including default-global, `--guild`, and `--global` command scope
  checks.
- Installed interaction webhook preflight through `dcc_interaction_webhook
  --check`, including service route setup and deployment template checks.
- Installed `share/dcc/examples/hot_reload` build validation, proving the
  packaged Makefile can rebuild the example module through `find_package(dcc)`
  and stage the isolated host/worker pair.
- A minimal package build with examples/tests disabled that still installs the
  isolated hot reload host/worker pair plus the interaction webhook tool and
  supports external CMake consumers.
- Optional token-backed hot reload live soak with
  `BOT_TOKEN=... make -C examples/hot_reload live-soak`.
- Optional token-backed voice join probe with
  `BOT_TOKEN=... DCC_VOICE_GUILD_ID=... DCC_VOICE_CHANNEL_ID=... DCC_VOICE_USER_ID=... ./build/dcc_live_voice_probe`.
- Installed hot reload example files, including the live-soak helper script.
- Installed deployment templates for interaction webhooks and isolated hot
  reload host/worker service operation, with service env, systemd, Kubernetes,
  and reverse-proxy templates checked against `dcc_interaction_webhook --check`
  and `dcc_hot_reload_host --check`.

Before tagging, also review:

- `docs/api.md` for public API ownership and module boundaries.
- `docs/production-playbooks.md` for Gateway, REST, interaction, hot reload,
  voice, wait-policy, and release operations.
- `docs/live-validation.md` for opt-in Discord soak and voice validation.
