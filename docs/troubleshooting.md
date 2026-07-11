# Troubleshooting

Start with the first failing boundary: configuration, build, authentication,
Gateway READY, command sync, interaction response, or shutdown. Avoid changing
multiple layers at once.

## CMake Cannot Find DCC Or LLAM

For an installed package, point CMake at the shared prefix:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH="$HOME/.local"
```

For sibling source trees:

```sh
cmake -S . -B build \
  -DDCC_LLAM_USE_SUBDIRECTORY=ON \
  -DDCC_LLAM_ROOT=../LLAM
```

DCC 1.4.1 requires LLAM 2.2.0 or newer with ABI major 2. Remove an old CMake
build directory, or explicitly update `DCC_LLAM_REQUIRED_VERSION`, when a cache
still reports an earlier requirement.

## The Bot Does Not Reach READY

Check these in order:

1. The token belongs to the bot user, not the application secret or public key.
2. `.env` is loaded from the process working directory.
3. The bot is invited to the target server.
4. Required Gateway intents are enabled both in code and, for privileged
   intents, in the Discord Developer Portal.
5. Log `dcc_status_string(status)` and `dcc_client_last_error(client)` before
   destroying the client.

Never print the token while debugging.

## Slash Commands Do Not Appear

- Use `DCC_COMMAND_GUILD_ID` during development; guild commands update much
  faster than global commands.
- Confirm the app reaches READY before automatic command reconciliation.
- Run `dcc_command_sync --plan` to inspect create, update, and delete actions.
- Confirm the application ID and guild ID are snowflakes for the same bot and
  development server.
- Check that the bot invite includes the application-command scope.

See [Slash Command Registry](guides/command-registry.md) for explicit plan,
check, and apply workflows.

## Message Content Is Empty

`MESSAGE_CONTENT` is a privileged intent. Enable `DCC_INTENT_MESSAGE_CONTENT`
in the client intent set and enable Message Content Intent for the bot in the
Developer Portal. Slash-command interactions do not require message-content
parsing and are usually the better default.

## An Interaction Shows "Application Did Not Respond"

- Send or defer the initial interaction response within Discord's deadline.
- Use DCC auto-defer for handlers that may take longer than a short operation.
- After deferring, edit the original response or send a followup; do not attempt
  a second initial response.
- Check both `response->error` and the HTTP status in REST callbacks.

See [Interaction Flows](guides/interaction-flows.md) for response state and
auto-defer patterns.

## A Pointer Becomes Invalid

Event accessors, cache getters, and REST callback bodies commonly return
borrowed views. Clone or copy data before retaining it beyond the callback or
cache mutation. Read [Ownership And Async](concepts/ownership-and-async.md).

## A REST Call Failed

Check three values independently:

1. the immediate `dcc_status_t` returned when submitting the operation;
2. `response->error` inside the callback;
3. `response->status` for Discord's HTTP result.

`DCC_ERR_RATE_LIMITED` describes a DCC-level rate-limit outcome. An HTTP 4xx or
5xx can still arrive with `response->error == DCC_OK` because transport and HTTP
application status are separate.

## Shutdown Hangs Or Returns `DCC_ERR_STATE`

- Stop accepting new work.
- Cancel or drain queued REST work.
- Let active callbacks return.
- Stop the Gateway/app.
- Destroy futures only after completion.
- Destroy child objects before their owning app/client.

For production ordering and health checks, use
[Production Playbooks](production-playbooks.md).

## Collect Useful Evidence

When reporting a problem, include:

- DCC and LLAM versions;
- operating system, compiler, and CMake version;
- the smallest reproducing handler or request;
- `dcc_status_string(status)` plus the owning object's last-error text;
- HTTP status and a redacted response body when REST is involved;
- whether the same failure occurs in a clean Debug build.

Remove bot tokens, webhook tokens, interaction tokens, and user data before
sharing logs.
