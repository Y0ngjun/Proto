# Proto.vcxproj 수정 스크립트
$projFile = "C:\Users\4rest\Desktop\Yongjun\Proto\Proto\Proto.vcxproj"
$content = Get-Content $projFile -Raw

# Debug|x64 - AdditionalLibraryDirectories 수정
$content = $content -replace `
    '(<ItemDefinitionGroup Condition=".*Debug\|x64.*?>.*?<AdditionalLibraryDirectories>).*?yaml-cpp\.lib;\$\(SolutionDir\)Dependencies', `
    '$1$(SolutionDir)Dependencies\yaml-cpp\build\Debug;$(SolutionDir)Dependencies'

# Debug|x64 - AdditionalDependencies 수정 (yaml-cppd.lib 추가)
$content = $content -replace `
    '(<ItemDefinitionGroup Condition=".*Debug\|x64.*?.*?<AdditionalDependencies>)glfw3', `
    '$1yaml-cppd.lib;glfw3'

# Release|x64 - AdditionalLibraryDirectories 수정
$content = $content -replace `
    '(<ItemDefinitionGroup Condition=".*Release\|x64.*?>.*?<AdditionalLibraryDirectories>).*?\$\(SolutionDir\)Dependencies', `
    '$1$(SolutionDir)Dependencies\yaml-cpp\build\Release;$(SolutionDir)Dependencies'

# Release|x64 - AdditionalDependencies 수정 (yaml-cpp.lib 추가)
$content = $content -replace `
    '(<ItemDefinitionGroup Condition=".*Release\|x64.*?.*?<AdditionalDependencies>)glfw3', `
    '$1yaml-cpp.lib;glfw3'

Set-Content $projFile $content
Write-Host "Proto.vcxproj 수정 완료"
