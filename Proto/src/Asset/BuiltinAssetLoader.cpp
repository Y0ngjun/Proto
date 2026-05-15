/*
 * 엔진 기본 에셋(기본 메쉬, 셰이더 등)을 생성하고 로드하는 클래스입니다.
 */

#include "BuiltinAssetLoader.h"
#include "AssetManager.h"
#include "../Renderer/MeshLoader.h"
#include "../Renderer/VertexArray.h"
#include "../Renderer/Shader.h"
#include "../Core/Project.h"

namespace Proto
{
	void BuiltinAssetLoader::Load()
	{
		const auto cube = MeshLoader::CreateCube();
		cube->Handle = UUID(DefaultAsset::CUBE);
		AssetManager::AddAsset(cube);

		const auto plane = MeshLoader::CreatePlane();
		plane->Handle = UUID(DefaultAsset::PLANE);
		AssetManager::AddAsset(plane);

		const auto sphere = MeshLoader::CreateSphere();
		sphere->Handle = UUID(DefaultAsset::SPHERE);
		AssetManager::AddAsset(sphere);

		const auto cylinder = MeshLoader::CreateCylinder();
		cylinder->Handle = UUID(DefaultAsset::CYLINDER);
		AssetManager::AddAsset(cylinder);

		const auto resourceDir = Project::GetEngineResourceDirectory();
		const auto defaultShader = Shader::LoadFromFile(
			(resourceDir / "shaders/default.vert").string(),
			(resourceDir / "shaders/default.frag").string()
		);

		if (defaultShader)
		{
			defaultShader->Handle = UUID(DefaultAsset::SHADER);
			AssetManager::AddAsset(defaultShader);
		}
	}
}
