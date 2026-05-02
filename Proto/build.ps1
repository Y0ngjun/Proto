# build.ps1
# Visual Studio(MSBuild) 경로를 찾아 자동으로 빌드를 수행합니다.

# 1. vswhere를 사용해 MSBuild.exe의 경로 탐색
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) {
    Write-Error "vswhere.exe를 찾을 수 없습니다. Visual Studio Installer가 필요합니다."
    exit 1
}

$msbuild = & $vswhere -latest -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe"

if (-not $msbuild) {
    Write-Error "MSBuild를 찾을 수 없습니다. Visual Studio가 설치되어 있는지 확인하세요."
    exit 1
}

# 여러 개가 검색될 수 있으므로 첫 번째 것만 사용
if ($msbuild -is [array]) {
    $msbuild = $msbuild[0]
}

# 2. MSBuild 실행
Write-Host "✓ MSBuild 발견: $msbuild" -ForegroundColor Green
Write-Host "🚀 빌드를 시작합니다 (Proto.slnx)..." -ForegroundColor Cyan

$configuration = "Debug"
$platform = "x64"

$argsArray = @("/p:Configuration=$configuration", "/p:Platform=$platform", "/m") + $args
& $msbuild "Proto.slnx" @argsArray
$exitCode = $LASTEXITCODE

if ($exitCode -eq 0) {
    Write-Host "✅ 빌드 성공! 🎉" -ForegroundColor Green
} else {
    Write-Host "❌ 빌드 실패 (종료 코드: $exitCode) 😥" -ForegroundColor Red
}

exit $exitCode
