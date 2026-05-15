/*
 * 엔진 내부에서 에셋을 관리하고 조회하는 정적 매니저 클래스입니다.
 */

#pragma once

#include <unordered_map>
#include <memory>
#include "Asset.h"

namespace Proto
{
	class AssetManager
	{
	public:
		static void AddAsset(const std::shared_ptr<Asset>& asset);
		static std::shared_ptr<Asset> GetAsset(UUID id);
		static bool RemoveAsset(UUID id);
		static void Clear();

		template<typename T>
		static std::shared_ptr<T> GetAssetAs(UUID id)
		{
			const auto asset = GetAsset(id);
			if (!asset)
			{
				return nullptr;
			}

			return std::dynamic_pointer_cast<T>(asset);
		}

	private:
		static std::unordered_map<UUID, std::shared_ptr<Asset>> m_Assets;
	};
}
