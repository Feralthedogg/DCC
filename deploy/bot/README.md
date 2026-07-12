# Production Bot Template

This template runs a normal DCC bot with production-safe shutdown and a local
preflight check. Build your application as `bot`; `DCC_BOT(...)` must be used in
production so READY does not mutate commands.

## systemd

1. Install the bot at `/opt/dcc-bot/bin/bot` and DCC tools at `/usr/local/bin`.
2. Create the `dcc` system user and copy `dcc-bot.env` to
   `/etc/dcc/dcc-bot.env` with mode `0600`.
3. Install `dcc-bot.tmpfiles.conf`, run `systemd-tmpfiles --create`, then install
   and enable `dcc-bot.service`.

`ExecStartPre` runs `dcc_doctor --require-token`. SIGTERM reaches DCC's
signal-safe monitor and gives the process 30 seconds to stop cleanly.

## Container / Compose

Stage the bot, doctor, and runtime libraries under `bin/` and `lib/`, then run:

```sh
docker compose up --build -d
```

Replace the token in `dcc-bot.env`; do not commit that file after editing it.
The service uses a read-only root filesystem, a writable state volume, no new
privileges, and a 30-second stop grace period.

## Kubernetes

Replace the image and Secret placeholder in `kubernetes.yaml`, choose a
persistent volume when bot state must survive pod replacement, then apply it:

```sh
kubectl apply -f kubernetes.yaml
```

The init container runs `dcc_doctor --require-token --json`. The pod runs as a
non-root user with dropped capabilities and a read-only root filesystem.
Deploy slash-command changes separately with `dcc_command_sync --plan` and
`--apply` before rolling the bot image.
