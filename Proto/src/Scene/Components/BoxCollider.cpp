#include "BoxCollider.h"

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
			auto off = node["Offset"];
			Offset = { off[0].as<float>(), off[1].as<float>(), off[2].as<float>() };
		}
		if (node["Size"])
		{
			auto sz = node["Size"];
			Size = { sz[0].as<float>(), sz[1].as<float>(), sz[2].as<float>() };
		}
		if (node["IsTrigger"]) IsTrigger = node["IsTrigger"].as<bool>();
	}
}
