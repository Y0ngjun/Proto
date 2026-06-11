/*
 * 메쉬 데이터와 셰이더를 조합하여 게임 오브젝트를 화면에 그리는 렌더링 컴포넌트입니다.
 * 사용하는 3D 모델(VertexArray)과 재질(Shader), 오브젝트 색상(Color)에 대한 참조를 관리합니다.
 */

#include "MeshRenderer.h"
#include "../../Asset/Asset.h"
#include "../../Asset/AssetManager.h"
#include "../../Renderer/VertexArray.h"
#include "../../Renderer/Shader.h"
#include "../../Core/YAMLHelpers.h"

#include <array>
#include <string_view>

namespace Proto
{
	namespace
	{
		struct BuiltinMeshEntry { uint64_t uuid; std::string_view name; };

		static constexpr std::array<BuiltinMeshEntry, 4> s_BuiltinMeshTable = {{
			{ DefaultAsset::CUBE,     "Cube"     },
			{ DefaultAsset::PLANE,    "Plane"    },
			{ DefaultAsset::SPHERE,   "Sphere"   },
			{ DefaultAsset::CYLINDER, "Cylinder" },
		}};

		std::string_view UUIDToMeshTypeName(uint64_t uuid)
		{
			for (const auto& e : s_BuiltinMeshTable)
				if (e.uuid == uuid) return e.name;
			return {};
		}

		bool MeshTypeNameToUUID(std::string_view name, uint64_t& outUUID)
		{
			for (const auto& e : s_BuiltinMeshTable)
				if (e.name == name) { outUUID = e.uuid; return true; }
			return false;
		}
	}

	std::string MeshRenderer::GetMeshTypeName() const
	{
		if (!m_VertexArray) return {};
		return std::string(UUIDToMeshTypeName(static_cast<uint64_t>(m_VertexArray->Handle)));
	}

	void MeshRenderer::Reset()
	{
		m_VertexArray = nullptr;
		Color         = { 1.0f, 1.0f, 1.0f };
	}

	void MeshRenderer::Serialize(YAML::Emitter& out) const
	{
		const uint64_t meshUUID = m_VertexArray ? static_cast<uint64_t>(m_VertexArray->Handle) : 0;

		out << YAML::BeginMap;
		out << YAML::Key << "Component"    << YAML::Value << "MeshRenderer";
		out << YAML::Key << "MeshUUID"     << YAML::Value << meshUUID;
		out << YAML::Key << "ShaderUUID"   << YAML::Value << (m_Shader ? static_cast<uint64_t>(m_Shader->Handle) : 0);
		out << YAML::Key << "MeshTypeName" << YAML::Value << std::string(UUIDToMeshTypeName(meshUUID));
		out << YAML::Key << "Color"        << YAML::Value << Color;
		out << YAML::EndMap;
	}

	void MeshRenderer::Deserialize(const YAML::Node& node)
	{
		uint64_t meshUUID = 0;
		if (node["MeshUUID"])
			meshUUID = node["MeshUUID"].as<uint64_t>();

		if (meshUUID != 0)
			m_VertexArray = AssetManager::GetAssetAs<VertexArray>(UUID(meshUUID));

		if (!m_VertexArray && node["MeshTypeName"])
		{
			const std::string typeName = node["MeshTypeName"].as<std::string>();
			uint64_t builtinUUID = 0;
			if (MeshTypeNameToUUID(typeName, builtinUUID))
				m_VertexArray = AssetManager::GetAssetAs<VertexArray>(UUID(builtinUUID));
		}

		if (node["ShaderUUID"])
		{
			const uint64_t shaderUUID = node["ShaderUUID"].as<uint64_t>();
			m_Shader = AssetManager::GetAssetAs<Shader>(UUID(shaderUUID));
		}

		if (node["Color"])
			Color = node["Color"].as<glm::vec3>();
	}
}
