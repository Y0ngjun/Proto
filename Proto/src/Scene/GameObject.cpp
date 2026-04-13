#include "GameObject.h"

namespace Proto
{
	GameObject::GameObject(const std::string& name)
		: m_Name(name) {}

	GameObject::~GameObject()
	{
		for (auto& comp : m_Components)
		{
			comp->OnDestroy();
		}
	}

	void GameObject::Update(float deltaTime)
	{
		for (auto& comp : m_Components)
		{
			comp->OnUpdate(deltaTime);
		}
	}

}