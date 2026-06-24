param(
    [string]$Prefix = $env:DCC_INSTALL_PREFIX,
    [string]$Version = $env:DCC_INSTALL_VERSION,
    [string]$Target = $env:DCC_INSTALL_TARGET,
    [string]$BaseUrl = $env:DCC_INSTALL_BASE_URL,
    [string]$LlamVersion = $env:DCC_INSTALL_LLAM_VERSION,
    [string]$LlamInstallerUrl = $env:DCC_LLAM_INSTALLER_URL,
    [switch]$InstallLlam,
    [switch]$SkipLlam,
    [switch]$DryRun,
    [switch]$Force
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($Prefix)) {
    $Prefix = if ($env:LOCALAPPDATA) { Join-Path $env:LOCALAPPDATA "DCC" } else { Join-Path $HOME ".local\dcc" }
}
if ([string]::IsNullOrWhiteSpace($Version)) {
    $Version = "latest"
}
if ([string]::IsNullOrWhiteSpace($Target)) {
    $Target = "windows-x86_64"
}
if ([string]::IsNullOrWhiteSpace($LlamVersion)) {
    $LlamVersion = "2.1.0"
}
if (($env:DCC_INSTALL_LLAM) -and ($env:DCC_INSTALL_LLAM -ne "skip")) {
    $InstallLlam = $true
}
if ($PSBoundParameters.ContainsKey("LlamVersion")) {
    $InstallLlam = $true
}
if ($SkipLlam) {
    $InstallLlam = $false
}
function Assert-ReleaseComponent {
    param(
        [string]$Name,
        [string]$Value
    )

    if ([string]::IsNullOrWhiteSpace($Value) -or ($Value -notmatch '^[A-Za-z0-9._+-]+$')) {
        throw "invalid ${Name}: ${Value}"
    }
}

function Get-LatestReleaseTag {
    param([string]$Repository)

    $release = Invoke-RestMethod -Uri "https://api.github.com/repos/$Repository/releases/latest" -UseBasicParsing
    if ([string]::IsNullOrWhiteSpace($release.tag_name)) {
        throw "could not resolve latest release tag for $Repository"
    }
    return $release.tag_name
}

function Assert-ArchiveChecksum {
    param(
        [string]$ArchivePath,
        [string]$ChecksumPath,
        [string]$ArchiveName
    )

    $line = Get-Content -LiteralPath $ChecksumPath | Where-Object { -not [string]::IsNullOrWhiteSpace($_) } | Select-Object -First 1
    if ([string]::IsNullOrWhiteSpace($line)) {
        throw "empty checksum file for $ArchiveName"
    }
    $parts = $line -split '\s+'
    if ($parts.Count -lt 2) {
        throw "invalid checksum file for $ArchiveName"
    }
    $expected = $parts[0].ToLowerInvariant()
    $targetName = $parts[1].TrimStart("*")
    if (($expected -notmatch '^[0-9a-f]{64}$') -or ($targetName -ne $ArchiveName)) {
        throw "invalid checksum entry for $ArchiveName"
    }
    $actual = (Get-FileHash -LiteralPath $ArchivePath -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actual -ne $expected) {
        throw "checksum mismatch for $ArchiveName"
    }
}

