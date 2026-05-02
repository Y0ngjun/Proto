#define GLM_ENABLE_EXPERIMENTAL

#include <glfw/glfw3.h>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

#include "EditorCamera.h"
#include "../Core/RawInput.h"

namespace Proto
{
	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
	{
		UpdateProjection();
		UpdateView();
	}

	void EditorCamera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void EditorCamera::UpdateView()
	{
		m_Position = CalculatePosition();

		glm::quat orientation = GetOrientation();
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	glm::vec3 EditorCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

	glm::vec3 EditorCamera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	void EditorCamera::OnUpdate(float deltaTime)
	{
		double xpos, ypos;
		RawInput::GetMousePosition(xpos, ypos);
		glm::vec2 mouse{ (float)xpos, (float)ypos };
		glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
		m_InitialMousePosition = mouse;

		if (RawInput::GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT))
		{
			MouseRotate(delta);
			// Update focal point to keep position fixed (Free Look style rotation)
			m_FocalPoint = m_Position + GetForwardDirection() * m_Distance;

			float speed = 5.0f * deltaTime;
			if (RawInput::GetKey(GLFW_KEY_W))
				m_FocalPoint += GetForwardDirection() * speed;
			if (RawInput::GetKey(GLFW_KEY_S))
				m_FocalPoint -= GetForwardDirection() * speed;
			if (RawInput::GetKey(GLFW_KEY_A))
				m_FocalPoint -= GetRightDirection() * speed;
			if (RawInput::GetKey(GLFW_KEY_D))
				m_FocalPoint += GetRightDirection() * speed;
			if (RawInput::GetKey(GLFW_KEY_E))
				m_FocalPoint += GetUpDirection() * speed;
			if (RawInput::GetKey(GLFW_KEY_Q))
				m_FocalPoint -= GetUpDirection() * speed;
		}
		else if (RawInput::GetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE))
		{
			MousePan(delta);
		}

		UpdateView();
	}

	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		// Optional: basic pan logic
		float speed = m_Distance * 0.5f;
		m_FocalPoint += -GetRightDirection() * delta.x * speed;
		m_FocalPoint += GetUpDirection() * delta.y * speed;
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * 0.8f;
		m_Pitch += delta.y * 0.8f;
	}

	void EditorCamera::MouseZoom(float delta)
	{
		m_Distance -= delta * 0.5f;
		if (m_Distance < 0.1f)
			m_Distance = 0.1f;
	}
}
