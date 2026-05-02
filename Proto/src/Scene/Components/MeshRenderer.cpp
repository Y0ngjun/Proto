#include "MeshRenderer.h"
#include "../../Renderer/MeshLoader.h"

namespace Proto
{
    void MeshRenderer::Serialize(YAML::Emitter& out) const
    {
        out << YAML::BeginMap;
        out << YAML::Key << "Component" << YAML::Value << "MeshRenderer";
        out << YAML::Key << "MeshType" << YAML::Value << m_MeshTypeName;
        out << YAML::EndMap;
    }

    void MeshRenderer::Deserialize(const YAML::Node& node)
    {
        if (node["MeshType"])
        {
            m_MeshTypeName = node["MeshType"].as<std::string>();

            // MeshType에 따라 메시와 셰이더 로드
            if (m_MeshTypeName == "Cube")
            {
                m_VertexArray = MeshLoader::CreateCube();
                m_Shader = Shader::LoadFromFile("assets/shaders/standard.vert", "assets/shaders/standard.frag");
            }
            else if (m_MeshTypeName == "Plane")
            {
                m_VertexArray = MeshLoader::CreatePlane();
                m_Shader = Shader::LoadFromFile("assets/shaders/standard.vert", "assets/shaders/plane.frag");
            }
        }
    }
}


