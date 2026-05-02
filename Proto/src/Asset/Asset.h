#pragma once

#include "../Core/UUID.h"

namespace Proto {

	enum class AssetType
	{
		None = 0,
		Scene,
		Mesh,
		Shader,
		Material,
		Texture
	};

	class Asset
	{
	public:
		UUID Handle;
		
		virtual ~Asset() = default;
		virtual AssetType GetType() const = 0;
	};

}
