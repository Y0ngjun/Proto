/*
 * 엔진에서 사용하는 에셋의 기본 인터페이스와 타입을 정의합니다.
 */

#pragma once

#include "../Core/UUID.h"

namespace Proto
{
	namespace DefaultAsset
	{
		static constexpr uint64_t CUBE = 1;
		static constexpr uint64_t PLANE = 2;
		static constexpr uint64_t SPHERE = 3;
		static constexpr uint64_t CYLINDER = 4;
		static constexpr uint64_t SHADER = 100;
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