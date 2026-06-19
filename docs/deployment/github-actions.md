# GitHub Actions

DCC ships three workflow groups.

## CI

`.github/workflows/ci.yml` runs the release check on Linux and macOS. It checks
out DCC and LLAM side by side, builds with CMake, runs CTest, validates CPack,
installs the package, and builds an installed-package consumer.

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
to a GitHub Release.

Automatic release:

```sh
git tag v1.0.0-beta.1
git push origin v1.0.0-beta.1
```

Manual release:

1. Open `Actions -> Release`.
2. Run workflow.
3. Optionally set a release version such as `v1.0.0-beta.1`.

Artifacts are placed under `target/dist/` locally and uploaded as workflow
artifacts before the publish job creates the GitHub Release.
