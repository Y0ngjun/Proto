#include "CameraComponent.h"
#include <glm/gtc/type_ptr.hpp>

namespace Proto
{
    void CameraComponent::Serialize(YAML::Emitter& out) const
    {
        out << YAML::BeginMap;
        out << YAML::Key << "Component" << YAML::Value << "CameraComponent";
        out << YAML::Key << "Primary" << YAML::Value << Primary;
        out << YAML::Key << "FixedAspectRatio" << YAML::Value << FixedAspectRatio;

        // Camera 설정 저장
        out << YAML::Key << "FOV" << YAML::Value << Camera.GetPerspectiveVerticalFOV();

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

        // Camera 설정 복원
        if (node["FOV"])
        {
            float fov = node["FOV"].as<float>();
            // FOV는 SetPerspective를 통해 설정해야 하는데, 
            // 뷰포트 크기가 아직 설정되지 않았을 수 있음
            // 따라서 여기서는 직접 저장된 FOV만 기록
            Camera.SetPerspective(fov, 0.1f, 1000.0f);
        }
    }
}
