param(
    [ValidateSet(0, 1, 2)][int[]]$Backends = @(0, 1, 2),
    [ValidateRange(0, 100000)][int[]]$BenchmarkIterations = @(0, 100),
    [switch]$RunGpuSmokeTest,
    [switch]$NativeOnly
)
$ErrorActionPreference = 'Stop'
# Run in an x86 VS2022 Developer PowerShell. No proprietary SDK .lib is needed
# for this syntax-only check; building/linking the actual .mfx remains separate.
if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) {
    throw 'Run this script in an x86 VS2022 Developer PowerShell with a current Windows SDK.'
}
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../../..')).Path
$neo = Join-Path $repo 'Extensions/NeoPic'
$common = @('/nologo', '/std:c++20', '/EHsc', '/Zs', '/DWIN32', '/D_WINDOWS',
    '/DSTRICT', '/DUNICODE', '/D_UNICODE', '/DHWABETA',
    "/I$repo/Inc", "/I$repo/Extensions/_DeLib", "/I$repo/Extensions/_3rdLib",
    "/I$repo/Extensions/_3rdLib/Boost")
if (-not $NativeOnly -and -not (Test-Path "$repo/Inc/ccxhdr.h")) {
    throw 'Fusion SDK headers are required in Inc/ for full plugin syntax checks. -NativeOnly explicitly checks only the independent WGC/GPU components.'
}
foreach ($backend in $Backends) {
    foreach ($iterations in $BenchmarkIterations) {
        $benchmark = [int]($iterations -gt 0)
        Write-Host "Checking capture backend $backend benchmark=$benchmark iterations=$iterations"
        $defines = @("/DNEOPIC_CAPTURE_BACKEND=$backend", "/DNEOPIC_CAPTURE_BENCHMARK=$benchmark", "/DNEOPIC_CAPTURE_BENCHMARK_ITERATIONS=$iterations")
        # The PIMPL implementation has native wchar_t; Fusion keeps its existing ABI.
        & cl.exe @common '/Zc:wchar_t' @defines "$neo/WgcCapture.cpp"
        if ($LASTEXITCODE -ne 0) { throw "WGC translation unit failed for backend $backend benchmark=$iterations" }
        if ($NativeOnly) {
            Write-Host 'Native-only scope: Fusion Main/Runtime/header syntax checks are NOT selected.'
            continue
        }
        foreach ($source in @('Main.cpp', 'Runtime.cpp', 'tests/CaptureHeaderCompile.cpp')) {
            & cl.exe @common '/Zc:wchar_t-' @defines "$neo/$source"
            if ($LASTEXITCODE -ne 0) { throw "$source failed for backend $backend benchmark=$iterations" }
        }
    }
}

if ($RunGpuSmokeTest) {
    $temp = Join-Path ([IO.Path]::GetTempPath()) ([guid]::NewGuid().ToString('N'))
    New-Item -ItemType Directory -Path $temp | Out-Null
    try {
        $exe = Join-Path $temp 'CaptureBlitWarpTest.exe'
        $obj = Join-Path $temp 'CaptureBlitWarpTest.obj'
        & cl.exe '/nologo' '/std:c++20' '/EHsc' '/Zc:wchar_t' "/I$repo/Extensions/_3rdLib" "/Fe$exe" "/Fo$obj" "$neo/tests/CaptureBlitWarpTest.cpp"
        if ($LASTEXITCODE -ne 0) { throw 'WARP test compilation failed' }
        & $exe
        if ($LASTEXITCODE -ne 0) { throw 'WARP capture-normalization pixel test failed' }
        $exe = Join-Path $temp 'CaptureBenchmarkTest.exe'
        $obj = Join-Path $temp 'CaptureBenchmarkTest.obj'
        & cl.exe '/nologo' '/std:c++20' '/EHsc' '/W4' '/WX' "/Fe$exe" "/Fo$obj" "$neo/tests/CaptureBenchmarkTest.cpp"
        if ($LASTEXITCODE -ne 0) { throw 'Benchmark statistics test compilation failed' }
        & $exe
        if ($LASTEXITCODE -ne 0) { throw 'Benchmark statistics test failed' }
    } finally {
        Remove-Item -Recurse -Force $temp
    }
}
