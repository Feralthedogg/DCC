# Getting Started

## Install

Install LLAM 2.2.1 and DCC 2.0.1 into the same prefix, then verify the runtime:

```sh
sh dcc-install.sh --prefix "$HOME/.local" --install-llam
"$HOME/.local/bin/dcc_doctor" --json
```

## Create a project

```sh
dcc_new_app mybot --name mybot
cmake -S mybot -B mybot/build -DCMAKE_PREFIX_PATH="$HOME/.local"
cmake --build mybot/build
```

The generated starter includes `<dcc/bot.h>`, returns every handler status, and
uses the development runner explicitly.

## Listener configuration

Short constructors cover the common path. Advanced listeners start with
`DCC_LISTENER_CONFIG()`, assign borrowed policy/binding/validation descriptors,
and pass the value to the matching `_WITH` constructor. `dcc_app_listen()`
validates and deep-copies descriptor metadata before returning; callback user
data remains borrowed until unlisten or App destruction.

## Production

Switch the main macro to `DCC_BOT_MAIN`, provide the token through the selected
environment variable or App options, and run command changes separately with
`dcc_command_sync --plan` and `--apply`.

Continue with [API layers](concepts/api-layers.md) and
[ownership](concepts/ownership-and-async.md).
