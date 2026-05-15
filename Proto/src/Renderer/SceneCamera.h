/*
 * 런타임 씬에서 사용되는 카메라 클래스입니다.
 * 종횡비와 투영(Perspective) 관련 속성들을 관리합니다.
 */

#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

namespace Proto
{
	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetPerspective(float verticalFOV, float nearClip, float farClip);
		void SetViewportSize(uint32_t width, uint32_t height);

		float GetPerspectiveVerticalFOV() const
		{
			return m_PerspectiveFOV;
		}

		float GetPerspectiveNearClip() const
		{
			return m_PerspectiveNear;
		}

		float GetPerspectiveFarClip() const
		{
			return m_PerspectiveFar;
		}

		void SetPerspectiveVerticalFOV(float fov)
		{
			m_PerspectiveFOV = fov;
			RecalculateProjection();
		}

		void SetPerspectiveNearClip(float nearClip)
		{
			m_PerspectiveNear = nearClip;
			RecalculateProjection();
		}

		void SetPerspectiveFarClip(float farClip)
		{
			m_PerspectiveFar = farClip;
			RecalculateProjection();
		}

	private:
		void RecalculateProjection();

		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.1f;
		float m_PerspectiveFar = 1000.0f;
		float m_AspectRatio = 0.0f;
	};
}