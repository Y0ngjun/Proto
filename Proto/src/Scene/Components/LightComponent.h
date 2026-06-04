/*
 * 게임 오브젝트에 빛(Light) 속성을 부여하는 컴포넌트입니다.
 * 빛의 색상(Color)과 강도(Intensity)를 조절할 수 있습니다.
 */

#pragma once

#include <glm/glm.hpp>
#include "../Component.h"

namespace Proto
{
	class LightComponent : public Component
	{
	public:
		glm::vec3 Color = glm::vec3(1.0f);
		float Intensity = 1.0f;

		LightComponent() = default;
		LightComponent(const glm::vec3& color, float intensity)
			: Color(color), Intensity(intensity)
		{}

		virtual ~LightComponent() = default;

		virtual void Reset() override;
		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(const YAML::Node& node) override;

		virtual const char* GetComponentTypeName() const override
		{
			return "LightComponent";
		}
	};
}
