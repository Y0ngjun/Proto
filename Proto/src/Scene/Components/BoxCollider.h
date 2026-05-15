/*
 * 직육면체(Box) 형태의 충돌 영역을 정의하는 컴포넌트입니다.
 * Collider 클래스를 상속받아 크기(Size) 속성을 추가로 가집니다.
 */

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
		virtual ~BoxCollider() = default;

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(const YAML::Node& node) override;

		virtual const char* GetComponentTypeName() const override
		{
			return "BoxCollider";
		}
	};
}
