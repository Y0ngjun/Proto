/*
 * 모든 충돌체(Collider) 컴포넌트의 기본이 되는 추상화 클래스입니다.
 * 충돌체의 오프셋(Offset)과 트리거(IsTrigger) 여부를 공통으로 관리합니다.
 */

#pragma once

#include "../Component.h"
#include <glm/glm.hpp>

namespace Proto
{
	class Collider : public Component
	{
	public:
		glm::vec3 Offset = glm::vec3(0.0f);
		bool IsTrigger = true;

		virtual ~Collider() = default;

		virtual const char* GetComponentTypeName() const override
		{
			return "Collider";
		}
	};
}
