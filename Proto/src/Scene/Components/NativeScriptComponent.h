#pragma once

#include <typeinfo>
#include "../Component.h"
#include "../GameObject.h"
#include "../../Core/Input.h"

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

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

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
				Instance->OnUpdate(deltaTime);
		}

		void OnDestroy() override
		{
			if (Instance)
			{
				Instance->OnDestroy();
				DestroyScript(this);
			}
		}

		// 異⑸룎 ?대깽?몃? ?ㅽ겕由쏀듃濡??꾨떖
		void DispatchCollisionEnter(GameObject* other) { if (Instance) Instance->OnCollisionEnter(other); }

		virtual void Serialize(YAML::Emitter& out) const override
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Component" << YAML::Value << "NativeScriptComponent";
			out << YAML::Key << "ScriptName" << YAML::Value << ScriptName;
			out << YAML::Key << "Bound" << YAML::Value << (InstantiateScript != nullptr);
			out << YAML::EndMap;
		}

		virtual void Deserialize(const YAML::Node& node) override
		{
			// 由ы뵆?됱뀡 ?놁씠 ?ㅽ겕由쏀듃 ???蹂듭썝 遺덇?
			// 肄붾뱶 ?덈꺼(SetupCubeTest ???먯꽌 Bind ?댁빞 ?⑸땲??
		}

		virtual const char* GetComponentTypeName() const override { return "NativeScriptComponent"; }
	};
}
