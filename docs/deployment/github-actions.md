# GitHub Actions

DCC ships four workflow groups.

## CI

`.github/workflows/ci.yml` runs the release check on Linux and macOS. It checks
out DCC and LLAM side by side, builds with CMake, runs CTest, validates CPack,
installs the package, and builds an installed-package consumer.

`.github/workflows/bsd.yml` runs BSD smoke gates through VM-backed GitHub
Actions jobs. FreeBSD, OpenBSD, and NetBSD are hard gates; DragonFlyBSD is kept
as an experimental allowed-failure target because public package mirrors and VM
availability are less stable.

## Docs

`.github/workflows/docs.yml` builds this MkDocs site. Pull requests build the
site for validation. Pushes to `main` deploy the generated `site/` directory to
GitHub Pages.

Repository settings required:

1. Open `Settings -> Pages`.
2. Set source to `GitHub Actions`.
3. Push to `main` or run the workflow manually.

## Release

`.github/workflows/release.yml` builds native CPack archives and uploads them
to a GitHub Release. Linux, macOS, Windows, FreeBSD, OpenBSD, and NetBSD
artifacts are published from this workflow.

Automatic release:

```sh
git tag v1.1.1
git push origin v1.1.1
```

Manual release:

1. Open `Actions -> Release`.
2. Run workflow.
3. Optionally set a release version such as `v1.1.1`.

Artifacts are placed under `target/dist/` locally and uploaded as workflow
artifacts before the publish job creates the GitHub Release.
