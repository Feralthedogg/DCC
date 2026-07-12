# DCC

DCC is a C11 Discord client runtime built on LLAM 2.2.0. It covers Gateway,
REST, interaction webhooks, Components V2, voice with the official libdave
backend, sharding, replay tests, and isolated hot reload.

Version 1.5.0 separates safe production behavior from development convenience:
`DCC_BOT(...)` never changes commands on READY; `DCC_DEV_BOT(...)` opts into
automatic development sync.

Documentation: <https://Feralthedogg.github.io/DCC/>

## Install

Install the tested LLAM runtime and DCC into the same prefix:

```sh
curl -fsSL https://github.com/Feralthedogg/DCC/releases/latest/download/install.sh \
  -o dcc-install.sh
sh dcc-install.sh --prefix "$HOME/.local" --install-llam
"$HOME/.local/bin/dcc_doctor" --json
```

Release archives have SHA-256 sidecars, a `SHA256SUMS` manifest, a CycloneDX
SBOM, and GitHub build provenance attestations. See
[the release checklist](docs/release.md) for verification commands.

## First Bot

The scaffold creates a typed feature layout and explicit development preset:

```sh
dcc_new_app mybot --name mybot
cmake -S mybot -B mybot/build -DCMAKE_PREFIX_PATH="$HOME/.local"
cmake --build mybot/build
```

A minimal development-only program can use:

```c
#include <dcc/sugar.h>

DCC_SLASH_FN(ping) {
    (void)user_data;
    (void)DCC_REPLY_TEXT(ctx, "pong");
}

DCC_DEV_BOT_MAIN(
    "minimal",
    DCC_APP_LISTENERS(DCC_LISTEN_SLASH("ping", "Reply with pong", ping))
)
```

Before deployment, use `DCC_BOT_MAIN(...)` and apply command changes separately
with `dcc_command_sync --plan` and `--apply`.

## Build From Source

```sh
git clone https://github.com/Feralthedogg/LLAM.git
git -C LLAM checkout 2808e99c0339b8b3d6ed5ef7fab3dc231927c568
git clone https://github.com/Feralthedogg/DCC.git
cmake -S DCC -B DCC/build \
  -DDCC_LLAM_USE_SUBDIRECTORY=ON \
  -DDCC_LLAM_ROOT="$PWD/LLAM" \
  -DDCC_BUILD_TESTS=ON \
  -DDCC_BUILD_EXAMPLES=ON
cmake --build DCC/build
ctest --test-dir DCC/build --output-on-failure
```

Start with [Getting Started](docs/getting-started.md), then use the
[API-layer guide](docs/concepts/api-layers.md) and
[production playbooks](docs/production-playbooks.md).

## Project Policy

- [Contributing](CONTRIBUTING.md)
- [Security policy](SECURITY.md)
- [Support](SUPPORT.md)
- [Roadmap](ROADMAP.md)
- [Code of Conduct](CODE_OF_CONDUCT.md)
- [Versioning and compatibility](docs/versioning.md)

License: MIT. See [LICENSE](LICENSE).
