#pragma once

#include <yaml-cpp/yaml.h>

namespace Proto
{
	class GameObject;

	class Component
	{
	public:
		virtual ~Component() = default;

		virtual void OnStart() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnDestroy() {}

		GameObject* GetGameObject() const { return m_GameObject; }

		// 직렬화 인터페이스
		virtual void Serialize(YAML::Emitter& out) const {}
		virtual void Deserialize(const YAML::Node& node) {}
		virtual const char* GetComponentTypeName() const = 0;

	protected:
		GameObject* m_GameObject = nullptr;

		friend class GameObject;
	};
}
