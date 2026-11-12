param([switch]$RunGpuSmokeTest, [switch]$NativeOnly)
$ErrorActionPreference = 'Stop'
# Run in an x86 VS2022 Developer PowerShell. MFX linking remains separate.
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
    throw 'Fusion SDK headers are required in Inc/ for real SDK syntax checks. -NativeOnly explicitly selects independent native components and test-double coverage.'
}
# All backends coexist now. This is always the real WGC implementation, not an
# empty translation unit selected by a disabled backend macro.
Write-Host 'Checking independent WGC implementation (native wchar_t)'
& cl.exe @common '/Zc:wchar_t' "$neo/WgcCapture.cpp"
if ($LASTEXITCODE -ne 0) { throw 'WGC translation unit failed' }
if ($NativeOnly) {
    Write-Host 'Native-only scope: real Fusion SDK Main/Runtime/Edittime/General/header checks are NOT selected.'
} else {
    foreach ($source in @('Main.cpp', 'Runtime.cpp', 'Edittime.cpp', 'General.cpp', 'FrameCapture.cpp', 'tests/CaptureHeaderCompile.cpp')) {
        & cl.exe @common '/Zc:wchar_t-' "$neo/$source"
        if ($LASTEXITCODE -ne 0) { throw "$source failed against the Fusion SDK" }
    }
    $res = Join-Path ([IO.Path]::GetTempPath()) (([guid]::NewGuid().ToString('N')) + '.res')
    try {
        & rc.exe '/nologo' "/I$repo/Inc" "/I$neo" "/fo$res" "$neo/Ext.rc"
        if ($LASTEXITCODE -ne 0) { throw 'NeoPic property resources failed to compile' }
    } finally {
        if (Test-Path $res) { Remove-Item -Force $res }
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
        foreach ($test in @('CaptureBenchmarkTest', 'EditDataUpgradeTest')) {
            $exe = Join-Path $temp "$test.exe"
            $obj = Join-Path $temp "$test.obj"
            & cl.exe '/nologo' '/std:c++20' '/EHsc' '/W4' '/WX' "/Fe$exe" "/Fo$obj" "$neo/tests/$test.cpp"
            if ($LASTEXITCODE -ne 0) { throw "$test compilation failed" }
            & $exe
            if ($LASTEXITCODE -ne 0) { throw "$test failed" }
        }
        Write-Host 'Checking production adapter inheritance/ownership against test doubles, not the proprietary Fusion SDK.'
        $exe = Join-Path $temp 'CaptureInheritanceTest.exe'
        & cl.exe '/nologo' '/std:c++20' '/EHsc' '/Zc:wchar_t-' '/DUNICODE' '/D_UNICODE' "/I$repo/Extensions/_3rdLib" "/FI$neo/tests/CaptureAdapterMocks.h" "/Fe$exe" "/Fo$temp/" "$neo/FrameCapture.cpp" "$neo/tests/CaptureInheritanceTest.cpp"
        if ($LASTEXITCODE -ne 0) { throw 'Capture inheritance test compilation failed' }
        & $exe
        if ($LASTEXITCODE -ne 0) { throw 'Capture inheritance/ownership test failed' }
    } finally {
        Remove-Item -Recurse -Force $temp
    }
}
