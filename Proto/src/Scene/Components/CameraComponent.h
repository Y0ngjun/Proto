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
	};
}