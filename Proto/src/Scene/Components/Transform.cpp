/*
 * 게임 오브젝트의 위치(Translation), 회전(Rotation), 크기(Scale)를 관리하는 핵심 컴포넌트입니다.
 * 이 세 값을 조합하여 최종 변환 행렬(Transform Matrix)을 생성합니다.
 */

#include "Transform.h"
#include "../../Core/YAMLHelpers.h"

namespace Proto
{
	glm::mat4 Transform::GetTransform() const
	{
		// 오일러 각도를 쿼터니언으로 변환하여 회전 행렬 생성
		const glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

		return glm::translate(glm::mat4(1.0f), Translation)
			* rotation
			* glm::scale(glm::mat4(1.0f), Scale);
	}

	void Transform::Serialize(YAML::Emitter& out) const
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Component" << YAML::Value << "Transform";
		out << YAML::Key << "Translation" << YAML::Value << YAML::Flow << YAML::BeginSeq << Translation.x << Translation.y << Translation.z << YAML::EndSeq;
		out << YAML::Key << "Rotation" << YAML::Value << YAML::Flow << YAML::BeginSeq << Rotation.x << Rotation.y << Rotation.z << YAML::EndSeq;
		out << YAML::Key << "Scale" << YAML::Value << YAML::Flow << YAML::BeginSeq << Scale.x << Scale.y << Scale.z << YAML::EndSeq;
		out << YAML::EndMap;
	}

	void Transform::Deserialize(const YAML::Node& node)
	{
		if (node["Translation"])
		{
			const auto& trans = node["Translation"];
			Translation.x = trans[0].as<float>();
			Translation.y = trans[1].as<float>();
			Translation.z = trans[2].as<float>();
		}

		if (node["Rotation"])
		{
			const auto& rot = node["Rotation"];
			Rotation.x = rot[0].as<float>();
			Rotation.y = rot[1].as<float>();
			Rotation.z = rot[2].as<float>();
		}

		if (node["Scale"])
		{
			const auto& scl = node["Scale"];
			Scale.x = scl[0].as<float>();
			Scale.y = scl[1].as<float>();
			Scale.z = scl[2].as<float>();
		}
	}
}
