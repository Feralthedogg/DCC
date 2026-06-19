param(
    [string]$Target = "windows-x86_64",
    [string]$BuildDir = "build\release-windows",
    [string]$Configuration = "Release",
    [string]$Version = $env:DCC_RELEASE_VERSION
)

$ErrorActionPreference = "Stop"

function Assert-ReleaseComponent {
    param(
        [string]$Name,
        [string]$Value
    )

    if ([string]::IsNullOrWhiteSpace($Value) -or ($Value -notmatch '^[A-Za-z0-9._+-]+$')) {
        throw "invalid ${Name}: ${Value}"
    }
}

function Get-ProjectVersion {
    param([string]$Root)

    $cmake = Get-Content -LiteralPath (Join-Path $Root "CMakeLists.txt") -Raw
    if ($cmake -notmatch '(?ms)project\s*\(\s*dcc\s+VERSION\s+([0-9][0-9A-Za-z.+-]*)') {
        throw "cannot determine CMake project version"
    }
    return $Matches[1]
}

function Write-Sha256Sidecar {
    param([string]$Path)

    $name = Split-Path -Leaf $Path
    $hash = (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
    Set-Content -LiteralPath "$Path.sha256" -NoNewline -Value "$hash  $name`n"
}

function Assert-ZipContains {
    param(
        [string]$Archive,
        [string[]]$Patterns
    )

    Add-Type -AssemblyName System.IO.Compression.FileSystem
    $zip = [System.IO.Compression.ZipFile]::OpenRead($Archive)
    try {
        $entries = @($zip.Entries | ForEach-Object { $_.FullName.Replace("\", "/") })
        foreach ($pattern in $Patterns) {
            if (-not ($entries | Where-Object { $_ -match $pattern } | Select-Object -First 1)) {
                throw "Windows package is missing archive member matching: $pattern"
            }
        }
    } finally {
        $zip.Dispose()
    }
}

$Root = Resolve-Path -LiteralPath (Join-Path $PSScriptRoot "..")
$Root = $Root.Path
$ProjectVersion = Get-ProjectVersion $Root

if ([string]::IsNullOrWhiteSpace($Version)) {
    $Version = if ($env:GITHUB_REF_NAME) { $env:GITHUB_REF_NAME } else { $ProjectVersion }
}
$Version = $Version -replace '^v', ''
$VersionBase = $Version -replace '-.*$', ''

Assert-ReleaseComponent "version" $Version
Assert-ReleaseComponent "target" $Target

if ($Target -ne "windows-x86_64") {
    throw "unsupported Windows release target: $Target"
}
if (($VersionBase -ne $ProjectVersion) -and ($env:DCC_ALLOW_VERSION_MISMATCH -ne "1")) {
    throw "release version $Version does not match CMake project version $ProjectVersion"
}

if ([System.IO.Path]::IsPathRooted($BuildDir)) {
    $BuildRoot = $BuildDir
} else {
    $BuildRoot = Join-Path $Root $BuildDir
}
$DistDir = Join-Path $Root "target\dist"
$PackageOutput = Join-Path $BuildRoot "package-output"
$LlamRoot = if ($env:DCC_LLAM_ROOT) { $env:DCC_LLAM_ROOT } else { Join-Path $Root "..\LLAM" }

New-Item -ItemType Directory -Force -Path $DistDir | Out-Null
Remove-Item -Recurse -Force -LiteralPath $PackageOutput -ErrorAction SilentlyContinue

$generator = if ($env:DCC_CMAKE_GENERATOR) { $env:DCC_CMAKE_GENERATOR } else { "Visual Studio 17 2022" }
$configureArgs = @(
    "-S", $Root,
    "-B", $BuildRoot,
    "-G", $generator,
    "-DCMAKE_BUILD_TYPE=$Configuration",
    "-DDCC_LLAM_ROOT=$LlamRoot",
    "-DDCC_LLAM_USE_SUBDIRECTORY=ON",
    "-DDCC_BUNDLE_LLAM=ON",
    "-DDCC_BUILD_EXAMPLES=OFF",
    "-DDCC_BUILD_TESTS=OFF",
    "-DDCC_WITH_OPUS=OFF"
)
if ($generator -match "Visual Studio") {
    $configureArgs += @("-A", "x64")
}
if ($env:DCC_WINDOWS_CMAKE_TOOLCHAIN) {
    $configureArgs += "-DCMAKE_TOOLCHAIN_FILE=$($env:DCC_WINDOWS_CMAKE_TOOLCHAIN)"
}
if ($env:OPENSSL_ROOT_DIR) {
    $configureArgs += "-DOPENSSL_ROOT_DIR=$($env:OPENSSL_ROOT_DIR)"
}

& cmake @configureArgs
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

$parallel = if ($env:DCC_BUILD_PARALLEL) { $env:DCC_BUILD_PARALLEL } else { "4" }
& cmake --build $BuildRoot --config $Configuration --parallel $parallel
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

if ($env:DCC_SKIP_RELEASE_TESTS -ne "1") {
    $timeout = if ($env:DCC_CTEST_TIMEOUT) { $env:DCC_CTEST_TIMEOUT } else { "180" }
    $regex = if ($env:DCC_RELEASE_CTEST_REGEX) { $env:DCC_RELEASE_CTEST_REGEX } else { "^dcc_" }
    $exclude = if ($env:DCC_RELEASE_CTEST_EXCLUDE) { $env:DCC_RELEASE_CTEST_EXCLUDE } else { "^dcc_cluster_chaos_smoke$" }
    if ([string]::IsNullOrWhiteSpace($exclude)) {
        & ctest --test-dir $BuildRoot -R $regex --output-on-failure --timeout $timeout -C $Configuration
    } else {
        & ctest --test-dir $BuildRoot -R $regex -E $exclude --output-on-failure --timeout $timeout -C $Configuration
    }
    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }
}

& cpack -G ZIP `
    --config (Join-Path $BuildRoot "CPackConfig.cmake") `
    -B $PackageOutput `
    -C $Configuration `
    -D "CPACK_PACKAGE_FILE_NAME=dcc-$Version-$Target"
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

$packageArchive = Get-ChildItem -LiteralPath $PackageOutput -File -Filter "dcc-*.zip" |
    Where-Object { $_.Name -notmatch "Source" } |
    Select-Object -First 1
if (-not $packageArchive) {
    throw "binary package archive was not created"
}

$outArchive = Join-Path $DistDir "dcc-$Version-$Target.zip"
Copy-Item -Force -LiteralPath $packageArchive.FullName -Destination $outArchive
Write-Sha256Sidecar $outArchive

Assert-ZipContains $outArchive @(
    '/include/dcc/dcc\.h$',
    '/include/llam/runtime\.h$',
    '/lib/(lib)?llam_runtime.*\.(lib|dll|a)$',
    '/lib/cmake/llam/llam-config\.cmake$',
    '/lib/pkgconfig/llam\.pc$',
    '/lib/pkgconfig/dcc\.pc$',
    '/share/dcc/docs/api\.md$'
)

Write-Output $outArchive
