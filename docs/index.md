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

DCC_SIMPLE_BOT_MAIN(
    "minimal",
    DCC_LISTEN_SLASH("ping", "Reply with pong", on_ping)
)
```

## Start Here

Choose one path before browsing individual headers. Most bot applications
should stay on the Sugar path until they have a concrete reason to move lower.

| Goal | Start with | Read next |
| --- | --- | --- |
| Build a bot quickly | `<dcc/sugar.h>` and `dcc_new_app` | [Quickstart](getting-started.md) |
| Control app lifecycle and registration explicitly | `<dcc/app.h>` | [App Runtime](guides/app-runtime.md) |
| Embed DCC or implement custom runtime behavior | Focused Core, REST, and Gateway headers | [Choose An API Layer](concepts/api-layers.md) |
| Keep event or REST data after a callback | Clone APIs or REST futures | [Ownership And Async](concepts/ownership-and-async.md) |
| Diagnose a build, command, intent, or connection problem | Error status plus the owning object's `*_last_error()` | [Troubleshooting](troubleshooting.md) |

!!! tip "Recommended application path"
    Scaffold one app, get `/ping` working, and add one feature at a time. You do
    not need to learn the complete REST or Gateway surface before shipping a
    normal command bot.

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

Install LLAM first, then let the release installer detect the current platform
and install DCC. Pass `--install-llam` when you want the DCC installer to fetch
the tested LLAM runtime into the same prefix.

```sh
curl -fsSL https://github.com/Feralthedogg/LLAM/releases/latest/download/install.sh |
  sh -s -- --prefix "$HOME/.local"
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
```

DCC suppresses LLAM's install rules in this combined source build, so
`cmake --install build` installs DCC files only.

## Where To Go Next

- Start with [Quickstart](getting-started.md) for a minimal bot and local build.
- Use [Sugar-First API](guides/sugar.md) for compact command, message, component, and runtime options.
- Use [Slash Command Registry](guides/command-registry.md) before deploying commands.
- Use [Component Sessions](guides/component-sessions.md) for buttons, selects, and modals.
- Use [Hot Reload](guides/hot-reload.md) when bot logic should change without dropping Gateway.
- Use [Production Playbooks](production-playbooks.md) before running a public bot.
