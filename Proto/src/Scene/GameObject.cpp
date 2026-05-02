#include "GameObject.h"

namespace Proto
{
	static uint32_t s_EntityIDCounter = 1; // 0은 '아무것도 없음'을 의미하도록 1부터 시작

	GameObject::GameObject(const std::string& name)
		: m_ID(s_EntityIDCounter++), m_Name(name) {}

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