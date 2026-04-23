#pragma once

#include <glm/glm.hpp>
#include "../Component.h"

namespace Proto
{
	class LightComponent : public Component
	{
	public:
		glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;

		LightComponent() = default;
		LightComponent(const glm::vec3& color, float intensity)
			: Color(color), Intensity(intensity) {}
	};
}
