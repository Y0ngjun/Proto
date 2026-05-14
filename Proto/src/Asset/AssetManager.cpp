#include "AssetManager.h"

namespace Proto 
{
	std::unordered_map<UUID, std::shared_ptr<Asset>> AssetManager::s_Assets;

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
