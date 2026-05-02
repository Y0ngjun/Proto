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

		std::string SerializeToString();
		bool DeserializeFromString(const std::string& yamlString);

	private:
		bool DeserializeNode(const YAML::Node& data);
		Scene* m_Scene;
	};

}
