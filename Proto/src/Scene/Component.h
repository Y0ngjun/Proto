/*
 * 씬 내의 게임 오브젝트(GameObject)에 부착되는 모든 컴포넌트의 베이스 클래스입니다.
 * 생명주기(Lifecycle) 메서드와 직렬화(Serialization) 인터페이스를 정의합니다.
 */

#pragma once

namespace YAML
{
	class Emitter;
	class Node;
}

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

		GameObject* GetGameObject() const
		{
			return m_GameObject;
		}

		// 직렬화 인터페이스
		virtual void Serialize(YAML::Emitter& out) const {}
		virtual void Deserialize(const YAML::Node& node) {}
		virtual const char* GetComponentTypeName() const = 0;

	protected:
		GameObject* m_GameObject = nullptr;

		friend class GameObject;
	};
}
