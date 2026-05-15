/*
 * OpenGL 셰이더(Shader) 프로그램을 생성하고 관리하는 클래스입니다.
 * 정점 셰이더와 프래그먼트 셰이더를 컴파일 및 링크하며, 유니폼(Uniform) 변수 업로드 기능을 제공합니다.
 */

#pragma once

#include <glm/glm.hpp>
#include <string>
#include <cstdint>
#include <memory>
#include "../Asset/Asset.h"

namespace Proto
{
	class Shader : public Asset
	{
	public:
		virtual AssetType GetType() const override
		{
			return AssetType::Shader;
		}

		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~Shader();

		void Bind() const;
		void Unbind() const;

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformInt(const std::string& name, int value);

		static std::shared_ptr<Shader> LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath);

	private:
		uint32_t m_RendererID = 0;

		void CompileShader(uint32_t shader, const std::string& source);
		static std::string ReadFile(const std::string& filePath);
	};
}
