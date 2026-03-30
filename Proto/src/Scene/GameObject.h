#pragma once

#include "Component.h"
#include <string>
#include <vector>
#include <memory>
#include <typeinfo>

namespace Proto {

	class GameObject {
	public:
		GameObject(const std::string& name = "GameObject");
		~GameObject();

		const std::string& GetName() const { return m_Name; }
		void Update(float deltaTime);

		template<typename T, typename... Args>
		T* AddComponent(Args... args) {
			T* component = new T(std::forward<Args>(args)...);
			component->m_GameObject = this;
			m_Components.push_back(std::unique_ptr<Component>(component));
			component->OnStart();
			return component;
		}

		template<typename T>
		T* GetComponent() {
			for (auto& comp : m_Components) {
				T* target = dynamic_cast<T*>(comp.get());
				if (target) {
					return target;
				}
			}
			return nullptr;
		}

	private:
		std::string m_Name;
		std::vector<std::unique_ptr<Component>> m_Components;
	};

}