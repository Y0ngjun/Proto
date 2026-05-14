#pragma once

#include <string>
#include <vector>
#include <memory>
#include <typeinfo>

#include "Component.h"
#include "../Core/UUID.h"

namespace Proto
{
	class GameObject
	{
	public:
		GameObject(const std::string& name = "GameObject");
		~GameObject();

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }
		UUID GetUUID() const { return m_UUID; }
		void SetUUID(UUID uuid) { m_UUID = uuid; }
		uint32_t GetRuntimeID() const { return m_RuntimeID; }
		void SetRuntimeID(uint32_t id) { m_RuntimeID = id; }
		void Update(float deltaTime);

		static void ResetIDCounter();
		static void SetMaxIDCounter(uint32_t maxID);

		template<typename T, typename... Args>
		T* AddComponent(Args... args)
		{
			T* component = new T(std::forward<Args>(args)...);
			component->m_GameObject = this;
			m_Components.push_back(std::unique_ptr<Component>(component));
			return component;
		}

		template<typename T>
		T* GetComponent()
		{
			for (auto& comp : m_Components)
			{
				T* target = dynamic_cast<T*>(comp.get());
				if (target)
				{
					return target;
				}
			}
			return nullptr;
		}

		// 컴포넌트 삭제
		template<typename T>
		void RemoveComponent()
		{
			auto it = std::find_if(m_Components.begin(), m_Components.end(),
				[](const std::unique_ptr<Component>& comp) {
					return dynamic_cast<T*>(comp.get()) != nullptr;
				});

			if (it != m_Components.end())
			{
				m_Components.erase(it);
			}
		}

		const std::vector<std::unique_ptr<Component>>& GetComponents() const { return m_Components; }

	private:
		UUID m_UUID;
		uint32_t m_RuntimeID;
		std::string m_Name;
		std::vector<std::unique_ptr<Component>> m_Components;
	};
}
