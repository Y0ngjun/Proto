/*
 * C++ 코드로 작성된 사용자 정의 스크립트(Native Script)를 게임 오브젝트에 부착하고 관리하는 컴포넌트입니다.
 * ScriptableEntity를 상속받은 커스텀 스크립트 클래스의 인스턴스화, 생명주기, 이벤트를 처리합니다.
 */

#pragma once

#include <string>
#include <functional>
#include <typeinfo>
#include "../Component.h"
#include "../GameObject.h"

namespace Proto
{
	class ScriptableEntity;
	class NativeScriptComponent;

	using ScriptCreatorFunc = std::function<ScriptableEntity* ()>;

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() = default;

		template<typename T>
		T* GetComponent()
		{
			return m_GameObject->GetComponent<T>();
		}

		GameObject* GetGameObject()
		{
			return m_GameObject;
		}

	protected:
		virtual void OnStart() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnDestroy() {}
		virtual void OnCollisionEnter(GameObject* other) {}

	private:
		GameObject* m_GameObject = nullptr;

		friend class NativeScriptComponent;
	};

	class NativeScriptComponent : public Component
	{
	public:
		std::string ScriptName;
		ScriptableEntity* Instance = nullptr;

		ScriptCreatorFunc InstantiateScript;
		std::function<void(NativeScriptComponent*)> DestroyScript;

		NativeScriptComponent() = default;
		virtual ~NativeScriptComponent() = default;

		template<typename T>
		void Bind()
		{
			ScriptName = typeid(T).name();
			InstantiateScript = []()
				{
					return static_cast<ScriptableEntity*>(new T());
				};

			DestroyScript = [](NativeScriptComponent* component)
				{
					delete component->Instance;
					component->Instance = nullptr;
				};
		}

		void OnStart() override
		{
			if (!InstantiateScript || Instance)
			{
				return;
			}

			Instance = InstantiateScript();
			if (Instance)
			{
				Instance->m_GameObject = m_GameObject;
				Instance->OnStart();
			}
		}

		void OnUpdate(float deltaTime) override
		{
			if (Instance)
			{
				Instance->OnUpdate(deltaTime);
			}
		}

		void OnDestroy() override
		{
			if (!Instance)
			{
				return;
			}

			Instance->OnDestroy();
			if (DestroyScript)
			{
				DestroyScript(this);
			}
		}

		void DispatchCollisionEnter(GameObject* other)
		{
			if (Instance)
			{
				Instance->OnCollisionEnter(other);
			}
		}

		virtual void Reset() override;
		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(const YAML::Node& node) override;

		virtual const char* GetComponentTypeName() const override
		{
			return "NativeScriptComponent";
		}
	};
}
