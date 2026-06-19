# Third-party Dependencies

DCC currently vendors no third-party source code.

Runtime and build dependencies are resolved externally:

- LLAM runtime: configured through `DCC_LLAM_ROOT` and linked as `llam::runtime`.
- OpenSSL: found by CMake.
- Threads: found by CMake.
- Opus: optional, found by CMake when available.

If code is vendored later, add one subdirectory per dependency and document source, version,
license, and local modifications here.
