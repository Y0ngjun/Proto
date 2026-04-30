#pragma once

#include "../Component.h"
#include "../../Renderer/SceneCamera.h"

namespace Proto
{
	class CameraComponent : public Component
	{
	public:
		SceneCamera Camera;

		bool Primary = true;

		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

		// 직렬화
		void Serialize(YAML::Emitter& out) const override;
		void Deserialize(const YAML::Node& node) override;
		const char* GetComponentTypeName() const override { return "CameraComponent"; }
	};
}
