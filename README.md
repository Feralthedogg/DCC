# DCC

DCC is a Discord C Client runtime built in C11 on top of LLAM.

It is a separate project from DPP: the goal is to keep the useful Discord
client structure, expose a C API, and provide production-oriented runtime
features such as Gateway handling, REST helpers, interaction workflows, command
reconciliation, replay testing, and hot-reloadable bot modules.

Documentation is published at:

https://Feralthedogg.github.io/DCC/

Install the latest release:

```sh
curl -fsSL https://github.com/Feralthedogg/DCC/releases/latest/download/install.sh |
  sh -s -- --prefix "$HOME/.local"
```

The release archive includes a compatible LLAM runtime, and the installer also
refreshes the DCC-tested LLAM 2.1.0 runtime by default.

Local documentation preview:

```sh
python3 -m pip install -r docs/requirements.txt
mkdocs serve
```

Basic build:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DDCC_LLAM_USE_SUBDIRECTORY=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

License: MIT. See [LICENSE](LICENSE).
