#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../Component.h"

namespace Proto
{
	class Transform : public Component
	{
	public:
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		Transform() = default;
		Transform(const glm::vec3& translation)
			: Translation(translation) {}

		glm::mat4 GetTransform() const;
	};
}