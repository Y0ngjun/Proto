/*
 * 게임 오브젝트의 위치(Translation), 회전(Rotation), 크기(Scale)를 관리하는 핵심 컴포넌트입니다.
 * 이 세 값을 조합하여 최종 변환 행렬(Transform Matrix)을 생성합니다.
 */

#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../Component.h"

namespace Proto
{
	class Transform : public Component
	{
	public:
		glm::vec3 Translation = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);

		Transform() = default;
		Transform(const glm::vec3& translation)
			: Translation(translation)
		{}

		virtual ~Transform() = default;

		glm::mat4 GetTransform() const;

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(const YAML::Node& node) override;

		virtual const char* GetComponentTypeName() const override
		{
			return "Transform";
		}
	};
}
