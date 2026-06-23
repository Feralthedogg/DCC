<section class="dcc-hero dcc-hero-download">
  <div class="dcc-hero-copy">
    <p class="dcc-eyebrow">Discord C Client runtime</p>
    <h1>DCC is a production Discord runtime for C.</h1>
    <p>
      A DPP-inspired API surface with LLAM-backed networking, command
      reconciliation, stateful components, hot-reload isolation, replay testing,
      and operational guard rails.
    </p>
  </div>

  <div class="dcc-download-panel" aria-label="Install DCC">
    <div class="dcc-download-head">
      <span>Install DCC</span>
      <span>latest release</span>
    </div>
    <div class="dcc-command-box">
      <code><span><span class="dcc-prompt">$</span>curl -fsSL</span><span>https://github.com/Feralthedogg/DCC/releases/latest/download/install.sh</span><span>| sh -s -- --prefix "$HOME/.local"</span></code>
    </div>
    <div class="dcc-platform-row">
      <span>macOS</span>
      <span>Linux</span>
      <span>BSD</span>
    </div>
    <div class="dcc-windows-command">
      <span>Windows PowerShell</span>
      <code><span>irm https://github.com/Feralthedogg/DCC/releases/latest/download/install.ps1</span><span>| iex</span></code>
    </div>
  </div>
</section>

## Minimal App Shape

```c
#include <dcc/sugar.h>

DCC_SLASH_FN(on_ping) {
    (void)user_data;
    (void)DCC_REPLY_TEXT(ctx, "pong");
}

DCC_BOT_ROUTES_MAIN(
    "minimal",
    DCC_ROUTE_COMMAND("ping", "Reply with pong", on_ping)
)
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

The release installer detects the current platform, installs DCC, and refreshes
LLAM through LLAM's latest release installer.

```sh
curl -fsSL https://github.com/Feralthedogg/DCC/releases/latest/download/install.sh |
  sh -s -- --prefix "$HOME/.local"
```

Use a system prefix when the target machine expects globally installed CMake
packages:

```sh
curl -fsSL https://github.com/Feralthedogg/DCC/releases/latest/download/install.sh |
  sudo sh -s -- --prefix /usr/local
```

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
- Use [Sugar-First API](guides/sugar.md) for compact command, message, component, and runtime options.
- Use [Slash Command Registry](guides/command-registry.md) before deploying commands.
- Use [Component Sessions](guides/component-sessions.md) for buttons, selects, and modals.
- Use [Hot Reload](guides/hot-reload.md) when bot logic should change without dropping Gateway.
- Use [Production Playbooks](production-playbooks.md) before running a public bot.
