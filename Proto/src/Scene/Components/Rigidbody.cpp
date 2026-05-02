#include "Rigidbody.h"

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
			auto vel = node["Velocity"];
			Velocity = { vel[0].as<float>(), vel[1].as<float>(), vel[2].as<float>() };
		}
		if (node["Mass"]) Mass = node["Mass"].as<float>();
		if (node["UseGravity"]) UseGravity = node["UseGravity"].as<bool>();
		if (node["Drag"]) Drag = node["Drag"].as<float>();
	}
}
