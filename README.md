# DCC

DCC is a Discord C Client runtime built in C11 on top of LLAM.

It is a separate project from DPP: the goal is to keep the useful Discord
client structure, expose a C API, and provide production-oriented runtime
features such as Gateway handling, REST helpers, interaction workflows, command
reconciliation, replay testing, and hot-reloadable bot modules.

Documentation is published at:

https://Feralthedogg.github.io/DCC/

Install LLAM, then install the latest DCC release:

```sh
curl -fsSL https://github.com/Feralthedogg/LLAM/releases/latest/download/install.sh |
  sh -s -- --prefix "$HOME/.local"
curl -fsSL https://github.com/Feralthedogg/DCC/releases/latest/download/install.sh |
  sh -s -- --prefix "$HOME/.local"
```

DCC does not vendor LLAM into release archives by default. The DCC installer can
install the tested LLAM runtime with `--install-llam`, but ordinary installs keep
the two runtimes separate.

Local documentation preview:

```sh
python3 -m pip install -r docs/requirements.txt
mkdocs serve
```

Basic build:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="$HOME/.local"
cmake --build build
```

Developer test build:

```sh
cmake -S . -B build-dev \
  -DCMAKE_BUILD_TYPE=Debug \
  -DDCC_BUILD_EXAMPLES=ON \
  -DDCC_BUILD_TESTS=ON \
  -DDCC_LLAM_USE_SUBDIRECTORY=ON \
  -DDCC_LLAM_ROOT=../LLAM
cmake --build build-dev
ctest --test-dir build-dev --output-on-failure
```

License: MIT. See [LICENSE](LICENSE).
