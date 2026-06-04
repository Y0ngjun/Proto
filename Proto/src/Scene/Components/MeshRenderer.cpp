/*
 * 메쉬 데이터와 셰이더를 조합하여 게임 오브젝트를 화면에 그리는 렌더링 컴포넌트입니다.
 * 사용하는 3D 모델(VertexArray)과 재질(Shader)에 대한 참조를 관리합니다.
 */

#include "MeshRenderer.h"
#include "../../Asset/Asset.h"
#include "../../Asset/AssetManager.h"
#include "../../Renderer/VertexArray.h"
#include "../../Renderer/Shader.h"
#include "../../Core/YAMLHelpers.h"

namespace Proto
{
	namespace
	{
		bool TryGetBuiltinMeshUUID(const std::string& meshTypeName, UUID& outUUID)
		{
			if (meshTypeName == "Cube")
			{
				outUUID = UUID(DefaultAsset::CUBE);
				return true;
			}

			if (meshTypeName == "Plane")
			{
				outUUID = UUID(DefaultAsset::PLANE);
				return true;
			}

			if (meshTypeName == "Sphere")
			{
				outUUID = UUID(DefaultAsset::SPHERE);
				return true;
			}

			if (meshTypeName == "Cylinder")
			{
				outUUID = UUID(DefaultAsset::CYLINDER);
				return true;
			}

			return false;
		}

		bool TryGetBuiltinMeshTypeName(uint64_t meshUUID, std::string& outName)
		{
			if (meshUUID == DefaultAsset::CUBE)
			{
				outName = "Cube";
				return true;
			}

			if (meshUUID == DefaultAsset::PLANE)
			{
				outName = "Plane";
				return true;
			}

			if (meshUUID == DefaultAsset::SPHERE)
			{
				outName = "Sphere";
				return true;
			}

			if (meshUUID == DefaultAsset::CYLINDER)
			{
				outName = "Cylinder";
				return true;
			}

			return false;
		}
	}

	void MeshRenderer::Reset()
	{
		m_VertexArray = nullptr;
		m_Shader      = nullptr;
		m_MeshTypeName = "";
	}

	void MeshRenderer::Serialize(YAML::Emitter& out) const
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Component" << YAML::Value << "MeshRenderer";
		out << YAML::Key << "MeshUUID" << YAML::Value << (m_VertexArray ? static_cast<uint64_t>(m_VertexArray->Handle) : 0);
		out << YAML::Key << "ShaderUUID" << YAML::Value << (m_Shader ? static_cast<uint64_t>(m_Shader->Handle) : 0);
		out << YAML::Key << "MeshTypeName" << YAML::Value << m_MeshTypeName;
		out << YAML::EndMap;
	}

	void MeshRenderer::Deserialize(const YAML::Node& node)
	{
		uint64_t meshUUID = 0;
		if (node["MeshUUID"])
		{
			meshUUID = node["MeshUUID"].as<uint64_t>();
			if (meshUUID != 0)
			{
				m_VertexArray = AssetManager::GetAssetAs<VertexArray>(UUID(meshUUID));
			}
		}

		if (node["ShaderUUID"])
		{
			const uint64_t shaderUUID = node["ShaderUUID"].as<uint64_t>();
			m_Shader = AssetManager::GetAssetAs<Shader>(UUID(shaderUUID));
		}

		if (node["MeshTypeName"])
		{
			m_MeshTypeName = node["MeshTypeName"].as<std::string>();
		}

		if (!m_MeshTypeName.empty() && !m_VertexArray)
		{
			UUID builtinUUID;
			if (TryGetBuiltinMeshUUID(m_MeshTypeName, builtinUUID))
			{
				m_VertexArray = AssetManager::GetAssetAs<VertexArray>(builtinUUID);
			}
		}

		if (m_MeshTypeName.empty() && meshUUID != 0)
		{
			std::string typeName;
			if (TryGetBuiltinMeshTypeName(meshUUID, typeName))
			{
				m_MeshTypeName = typeName;
			}
		}
	}
}
