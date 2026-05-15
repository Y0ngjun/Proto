/*
 * 게임 오브젝트에 물리 법칙(중력, 속도, 마찰 등)을 적용하는 리지드바디 컴포넌트입니다.
 * 질량(Mass)과 공기 저항(Drag) 등을 통해 물리 시뮬레이션의 계산에 사용됩니다.
 */

#pragma once

#include "../Component.h"
#include <glm/glm.hpp>

namespace Proto
{
	class Rigidbody : public Component
	{
	public:
		glm::vec3 Velocity = glm::vec3(0.0f);
		glm::vec3 Acceleration = glm::vec3(0.0f);

		float Mass = 1.0f;
		bool UseGravity = false;

		// 속도 감쇠(저항)
		float Drag = 0.0f;

		Rigidbody() = default;
		virtual ~Rigidbody() = default;

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(const YAML::Node& node) override;

		virtual const char* GetComponentTypeName() const override
		{
			return "Rigidbody";
		}
	};
}
