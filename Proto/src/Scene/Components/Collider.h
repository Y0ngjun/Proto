#pragma once

#include "../Component.h"
#include <glm/glm.hpp>

namespace Proto
{
	// 충돌체의 공통 추상 인터페이스
	// BoxCollider, SphereCollider가 이 클래스를 상속
	class Collider : public Component
	{
	public:
		glm::vec3 Offset = { 0.0f, 0.0f, 0.0f };
		bool IsTrigger = true;

		virtual ~Collider() = default;

		virtual const char* GetComponentTypeName() const override { return "Collider"; }
	};
}
