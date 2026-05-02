#pragma once

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>
#include "UUID.h"

namespace YAML {

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Proto::UUID>
	{
		static Node encode(const Proto::UUID& rhs)
		{
			Node node;
			node = (uint64_t)rhs;
			return node;
		}

		static bool decode(const Node& node, Proto::UUID& rhs)
		{
			rhs = Proto::UUID(node.as<uint64_t>());
			return true;
		}
	};
}

namespace Proto {
	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
}
