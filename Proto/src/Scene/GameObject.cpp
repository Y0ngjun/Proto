#include "GameObject.h"

namespace Proto
{
	static uint32_t s_EntityIDCounter = 1; // 0 is reserved for 'None', start from 1

	GameObject::GameObject(const std::string& name)
		: m_UUID(), m_RuntimeID(s_EntityIDCounter++), m_Name(name)
	{
	}

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

	void GameObject::ResetIDCounter()
	{
		s_EntityIDCounter = 1;
	}

	void GameObject::SetMaxIDCounter(uint32_t maxID)
	{
		s_EntityIDCounter = maxID + 1;
	}
}