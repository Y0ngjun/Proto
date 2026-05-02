#pragma once

#include "Scene.h"
#include <memory>
#include <string>

namespace Proto {

	class SceneSerializer
	{
	public:
		SceneSerializer(Scene* scene);

		void Serialize(const std::string& filepath);
		bool Deserialize(const std::string& filepath);

	private:
		Scene* m_Scene;
	};

}
