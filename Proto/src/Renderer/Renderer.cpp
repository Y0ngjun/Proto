/*
 * OpenGL 렌더링 파이프라인의 기본 명령을 수행하는 클래스입니다.
 * 화면 지우기(Clear), 배경색 설정, 드로우 콜(Draw Call) 제출 기능을 담당합니다.
 */

#include <glad/glad.h>

#include "Renderer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Buffer.h"

namespace Proto
{
	void Renderer::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader)
	{
		if (!vertexArray || !shader)
		{
			return;
		}

		shader->Bind();
		vertexArray->Bind();

		const auto& indexBuffer = vertexArray->GetIndexBuffer();
		if (indexBuffer)
		{
			glDrawElements(GL_TRIANGLES, indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
		}
	}
}