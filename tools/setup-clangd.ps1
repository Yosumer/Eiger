[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$ProjectRoot,

    [string]$ProjectFile,

    [string]$TargetName,

    [string]$ClangdPath,

    [string]$ToolchainIncludePath
)

$ErrorActionPreference = 'Stop'

function Resolve-FullPath {
    param([string]$BasePath, [string]$Path)

    if ([System.IO.Path]::IsPathRooted($Path)) {
        return [System.IO.Path]::GetFullPath($Path)
    }
    return [System.IO.Path]::GetFullPath((Join-Path $BasePath $Path))
}

function Split-KeilList {
    param([object]$Value)

    if ($null -eq $Value) {
        return @()
    }

    return @([string]$Value -split '[;,]' | ForEach-Object { $_.Trim() } | Where-Object { $_ })
}

function Get-ControlValues {
    param([object]$Controls, [string]$ProjectDirectory)

    $result = [ordered]@{
        Defines = @()
        Undefines = @()
        Includes = @()
    }

    if ($null -eq $Controls) {
        return $result
    }

    $result.Defines = @(Split-KeilList $Controls.Define)
    $result.Undefines = @(Split-KeilList $Controls.Undefine)
    $result.Includes = @(Split-KeilList $Controls.IncludePath | ForEach-Object {
        Resolve-FullPath $ProjectDirectory $_
    })
    return $result
}

function Add-UniqueStrings {
    param(
        [System.Collections.Generic.List[string]]$Destination,
        [System.Collections.Generic.HashSet[string]]$Seen,
        [object[]]$Values
    )

    foreach ($value in @($Values)) {
        if ($null -ne $value -and $Seen.Add([string]$value)) {
            $Destination.Add([string]$value)
        }
    }
}

function Set-JsonProperty {
    param([object]$Object, [string]$Name, [object]$Value)

    $property = $Object.PSObject.Properties[$Name]
    if ($null -eq $property) {
        $Object | Add-Member -NotePropertyName $Name -NotePropertyValue $Value
    }
    else {
        $property.Value = $Value
    }
}

function Write-Utf8NoBomAtomic {
    param([string]$Path, [string]$Content)

    $directory = Split-Path -Parent $Path
    if (-not (Test-Path -LiteralPath $directory)) {
        New-Item -ItemType Directory -Force -Path $directory | Out-Null
    }

    $temporaryPath = "$Path.tmp"
    $encoding = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllText($temporaryPath, $Content, $encoding)
    Move-Item -Force -LiteralPath $temporaryPath -Destination $Path
}

$ProjectRoot = Resolve-FullPath (Get-Location).Path $ProjectRoot
if (-not (Test-Path -LiteralPath $ProjectRoot -PathType Container)) {
    throw "Project root does not exist: $ProjectRoot"
}

if ([string]::IsNullOrWhiteSpace($ProjectFile)) {
    $projectCandidates = @(Get-ChildItem -LiteralPath $ProjectRoot -File -Filter '*.uvprojx')
    if ($projectCandidates.Count -ne 1) {
        throw "Expected exactly one .uvprojx in $ProjectRoot, found $($projectCandidates.Count). Pass -ProjectFile explicitly."
    }
    $ProjectFile = $projectCandidates[0].FullName
}
else {
    $ProjectFile = Resolve-FullPath $ProjectRoot $ProjectFile
}

if (-not (Test-Path -LiteralPath $ProjectFile -PathType Leaf)) {
    throw "Keil project does not exist: $ProjectFile"
}

[xml]$projectXml = Get-Content -Raw -LiteralPath $ProjectFile
$targets = @($projectXml.Project.Targets.Target)
if ($targets.Count -eq 0) {
    throw "No Keil targets found in $ProjectFile"
}

if ([string]::IsNullOrWhiteSpace($TargetName)) {
    $target = $targets[0]
}
else {
    $target = @($targets | Where-Object { [string]$_.TargetName -eq $TargetName })[0]
    if ($null -eq $target) {
        $available = ($targets | ForEach-Object { [string]$_.TargetName }) -join ', '
        throw "Keil target '$TargetName' was not found. Available targets: $available"
    }
}

$selectedTargetName = [string]$target.TargetName
$cpuMetadata = [string]$target.TargetOption.TargetCommonOption.Cpu
$cpuName = 'cortex-m7'
if ($cpuMetadata -match 'CPUTYPE\("([^"]+)"\)') {
    $cpuName = $Matches[1].ToLowerInvariant()
}

