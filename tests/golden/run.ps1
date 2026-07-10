# Build and run the golden tests (Windows, MinGW).
# Usage: .\tests\golden\run.ps1 [-Dump]
param([switch]$Dump)

$ErrorActionPreference = "Stop"
$root = Resolve-Path "$PSScriptRoot\..\.."
$cu = "$root\cubiomes"

$libsrc = @(
    "$cu\biomenoise.c", "$cu\biomes.c", "$cu\finders.c", "$cu\generator.c",
    "$cu\layers.c", "$cu\noise.c", "$cu\quadbase.c", "$cu\terrainnoise.c",
    "$cu\util.c", "$cu\xradv.c", "$cu\features\stronghold.c"
)

New-Item -ItemType Directory -Force "$root\build" | Out-Null
gcc -std=gnu2x -O2 -fwrapv -I"$cu" "$PSScriptRoot\golden.c" @libsrc -lm -o "$root\build\golden.exe"
if ($LASTEXITCODE -ne 0) { exit 1 }

if ($Dump) { & "$root\build\golden.exe" --dump } else { & "$root\build\golden.exe" }
exit $LASTEXITCODE
