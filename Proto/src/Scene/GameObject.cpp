/*
 * 씬 내의 기본 엔티티 단위인 게임 오브젝트 클래스입니다.
 * 고유 ID(UUID, RuntimeID)를 가지며 여러 컴포넌트를 부착(Add)하고 관리할 수 있습니다.
 */

#include "GameObject.h"

namespace Proto
{
	namespace
	{
		// 0은 'None'을 위해 예약되어 있으므로 1부터 시작합니다.
		static constexpr uint32_t INITIAL_RUNTIME_ID = 1;
	}

	static uint32_t s_EntityIDCounter = INITIAL_RUNTIME_ID;

	GameObject::GameObject(const std::string& name)
		: m_UUID(), m_RuntimeID(s_EntityIDCounter++), m_Name(name)
	{}

	GameObject::~GameObject()
	{
		for (const auto& comp : m_Components)
		{
			comp->OnDestroy();
		}
	}

	void GameObject::Update(float deltaTime)
	{
		for (const auto& comp : m_Components)
		{
			comp->OnUpdate(deltaTime);
		}
	}

	void GameObject::ResetIDCounter()
	{
		s_EntityIDCounter = INITIAL_RUNTIME_ID;
	}

	void GameObject::SetMaxIDCounter(uint32_t maxID)
	{
		s_EntityIDCounter = maxID + 1;
	}
}