function Copy-Directory {
    param(
        [string]$Source,
        [string]$Destination
    )

    if (-not (Test-Path -LiteralPath $Source -PathType Container)) {
        return
    }
    if ((Test-Path -LiteralPath $Destination) -and -not $Force) {
        $sourceRoot = (Resolve-Path -LiteralPath $Source).Path.TrimEnd("\", "/")
        foreach ($item in Get-ChildItem -LiteralPath $Source -Recurse -File -Force) {
            $relative = $item.FullName.Substring($sourceRoot.Length).TrimStart("\", "/")
            if (Test-Path -LiteralPath (Join-Path $Destination $relative)) {
                throw "refusing to overwrite $(Join-Path $Destination $relative); pass -Force"
            }
        }
    }
    if ($DryRun) {
        Write-Host "copy $Source -> $Destination"
        return
    }
    New-Item -ItemType Directory -Force -Path $Destination | Out-Null
    foreach ($item in Get-ChildItem -LiteralPath $Source -Force) {
        Copy-Item -Recurse -Force -LiteralPath $item.FullName -Destination $Destination
    }
}

function Install-DccTree {
    param([string]$Root)

    if (-not (Test-Path -LiteralPath (Join-Path $Root "include\dcc\dcc.h"))) {
        throw "DCC archive does not contain include\dcc\dcc.h"
    }

    Copy-Directory (Join-Path $Root "include") (Join-Path $Prefix "include")
    Copy-Directory (Join-Path $Root "lib") (Join-Path $Prefix "lib")
    Copy-Directory (Join-Path $Root "bin") (Join-Path $Prefix "bin")
    Copy-Directory (Join-Path $Root "share") (Join-Path $Prefix "share")
    if (-not $DryRun) {
        Write-Host "installed DCC into $Prefix"
    }
}

function Install-LatestLlam {
    if (-not $InstallLlam) {
        return
    }

    $resolved = $LlamVersion
    if ($resolved -eq "latest") {
        $resolved = Get-LatestReleaseTag "Feralthedogg/LLAM"
    }
    $resolved = $resolved -replace '^v', ''
    Assert-ReleaseComponent "LLAM version" $resolved
    $installerUrl = $LlamInstallerUrl
    if ([string]::IsNullOrWhiteSpace($installerUrl)) {
        $installerUrl = "https://github.com/Feralthedogg/LLAM/releases/download/v$resolved/install.ps1"
    }

    if ($DryRun) {
        Write-Host "download $installerUrl"
        Write-Host "install LLAM $resolved into $Prefix"
        return
    }

    $installer = Join-Path $script:TempDir "llam-install.ps1"
    Invoke-WebRequest -Uri $installerUrl -OutFile $installer -UseBasicParsing
    & $installer -Prefix $Prefix -Version $resolved -Target $Target -Force
}

if ($Target -ne "windows-x86_64") {
    throw "unsupported DCC Windows installer target: $Target"
}
Assert-ReleaseComponent "target" $Target

if ($Version -eq "latest") {
    $Version = Get-LatestReleaseTag "Feralthedogg/DCC"
}
$releaseTag = $Version
$Version = $Version -replace '^v', ''
Assert-ReleaseComponent "version" $Version
if (-not $releaseTag.StartsWith("v")) {
    $releaseTag = "v$releaseTag"
}
if ([string]::IsNullOrWhiteSpace($BaseUrl)) {
    $BaseUrl = "https://github.com/Feralthedogg/DCC/releases/download/$releaseTag"
}

$package = "dcc-$Version-$Target"
$archive = "$package.zip"
$script:TempDir = Join-Path ([System.IO.Path]::GetTempPath()) ("dcc-install-" + [Guid]::NewGuid().ToString("N"))
New-Item -ItemType Directory -Force -Path $script:TempDir | Out-Null
try {
    if ($DryRun) {
        Write-Host "download $BaseUrl/$archive"
        Write-Host "download $BaseUrl/$archive.sha256"
        Write-Host "verify $archive.sha256"
        Write-Host "extract $archive"
        Write-Host "install extracted $package -> $Prefix"
        Install-LatestLlam
        exit 0
    }

    $archivePath = Join-Path $script:TempDir $archive
    $checksumPath = Join-Path $script:TempDir "$archive.sha256"
    Invoke-WebRequest -Uri "$BaseUrl/$archive" -OutFile $archivePath -UseBasicParsing
    Invoke-WebRequest -Uri "$BaseUrl/$archive.sha256" -OutFile $checksumPath -UseBasicParsing
    Assert-ArchiveChecksum $archivePath $checksumPath $archive
    Expand-Archive -LiteralPath $archivePath -DestinationPath $script:TempDir -Force

    $packageRoot = Get-ChildItem -LiteralPath $script:TempDir -Recurse -File -Filter "dcc.h" |
        Where-Object { $_.FullName -match '\\include\\dcc\\dcc\.h$' } |
        Select-Object -First 1 |
        ForEach-Object { Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $_.FullName)) }
    if (-not $packageRoot) {
        throw "could not find extracted DCC package root"
    }
    Install-DccTree $packageRoot
    Install-LatestLlam
} finally {
    Remove-Item -Recurse -Force -LiteralPath $script:TempDir -ErrorAction SilentlyContinue
}
