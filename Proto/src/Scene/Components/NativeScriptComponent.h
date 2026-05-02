#pragma once

#include "../Component.h"
#include "../GameObject.h"

namespace Proto
{
	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() = default;

		template<typename T>
		T* GetComponent()
		{
			return m_GameObject->GetComponent<T>();
		}

	protected:
		virtual void OnStart() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnDestroy() {}

	private:
		GameObject* m_GameObject = nullptr;

		friend class NativeScriptComponent;
	};

	class NativeScriptComponent : public Component
	{
	public:
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}

		void OnStart() override
		{
			if (InstantiateScript)
			{
				Instance = InstantiateScript();
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
			if (Instance)
			{
				Instance->OnDestroy();
				DestroyScript(this);
			}
		}

		virtual void Serialize(YAML::Emitter& out) const override
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Component" << YAML::Value << "NativeScriptComponent";
			out << YAML::Key << "Bound" << YAML::Value << (InstantiateScript != nullptr);
			out << YAML::EndMap;
		}

		virtual void Deserialize(const YAML::Node& node) override
		{
			// 현재는 리플렉션이 없어 직렬화된 이름으로 클래스를 동적 생성하기 어려우므로, 
			// 스크립트는 코드 레벨(SetupCubeTest 등)에서 Bind 해야 합니다.
		}

		virtual const char* GetComponentTypeName() const override { return "NativeScriptComponent"; }
	};
}
