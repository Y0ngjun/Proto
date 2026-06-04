# Proto 빌드 스크립트
# 사용법: .\build.ps1 [-Config Debug|Release] [-Run] [-Tests]
#   -Config  : 빌드 구성 (기본값: Debug)
#   -Run     : 빌드 성공 후 Proto.exe 실행
#   -Tests   : 빌드 성공 후 ProtoTests.exe 실행

param(
    [ValidateSet("Debug", "Release")]
    [string]$Config = "Debug",
    [switch]$Run,
    [switch]$Tests
)

$MSBUILD      = "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"
$MAIN_PROJ    = "$PSScriptRoot\Proto.vcxproj"
$TESTS_PROJ   = "$PSScriptRoot\ProtoTests.vcxproj"
$OUT_DIR      = "$PSScriptRoot\x64\$Config"

Write-Host "=== Proto Build [$Config] ===" -ForegroundColor Cyan

# ── 메인 프로젝트 빌드 ────────────────────────────────────────────────────────
& $MSBUILD $MAIN_PROJ /p:Configuration=$Config /p:Platform=x64 /t:Build /nologo /v:minimal
if ($LASTEXITCODE -ne 0)
{
    Write-Host "빌드 실패." -ForegroundColor Red
    exit 1
}
Write-Host "빌드 성공: $OUT_DIR\Proto.exe" -ForegroundColor Green

# ── 테스트 프로젝트 빌드 & 실행 ──────────────────────────────────────────────
if ($Tests)
{
    Write-Host ""
    Write-Host "=== ProtoTests Build [$Config] ===" -ForegroundColor Cyan
    & $MSBUILD $TESTS_PROJ /p:Configuration=$Config /p:Platform=x64 /t:Build /nologo /v:minimal
    if ($LASTEXITCODE -ne 0)
    {
        Write-Host "테스트 빌드 실패." -ForegroundColor Red
        exit 1
    }

    Write-Host ""
    Write-Host "=== 테스트 실행 ===" -ForegroundColor Cyan
    & "$OUT_DIR\ProtoTests.exe"
    if ($LASTEXITCODE -ne 0)
    {
        Write-Host "테스트 실패." -ForegroundColor Red
        exit 1
    }
    Write-Host "모든 테스트 통과." -ForegroundColor Green
}

# ── 실행 ──────────────────────────────────────────────────────────────────────
if ($Run)
{
    Write-Host ""
    Write-Host "=== Proto 실행 ===" -ForegroundColor Cyan
    & "$OUT_DIR\Proto.exe"
}
