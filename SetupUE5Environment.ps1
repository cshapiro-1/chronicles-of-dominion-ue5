# ==============================================================================
# CHRONICLES OF DOMINION: UNREAL ENGINE 5 AUTOMATED SETUP SCRIPT
# ==============================================================================
param (
    [switch]$InstallPrerequisites = $false
)

Write-Host "==================================================================" -ForegroundColor Cyan
Write-Host "  CHRONICLES OF DOMINION: TRIPLE-A UNREAL ENGINE 5 SETUP" -ForegroundColor Yellow
Write-Host "==================================================================" -ForegroundColor Cyan

$projectPath = "$PSScriptRoot\ChroniclesOfDominion.uproject"
Write-Host "Project File: $projectPath" -ForegroundColor White

# 1. Check for Visual Studio 2022 / C++ Build Tools
Write-Host "`n[1/3] Checking Visual Studio 2022 C++ Tools..." -ForegroundColor Cyan
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$hasVS = $false

if (Test-Path $vsWhere) {
    $vsPath = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if ($vsPath) {
        Write-Host "  [OK] Visual Studio C++ Compiler found: $vsPath" -ForegroundColor Green
        $hasVS = $true
    }
}

if (-not $hasVS) {
    Write-Host "  [MISSING] Visual Studio 2022 with C++ Game Development workload is not installed." -ForegroundColor Yellow
    if ($InstallPrerequisites) {
        Write-Host "  Installing Visual Studio 2022 Community via winget..." -ForegroundColor Cyan
        winget install --id Microsoft.VisualStudio.2022.Community -e --accept-package-agreements --accept-source-agreements --override "--add Microsoft.VisualStudio.Workload.NativeGame --includeRecommended --passive"
    } else {
        Write-Host "  -> To install automatically, run with: .\SetupUE5Environment.ps1 -InstallPrerequisites" -ForegroundColor Gray
    }
}

# 2. Check for Epic Games Launcher & Unreal Engine 5
Write-Host "`n[2/3] Checking Epic Games Launcher & Unreal Engine 5..." -ForegroundColor Cyan
$epicLauncher = "C:\Program Files (x86)\Epic Games\Launcher\Portal\Binaries\Win64\EpicGamesLauncher.exe"
$hasEpic = Test-Path $epicLauncher

if ($hasEpic) {
    Write-Host "  [OK] Epic Games Launcher found: $epicLauncher" -ForegroundColor Green
} else {
    Write-Host "  [MISSING] Epic Games Launcher is not installed." -ForegroundColor Yellow
    if ($InstallPrerequisites) {
        Write-Host "  Installing Epic Games Launcher via winget..." -ForegroundColor Cyan
        winget install --id EpicGames.EpicGamesLauncher -e --accept-package-agreements --accept-source-agreements
    } else {
        Write-Host "  -> To install automatically, run with: .\SetupUE5Environment.ps1 -InstallPrerequisites" -ForegroundColor Gray
    }
}

# 3. Check for Unreal Engine 5.4+ Engine Installation
Write-Host "`n[3/3] Checking for Unreal Engine 5 Engine installation..." -ForegroundColor Cyan
$uePaths = @(
    "C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor.exe",
    "C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe",
    "D:\Program Files\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor.exe",
    "D:\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor.exe",
    "E:\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor.exe"
)

$foundUE = $null
foreach ($p in $uePaths) {
    if (Test-Path $p) {
        $foundUE = $p
        break
    }
}

if ($foundUE) {
    Write-Host "  [OK] Unreal Engine Editor found: $foundUE" -ForegroundColor Green
    Write-Host "`nLaunching Chronicles of Dominion in Unreal Engine 5..." -ForegroundColor Yellow
    Start-Process -FilePath $foundUE -ArgumentList "`"$projectPath`""
} else {
    Write-Host "  [ACTION REQUIRED] Unreal Engine 5.4+ is not yet downloaded." -ForegroundColor Yellow
    Write-Host "  Once Epic Games Launcher opens, click 'Unreal Engine' -> 'Library' -> Install 'Unreal Engine 5.4'." -ForegroundColor White
}

Write-Host "`n==================================================================" -ForegroundColor Cyan
Write-Host "Setup Check Complete!" -ForegroundColor Green
Write-Host "==================================================================" -ForegroundColor Cyan
