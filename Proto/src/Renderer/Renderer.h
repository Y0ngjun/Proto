#pragma once

#include "VertexArray.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <memory>

namespace Proto {

	class Renderer {
	public:
		static void SetClearColor(const glm::vec4& color);
		static void Clear();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader);
	};

}