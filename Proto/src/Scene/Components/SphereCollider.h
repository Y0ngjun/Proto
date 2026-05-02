#pragma once

#include "Collider.h"
#include <glm/glm.hpp>

namespace Proto
{
	class SphereCollider : public Collider
	{
	public:
		float Radius = 0.5f;

		SphereCollider() = default;

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(const YAML::Node& node) override;
		virtual const char* GetComponentTypeName() const override { return "SphereCollider"; }
	};
}
