/*
 * 두 게임 오브젝트 간 충돌 발생 시 EventBus를 통해 발행되는 이벤트입니다.
 */

#pragma once

namespace Proto
{
	class GameObject;

	struct CollisionEvent
	{
		GameObject* ObjectA;
		GameObject* ObjectB;
	};
}
