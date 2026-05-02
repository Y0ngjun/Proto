#include "NativeScriptComponent.h"
#include "../ScriptRegistry.h"
#include <yaml-cpp/yaml.h>

namespace Proto
{
	void NativeScriptComponent::Serialize(YAML::Emitter& out) const
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Component" << YAML::Value << "NativeScriptComponent";
		out << YAML::Key << "ScriptName" << YAML::Value << ScriptName;
		out << YAML::Key << "Bound" << YAML::Value << (InstantiateScript != nullptr);
		out << YAML::EndMap;
	}

	void NativeScriptComponent::Deserialize(const YAML::Node& node)
	{
		if (node["ScriptName"])
		{
			std::string name = node["ScriptName"].as<std::string>();
			// ScriptRegistry를 통해 이름 기반으로 자동 바인딩 수행
			ScriptRegistry::BindByName(name, this);
		}
	}
}
