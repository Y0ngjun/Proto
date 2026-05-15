/*
 * 모든 카메라 클래스의 기본이 되는 베이스 클래스입니다.
 * 투영 행렬(Projection Matrix) 정보를 관리합니다.
 */

#pragma once

#include <glm/glm.hpp>

namespace Proto
{
	class Camera
	{
	public:
		Camera() = default;
		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const
		{
			return m_Projection;
		}

	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};
}