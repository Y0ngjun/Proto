/*
 * yaml-cpp 라이브러리를 위한 도움말 함수 및 템플릿 특수화 정의 파일입니다.
 * glm::vec3, Proto::UUID 등의 커스텀 타입을 YAML 형식으로 직렬화/역직렬화할 수 있게 합니다.
 */

#pragma once

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>
#include "UUID.h"

namespace YAML
{
	// glm::vec3 타입에 대한 YAML 변환 특수화
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
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	// Proto::UUID 타입에 대한 YAML 변환 특수화
	template<>
	struct convert<Proto::UUID>
	{
		static Node encode(const Proto::UUID& rhs)
		{
			Node node;
			node = static_cast<uint64_t>(rhs);
			return node;
		}

		static bool decode(const Node& node, Proto::UUID& rhs)
		{
			rhs = Proto::UUID(node.as<uint64_t>());
			return true;
		}
	};
}

namespace Proto
{
	// glm::vec3를 YAML Emitter로 직렬화하기 위한 연산자 오버로딩
	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
}
