#include "Transform.h"

namespace Proto
{
	glm::mat4 Transform::GetTransform() const
	{
		glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

		return glm::translate(glm::mat4(1.0f), Translation)
			* rotation
			* glm::scale(glm::mat4(1.0f), Scale);
	}

	void Transform::Serialize(YAML::Emitter& out) const
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Component" << YAML::Value << "Transform";
		out << YAML::Key << "Translation" << YAML::Value << YAML::Flow << YAML::BeginSeq
			<< Translation.x << Translation.y << Translation.z << YAML::EndSeq;
		out << YAML::Key << "Rotation" << YAML::Value << YAML::Flow << YAML::BeginSeq
			<< Rotation.x << Rotation.y << Rotation.z << YAML::EndSeq;
		out << YAML::Key << "Scale" << YAML::Value << YAML::Flow << YAML::BeginSeq
			<< Scale.x << Scale.y << Scale.z << YAML::EndSeq;
		out << YAML::EndMap;
	}

	void Transform::Deserialize(const YAML::Node& node)
	{
		if (node["Translation"])
		{
			auto trans = node["Translation"];
			Translation.x = trans[0].as<float>();
			Translation.y = trans[1].as<float>();
			Translation.z = trans[2].as<float>();
		}

		if (node["Rotation"])
		{
			auto rot = node["Rotation"];
			Rotation.x = rot[0].as<float>();
			Rotation.y = rot[1].as<float>();
			Rotation.z = rot[2].as<float>();
		}

		if (node["Scale"])
		{
			auto scl = node["Scale"];
			Scale.x = scl[0].as<float>();
			Scale.y = scl[1].as<float>();
			Scale.z = scl[2].as<float>();
		}
	}
}
