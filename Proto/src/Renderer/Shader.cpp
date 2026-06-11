/*
 * OpenGL 셰이더(Shader) 프로그램을 생성하고 관리하는 클래스입니다.
 * 정점 셰이더와 프래그먼트 셰이더를 컴파일 및 링크하며, 유니폼(Uniform) 변수 업로드 기능을 제공합니다.
 */

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include "Shader.h"
#include "../Core/Log.h"

namespace Proto
{
	namespace
	{
		static constexpr uint32_t UNBIND_TARGET_ID = 0;
	}

	Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		const uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
		CompileShader(vertexShader, vertexSrc);

		const uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		CompileShader(fragmentShader, fragmentSrc);

		m_RendererID = glCreateProgram();
		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);
		glLinkProgram(m_RendererID);

		int isLinked = 0;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &isLinked);

		if (isLinked == GL_FALSE)
		{
			int maxLength = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<char> infoLog(maxLength);
			glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(m_RendererID);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			PROTO_LOG_ERROR("[셰이더 링크 오류] " + std::string(infoLog.data()));
			return;
		}

		glDetachShader(m_RendererID, vertexShader);
		glDetachShader(m_RendererID, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}

	void Shader::CompileShader(uint32_t shader, const std::string& source)
	{
		const char* src = source.c_str();
		glShaderSource(shader, 1, &src, 0);
		glCompileShader(shader);

		int isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

		if (isCompiled == GL_FALSE)
		{
			int maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<char> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(shader);

			PROTO_LOG_ERROR("[셰이더 컴파일 오류] " + std::string(infoLog.data()));
		}
	}

	void Shader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void Shader::Unbind() const
	{
		glUseProgram(UNBIND_TARGET_ID);
	}

	void Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		const int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
	{
		const int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, values.x, values.y, values.z, values.w);
	}

	void Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		const int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, values.x, values.y, values.z);
	}

	void Shader::UploadUniformFloat(const std::string& name, float value)
	{
		const int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void Shader::UploadUniformInt(const std::string& name, int value)
	{
		const int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	std::string Shader::ReadFile(const std::string& filePath)
	{
		std::ifstream file(filePath);
		if (!file.is_open())
		{
			PROTO_LOG_ERROR("[셰이더 파일 오류] 파일을 열 수 없습니다: " + filePath);
			return "";
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	std::shared_ptr<Shader> Shader::LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath)
	{
		const std::string vertexSrc = ReadFile(vertexPath);
		const std::string fragmentSrc = ReadFile(fragmentPath);

		if (vertexSrc.empty() || fragmentSrc.empty())
		{
			PROTO_LOG_ERROR("[셰이더 로드 오류] 파일에서 셰이더를 로드하지 못했습니다.");
			return nullptr;
		}

		return std::make_shared<Shader>(vertexSrc, fragmentSrc);
	}
}