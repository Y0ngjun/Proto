#pragma once

#include <unordered_map>
#include <memory>
#include "Asset.h"

namespace Proto {

	class AssetManager
	{
	public:
		static void Init();
		static void AddAsset(const std::shared_ptr<Asset>& asset);
		static std::shared_ptr<Asset> GetAsset(UUID id);
		static bool RemoveAsset(UUID id);
		
		static void Clear();

		template<typename T>
		static std::shared_ptr<T> GetAssetAs(UUID id)
		{
			auto asset = GetAsset(id);
			if (asset)
				return std::dynamic_pointer_cast<T>(asset);
			return nullptr;
		}

	private:
		static std::unordered_map<UUID, std::shared_ptr<Asset>> s_Assets;
	};

}
