/*
 * OpenGL 렌더링 파이프라인의 기본 명령을 수행하는 클래스입니다.
 * 화면 지우기(Clear), 배경색 설정, 드로우 콜(Draw Call) 제출 기능을 담당합니다.
 */

#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace Proto
{
	class VertexArray;
	class Shader;

	class Renderer
	{
	public:
		static void SetClearColor(const glm::vec4& color);
		static void Clear();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader);

	private:
		Renderer() = default;
		~Renderer() = default;
	};
}