#include "MeshRenderer.h"

namespace Proto
{
    void MeshRenderer::Serialize(YAML::Emitter& out) const
    {
        out << YAML::BeginMap;
        out << YAML::Key << "Component" << YAML::Value << "MeshRenderer";
        // 메시와 셰이더는 런타임 리소스이므로 직렬화하지 않음
        // 향후 AssetManager 구현 시 ID로 참조 가능하도록 변경
        out << YAML::EndMap;
    }

    void MeshRenderer::Deserialize(const YAML::Node& node)
    {
        // 메시와 셰이더는 런타임에 별도로 설정
    }
}
