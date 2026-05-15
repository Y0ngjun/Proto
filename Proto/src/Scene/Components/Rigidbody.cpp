/*
 * 게임 오브젝트에 물리 법칙(중력, 속도, 마찰 등)을 적용하는 리지드바디 컴포넌트입니다.
 * 질량(Mass)과 공기 저항(Drag) 등을 통해 물리 시뮬레이션의 계산에 사용됩니다.
 */

#include "Rigidbody.h"
#include <yaml-cpp/yaml.h>

namespace Proto
{
	void Rigidbody::Serialize(YAML::Emitter& out) const
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Component" << YAML::Value << "Rigidbody";
		out << YAML::Key << "Velocity" << YAML::Value << YAML::Flow << YAML::BeginSeq << Velocity.x << Velocity.y << Velocity.z << YAML::EndSeq;
		out << YAML::Key << "Mass" << YAML::Value << Mass;
		out << YAML::Key << "UseGravity" << YAML::Value << UseGravity;
		out << YAML::Key << "Drag" << YAML::Value << Drag;
		out << YAML::EndMap;
	}

	void Rigidbody::Deserialize(const YAML::Node& node)
	{
		if (node["Velocity"])
		{
			const auto& vel = node["Velocity"];
			Velocity = { vel[0].as<float>(), vel[1].as<float>(), vel[2].as<float>() };
		}

		if (node["Mass"])
		{
			Mass = node["Mass"].as<float>();
		}

		if (node["UseGravity"])
		{
			UseGravity = node["UseGravity"].as<bool>();
		}

		if (node["Drag"])
		{
			Drag = node["Drag"].as<float>();
		}
	}
}
