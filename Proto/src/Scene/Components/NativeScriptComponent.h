#pragma once

#include <string>
#include <functional>
#include <typeinfo>
#include "../Component.h"
#include "../GameObject.h"
#include "../../Core/Input.h"

namespace Proto
{
	class ScriptableEntity;
	class NativeScriptComponent;

	// 스크립트 생성을 위한 함수 타입 (ScriptRegistry와 동일하게 맞춤)
	using ScriptCreatorFunc = std::function<ScriptableEntity*()>;

	/**
	 * @brief 사용자가 상속받아 로직을 작성하는 베이스 클래스
	 */
	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() = default;

		template<typename T>
		T* GetComponent()
		{
			return m_GameObject->GetComponent<T>();
		}

		GameObject* GetGameObject() { return m_GameObject; }

	protected:
		virtual void OnStart() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnDestroy() {}
		virtual void OnCollisionEnter(GameObject* other) {}

	private:
		GameObject* m_GameObject = nullptr;

		friend class NativeScriptComponent;
	};

	/**
	 * @brief 게임 오브젝트에 C++ 스크립트를 부착할 수 있게 해주는 컴포넌트
	 */
	class NativeScriptComponent : public Component
	{
	public:
		std::string ScriptName;
		ScriptableEntity* Instance = nullptr;

		// 스크립트 인스턴스 생성 및 파괴를 위한 함수
		ScriptCreatorFunc InstantiateScript;
		std::function<void(NativeScriptComponent*)> DestroyScript;

		/**
		 * @brief 컴파일 타임에 스크립트 타입을 직접 바인딩
		 */
		template<typename T>
		void Bind()
		{
			ScriptName = typeid(T).name();
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc)
			{
				delete nsc->Instance;
				nsc->Instance = nullptr;
			};
		}

		void OnStart() override
		{
			if (InstantiateScript && !Instance)
			{
				Instance = InstantiateScript();
				if (Instance)
				{
					Instance->m_GameObject = m_GameObject;
					Instance->OnStart();
				}
			}
		}

		void OnUpdate(float deltaTime) override
		{
			if (Instance)
				Instance->OnUpdate(deltaTime);
		}

		void OnDestroy() override
		{
			if (Instance)
			{
				Instance->OnDestroy();
				if (DestroyScript)
					DestroyScript(this);
			}
		}

		// 충돌 이벤트를 스크립트 인스턴스로 전달
		void DispatchCollisionEnter(GameObject* other) { if (Instance) Instance->OnCollisionEnter(other); }

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(const YAML::Node& node) override;

		virtual const char* GetComponentTypeName() const override { return "NativeScriptComponent"; }
	};
}
