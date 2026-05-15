/*
 * 구형(Sphere) 형태의 충돌 영역을 정의하는 컴포넌트입니다.
 * Collider 클래스를 상속받아 반지름(Radius) 속성을 추가로 가집니다.
 */

#include "SphereCollider.h"
#include <yaml-cpp/yaml.h>

namespace Proto
{
	void SphereCollider::Serialize(YAML::Emitter& out) const
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Component" << YAML::Value << "SphereCollider";
		out << YAML::Key << "Offset" << YAML::Value << YAML::Flow << YAML::BeginSeq << Offset.x << Offset.y << Offset.z << YAML::EndSeq;
		out << YAML::Key << "Radius" << YAML::Value << Radius;
		out << YAML::Key << "IsTrigger" << YAML::Value << IsTrigger;
		out << YAML::EndMap;
	}

	void SphereCollider::Deserialize(const YAML::Node& node)
	{
		if (node["Offset"])
		{
			const auto& off = node["Offset"];
			Offset = { off[0].as<float>(), off[1].as<float>(), off[2].as<float>() };
		}

		if (node["Radius"])
		{
			Radius = node["Radius"].as<float>();
		}

		if (node["IsTrigger"])
		{
			IsTrigger = node["IsTrigger"].as<bool>();
		}
	}
}
