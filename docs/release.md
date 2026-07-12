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
example `dcc-1.5.0-macos-aarch64.tar.gz`.

Binary release archives include:

- DCC public headers, library, CMake package, and pkg-config metadata.
- DCC command-line tools, examples, deployment templates, and docs.

Users install LLAM separately before installing DCC, or explicitly ask the DCC
installer to fetch LLAM. The public POSIX install path is:

```sh
curl -fsSL https://github.com/Feralthedogg/LLAM/releases/latest/download/install.sh |
  sh -s -- --prefix "$HOME/.local"
curl -fsSL https://github.com/Feralthedogg/DCC/releases/download/v1.5.0/install.sh |
  sh -s -- --version 1.5.0 \
    --base-url "https://github.com/Feralthedogg/DCC/releases/download/v1.5.0" \
    --prefix "$HOME/.local"
```

Windows installs use the release PowerShell installer:

```powershell
Invoke-WebRequest "https://github.com/Feralthedogg/DCC/releases/download/v1.5.0/install.ps1" -OutFile install.ps1
.\install.ps1 -Version 1.5.0 -BaseUrl "https://github.com/Feralthedogg/DCC/releases/download/v1.5.0" -Prefix "$env:LOCALAPPDATA\DCC"
```

Pass `--install-llam` when you want the POSIX installer to fetch the DCC-tested
LLAM 2.2.0 runtime into the same prefix.
On POSIX, `--target` is normally detected from the host. Manually selecting a
different target is allowed for staging, but the installer warns because the
resulting binaries and libraries may not run on the current machine.

GitHub Actions publishes releases automatically from version tags:

```sh
git tag v1.5.0
git push origin v1.5.0
```

The `Release` workflow checks out `DCC` and `LLAM` side by side, builds against
LLAM through `DCC_LLAM_USE_SUBDIRECTORY=ON`, keeps LLAM's install rules out of
the DCC package, runs the DCC test suite, creates CPack binary/source archives,
uploads artifacts, builds `SHA256SUMS`, and creates the GitHub Release. Tag
versions must match `project(dcc VERSION ...)`. The workflow also publishes a
CycloneDX SBOM and signs artifact provenance through GitHub attestations.
Windows archives are built with the Visual Studio 2022 generator and its
ClangCL toolset. This preserves the MSVC-compatible Windows ABI while compiling
DCC's public C11 decorator macros with a conforming C frontend.

Verify a downloaded archive before installation:

```sh
sha256sum -c dcc-1.5.0-linux-x86_64.tar.gz.sha256
gh attestation verify dcc-1.5.0-linux-x86_64.tar.gz --repo Feralthedogg/DCC
```

After installation, run `dcc_doctor --json`; production hosts should add
`--require-token`, and voice deployments that require E2EE should also add
`--require-dave`.

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
- LLAM is installed separately or supplied to consumers through
  `DCC_LLAM_ROOT` and `DCC_LLAM_LIBRARY`.
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
  `DCC_TOKEN=... make -C examples/hot_reload live-soak`.
- Optional token-backed voice join probe with
  `DCC_TOKEN=... DCC_VOICE_GUILD_ID=... DCC_VOICE_CHANNEL_ID=... DCC_VOICE_USER_ID=... ./build/dcc_live_voice_probe`.
- Installed hot reload example files, including the live-soak helper script.
- Installed deployment templates for interaction webhooks and isolated hot
  reload host/worker service operation, with service env, systemd, Kubernetes,
  and reverse-proxy templates checked against `dcc_interaction_webhook --check`
  and `dcc_hot_reload_host --check`.
- Installed normal-bot systemd, Compose, and Kubernetes templates gated by
  `dcc_doctor --require-token`.
- Every external GitHub Action and the LLAM 2.2.0 checkout pinned to immutable
  commit SHA values.
- CycloneDX SBOM generation, checksum sidecars, `SHA256SUMS`, and GitHub build
  provenance attestations.

Before tagging, also review:

- `docs/api.md` for public API ownership and module boundaries.
- `docs/production-playbooks.md` for Gateway, REST, interaction, hot reload,
  voice, wait-policy, and release operations.
- `docs/live-validation.md` for opt-in Discord soak and voice validation.
