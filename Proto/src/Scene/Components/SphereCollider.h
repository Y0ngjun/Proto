/*
 * 구형(Sphere) 형태의 충돌 영역을 정의하는 컴포넌트입니다.
 * Collider 클래스를 상속받아 반지름(Radius) 속성을 추가로 가집니다.
 */

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
		virtual ~SphereCollider() = default;

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(const YAML::Node& node) override;

		virtual const char* GetComponentTypeName() const override
		{
			return "SphereCollider";
		}
	};
}
