#pragma once

#include "../Component.h"
#include <glm/glm.hpp>

namespace Proto
{
	class Rigidbody : public Component
	{
	public:
		glm::vec3 Velocity = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Acceleration = { 0.0f, 0.0f, 0.0f };
		float Mass = 1.0f;
		bool UseGravity = false;
		float Drag = 0.0f; // 공기 저항 (속도 감쇠)

		Rigidbody() = default;

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(const YAML::Node& node) override;
		virtual const char* GetComponentTypeName() const override { return "Rigidbody"; }
	};
}
