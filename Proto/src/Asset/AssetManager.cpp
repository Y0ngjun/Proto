/*
 * 엔진 내부에서 에셋을 관리하고 조회하는 정적 매니저 클래스입니다.
 */

#include "AssetManager.h"

namespace Proto
{
	std::unordered_map<UUID, std::shared_ptr<Asset>> AssetManager::m_Assets;

	void AssetManager::AddAsset(const std::shared_ptr<Asset>& asset)
	{
		if (!asset)
		{
			return;
		}

		m_Assets[asset->Handle] = asset;
	}

	std::shared_ptr<Asset> AssetManager::GetAsset(UUID id)
	{
		const auto it = m_Assets.find(id);
		if (it == m_Assets.end())
		{
			return nullptr;
		}

		return it->second;
	}

	bool AssetManager::RemoveAsset(UUID id)
	{
		return m_Assets.erase(id) > 0;
	}

	void AssetManager::Clear()
	{
		m_Assets.clear();
	}
}
