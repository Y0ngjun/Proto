# run_tests.ps1
# 테스트 프로젝트를 빌드하고 실행하는 자동화 스크립트입니다.

# 1. MSBuild.exe 경로 찾기
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) {
    Write-Error "vswhere.exe를 찾을 수 없습니다."
    exit 1
}

$msbuild = & $vswhere -latest -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe"
if (-not $msbuild) {
    Write-Error "MSBuild를 찾을 수 없습니다."
    exit 1
}
if ($msbuild -is [array]) {
    $msbuild = $msbuild[0]
}

Write-Host "MSBuild found: $msbuild"
Write-Host "Building ProtoTests..."

$configuration = "Debug"
$platform = "x64"

# 테스트 프로젝트 빌드
& $msbuild "Proto.slnx" /t:ProtoTests "/p:Configuration=$configuration" "/p:Platform=$platform" /m
$exitCode = $LASTEXITCODE

if ($exitCode -ne 0) {
    Write-Host "Build failed with exit code $exitCode"
    exit $exitCode
}

Write-Host "Build success! Running tests..."

$testExe = "x64\Debug\ProtoTests.exe"
if (Test-Path $testExe) {
    & $testExe
    $testExitCode = $LASTEXITCODE
    if ($testExitCode -eq 0) {
        Write-Host "All tests passed successfully!"
    } else {
        Write-Host "Some tests failed with exit code $testExitCode"
    }
    exit $testExitCode
} else {
    Write-Host "Test executable not found: $testExe"
    exit 1
}
