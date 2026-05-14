#define GLM_ENABLE_EXPERIMENTAL

#include <glfw/glfw3.h>
#include <glm/gtx/quaternion.hpp>
#include <algorithm>

#include "EditorCamera.h"
#include "../Core/RawInput.h"

namespace Proto
{
	static constexpr float MouseSensitivity = 0.003f;
	static constexpr float RotationSensitivity = 0.8f;
	static constexpr float MovementSpeed = 5.0f;

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

		const glm::quat orientation = GetOrientation();
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
		const glm::vec2 mouse{ (float)xpos, (float)ypos };
		const glm::vec2 delta = (mouse - m_InitialMousePosition) * MouseSensitivity;
		m_InitialMousePosition = mouse;

		HandleMouseInput(delta);
		HandleKeyboardInput(deltaTime);

		UpdateView();
	}

	void EditorCamera::HandleMouseInput(const glm::vec2& delta)
	{
		if (RawInput::GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT))
		{
			MouseRotate(delta);
			// Update focal point to keep position fixed (Free Look style rotation)
			m_FocalPoint = m_Position + GetForwardDirection() * m_Distance;
		}
		else if (RawInput::GetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE))
		{
			MousePan(delta);
		}
	}

	void EditorCamera::HandleKeyboardInput(float deltaTime)
	{
		if (!RawInput::GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT))
			return;

		const float speed = MovementSpeed * deltaTime;
		const glm::vec3 forward = GetForwardDirection();
		const glm::vec3 right = GetRightDirection();
		const glm::vec3 up = GetUpDirection();

		if (RawInput::GetKey(GLFW_KEY_W)) m_FocalPoint += forward * speed;
		if (RawInput::GetKey(GLFW_KEY_S)) m_FocalPoint -= forward * speed;
		if (RawInput::GetKey(GLFW_KEY_A)) m_FocalPoint -= right * speed;
		if (RawInput::GetKey(GLFW_KEY_D)) m_FocalPoint += right * speed;
		if (RawInput::GetKey(GLFW_KEY_E)) m_FocalPoint += up * speed;
		if (RawInput::GetKey(GLFW_KEY_Q)) m_FocalPoint -= up * speed;
	}

	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		const float speed = m_Distance * 0.5f;
		m_FocalPoint += -GetRightDirection() * delta.x * speed;
		m_FocalPoint += GetUpDirection() * delta.y * speed;
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		const float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * RotationSensitivity;
		m_Pitch += delta.y * RotationSensitivity;
	}
}
