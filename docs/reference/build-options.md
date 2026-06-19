# Build Options

DCC is configured through CMake.

| Option | Default | Purpose |
| --- | --- | --- |
| `DCC_BUILD_SHARED` | `OFF` | Build `dcc` as a shared library. |
| `DCC_BUILD_EXAMPLES` | `ON` | Build example programs. |
| `DCC_BUILD_TESTS` | `ON` | Build smoke and audit tests. |
| `DCC_BUILD_TOOLS` | `ON` | Build and install runtime tools. |
| `DCC_BUILD_BENCHMARKS` | `OFF` | Build micro benchmarks. |
| `DCC_BUILD_FUZZERS` | `OFF` | Build JSON fuzz harnesses. |
| `DCC_ENABLE_SANITIZERS` | `OFF` | Enable ASAN/UBSAN for supported compilers. |
| `DCC_LLAM_USE_SUBDIRECTORY` | `OFF` | Build LLAM from source through `add_subdirectory`. |
| `DCC_BUNDLE_LLAM` | `ON` | Install LLAM runtime package files with DCC when LLAM is built as a subdirectory. |
| `DCC_LLAM_ROOT` | `../LLAM` | LLAM source or install root. |
| `DCC_LLAM_LIBRARY` | `../LLAM/libllam_runtime.a` | Prebuilt LLAM static library. |
| `DCC_WITH_OPUS` | `ON` | Use optional libopus voice codec backend when found. |

## Release Install

Release users normally do not pass LLAM build options:

```sh
curl -fsSL https://raw.githubusercontent.com/Feralthedogg/DCC/main/tools/install.sh |
  sh -s -- --prefix "$HOME/.local"
```

The release archive includes bundled LLAM package files, and the installer
refreshes LLAM from the latest LLAM release installer by default.

## Development Build

```sh
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DDCC_LLAM_USE_SUBDIRECTORY=ON \
  -DDCC_LLAM_ROOT=../LLAM
cmake --build build
ctest --test-dir build --output-on-failure
```

## CI Build With LLAM Source

```sh
cmake -S . -B build \
  -DDCC_LLAM_USE_SUBDIRECTORY=ON \
  -DDCC_LLAM_ROOT=../LLAM
cmake --build build
```

## Package Build

```sh
cmake -S . -B build-package \
  -DCMAKE_BUILD_TYPE=Release \
  -DDCC_BUILD_EXAMPLES=OFF \
  -DDCC_BUILD_TESTS=OFF \
  -DDCC_LLAM_USE_SUBDIRECTORY=ON \
  -DDCC_LLAM_ROOT=../LLAM \
  -DDCC_BUNDLE_LLAM=ON
cmake --build build-package
cmake --install build-package --prefix /usr/local
```
