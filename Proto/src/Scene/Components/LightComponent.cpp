#include "LightComponent.h"

namespace Proto
{
    void LightComponent::Serialize(YAML::Emitter& out) const
    {
        out << YAML::BeginMap;
        out << YAML::Key << "Component" << YAML::Value << "LightComponent";
        out << YAML::Key << "Color" << YAML::Value << YAML::Flow << YAML::BeginSeq
            << Color.x << Color.y << Color.z << YAML::EndSeq;
        out << YAML::Key << "Intensity" << YAML::Value << Intensity;
        out << YAML::EndMap;
    }

    void LightComponent::Deserialize(const YAML::Node& node)
    {
        if (node["Color"])
        {
            auto color = node["Color"];
            Color.x = color[0].as<float>();
            Color.y = color[1].as<float>();
            Color.z = color[2].as<float>();
        }

        if (node["Intensity"])
        {
            Intensity = node["Intensity"].as<float>();
        }
    }
}
