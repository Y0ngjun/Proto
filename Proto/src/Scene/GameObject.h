/*
 * 씬 내의 기본 엔티티 단위인 게임 오브젝트 클래스입니다.
 * 고유 ID(UUID, RuntimeID)를 가지며 여러 컴포넌트를 부착(Add)하고 관리할 수 있습니다.
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <typeinfo>
#include <algorithm>

#include "Component.h"
#include "../Core/UUID.h"

namespace Proto
{
	class GameObject
	{
	public:
		GameObject(const std::string& name = "GameObject");
		~GameObject();

		const std::string& GetName() const
		{
			return m_Name;
		}

		void SetName(const std::string& name)
		{
			m_Name = name;
		}

		UUID GetUUID() const
		{
			return m_UUID;
		}

		void SetUUID(UUID uuid)
		{
			m_UUID = uuid;
		}

		uint32_t GetRuntimeID() const
		{
			return m_RuntimeID;
		}

		void SetRuntimeID(uint32_t id)
		{
			m_RuntimeID = id;
		}

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

		template<typename T>
		void RemoveComponent()
		{
			auto it = std::find_if(m_Components.begin(), m_Components.end(),
				[](const std::unique_ptr<Component>& comp)
				{
					return dynamic_cast<T*>(comp.get()) != nullptr;
				});

			if (it != m_Components.end())
			{
				m_Components.erase(it);
			}
		}

		template<typename T>
		bool HasComponent()
		{
			return GetComponent<T>() != nullptr;
		}

		template<typename T>
		T& GetComponentRef()
		{
			T* component = GetComponent<T>();
			return *component;
		}

		const std::vector<std::unique_ptr<Component>>& GetComponents() const
		{
			return m_Components;
		}

	private:
		UUID m_UUID;
		uint32_t m_RuntimeID;
		std::string m_Name;
		std::vector<std::unique_ptr<Component>> m_Components;
	};
}
