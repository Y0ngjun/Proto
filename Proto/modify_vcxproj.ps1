$xml = [xml](Get-Content "Proto.vcxproj")

# Debug|x64
$debugNode = $xml.Project.ItemDefinitionGroup | Where-Object { $_.'Condition' -like "*Debug|x64*" }
$debugNode.ClCompile.PreprocessorDefinitions = "_DEBUG;_CONSOLE;YAML_CPP_STATIC_DEFINE;%(PreprocessorDefinitions)"

# Release|x64
$releaseNode = $xml.Project.ItemDefinitionGroup | Where-Object { $_.'Condition' -like "*Release|x64*" }
$releaseNode.ClCompile.PreprocessorDefinitions = "NDEBUG;_CONSOLE;YAML_CPP_STATIC_DEFINE;%(PreprocessorDefinitions)"

$xml.Save("Proto.vcxproj")
Write-Host "SUCCESS: Proto.vcxproj updated with YAML_CPP_STATIC_DEFINE"
