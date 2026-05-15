/*
 * 게임 오브젝트에 빛(Light) 속성을 부여하는 컴포넌트입니다.
 * 빛의 색상(Color)과 강도(Intensity)를 조절할 수 있습니다.
 */

#include "LightComponent.h"
#include <yaml-cpp/yaml.h>

namespace Proto
{
	void LightComponent::Serialize(YAML::Emitter& out) const
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Component" << YAML::Value << "LightComponent";
		out << YAML::Key << "Color" << YAML::Value << YAML::Flow << YAML::BeginSeq << Color.x << Color.y << Color.z << YAML::EndSeq;
		out << YAML::Key << "Intensity" << YAML::Value << Intensity;
		out << YAML::EndMap;
	}

	void LightComponent::Deserialize(const YAML::Node& node)
	{
		if (node["Color"])
		{
			const auto& color = node["Color"];
			Color.x = color[0].as<float>();
			Color.y = color[1].as<float>();
			Color.z = color[2].as<float>();
		}

		if (node["Intensity"])
		{
			Intensity = node["Intensity"].as<float>();
		}
	}
}
