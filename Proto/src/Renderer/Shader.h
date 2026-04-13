#pragma once

#include <glm/glm.hpp>
#include <string>
#include <cstdint>

namespace Proto
{
	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformInt(const std::string& name, int value);

	private:
		uint32_t m_RendererID;

		void CompileShader(uint32_t shader, const std::string& source);
	};
}