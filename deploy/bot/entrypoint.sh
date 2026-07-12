#!/bin/sh
set -eu

/usr/local/bin/dcc_doctor --require-token --json
exec /usr/local/bin/dcc-bot "$@"
