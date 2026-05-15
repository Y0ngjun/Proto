/*
 * 직육면체(Box) 형태의 충돌 영역을 정의하는 컴포넌트입니다.
 * Collider 클래스를 상속받아 크기(Size) 속성을 추가로 가집니다.
 */

#include "BoxCollider.h"
#include <yaml-cpp/yaml.h>

namespace Proto
{
	void BoxCollider::Serialize(YAML::Emitter& out) const
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Component" << YAML::Value << "BoxCollider";
		out << YAML::Key << "Offset" << YAML::Value << YAML::Flow << YAML::BeginSeq << Offset.x << Offset.y << Offset.z << YAML::EndSeq;
		out << YAML::Key << "Size" << YAML::Value << YAML::Flow << YAML::BeginSeq << Size.x << Size.y << Size.z << YAML::EndSeq;
		out << YAML::Key << "IsTrigger" << YAML::Value << IsTrigger;
		out << YAML::EndMap;
	}

	void BoxCollider::Deserialize(const YAML::Node& node)
	{
		if (node["Offset"])
		{
			const auto& off = node["Offset"];
			Offset = { off[0].as<float>(), off[1].as<float>(), off[2].as<float>() };
		}

		if (node["Size"])
		{
			const auto& sz = node["Size"];
			Size = { sz[0].as<float>(), sz[1].as<float>(), sz[2].as<float>() };
		}

		if (node["IsTrigger"])
		{
			IsTrigger = node["IsTrigger"].as<bool>();
		}
	}
}
