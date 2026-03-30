#pragma once

namespace Proto {
	class GameObject;

	class Component {
	public:
		virtual ~Component() = default;

		virtual void OnStart() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnDestroy() {}

		GameObject* GetGameObject() const { return m_GameObject; }

	protected:
		GameObject* m_GameObject = nullptr;

		friend class GameObject;
	};
}