if ([string]::IsNullOrWhiteSpace($ToolchainIncludePath)) {
    $keilCompilerDirectory = $(if ([string]$target.uAC6 -eq '1') { 'ARMCLANG' } else { 'ARMCC' })
    $toolchainCandidates = @(
        $(if ($env:KEIL_ROOT) { Join-Path $env:KEIL_ROOT "ARM\$keilCompilerDirectory\include" }),
        "C:\Keil_v5\ARM\$keilCompilerDirectory\include"
    ) | Where-Object { $_ }
    $ToolchainIncludePath = @($toolchainCandidates | Where-Object { Test-Path -LiteralPath $_ -PathType Container })[0]
}
elseif (-not (Test-Path -LiteralPath $ToolchainIncludePath -PathType Container)) {
    throw "Toolchain include directory does not exist: $ToolchainIncludePath"
}

if (-not [string]::IsNullOrWhiteSpace($ToolchainIncludePath)) {
    $ToolchainIncludePath = Resolve-FullPath $ProjectRoot $ToolchainIncludePath
}

$commonArguments = New-Object 'System.Collections.Generic.List[string]'
$commonArguments.Add('--target=arm-none-eabi')
$commonArguments.Add("-mcpu=$cpuName")
$commonArguments.Add('-mthumb')
if ($cpuMetadata -match 'FPU3\(DFPU\)') {
    $commonArguments.Add('-mfpu=fpv5-d16')
    $commonArguments.Add('-mfloat-abi=hard')
}
elseif ($cpuMetadata -match 'FPU3\(SFPU\)') {
    $commonArguments.Add('-mfpu=fpv5-sp-d16')
    $commonArguments.Add('-mfloat-abi=hard')
}
$commonArguments.Add('-funsigned-char')
$commonArguments.Add('-fshort-enums')
$commonArguments.Add('-fshort-wchar')
$commonArguments.Add('-Wno-unknown-attributes')
$commonArguments.Add('-Wno-ignored-attributes')
$commonArguments.Add('-fdeclspec')
$commonArguments.Add('-D__weak=__attribute__((weak))')
$commonArguments.Add('-D__packed=__attribute__((packed))')
$commonArguments.Add('-D__align(n)=__attribute__((aligned(n)))')
$commonArguments.Add('-D__forceinline=inline __attribute__((always_inline))')
$commonArguments.Add('-D__value_in_regs=')

$targetControls = Get-ControlValues $target.TargetOption.TargetArmAds.Cads.VariousControls $ProjectRoot
$compileCommands = New-Object 'System.Collections.Generic.List[object]'
$missingIncludes = New-Object 'System.Collections.Generic.HashSet[string]' ([System.StringComparer]::OrdinalIgnoreCase)

foreach ($group in @($target.Groups.Group)) {
    $groupControls = Get-ControlValues $group.GroupOption.GroupArmAds.Cads.VariousControls $ProjectRoot

    foreach ($file in @($group.Files.File)) {
        $relativeFile = [string]$file.FilePath
        if ([string]::IsNullOrWhiteSpace($relativeFile)) {
            continue
        }

        $extension = [System.IO.Path]::GetExtension($relativeFile).ToLowerInvariant()
        if ($extension -notin @('.c', '.cc', '.cpp', '.cxx')) {
            continue
        }

        $absoluteFile = Resolve-FullPath $ProjectRoot $relativeFile
        if (-not (Test-Path -LiteralPath $absoluteFile -PathType Leaf)) {
            Write-Warning "Source listed by Keil does not exist: $absoluteFile"
            continue
        }

        $fileControls = Get-ControlValues $file.FileOption.FileArmAds.Cads.VariousControls $ProjectRoot
        $defines = New-Object 'System.Collections.Generic.List[string]'
        $undefines = New-Object 'System.Collections.Generic.List[string]'
        $includes = New-Object 'System.Collections.Generic.List[string]'
        $defineSeen = New-Object 'System.Collections.Generic.HashSet[string]' ([System.StringComparer]::Ordinal)
        $undefineSeen = New-Object 'System.Collections.Generic.HashSet[string]' ([System.StringComparer]::Ordinal)
        $includeSeen = New-Object 'System.Collections.Generic.HashSet[string]' ([System.StringComparer]::OrdinalIgnoreCase)

        Add-UniqueStrings $defines $defineSeen @($targetControls.Defines + $groupControls.Defines + $fileControls.Defines)
        Add-UniqueStrings $undefines $undefineSeen @($targetControls.Undefines + $groupControls.Undefines + $fileControls.Undefines)
        Add-UniqueStrings $includes $includeSeen @($targetControls.Includes + $groupControls.Includes + $fileControls.Includes)

        $arguments = New-Object 'System.Collections.Generic.List[string]'
        $arguments.Add($(if ($extension -eq '.c') { 'clang' } else { 'clang++' }))
        foreach ($argument in $commonArguments) { $arguments.Add($argument) }
        $arguments.Add($(if ($extension -eq '.c') { '-std=c99' } else { '-std=c++14' }))
        foreach ($define in $defines) { $arguments.Add("-D$define") }
        foreach ($undefine in $undefines) { $arguments.Add("-U$undefine") }
        if (-not [string]::IsNullOrWhiteSpace($ToolchainIncludePath)) {
            $arguments.Add('-isystem')
            $arguments.Add($ToolchainIncludePath)
        }
        foreach ($include in $includes) {
            $arguments.Add("-I$include")
            if (-not (Test-Path -LiteralPath $include -PathType Container)) {
                [void]$missingIncludes.Add($include)
            }
        }
        $arguments.Add('-fsyntax-only')
        $arguments.Add($absoluteFile)

        $compileCommands.Add([ordered]@{
            directory = $ProjectRoot
            file = $absoluteFile
            arguments = @($arguments)
        })
    }
}

