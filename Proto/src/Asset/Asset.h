#pragma once

#include "../Core/UUID.h"

namespace Proto 
{
	namespace DefaultAsset
	{
		constexpr uint64_t Cube = 1;
		constexpr uint64_t Plane = 2;
		constexpr uint64_t Sphere = 3;
		constexpr uint64_t Cylinder = 4;
		constexpr uint64_t Shader = 100;
	}

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