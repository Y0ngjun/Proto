#include "BuiltinAssetLoader.h"
#include "AssetManager.h"
#include "../Renderer/MeshLoader.h"
#include "../Renderer/Shader.h"
#include "../Core/Project.h"

namespace Proto
{
    void BuiltinAssetLoader::Load()
    {
        // 1. Primitive 메쉬 생성 및 등록
        auto cube = MeshLoader::CreateCube();
        cube->Handle = UUID(DefaultAsset::Cube);
        AssetManager::AddAsset(cube);

        auto plane = MeshLoader::CreatePlane();
        plane->Handle = UUID(DefaultAsset::Plane);
        AssetManager::AddAsset(plane);

        auto sphere = MeshLoader::CreateSphere();
        sphere->Handle = UUID(DefaultAsset::Sphere);
        AssetManager::AddAsset(sphere);

        auto cylinder = MeshLoader::CreateCylinder();
        cylinder->Handle = UUID(DefaultAsset::Cylinder);
        AssetManager::AddAsset(cylinder);

        // 2. 기본 셰이더 로드 및 등록
        auto resourceDir = Project::GetEngineResourceDirectory();
        auto defaultShader = Shader::LoadFromFile(
            (resourceDir / "shaders/default.vert").string(),
            (resourceDir / "shaders/default.frag").string()
        );
        
        if (defaultShader)
        {
            defaultShader->Handle = UUID(DefaultAsset::Shader);
            AssetManager::AddAsset(defaultShader);
        }
    }
}
