#include "MeshRenderer.h"
#include "../../Asset/AssetManager.h"

namespace Proto
{
    void MeshRenderer::Serialize(YAML::Emitter& out) const
    {
        out << YAML::BeginMap;
        out << YAML::Key << "Component" << YAML::Value << "MeshRenderer";
        out << YAML::Key << "MeshUUID" << YAML::Value << (m_VertexArray ? (uint64_t)m_VertexArray->Handle : 0);
        out << YAML::Key << "ShaderUUID" << YAML::Value << (m_Shader ? (uint64_t)m_Shader->Handle : 0);
        out << YAML::Key << "MeshTypeName" << YAML::Value << m_MeshTypeName;
        out << YAML::EndMap;
    }

    void MeshRenderer::Deserialize(const YAML::Node& node)
    {
        if (node["MeshUUID"])
        {
            uint64_t meshUUID = node["MeshUUID"].as<uint64_t>();
            m_VertexArray = AssetManager::GetAssetAs<VertexArray>(UUID(meshUUID));
        }
        if (node["ShaderUUID"])
        {
            uint64_t shaderUUID = node["ShaderUUID"].as<uint64_t>();
            m_Shader = AssetManager::GetAssetAs<Shader>(UUID(shaderUUID));
        }
        if (node["MeshTypeName"])
        {
            m_MeshTypeName = node["MeshTypeName"].as<std::string>();
        }
    }
}


