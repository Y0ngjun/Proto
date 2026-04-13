#pragma once

#include <memory>

#include "../Component.h"
#include "../../Renderer/VertexArray.h"
#include "../../Renderer/Shader.h"

namespace Proto
{
	class MeshRenderer : public Component
	{
	public:
		MeshRenderer() = default;
		MeshRenderer(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader)
			: m_VertexArray(vertexArray), m_Shader(shader) {}

		void SetMesh(const std::shared_ptr<VertexArray>& vertexArray) { m_VertexArray = vertexArray; }
		void SetShader(const std::shared_ptr<Shader>& shader) { m_Shader = shader; }

		const std::shared_ptr<VertexArray>& GetMesh() const { return m_VertexArray; }
		const std::shared_ptr<Shader>& GetShader() const { return m_Shader; }

	private:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<Shader> m_Shader;
	};
}