<section class="dcc-hero">
  <h1>DCC is a Discord C runtime for production bots.</h1>
  <p>
    DCC combines a DPP-inspired Discord API surface with LLAM-backed networking,
    command reconciliation, stateful components, hot-reload isolation, replay
    testing, and operational guard rails.
  </p>
</section>

```c
#include <dcc/dcc.h>

int main(void) {
    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = getenv("DISCORD_TOKEN"),
        .intents = DCC_INTENT_GUILDS,
    };

    dcc_client_t *client = NULL;
    if (dcc_client_create(&options, &client) != DCC_OK) {
        return 1;
    }

    dcc_client_start(client);
    dcc_client_wait(client);
    dcc_client_destroy(client);
    return 0;
}
```

<div class="dcc-card-grid">
  <div class="dcc-card">
    <h3>Gateway And Events</h3>
    <p>Typed dispatch, shard health, event waits, cache updates, and LLAM-managed runtime tasks.</p>
  </div>
  <div class="dcc-card">
    <h3>Slash Commands</h3>
    <p>Register commands globally by default, target one guild explicitly, and reconcile drift safely.</p>
  </div>
  <div class="dcc-card">
    <h3>Hot Reload</h3>
    <p>Run bot logic in an isolated worker, canary new modules, and roll back to last-good.</p>
  </div>
  <div class="dcc-card">
    <h3>Replay Testing</h3>
    <p>Capture Discord payloads as JSONL and replay them into bot modules locally.</p>
  </div>
</div>

## Install

```sh
curl -fsSL https://raw.githubusercontent.com/Feralthedogg/DCC/main/tools/install.sh |
  sh -s -- --prefix "$HOME/.local"
```

DCC release archives include a compatible LLAM runtime. The installer also
refreshes LLAM through LLAM's latest release installer, so release users do not
need a neighboring LLAM checkout.

## Build From Source

```sh
git clone https://github.com/Feralthedogg/DCC.git
git clone https://github.com/Feralthedogg/LLAM.git
cd DCC
cmake -S . -B build -DDCC_LLAM_USE_SUBDIRECTORY=ON -DDCC_LLAM_ROOT=../LLAM
cmake --build build
ctest --test-dir build --output-on-failure
```

## Where To Go Next

- Start with [Quickstart](getting-started.md) for a minimal bot and local build.
- Use [Slash Command Registry](guides/command-registry.md) before deploying commands.
- Use [Component Sessions](guides/component-sessions.md) for buttons, selects, and modals.
- Use [Hot Reload](guides/hot-reload.md) when bot logic should change without dropping Gateway.
- Use [Production Playbooks](production-playbooks.md) before running a public bot.
