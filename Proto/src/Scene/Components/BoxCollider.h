#pragma once

#include "Collider.h"
#include <glm/glm.hpp>

namespace Proto
{
	class BoxCollider : public Collider
	{
	public:
		glm::vec3 Size = { 1.0f, 1.0f, 1.0f };

		BoxCollider() = default;

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(const YAML::Node& node) override;
		virtual const char* GetComponentTypeName() const override { return "BoxCollider"; }
	};
}