if ($compileCommands.Count -eq 0) {
    throw "Target '$selectedTargetName' contains no existing C/C++ source files."
}

$compileCommandsPath = Join-Path $ProjectRoot 'compile_commands.json'
$commandArray = [object[]]@($compileCommands | ForEach-Object { $_ })
$compileCommandsJson = ConvertTo-Json -InputObject $commandArray -Depth 8
Write-Utf8NoBomAtomic $compileCommandsPath ($compileCommandsJson + [Environment]::NewLine)

$clangdConfig = @'
CompileFlags:
  CompilationDatabase: .
Index:
  Background: Build
Diagnostics:
  UnusedIncludes: None
'@
Write-Utf8NoBomAtomic (Join-Path $ProjectRoot '.clangd') ($clangdConfig.TrimStart() + [Environment]::NewLine)

if ([string]::IsNullOrWhiteSpace($ClangdPath)) {
    $clangdCommand = Get-Command clangd -ErrorAction SilentlyContinue
    if ($null -ne $clangdCommand) {
        $ClangdPath = $clangdCommand.Source
    }
    elseif (Test-Path -LiteralPath 'C:\Program Files\LLVM\bin\clangd.exe') {
        $ClangdPath = 'C:\Program Files\LLVM\bin\clangd.exe'
    }
    else {
        $ClangdPath = 'clangd'
        Write-Warning 'clangd.exe was not found. Install LLVM with: winget install --id LLVM.LLVM -e'
    }
}

$settingsDirectory = Join-Path $ProjectRoot '.vscode'
$settingsPath = Join-Path $settingsDirectory 'settings.json'
if (Test-Path -LiteralPath $settingsPath) {
    $settings = Get-Content -Raw -LiteralPath $settingsPath | ConvertFrom-Json
}
else {
    $settings = [pscustomobject]@{}
}

Set-JsonProperty $settings 'clangd.path' $ClangdPath
Set-JsonProperty $settings 'clangd.arguments' @('--compile-commands-dir=${workspaceFolder}', '--background-index', '--header-insertion=never')
Set-JsonProperty $settings 'clangd.restartAfterCrash' $true
Set-JsonProperty $settings 'C_Cpp.intelliSenseEngine' 'disabled'
Set-JsonProperty $settings 'C_Cpp.errorSquiggles' 'disabled'
Set-JsonProperty $settings 'files.autoGuessEncoding' $true
$settingsJson = $settings | ConvertTo-Json -Depth 20
Write-Utf8NoBomAtomic $settingsPath ($settingsJson + [Environment]::NewLine)

foreach ($missingInclude in $missingIncludes) {
    Write-Warning "Include directory does not exist: $missingInclude"
}

Write-Host "Configured clangd for Keil target '$selectedTargetName'."
Write-Host "Compilation commands: $($compileCommands.Count)"
Write-Host "Project: $ProjectRoot"
Write-Host "clangd: $ClangdPath"
