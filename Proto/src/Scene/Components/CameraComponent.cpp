#include "CameraComponent.h"

namespace Proto
{
    void CameraComponent::Serialize(YAML::Emitter& out) const
    {
        out << YAML::BeginMap;
        out << YAML::Key << "Component" << YAML::Value << "CameraComponent";
        out << YAML::Key << "Primary" << YAML::Value << Primary;
        out << YAML::Key << "FixedAspectRatio" << YAML::Value << FixedAspectRatio;
        out << YAML::EndMap;
    }

    void CameraComponent::Deserialize(const YAML::Node& node)
    {
        if (node["Primary"])
        {
            Primary = node["Primary"].as<bool>();
        }

        if (node["FixedAspectRatio"])
        {
            FixedAspectRatio = node["FixedAspectRatio"].as<bool>();
        }
    }
}
