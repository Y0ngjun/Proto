/*
 * 게임 오브젝트에 부착되어 해당 위치/회전에서 씬을 렌더링하는 카메라 컴포넌트입니다.
 * 주 카메라(Primary) 여부 및 고정 종횡비 옵션을 포함합니다.
 */

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
		virtual ~CameraComponent() = default;

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(const YAML::Node& node) override;

		virtual const char* GetComponentTypeName() const override
		{
			return "CameraComponent";
		}
	};
}
