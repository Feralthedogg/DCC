# Support

Use GitHub Issues for reproducible DCC defects and documentation gaps. Use a
feature request for proposed API or workflow changes. Security reports must
follow [SECURITY.md](SECURITY.md) instead of a public issue.

Before opening an issue:

1. Run `dcc_doctor --json` and remove all secrets from the output.
2. Confirm the DCC and LLAM versions.
3. Reduce the problem to the smallest public API example.
4. Include the platform, compiler, CMake options, status code, last-error text,
   and focused test output.

Support covers released DCC versions on the platforms listed in the release
artifacts. Custom forks, unpinned LLAM snapshots, Discord account policy, and
general C programming questions may need separate investigation.
