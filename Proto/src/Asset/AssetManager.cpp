#include "AssetManager.h"
#include "../Renderer/MeshLoader.h"
#include "../Renderer/Shader.h"
#include "../Core/Project.h"

namespace Proto 
{
	std::unordered_map<UUID, std::shared_ptr<Asset>> AssetManager::s_Assets;

	void AssetManager::Init()
	{
		auto cube = MeshLoader::CreateCube();
		cube->Handle = UUID(DefaultAsset::Cube);
		AddAsset(cube);

		auto plane = MeshLoader::CreatePlane();
		plane->Handle = UUID(DefaultAsset::Plane);
		AddAsset(plane);

		auto sphere = MeshLoader::CreateSphere();
		sphere->Handle = UUID(DefaultAsset::Sphere);
		AddAsset(sphere);

		auto cylinder = MeshLoader::CreateCylinder();
		cylinder->Handle = UUID(DefaultAsset::Cylinder);
		AddAsset(cylinder);

		auto resourceDir = Project::GetEngineResourceDirectory();
		auto defaultShader = Shader::LoadFromFile(
			(resourceDir / "shaders/default.vert").string(),
			(resourceDir / "shaders/default.frag").string()
		);
		defaultShader->Handle = UUID(DefaultAsset::Shader);
		AddAsset(defaultShader);
	}

	void AssetManager::AddAsset(const std::shared_ptr<Asset>& asset)
	{
		if (asset)
			s_Assets[asset->Handle] = asset;
	}

	std::shared_ptr<Asset> AssetManager::GetAsset(UUID id)
	{
		auto it = s_Assets.find(id);
		if (it != s_Assets.end())
			return it->second;
		return nullptr;
	}

	bool AssetManager::RemoveAsset(UUID id)
	{
		return s_Assets.erase(id) > 0;
	}

	void AssetManager::Clear()
	{
		s_Assets.clear();
	}

}
