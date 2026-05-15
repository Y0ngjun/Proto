/*
 * 메쉬 데이터와 셰이더를 조합하여 게임 오브젝트를 화면에 그리는 렌더링 컴포넌트입니다.
 * 사용하는 3D 모델(VertexArray)과 재질(Shader)에 대한 참조를 관리합니다.
 */

#pragma once

#include <memory>
#include <string>

#include "../Component.h"

namespace Proto
{
	class VertexArray;
	class Shader;

	class MeshRenderer : public Component
	{
	public:
		MeshRenderer() = default;
		MeshRenderer(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader)
			: m_VertexArray(vertexArray), m_Shader(shader)
		{}

		virtual ~MeshRenderer() = default;

		void SetMesh(const std::shared_ptr<VertexArray>& vertexArray)
		{
			m_VertexArray = vertexArray;
		}

		void SetShader(const std::shared_ptr<Shader>& shader)
		{
			m_Shader = shader;
		}

		const std::shared_ptr<VertexArray>& GetMesh() const
		{
			return m_VertexArray;
		}

		const std::shared_ptr<Shader>& GetShader() const
		{
			return m_Shader;
		}

		void SetMeshTypeName(const std::string& name)
		{
			m_MeshTypeName = name;
		}

		const std::string& GetMeshTypeName() const
		{
			return m_MeshTypeName;
		}

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(const YAML::Node& node) override;

		virtual const char* GetComponentTypeName() const override
		{
			return "MeshRenderer";
		}

	private:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<Shader> m_Shader;

		// 메쉬의 형태 식별자 (예: "Cube", "Sphere")
		std::string m_MeshTypeName;
	};
}
